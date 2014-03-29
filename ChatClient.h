/*
 * ChatClient.h
 *
 *  Created on: Feb 14, 2014
 *      Author: rodrigopex
 */

#ifndef CHATCLIENT_H_
#define CHATCLIENT_H_

#include <QObject>
#include <QHash>
//#include <bb/cascades/ArrayDataModel>

#include "network/Consumer.hpp"
#include "network/Action.hpp"
#include "bonjour/Record.hpp"

using namespace bonjour;

class ChatClient: public network::Consumer {
	Q_OBJECT
    Q_PROPERTY(QVariantList serviceModel READ serviceModel NOTIFY serviceModelChanged)
    Q_PROPERTY(QVariantList currentChatList READ currentChatList NOTIFY currentChatListChanged)
public:
	ChatClient(QLatin1String serviceType, QObject *parent = 0);
	virtual ~ChatClient();
    Q_INVOKABLE QVariantList serviceModel();
    Q_INVOKABLE QVariantList currentChatList();
	Q_INVOKABLE void selectService(int index);
	Q_INVOKABLE void openServiceSession(bonjour::Record record);
    Q_INVOKABLE void sendAction(network::Action action);
	Q_INVOKABLE void sendText(QString text);

	Q_INVOKABLE bool isSessionOpened();

	Q_SLOT void closeServiceSession();
	Q_SLOT void closeServiceSessionByUser();

	Q_SIGNAL void sessionClosed();
    Q_SIGNAL void currentChatListChanged(QVariantList currentChatList);
    Q_SIGNAL void serviceModelChanged(QVariantList serviceModel);
	Q_SIGNAL void sessionStarted();
private slots:
    void decodeAction(network::Action action);
    void recordAdded(const bonjour::Record& record);
    void recordRemoved(const bonjour::Record& record);
    void serviceSessionOpened(network::ServiceConnection *connection);
private:
	bool m_sessionOpened;
    bonjour::Record m_service;
    QVariantList m_serviceModel;
    network::ServiceConnection *m_serviceConnection;
    QMap<bonjour::Record, QVariantList  > m_chats;
};

#endif /* CHATCLIENT_H_ */
