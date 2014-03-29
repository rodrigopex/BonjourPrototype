/*
 * WService.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: rodrigopex
 */

#include <QDateTime>
#include "network/ServiceConnection.hpp"
#include "WService.hpp"

#include <qt4/QtGui/QImageReader>
#include "applicationinfo.hpp"

using namespace bb::cascades;

WService::WService(QLatin1String name, QLatin1String serviceType,
		QObject *parent) :
		network::P2PService(name, serviceType, parent), m_sessionOpened(false), m_serviceModel(
				new bb::cascades::ArrayDataModel(parent)), m_serviceConnection(
				0) {
	qDebug() << "WService::WService:"
			<< qPrintable(m_currentRecord.serviceName());
	qmlRegisterUncreatableType<WService>("wisland", 1, 0, "MessageStatus", "");
}

WService::~WService() {
}

void WService::decodeAction(network::Action action) {
	action.printToQDebug();
	ServiceConnection * peer =
			static_cast<ServiceConnection *>(QObject::sender());
	switch (action.action()) {
	case Action::RESPONSE: {
		QVariant id = action.content()["id"];
		ArrayDataModel * model = m_chats[m_service];
		if (model) {
			for (int i = 0; i < model->size(); ++i) {
				QVariantMap item = model->value(i).value<QVariantMap>();
				if (item["id"] == id) {
					item["status"] = action.content()["status"];
					model->replace(i, item);
				}
			}
		}
		break;
	}
	case Action::REQUEST: {
		this->sendUserDataAction(peer);
		break;
	}
	case Action::USER_INFO: {
		qDebug() << "WService::decodeAction:USER_INFO";
		QString from = action.content()["from"].value<QString>();
		for (int i = 0; i < m_serviceModel->size(); ++i) {
			QVariantMap item = m_serviceModel->value(i).value<QVariantMap>();
			if (item["serviceName"] == from) {
				item["name"] = action.content()["name"];
				item["status"] = action.content()["status"];
				QByteArray ba = action.content()["rawData"].value<QByteArray>(); // Construct a QByteArray object
				QBuffer buffer(&ba); // Construct a QBuffer object using the QbyteArray
				ApplicationInfo appInfo;
				qDebug() << "WService::decodeAction:USER_INFO:temp dir:" << QString("%1/%2.png").arg(QDir::tempPath()).arg(from);
				QImageReader(&buffer, "PNG").read().save(
						QString("%1/%2.png").arg(QDir::tempPath()).arg(from));
				//QImageReader("app/native/assets/me.png", "PNG").read().save(&buffer, "PNG");
				item["imageSource"] = QUrl(QString("%1/%2.png").arg(appInfo.tempPath()).arg(from));
				qDebug() << "WService::decodeAction:USER_INFO:profile image:" << item["imageSource"];
				m_serviceModel->replace(i, item);
			}
		}
		peer->abort();
		peer->deleteLater();
		break;
	}
	case Action::INFORMATION: {
		qDebug() << "WService::decodeAction:INFORMATION";
		QVariant id = action.content()["id"];
		ArrayDataModel * model = m_chats[m_service];
		/* if (model && action.content()["message"] == "read") {*/
		if (model) {
			for (int i = 0; i < model->size(); ++i) {
				QVariantMap item = model->value(i).value<QVariantMap>();
				//qDebug() << item["read"];
				if (item["status"] != READ) {
					item["status"] = READ;
					model->replace(i, item);
				}
			}
		} else {
			qDebug() << "WService::decodeAction:No model for the Record:"
					<< action.content()["from"];
		}
		break;
	}
	case Action::COMMAND: {
		QVariant id = action.content()["id"];
		Record peerRecord(action.content()["from"].value<QString>(), "", "");
		ArrayDataModel * model = m_chats[peerRecord];
		if (model) {

			model->append(action.content());
			Action response;
			response.setAction(Action::RESPONSE);
			QVariantMap content;
			content["id"] = id;
//			qDebug() << m_service.serviceName << "==" << peerRecord.serviceName;
			if (m_service.serviceName() == peerRecord.serviceName()) {
				content["status"] = READ;
			} else {
				content["status"] = DELIVERED;
			}
			response.setContent(content);
			peer->sendAction(response);
		} else {
			qDebug() << "WService::decodeAction:Problem during decoding";
		}
		break;
	}
	default:
		break;
	}
}

void WService::sendAction(network::Action action) {
	if (m_serviceConnection) {
		this->m_serviceConnection->sendAction(action);
		m_chats[m_service]->append(QVariant::fromValue(action.content()));
		action.printToQDebug();
	} else {
		qDebug()
				<< "WService::sendAction:ERRO:No current Service Connection available";
	}
}

void WService::sendAllReadAction() {
	if (m_serviceConnection) {
		Action action;
		action.setAction(Action::INFORMATION);
		QVariantMap content;
		content["from"] = m_currentRecord.serviceName();
		content["message"] = "read";
		action.setContent(content);
		action.printToQDebug();
		//m_chats[m_service]->append(QVariant::fromValue(content));
		this->m_serviceConnection->sendAction(action);
	} else {
		qDebug()
				<< "WService::sendAction:ERRO:No current Service Connection available";
	}
}

void WService::sendUserDataAction(ServiceConnection *connection) {
	qDebug() << "WService::sendUserDataAction";
	if (connection) {
		Action action;
		action.setAction(Action::USER_INFO);
		QVariantMap content;
		content["op"] = "getUserData";
		content["from"] = m_currentRecord.serviceName();
		QByteArray ba;              // Construct a QByteArray object
		QBuffer buffer(&ba);  // Construct a QBuffer object using the QbyteArray
		QImageReader("app/native/assets/me.png", "PNG").read().save(&buffer,
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

void WService::sendText(QString text) {
	if (m_serviceConnection) {
		Action action;
		action.setAction(Action::COMMAND);
		QVariantMap content;
		content["message"] = text;
		content["from"] = m_currentRecord.serviceName();
		content["status"] = SENT;
		//content["read"] = false;
		content["id"] = QDateTime::currentMSecsSinceEpoch();
		action.setContent(content);
		//action.setTimestamp(QDateTime::currentMSecsSinceEpoch());
		action.printToQDebug();
		m_chats[m_service]->append(QVariant::fromValue(content));
		this->m_serviceConnection->sendAction(action);
	} else {
		qDebug()
				<< "WService::sendAction:ERRO:No current Service Connection available";
	}
}

void WService::sendRequestUserInfo(ServiceConnection *connection) {
	qDebug() << "WService::sendRequestUserInfo:sent.";
	Action test;
	test.setAction(Action::REQUEST);
	QVariantMap content;
	content["op"] = "getUserData";
	test.setContent(content);
	//test.setTimestamp(QDateTime::currentMSecsSinceEpoch());
	connection->sendAction(test);
}

QString WService::currentServiceName() {
	return m_service.serviceName();
}

bool WService::isSessionOpened() {
	return m_sessionOpened;
}

void WService::recordAdded(const Record &record) {
	/*
	 * Model service
	 *   {
	 * 		serviceName -> string
	 * 		registeredType -> string
	 * 		replyDomain -> string
	 * 		name -> string
	 * 		status -> string
	 * 		imageSource -> string
	 *   }
	 */

	QVariantMap recordMap = record.toMap();
	qDebug() << "WService::recordAdded:Service Name:" << record.serviceName();
	if (m_currentRecord.serviceName() != record.serviceName() && m_serviceModel->indexOf(recordMap) == -1) {
	//if (m_serviceModel->indexOf(recordMap) == -1) {
		QVariantMap content;
		content["message"] = "This is a test for sending the read flag";
		content["from"] = "server";
		content["status"] = SENT;
		//content["read"] = false;
		content["id"] = QDateTime::currentMSecsSinceEpoch();
		ArrayDataModel *newModel = new bb::cascades::ArrayDataModel(this);
		newModel->append(QVariant::fromValue(content));
		m_chats.insert(record, newModel);
		recordMap["name"] = "";
		recordMap["status"] = "";
		recordMap["imageSource"] = "asset:///models/face8.png";
		m_serviceModel->append(recordMap);
		Consumer::connectToService(record);
		//this->openServiceSession(record);
	} else {
		qDebug() << "WService::recordAdded:WARNING:Record already exists";
	}
}

bb::cascades::ArrayDataModel * WService::serviceModel() {
	//m_serviceModel->clear();
//	m_serviceModel->append(m_chats.keys());
	return m_serviceModel;
}

void WService::openServiceSession(bonjour::Record record) {
	m_service = record;
	Consumer::connectToService(m_service);
}

void WService::closeServiceSession() {
	qDebug() << "WService::closeServiceSession";
	m_sessionOpened = false;
	emit this->sessionClosed();
	m_service = Record();
	m_serviceConnection = 0;
}

void WService::closeServiceSessionByUser() {
	qDebug() << "WService::closeServiceSession";
	m_sessionOpened = false;
	m_service = Record();
	if (m_serviceConnection) {
		m_serviceConnection->abort();
		m_serviceConnection->deleteLater();
		m_serviceConnection = 0;
	}
}

void WService::recordRemoved(const Record &record) {
	QVariantMap recordMap = record.toMap();
	qDebug() << "WService::recordRemoved" << record.serviceName()
			<< record.replyDomain() << record.registeredType();
	if (record == m_service) {
		this->closeServiceSession();
	}
	m_chats.remove(record);
	for (int i = 0; i < m_serviceModel->size(); ++i) {
		QVariantMap item = m_serviceModel->value(i).value<QVariantMap>();
		if (item["serviceName"] == record.serviceName()) {
			m_serviceModel->removeAt(i);
			break;
		}
	}
	//m_serviceModel->removeAt(m_serviceModel->indexOf(recordMap));
}

void WService::serviceSessionOpened(network::ServiceConnection *connection) {
	if (connection->serviceRecord() == m_service) {
		m_serviceConnection = connection;
		qDebug() << "WService::serviceConnectionCreated";
		m_sessionOpened = true;
		emit this->currentChatListChanged(m_chats[m_service]);
		emit this->sessionStarted();
	} else {
		this->sendRequestUserInfo(connection);
//		qDebug() << "WService::serviceConnectionCreated:ERROR:INCOMPATIBLE";
//		connection->abort();
//		connection->deleteLater();
//		m_service = Record();
	}
}

bb::cascades::ArrayDataModel * WService::currentChatList() {
	qDebug() << "WService::currentChatList" << m_service.toMap()
			<< m_chats[m_service];
	return m_chats[m_service];
}

void WService::selectService(int index) {
	this->openServiceSession(
			Record(m_serviceModel->data(QVariantList() << index).value<QVariantMap>()));
}
