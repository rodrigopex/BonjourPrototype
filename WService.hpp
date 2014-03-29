/*
 * ChatClient.h
 *
 *  Created on: Feb 14, 2014
 *      Author: rodrigopex
 */

#ifndef CHATCLIENT_H_
#define CHATCLIENT_H_

#include <QObject>
//#include <QHash>

#ifdef __QNXNTO__
    #include <bb/cascades/ArrayDataModel>
    typedef bb::cascades::ArrayDataModel WListModel;
#else
    typedef QVariantList WListModel;
#include <QVariantList>
#endif
#include "network/P2PService.h"
#include "network/Action.hpp"
#include "bonjour/Record.hpp"

using namespace bonjour;
using namespace network;

class WService: public network::P2PService {
	Q_OBJECT
	Q_ENUMS(MessageStatus)
	Q_PROPERTY(bb::cascades::ArrayDataModel *serviceModel READ serviceModel NOTIFY serviceModelChanged)
	Q_PROPERTY(bb::cascades::ArrayDataModel *currentChatList READ currentChatList NOTIFY currentChatListChanged)
public:
	WService(QLatin1String name, QLatin1String serviceType, QObject *parent = 0);
	virtual ~WService();
	enum MessageStatus {
			SENT, DELIVERED, READ
		};
	Q_INVOKABLE bb::cascades::ArrayDataModel *serviceModel();
	Q_INVOKABLE bb::cascades::ArrayDataModel *currentChatList();
	Q_INVOKABLE void selectService(int index);
	Q_INVOKABLE void openServiceSession(bonjour::Record record);
	Q_INVOKABLE void sendAction(network::Action action);
	Q_INVOKABLE void sendAllReadAction();
	Q_INVOKABLE void sendUserDataAction(ServiceConnection *connection);
	Q_INVOKABLE void sendText(QString text);
	Q_INVOKABLE void sendRequestUserInfo(ServiceConnection *connection);

	Q_INVOKABLE QString currentServiceName();

	Q_INVOKABLE bool isSessionOpened();

	Q_SLOT void closeServiceSession();
	Q_SLOT void closeServiceSessionByUser();

	Q_SIGNAL void sessionClosed();
	Q_SIGNAL void currentChatListChanged(bb::cascades::ArrayDataModel *currentChatList);
	Q_SIGNAL void serviceModelChanged(bb::cascades::ArrayDataModel *serviceModel);
	Q_SIGNAL void sessionStarted();
private slots:
	void decodeAction(network::Action action);
	void recordAdded(const bonjour::Record &record);
	void recordRemoved(const bonjour::Record &record);
	void serviceSessionOpened(network::ServiceConnection *connection);
private:
	bool m_sessionOpened;
    bonjour::Record m_service;
	bb::cascades::ArrayDataModel *m_serviceModel;
	network::ServiceConnection *m_serviceConnection;
	QMap<bonjour::Record, bb::cascades::ArrayDataModel * > m_chats;

};

#endif /* CHATCLIENT_H_ */
