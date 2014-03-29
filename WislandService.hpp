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

#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QImage>
#include <QImageReader>

#ifdef __QNXNTO__
#include <qt4/QtNetwork/QtNetwork>
#else
#include <QtNetwork>
#endif

#include "network/Action.hpp"
#include "network/ServiceConnection.hpp"
#include  "bonjour/Record.hpp"

class QTcpServer;
class QTcpSocket;

namespace bonjour {
    class ServiceRegister;
    class ServiceConnection;
    class Record;
}

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
private slots:
    void clientAdded();
    void decodeAction(network::Action action);
    void onServiceRegistered(const bonjour::Record &record);
    void onServiceManagerZombie();
private:
    void sendUserDataAction(network::ServiceConnection *connection);
    void sendAction(network::Action::ActionType actionType);
    QTcpServer *tcpServer;
    QStringList fortunes;
    qint64 timestamp;
    bonjour::ServiceRegister *bonjourRegister;
    bonjour::Record m_record;
};

#endif
