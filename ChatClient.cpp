/*
 * ChatClient.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: rodrigopex
 */

#include <QDateTime>
#include "network/ServiceConnection.hpp"
#include "ChatClient.h"

using namespace network;

ChatClient::ChatClient(QLatin1String serviceType, QObject *parent) :
        network::Consumer(serviceType, parent), m_sessionOpened(false), m_serviceConnection(
				0) {
    qDebug() << "ChatClient::ChatClient: Builded successfully!";
}

ChatClient::~ChatClient() {
}

void ChatClient::decodeAction(network::Action action) {
	action.printToQDebug();
	switch (action.action()) {
	case Action::RESPONSE: {
		QVariant id = action.content()["id"];
        QVariantList model = m_chats[m_service];
        if (model.size()) {
            for (int i = 0; i < model.size(); ++i) {
                QVariantMap item = model.value(i).value<QVariantMap>();
				if (item["id"] == id) {
					item["delivered"] = true;
                    model.replace(i, item);
				}
			}
		}
		break;
	}
	case Action::COMMAND: {
		ServiceConnection * peer = static_cast<ServiceConnection *>(QObject::sender());
		QVariant id = action.content()["id"];
		Record peerRecord(action.content()["from"].value<QString>(), "", "");
        QVariantList model = m_chats[peerRecord];
        if (model.size()) {
			action.content()["delivered"] = true;
            model.append(action.content());
			Action response;
			response.setAction(Action::RESPONSE);
			QVariantMap content;
			content["id"] = id;
			response.setContent(content);
			peer->sendAction(response);
		}
		break;
	}
	default:
		break;
	}
}

void ChatClient::sendAction(network::Action action) {
	if (m_serviceConnection) {
		this->m_serviceConnection->sendAction(action);
        m_chats[m_service].append(QVariant::fromValue(action.content()));
		action.printToQDebug();
	} else {
		qDebug()
				<< "ChatClient::sendAction:ERRO:No current Service Connection available";
	}
}

void ChatClient::sendText(QString text) {
	if (m_serviceConnection) {
		Action action;
		action.setAction(Action::COMMAND);
		QVariantMap content;
		content["message"] = text;
		content["from"] = "test";
		content["delivered"] = false;
		content["read"] = false;
		content["id"] = QDateTime::currentMSecsSinceEpoch();
		action.setContent(content);
		//action.setTimestamp(QDateTime::currentMSecsSinceEpoch());
		action.printToQDebug();
        m_chats[m_service].append(QVariant::fromValue(content));
		this->m_serviceConnection->sendAction(action);
	} else {
		qDebug()
				<< "ChatClient::sendAction:ERRO:No current Service Connection available";
	}
}

bool ChatClient::isSessionOpened() {
	return m_sessionOpened;
}

void ChatClient::recordAdded(const Record &record) {
	QVariantMap recordMap = record.toMap();
    qDebug() << "ChatClient::recordAdded:Service Name:" << record.serviceName();
    if (m_serviceModel.indexOf(recordMap) == -1) {
		QVariantMap content;
		content["message"] = "This is a test for sending the read flag";
		content["from"] = "server";
		content["delivered"] = false;
		content["read"] = false;
		content["id"] = QDateTime::currentMSecsSinceEpoch();
        QVariantList newModel;// = new bb::cascades::ArrayDataModel(this);
        newModel.append(QVariant::fromValue(content));
		m_chats.insert(record, newModel);
        m_serviceModel.append(recordMap);
		//this->openServiceSession(recordMap);
	} else {
		qDebug() << "ChatClient::recordAdded:WARNING:Record already exists";
	}
}

QVariantList  ChatClient::serviceModel() {
	//m_serviceModel->clear();
//	m_serviceModel->append(m_chats.keys());
	return m_serviceModel;
}

void ChatClient::openServiceSession(bonjour::Record record) {
	m_service = record;
	Consumer::connectToService(m_service);
}

void ChatClient::closeServiceSession() {
	qDebug() << "ChatClient::closeServiceSession";
	m_sessionOpened = false;
	emit this->sessionClosed();
	m_service = Record();
	m_serviceConnection = 0;
}

void ChatClient::closeServiceSessionByUser() {
	qDebug() << "ChatClient::closeServiceSession";
	m_sessionOpened = false;
	m_service = Record();
	if (m_serviceConnection) {
		m_serviceConnection->abort();
		m_serviceConnection->deleteLater();
		m_serviceConnection = 0;
	}
}

void ChatClient::recordRemoved(const Record &record) {
	QVariantMap recordMap = record.toMap();
    qDebug() << "ChatClient::recordRemoved" << record.serviceName()
            << record.replyDomain() << record.registeredType();
	if (record == m_service) {
		this->closeServiceSession();
	}
	m_chats.remove(record);
    m_serviceModel.removeAt(m_serviceModel.indexOf(recordMap));
}

void ChatClient::serviceSessionOpened(network::ServiceConnection *connection) {
	if (connection->serviceRecord() == m_service) {
		m_serviceConnection = connection;
		qDebug() << "ChatClient::serviceConnectionCreated";
		m_sessionOpened = true;
		emit this->currentChatListChanged(m_chats[m_service]);
		emit this->sessionStarted();
	} else {
		qDebug() << "ChatClient::serviceConnectionCreated:ERROR:INCOMPATIBLE";
		connection->abort();
		connection->deleteLater();
		m_service = Record();
	}
}

QVariantList ChatClient::currentChatList() {
	qDebug() << "ChatClient::currentChatList" << m_service.toMap()
			<< m_chats[m_service];
	return m_chats[m_service];
}

void ChatClient::selectService(int index) {
	this->openServiceSession(
            Record(
                    m_serviceModel.value(index).value<
							QVariantMap>()));
}
