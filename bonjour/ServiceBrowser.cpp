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

#include "ServiceBrowser.hpp"
#include <QDebug>
#include <QtCore/QSocketNotifier>

namespace bonjour {

ServiceBrowser::ServiceBrowser(QObject *parent) :
		QObject(parent), dnssref(0), bonjourSocket(0) {
}

ServiceBrowser::~ServiceBrowser() {
	if (dnssref) {
		DNSServiceRefDeallocate(dnssref);
		dnssref = 0;
	}
}

void ServiceBrowser::browseForServiceType(const QString &serviceType) {
	DNSServiceErrorType err = DNSServiceBrowse(&dnssref, 0, 0,
			serviceType.toUtf8().constData(), 0, bonjourBrowseReply, this);
	if (err != kDNSServiceErr_NoError) {
		emit error(err);
		qDebug() << "BonjourServiceBrowser::browseForServiceType:ERROR:" << err;
	} else {
		int sockfd = DNSServiceRefSockFD(dnssref);
		if (sockfd == -1) {
			emit error(kDNSServiceErr_Invalid);
		} else {
			bonjourSocket = new QSocketNotifier(sockfd, QSocketNotifier::Read,
					this);
			connect(bonjourSocket, SIGNAL(activated(int)), this,
					SLOT(socketReadyRead()));
		}
	}
}

void ServiceBrowser::socketReadyRead() {
	DNSServiceErrorType err = DNSServiceProcessResult(dnssref);
	if (err != kDNSServiceErr_NoError)
		emit error(err);
}

void ServiceBrowser::bonjourBrowseReply(DNSServiceRef, DNSServiceFlags flags,
		quint32, DNSServiceErrorType errorCode, const char *serviceName,
		const char *regType, const char *replyDomain, void *context) {
	ServiceBrowser *serviceBrowser = static_cast<ServiceBrowser *>(context);
	if (errorCode != kDNSServiceErr_NoError) {
		emit serviceBrowser->error(errorCode);
	} else {
		qDebug() << "BonjourServiceBrowser::bonjourBrowseReply:Reply domain:"
				<< serviceName << regType << replyDomain;
		Record bonjourRecord(serviceName, regType, replyDomain);
		if (flags & kDNSServiceFlagsAdd) {
			if (!serviceBrowser->bonjourRecords.contains(bonjourRecord)) {
				serviceBrowser->bonjourRecords.append(bonjourRecord);
				emit serviceBrowser->recordAdded(bonjourRecord);
			}
		} else {
			serviceBrowser->bonjourRecords.removeAll(bonjourRecord);
			emit serviceBrowser->recordRemoved(bonjourRecord);
		}
		if (!(flags & kDNSServiceFlagsMoreComing)) {
			emit serviceBrowser->currentRecordsChanged(
					serviceBrowser->bonjourRecords);
		}
	}
}

}  // namespace bonjour
