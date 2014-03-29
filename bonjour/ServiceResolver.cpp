/*
 Copyright (c) 2007, Trenton Schulz

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QtCore/QSocketNotifier>
#include <QtNetwork/QHostAddress>

#include "Record.hpp"
#include "ServiceResolver.hpp"

namespace bonjour {

ServiceResolver::ServiceResolver(QObject *parent) :
		QObject(parent), dnssref(0), bonjourSocket(0), bonjourPort(-1) {
	m_currentState = IDLE;
}

const char * ServiceResolver::currentStateString() {
	switch (m_currentState) {
	case IDLE:
		return "IDLE";
		break;
	case RESOLVING_RECORD:
		return "RESOLVING_RECORD";
		break;
	case RESOLVING_TARGET_ADDRESS:
		return "RESOLVING_TARGET_ADDRESS";
		break;
	default:
		break;
	}
	return "";
}

ServiceResolver::~ServiceResolver() {
	cleanupResolve();
}

void ServiceResolver::cleanupResolve() {
	qDebug() << "BonjourServiceResolver::cleanupResolve";
	if (dnssref) {
		delete bonjourSocket;
		bonjourPort = -1;
		DNSServiceRefDeallocate(dnssref);
		dnssref = 0;
	}
	m_currentState = IDLE;
}

void ServiceResolver::resolveRecord(const Record &record) {
	qDebug() << "BonjourServiceResolver::resolveBonjourRecord";
	if (m_currentState == IDLE) {
		if (dnssref) {
			qWarning("resolve in process, aborting");
			return;
		}
		DNSServiceErrorType err = DNSServiceResolve(&dnssref,
				kDNSServiceFlagsTimeout, 0,
				record.serviceName().toUtf8().constData(),
				record.registeredType().toUtf8().constData(),
				record.replyDomain().toUtf8().constData(),
				(DNSServiceResolveReply) bonjourResolveReply, this);
		if (err != kDNSServiceErr_NoError) {
			qDebug() << "BonjourServiceResolver::resolveBonjourRecord:ERROR"
					<< err;
			emit error(err);
		} else {
			int sockfd = DNSServiceRefSockFD(dnssref);
			if (sockfd == -1) {
				emit error(kDNSServiceErr_Invalid);
			} else {
				bonjourSocket = new QSocketNotifier(sockfd,
						QSocketNotifier::Read, this);
				connect(bonjourSocket, SIGNAL(activated(int)), this,
						SLOT(socketReadyRead()));
				m_currentState = RESOLVING_RECORD;
			}
		}
	}
}

void ServiceResolver::socketReadyRead() {
	DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
	if (err != kDNSServiceErr_NoError) {
		if (err == kDNSServiceErr_BadParam) {
			qDebug() << "BonjourServiceResolver::bonjourSocketReadyRead:"
					<< this->currentStateString()
					<< ": ERROR:" "kDNSServiceErr_BadParam";
			Q_ASSERT_X(false, "BonjourServiceResolver::bonjourSocketReadyRead",
					"kDNSServiceErr_BadParam");
		} else {
			qDebug() << "BonjourServiceResolver::bonjourSocketReadyRead:ERROR:"
					<< this->currentStateString() << err;
		}
		emit error(err);
	}
}

void ServiceResolver::bonjourResolveReply(DNSServiceRef, DNSServiceFlags,
		quint32, DNSServiceErrorType errorCode, const char *,
		const char *hosttarget, quint16 port, quint16 txtLen,
		const char * txtRecord, void *context) {
	Q_UNUSED(txtLen);
	ServiceResolver *serviceResolver = static_cast<ServiceResolver *>(context);
	if (errorCode != kDNSServiceErr_NoError) {
		emit serviceResolver->error(errorCode);
		return;
	}
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
	{
		port = 0 | ((port & 0x00ff) << 8) | ((port & 0xff00) >> 8);
	}
#endif
	qDebug() << "BonjourServiceResolver::bonjourResolveReply"
			<< qPrintable(QString::fromUtf8(hosttarget)) << port << "TXT"
			<< txtRecord;
	serviceResolver->bonjourPort = port;
	serviceResolver->resolveTargetAddress(hosttarget);
}

void ServiceResolver::concludResolve(const QHostAddress &hostAddress) {
	if (hostAddress.isNull()) {
		qDebug() << "BonjourServiceResolver::finishConnect:Lookup failed";
		return;
	}
	qDebug() << "BonjourServiceResolver::finishConnect";
	emit recordResolved(hostAddress, bonjourPort);
	QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void ServiceResolver::resolveBonjourTargetAddressReply(DNSServiceRef sdRef,
		DNSServiceFlags flags, quint32 interfaceIndex,
		DNSServiceErrorType errorCode, const char *hostName,
		const struct sockaddr* address, quint32 ttl, void *context) {
	Q_UNUSED(interfaceIndex);
	Q_UNUSED(sdRef);
	Q_UNUSED(ttl);

	ServiceResolver *serviceResolver = static_cast<ServiceResolver *>(context);
	if (errorCode != kDNSServiceErr_NoError) {
		emit serviceResolver->error(errorCode);
		return;
	}
	if ((flags & kDNSServiceFlagsAdd) != 0) {
		QHostAddress hAddress(address);
		qDebug() << "BonjourServiceResolver::bonjourGetAddrInfoReply:IP for"
				<< hostName << " is:" << qPrintable(hAddress.toString());
		serviceResolver->concludResolve(hAddress);
	} else {
		Q_ASSERT_X(true, "BonjourServiceResolver::bonjourGetAddrInfoReply",
				"Lookup Failed");
	}
}

void ServiceResolver::resolveTargetAddress(const char * hostName) {
	//if (!m_lookUpInprogress) {
	if (m_currentState == RESOLVING_RECORD) {
		DNSServiceErrorType err = DNSServiceGetAddrInfo(&dnssref,
				kDNSServiceFlagsForceMulticast, 0, kDNSServiceProtocol_IPv4,
				hostName,
				(DNSServiceGetAddrInfoReply) resolveBonjourTargetAddressReply,
				this);
		if (err != kDNSServiceErr_NoError) {
			emit error(err);
		} else {
			int sockfd = DNSServiceRefSockFD(dnssref);
			if (sockfd == -1) {
				qDebug()
						<< "BonjourServiceResolver::resolveBonjourTargetAddress:ERROR:kDNSServiceErr_Invalid";
				emit error(kDNSServiceErr_Invalid);
			} else {
				//m_lookUpInprogress = true;
				qDebug()
						<< "BonjourServiceResolver::resolveBonjourTargetAddress:SockFD:"
						<< sockfd;
				bonjourSocket = new QSocketNotifier(sockfd,
						QSocketNotifier::Read, this);
				connect(bonjourSocket, SIGNAL(activated(int)), this,
						SLOT(socketReadyRead()));
				m_currentState = RESOLVING_TARGET_ADDRESS;
			}
		}
	} else {
		qDebug()
				<< "BonjourServiceResolver::resolveBonjourTargetAddress:WARNING:Lookup process is already started.";
	}
}

}  // namespace bonjour
