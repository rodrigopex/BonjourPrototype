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

/*
 *This server implements the IEEE 1588 time syncronization protocol
 */

#include <stdlib.h>
#include "WislandService.hpp"
#include "bonjour/Record.hpp"
#include "bonjour/ServiceRegister.hpp"
#include "network/ServiceConnection.hpp"

using namespace network;
using namespace bonjour;

Server::Server(QObject *parent) :
    QObject(parent) {
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        qDebug()
                << tr("Unable to start the server: %1.").arg(
                       tcpServer->errorString());
        return;
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientAdded()));
    m_record =  Record(
                //tr("Soom server -- %1").arg(QHostInfo::localHostName()),
                "Wisland-DESKTOP-server",
                QLatin1String("_wisland_chat._tcp"), QString(""));
    bonjourRegister = new ServiceRegister(this);
    connect(bonjourRegister, SIGNAL(serviceRegistered(const bonjour::Record&)), this, SLOT(onServiceRegistered(const bonjour::Record&)));
    bonjourRegister->registerService(m_record, tcpServer->serverPort());
}

void Server::onServiceManagerZombie() {
    network::ServiceConnection * zombieClient = static_cast<network::ServiceConnection *>(QObject::sender());
    qDebug() << "Server::onServiceManagerZombie:KILL!";
    if (zombieClient) {
        disconnect(zombieClient, SIGNAL(becameZombie()), this,
                   SLOT(onServiceManagerZombie()));
        disconnect(zombieClient, SIGNAL(decodeRequired(network::Action)),
                   this, SLOT(decodeAction(network::Action)));
        qDebug() << "Server::clientRemoved:" << zombieClient->socket()->peerAddress();
        zombieClient->deleteLater();
    }
}
void Server::decodeAction(network::Action action) {
    qDebug() << "Server::decodeAction";
    network::ServiceConnection * client = static_cast<network::ServiceConnection *>(QObject::sender());
    action.printToQDebug();
    switch (action.action()) {
    case Action::COMMAND: {
        //ServiceConnection * client = static_cast<ServiceConnection *>(QObject::sender());
        qDebug() << "Server::decode" << action.toString();
        Action response;
        response.setAction(Action::RESPONSE);
        QVariantMap content;
        content["id"] = action.content()["id"];
        response.setContent(content);
        client->sendAction(response);
        break;
    }
    case Action::INFORMATION: {
        network::ServiceConnection * peer =
                        static_cast<network::ServiceConnection *>(QObject::sender());
        Action test;
        test.setAction(Action::REQUEST);
        QVariantMap content;
        content["op"] = "getUserData";
        test.setContent(content);
        //test.setTimestamp(QDateTime::currentMSecsSinceEpoch());
        peer->sendAction(test);
        break;
    }
    case Action::RESPONSE: {
        if(action.content()["op"] == "getUserData") {
            action.printToQDebug();
        }
        break;
    }

    case Action::REQUEST: {
        this->sendUserDataAction(client);
        /*Action action;
        action.setAction(Action::USER_INFO);
        QVariantMap content;
        content["op"] = "getUserData";
        content["from"] = m_record.serviceName();
        QByteArray ba;              // Construct a QByteArray object
        QBuffer buffer(&ba);  // Construct a QBuffer object using the QbyteArray
        QImageReader("test.png", "PNG").read().save(&buffer,
                "PNG");
        content["name"] = "fulano";
        content["status"] = "Em Maceió! De boa!";
        content["rawData"] = ba;
        action.setContent(content);
        action.printToQDebug();
        client->sendAction(action);*/
        break;
    }
    case Action::USER_INFO: {
        QImage test;
        test.loadFromData(action.content()["rawData"].value<QByteArray>());
        test.save("/tmp/test.png");
    }
    default:
        qDebug() << action.content();
        break;
    }
}

void Server::sendUserDataAction(network::ServiceConnection *connection) {
    qDebug() << "WService::sendUserDataAction";
    if (connection) {
        Action action;
        action.setAction(Action::USER_INFO);
        QVariantMap content;
        content["op"] = "getUserData";
        content["from"] = m_record.serviceName();
        QByteArray ba;              // Construct a QByteArray object
        QBuffer buffer(&ba);  // Construct a QBuffer object using the QbyteArray
        //qDebug() << QDir::currentPath();
        int a = rand();
        a = (a % 8) + 1;
        Q_ASSERT_X(QFile::exists("test.png"), "Server::sendUserDataAction", "Não encontra o arquivo!");
        QImageReader(QString("face%1.png").arg(QString::number(a)), "PNG").read().save(&buffer,
                "PNG");
        content["name"] = "fulano";
        content["status"] = "Em Maceió! De boa!";
        content["rawData"] = ba;
        action.setContent(content);
        action.printToQDebug();
        connection->sendAction(action);
        //this->m_serviceConnection->sendAction(action);
    } else {
        qDebug()
                << "WService::sendAction:ERRO:No current Service Connection available";
    }
}

void Server::onServiceRegistered(const bonjour::Record &record) {
    m_record = record;
}

void Server::clientAdded() {
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    network::ServiceConnection * client = new network::ServiceConnection(m_record, clientConnection, this);
    connect(client, SIGNAL(becameZombie()), this,
            SLOT(onServiceManagerZombie()));
    connect(client, SIGNAL(decodeRequired(network::Action)),
            this, SLOT(decodeAction(network::Action)));
    qDebug() << "Server::clientAdded:" << clientConnection->peerAddress().toString() << clientConnection->peerPort();

//    Action test;
//    test.setAction(Action::REQUEST);
//    QVariantMap content;
//    content["op"] = "getUserData";
//    test.setContent(content);
//    //test.setTimestamp(QDateTime::currentMSecsSinceEpoch());
//    client->sendAction(test);
}
