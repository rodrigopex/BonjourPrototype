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

//#include <QDialog>

//class QLabel;
//class QPushButton;
#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QtNetwork>

class QTcpServer;
class QTcpSocket;
class BonjourServiceRegister;

#include "Serializable.h"

class Action: public Serializable {
Q_OBJECT
Q_ENUMS(ActionType)
Q_PROPERTY( qint64 timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
Q_PROPERTY( ActionType action READ action WRITE setAction NOTIFY actionChanged)
public:
    Action(QObject *parent = 0) :
            Serializable(parent), m_timestamp(0), m_action(NONE) {
    }
    ~Action() {
    }
    enum ActionType {
        NONE, SYNC, DELAY_REQUEST, DELAY_REPONSE
    };
    qint64 timestamp() {
        return m_timestamp;
    }
    void setTimestamp(qint64 timestamp) {
        if (timestamp != m_timestamp) {
            m_timestamp = timestamp;
            emit timestampChanged(m_timestamp);
        }
    }
    Q_SIGNAL void timestampChanged(qint64 timestamp);

    ActionType action() {
        return m_action;
    }
    void setAction(ActionType action) {
        if (action != m_action) {
            m_action = action;
            emit actionChanged(m_action);
        }
    }
    Q_SIGNAL void actionChanged(qint64 timestamp);
private:
    qint64 m_timestamp;
    ActionType m_action;
};

class Server : public QObject
{
    Q_OBJECT

public:
    Server(QObject *parent = 0);
    Q_SIGNAL void clientsChanged(QTcpSocket * client);
private slots:
    void clientAdded();
    void clientRemoved();
    void readyClientData();
    void displayClientConnectionError(QAbstractSocket::SocketError socketError);

private:
    //QLabel *statusLabel;
    //QPushButton *quitButton;
    void sendSyncPacket(QTcpSocket * client, Action::ActionType actionType);
    QTcpServer *tcpServer;
    QStringList fortunes;
    qint64 timestamp;
    BonjourServiceRegister *bonjourRegister;
    QList<QTcpSocket *> m_clients;
};

#endif
