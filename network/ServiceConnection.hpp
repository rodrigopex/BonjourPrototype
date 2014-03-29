/*
 * ActionHandler.h
 *
 *  Created on: Feb 9, 2014
 *      Author: rodrigopex
 */

#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>
#include <QMetaObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QDataStream>
#include <QBuffer>

#include "Action.hpp"
#include "bonjour/Record.hpp"

namespace network {

class ServiceConnection: public QObject {
Q_OBJECT
public:
	ServiceConnection(const bonjour::Record &serviceRecord,
			const QHostAddress &address, int port, QObject *parent = 0);
	ServiceConnection(const bonjour::Record &serviceRecord, QTcpSocket * peerSocket,
			QObject *parent = 0);
	virtual ~ServiceConnection();

	QTcpSocket * socket();
	bonjour::Record serviceRecord();
	/* SLOT
	 * actionReceived()
	 *        |
	 *        +--> decodeDelegate.decode(action);
	 * Responsible for reading the data sent from the peer and assembling the
	 * action for decoding. After the assembling, the decodeAction method of the
	 * decodeDelegate is invoked automatically.
	 */
//	bool isMultiple(); // Multiple x Persistent connection
	Q_INVOKABLE void sendAction(const Action &action);
	Q_INVOKABLE void abort();
	Q_SLOT void actionReceived();
	Q_SLOT void connectToPeer();
	Q_SIGNAL void becameZombie(); //delete the BonjourServiceResolver
	Q_SIGNAL void decodeRequired(network::Action action);
private:
	Q_SLOT void onConnectionError(QAbstractSocket::SocketError socketError);
	void connectSocketSignals();
	//bool m_multiple;
	QHostAddress m_peerHostAddress;
	int m_peerPort;
	QTcpSocket *m_tcpSocket;
	bonjour::Record m_serviceRecord;
	qint64 m_bytesToDownload;
	QByteArray m_buffer;
};

}  // namespace bonjour

#endif /* SERVICEMANAGER_H */
