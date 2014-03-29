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

#ifndef BONJOURSERVICEBROWSER_H
#define BONJOURSERVICEBROWSER_H

#include <QtCore/QObject>
#include <dns_sd.h>
#include "Record.hpp"

class QSocketNotifier;

namespace bonjour {

class ServiceBrowser : public QObject
{
    Q_OBJECT
public:
    ServiceBrowser(QObject *parent = 0);
    ~ServiceBrowser();
    void browseForServiceType(const QString &serviceType);
    inline QList<Record> currentRecords() const { return bonjourRecords; }
    inline QString serviceType() const { return browsingType; }

signals:
    void currentRecordsChanged(const QList<bonjour::Record> &list);
    void recordAdded(bonjour::Record record);
    void recordRemoved(bonjour::Record record);
    void error(DNSServiceErrorType err);

private slots:
    void socketReadyRead();

private:
    static void DNSSD_API bonjourBrowseReply(DNSServiceRef , DNSServiceFlags flags, quint32,
                                   DNSServiceErrorType errorCode, const char *serviceName,
                                   const char *regType, const char *replyDomain, void *context);
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    QList<Record> bonjourRecords;
    QString browsingType;
};

}  // namespace bonjour

#endif // BONJOURSERVICEBROWSER_H
