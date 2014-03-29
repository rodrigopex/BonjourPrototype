/****************************************************************************
 **
 ** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://www.trolltech.com/products/qt/opensource.html
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://www.trolltech.com/products/qt/licensing.html or contact the
 ** sales department at sales@trolltech.com.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include <QtNetwork/QtNetwork>
#include "bonjour/ServiceBrowser.hpp"
#include "bonjour/ServiceResolver.hpp"

#include "Consumer.hpp"

using namespace bonjour;

namespace network {

Consumer::Consumer(QLatin1String serviceType, QObject *parent) :
		QObject(parent) {
	ServiceBrowser *bonjourBrowser = new ServiceBrowser(this);
	connect(bonjourBrowser,
			SIGNAL(currentRecordsChanged(const QList<bonjour::Record> &)),
			this, SLOT(updateRecords(const QList<bonjour::Record> &)));
	connect(bonjourBrowser, SIGNAL(recordAdded(bonjour::Record)), this,
			SLOT(recordAdded(bonjour::Record)));
	connect(bonjourBrowser, SIGNAL(recordRemoved(bonjour::Record)), this,
			SLOT(recordRemoved(bonjour::Record)));
	bonjourBrowser->browseForServiceType(
			QLatin1String(serviceType));
}

Consumer::~Consumer() {
//	if (m_currentRecord) {
//		delete m_currentRecord;
//	}
}

void Consumer::startServiceConnection(const QHostAddress &hostAddress, int port) {
	ServiceResolver * resolver =
			static_cast<ServiceResolver *>(QObject::sender());
	if (resolver) {
		disconnect(resolver, SIGNAL(recordResolved(QHostAddress,int)), this,
				SLOT(startServiceConnection(QHostAddress,int)));
		resolver->deleteLater();
	}
//	Q_ASSERT_X(m_currentRecord, "Consumer::startServiceManager",
//			"Problem with m_currentRecord,  this is NULL.");
	ServiceConnection * serviceConnection = new ServiceConnection(m_currentRecord,
			hostAddress, port);
	connect(serviceConnection, SIGNAL(becameZombie()), this,
			SLOT(onServiceManagerZombie()));
	connect(serviceConnection, SIGNAL(decodeRequired(network::Action)),
			this, SLOT(decodeAction(network::Action)));
	serviceConnection->connectToPeer();
	this->serviceSessionOpened(serviceConnection);
	//m_currentServiceConnection = serviceManager;
	//m_services.append(serviceManager);
}

void Consumer::onServiceManagerZombie() {
	ServiceConnection * zombie = static_cast<ServiceConnection *>(QObject::sender());
	if (zombie) {
		qDebug() << "Consumer::onServiceManagerZombie:KILL! Only";// << m_services.size() << "remain";
//		if(m_currentServiceConnection == zombie) {
//			m_currentServiceConnection = 0;
//		}
		zombie->deleteLater();
	}
}

void Consumer::connectToService(const Record &record) {
	ServiceResolver * resolver = new ServiceResolver(this);
	connect(resolver, SIGNAL(recordResolved(QHostAddress,int)), this,
			SLOT(startServiceConnection(QHostAddress,int)));
//	if (m_currentRecord) {
//		delete m_currentRecord;
//	}
	m_currentRecord = record;
	resolver->resolveRecord(m_currentRecord);
}


void Consumer::updateRecords(const QList<bonjour::Record> &list) {
	foreach (Record record, list)
	{
		qDebug() << "Consumer::updateRecords:Service Name:" << record.serviceName();
	}
}

}  // namespace bonjour
