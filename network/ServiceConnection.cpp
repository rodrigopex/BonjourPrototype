/*
 * ServiceConnection.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: rodrigopex
 */

#include "ServiceConnection.hpp"
#include <unistd.h>

using namespace bonjour;

namespace network {

ServiceConnection::ServiceConnection(const Record &serviceRecord,
		const QHostAddress &hostAddress, int port, QObject *parent) :
		QObject(parent), m_peerHostAddress(hostAddress), m_peerPort(port), m_tcpSocket(
				0), m_bytesToDownload(0) {
	m_serviceRecord = serviceRecord;
	m_tcpSocket = new QTcpSocket(this);
	this->connectSocketSignals();
}

ServiceConnection::ServiceConnection(const Record &serviceRecord,
		QTcpSocket * peerSocket, QObject *parent) :
		QObject(parent), m_peerHostAddress(QHostAddress()), m_peerPort(-1), m_tcpSocket(
				peerSocket), m_bytesToDownload(0) {
	m_tcpSocket->setParent(this);
	m_peerHostAddress = m_tcpSocket->peerAddress();
	m_peerPort = m_tcpSocket->peerPort();
	m_serviceRecord = serviceRecord;
	this->connectSocketSignals();
}

void ServiceConnection::connectSocketSignals() {
	connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(actionReceived()));
	connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this,
			SLOT(onConnectionError(QAbstractSocket::SocketError)));
}

ServiceConnection::~ServiceConnection() {
	m_tcpSocket->abort();
}

QTcpSocket * ServiceConnection::socket() {
	return m_tcpSocket;
}

bonjour::Record ServiceConnection::serviceRecord() {
	return m_serviceRecord;
}

void ServiceConnection::actionReceived() {
	qint64 bytesAvailable = m_tcpSocket->bytesAvailable();
	if(m_bytesToDownload > 0) {
		m_bytesToDownload -= bytesAvailable;
		qDebug() << "remains:" << m_bytesToDownload;
		m_buffer.append(m_tcpSocket->readAll());
		if(m_bytesToDownload == 0) {
			QDataStream in(m_buffer);
			in.setVersion(QDataStream::Qt_4_0);
			Action action;
			in >> action;
			emit decodeRequired(action);
		}
	} else {
		qDebug() << "ServiceConnection::actionReceived: Bytes available for reading:"
					<< bytesAvailable << ", minimal size:" << qint64(sizeof(Action));
		QDataStream in(m_tcpSocket);
		in.setVersion(QDataStream::Qt_4_0);
		Action action;
		in >> action;
		if(action.action() == Action::MULTI_PACKAGE) {
			m_bytesToDownload = action.content()["size"].value<qint64>();
			m_buffer.clear();
			qDebug() << "ServiceConnection::actionReceived: Receiving multi-part package";
		} else {
			emit decodeRequired(action);
		}

	}


//	qDebug() << "ServiceConnection::actionReceived:"
//			<< m_tcpSocket->bytesAvailable() << qint64(sizeof(Action));
//
//	QDataStream in(m_tcpSocket);
//	in.setVersion(QDataStream::Qt_4_0);
//	if (m_tcpSocket->bytesAvailable() < qint64(sizeof(Action))) {
//		qDebug()
//				<< "ServiceConnection::actionReceived:WARNING:Data is too small:"
//				<< m_tcpSocket->bytesAvailable() << qint64(sizeof(Action));
//	}
//	Action action;
//	in >> action;
//	emit decodeRequired(action);
}

void ServiceConnection::sendAction(const Action &action) {
	qDebug() << "ServiceConnection::sendAction:To" << m_tcpSocket->peerAddress()
			<< m_tcpSocket->peerPort();
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << action;

	int blockSize = block.size();

	if(blockSize > 1448) {
		qDebug() << "ServiceConnection::sendAction:SIZE:" << blockSize;
		Action prior;
		QVariantMap content;
		content["size"] = blockSize;
		prior.setContent(content);
		prior.setAction(Action::MULTI_PACKAGE);

		QByteArray priorBlock;
		QDataStream outPrior(&priorBlock, QIODevice::WriteOnly);
		outPrior.setVersion(QDataStream::Qt_4_0);
		outPrior << prior;

		m_tcpSocket->write(priorBlock);
		usleep(10000);
	}
	m_tcpSocket->write(block);
}

void ServiceConnection::abort() {
	m_tcpSocket->abort();
}

void ServiceConnection::connectToPeer() {
	m_tcpSocket->abort();
	if (m_peerHostAddress.protocol() == QAbstractSocket::IPv4Protocol
			|| m_peerHostAddress.protocol() == QAbstractSocket::IPv6Protocol) {
		qDebug()
				<<qPrintable(QString("ServiceConnection::connectToPeer:IP %1:%2")
						.arg(m_peerHostAddress.toString())
						.arg(m_peerPort));
		m_tcpSocket->connectToHost(m_peerHostAddress, m_peerPort);
	}
}

void ServiceConnection::onConnectionError(
		QAbstractSocket::SocketError socketError) {
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		qDebug() << "ServiceConnection::onConnectionError"
				<< tr("The remote host closed the connection.");
		m_tcpSocket->abort();
		emit this->becameZombie();
		break;
	case QAbstractSocket::HostNotFoundError:
		qDebug() << "ServiceConnection::onConnectionError"
				<< tr("The host was not found. Please check the "
						"host name and port settings.");
		break;
	case QAbstractSocket::ConnectionRefusedError:
		qDebug() << "ServiceConnection::onConnectionError"
				<< tr("The connection was refused by the peer. "
						"Make sure the fortune server is running, "
						"and check that the host name and port "
						"settings are correct.");
		break;
	default:
		qDebug() << "ServiceConnection::onConnectionError"
				<< tr("The following error occurred: %1.").arg(
						m_tcpSocket->errorString());
	}
}

}  // namespace bonjour

//bool bonjour::ServiceConnection::isMultiple() {
//	return m_multiple;
//}
