/*
 * P2PService.h
 *
 *  Created on: Mar 13, 2014
 *      Author: rodrigopex
 */

#ifndef P2PSERVICE_H_
#define P2PSERVICE_H_

#include <QObject>
#ifdef __QNXNTO__
#include <qt4/QtNetwork/QtNetwork>
#else
#include <QtNetwork>
#endif

#include "../bonjour/Record.hpp"
#include "Action.hpp"
#include "ServiceConnection.hpp"

#include "Consumer.hpp"

namespace network {

class P2PService: public Consumer {
	Q_OBJECT
public:
	P2PService(QLatin1String name, QLatin1String serviceType, QObject *parent = 0);
	virtual ~P2PService();
private slots:
	void incomeConnection();
	void serviceRegistered(const bonjour::Record& record);
protected:
	QTcpServer *m_tcpServer;
	bonjour::Record m_currentRecord;
};

} /* namespace bonjour */
#endif /* P2PSERVICE_H_ */
