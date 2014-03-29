/*
 * P2PService.cpp
 *
 *  Created on: Mar 13, 2014
 *      Author: rodrigopex
 */

#include "P2PService.h"

using namespace bonjour;

namespace network {

P2PService::P2PService(QLatin1String name, QLatin1String serviceType,
		QObject *parent) :
		Consumer(serviceType, parent) {
	m_currentRecord = Record(name, serviceType, QString(""));
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	m_tcpServer = new QTcpServer(this);
	if (!m_tcpServer->listen()) {
		qDebug()
				<< tr("Unable to start the server: %1.").arg(
						m_tcpServer->errorString());
		return;
	}
	connect(m_tcpServer, SIGNAL(newConnection()), this,
			SLOT(incomeConnection()));
	bonjour::ServiceRegister * bonjourRegister = new ServiceRegister(this);
	bonjourRegister->registerService(m_currentRecord,
			m_tcpServer->serverPort());
}

P2PService::~P2PService() {
	//m_currentRecord.clear(); // deleting m_currentRecord
}

void P2PService::incomeConnection() {
	QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();
	ServiceConnection * client = new ServiceConnection(m_currentRecord,
			clientConnection, this);
	connect(client, SIGNAL(becameZombie()), this,
			SLOT(onServiceManagerZombie()));
	connect(client, SIGNAL(decodeRequired(network::Action)), this,
			SLOT(decodeAction(network::Action)));
	qDebug() << "Server::clientAdded:" << clientConnection->peerAddress()
			<< clientConnection->peerPort();
}

void P2PService::serviceRegistered(const Record& record) {
	m_currentRecord = record;
}


} /* namespace bonjour */
