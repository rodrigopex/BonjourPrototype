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

#include "bonjourrecord.h"
#include "bonjourserviceresolver.h"

BonjourServiceResolver::BonjourServiceResolver(QObject *parent) :
		QObject(parent), dnssref(0), bonjourSocket(0), bonjourPort(-1) {
}

BonjourServiceResolver::~BonjourServiceResolver() {
	cleanupResolve();
}

void BonjourServiceResolver::cleanupResolve() {
	if (dnssref) {
		DNSServiceRefDeallocate(dnssref);
		dnssref = 0;
		delete bonjourSocket;
		bonjourPort = -1;
	}
}

void BonjourServiceResolver::resolveBonjourRecord(const BonjourRecord &record) {
	if (dnssref) {
		qWarning("resolve in process, aborting");
		return;
	}
	DNSServiceErrorType err = DNSServiceResolve(&dnssref, 0, 0,
			record.serviceName.toUtf8().constData(),
			record.registeredType.toUtf8().constData(),
			record.replyDomain.toUtf8().constData(),
			(DNSServiceResolveReply) bonjourResolveReply, this);
	if (err != kDNSServiceErr_NoError) {
		emit error(err);
	} else {
		int sockfd = DNSServiceRefSockFD(dnssref);
		if (sockfd == -1) {
			emit error(kDNSServiceErr_Invalid);
		} else {
			bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read,
					this);
			connect(bonjourSocket, SIGNAL(activated(int)), this,
					SLOT(bonjourSocketReadyRead()));
		}
	}
}

void BonjourServiceResolver::bonjourSocketReadyRead() {
	DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
	if (err != kDNSServiceErr_NoError) {
		qDebug() << "BonjourServiceResolver::bonjourSocketReadyRead:ERROR:"
				<< err;
		emit error(err);
	}
}

void BonjourServiceResolver::bonjourResolveReply(DNSServiceRef, DNSServiceFlags,
		quint32, DNSServiceErrorType errorCode, const char *,
		const char *hosttarget, quint16 port, quint16, const char *,
		void *context) {
	BonjourServiceResolver *serviceResolver =
			static_cast<BonjourServiceResolver *>(context);
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
			<< QString::fromUtf8(hosttarget) << port;
	serviceResolver->bonjourPort = port;
	serviceResolver->getAddrInfo(hosttarget);
	//QHostInfo::lookupHost(QString::fromUtf8(hosttarget), serviceResolver,
	//		SLOT(finishConnect(const QHostInfo &)));
}

void BonjourServiceResolver::finishConnect(const QHostAddress &hostAddress) {
	if (hostAddress.isNull()) {
		qDebug() << "BonjourServiceResolver::finishConnect:Lookup failed";
		return;
	}
	qDebug() << "BonjourServiceResolver::finishConnect"; // << hostInfo.addresses().first();
	emit bonjourRecordResolved(hostAddress, bonjourPort);
	QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void BonjourServiceResolver::bonjourGetAddrInfoReply(DNSServiceRef sdRef,
		DNSServiceFlags flags, quint32 interfaceIndex,
		DNSServiceErrorType errorCode, const char *hostName,
		const struct sockaddr* address, quint32 ttl, void *context) {
	BonjourServiceResolver *serviceResolver =
			static_cast<BonjourServiceResolver *>(context);
	if (errorCode != kDNSServiceErr_NoError) {
		emit serviceResolver->error(errorCode);
		return;
	}
	if ((flags & kDNSServiceFlagsAdd) != 0) {
		QHostAddress hAddress(address);
		qDebug() << "BonjourServiceResolver::bonjourGetAddrInfoReply:IP for"
				<< hostName << " is:" << qPrintable(hAddress.toString());
		serviceResolver->finishConnect(hAddress);
	}
}

void BonjourServiceResolver::getAddrInfo(const char * hostName) {
	DNSServiceErrorType err = DNSServiceGetAddrInfo(&dnssref, kDNSServiceFlagsForceMulticast, 0,
			kDNSServiceProtocol_IPv4, hostName,
			(DNSServiceGetAddrInfoReply) bonjourGetAddrInfoReply, this);
//	DNSServiceErrorType err = DNSServiceResolve(&dnssref, 0, 0,
//			record.serviceName.toUtf8().constData(),
//			record.registeredType.toUtf8().constData(),
//			record.replyDomain.toUtf8().constData(),
//			(DNSServiceResolveReply) bonjourResolveReply, this);
	if (err != kDNSServiceErr_NoError) {
		emit error(err);
	} else {
		int sockfd = DNSServiceRefSockFD(dnssref);
		if (sockfd == -1) {
			emit error(kDNSServiceErr_Invalid);
		} else {
			bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read,
					this);
			connect(bonjourSocket, SIGNAL(activated(int)), this,
					SLOT(bonjourSocketReadyRead()));
		}
	}
}
