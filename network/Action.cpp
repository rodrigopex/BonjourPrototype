/*
 * Action.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: rodrigopex
 */

#include "Action.hpp"

#include <QDebug>

namespace network {

Action::Action(QObject *parent) :
		Serializable(parent), m_action(UNDEFINED) {
	//qRegisterMetaType<network::Action>();
}

Action::Action(const Action &action) :
		Serializable(0), m_action(
				action.m_action), m_content(action.m_content) {
	//qRegisterMetaType<network::Action>();
}

Action::~Action() {
}

QString Action::toString() {
	switch (m_action) {
	case SYNC:
		return "Action-SYNC";
		break;
	case DELAY_REQUEST:
		return "Action-DELAY_REQUEST";
		break;
	case DELAY_REPONSE:
		return "Action-DELAY_REPONSE";
		break;
	case COMMAND:
		return "Action-COMMAND";
		break;
	case INFORMATION:
		return "Action-INFORMATION";
		break;
	case ERROR:
		return "Action-ERROR";
		break;
	case RESPONSE:
		return "Action-RESPONSE";
		break;
	case REQUEST:
		return "Action-REQUEST";
		break;
	case MULTI_PACKAGE:
		return "Action-MULTI_PACKAGE";
		break;
	case USER_INFO:
		return "Action-USER_INFO";
		break;
	case UNDEFINED:
		break;
	}
	return "Action-UNDEFINED";
}
/*
qint64 Action::timestamp() {
	return m_timestamp;
}
void Action::setTimestamp(qint64 timestamp) {
	if (timestamp != m_timestamp) {
		m_timestamp = timestamp;
		emit timestampChanged(m_timestamp);
	}
}
*/

Action::ActionType Action::action() {
	return m_action;
}
void Action::setAction(ActionType action) {
	if (action != m_action) {
		m_action = action;
		emit actionChanged(m_action);
	}
}

QVariantMap Action::content() {
	return m_content;
}

void Action::setContent(QVariantMap content) {
	if (m_content != content) {
		m_content = content;
		emit this->contentChanged(m_content);
	}
}

/*QByteArray Action::rawData() const {
	return m_rawData;
}

void Action::setRawData(QByteArray rawData) {
	if (m_rawData != rawData) {
		m_rawData = rawData;
		emit this->rawDataChanged(m_rawData);
	}
}

quint64 Action::rawDataSize() const {
	return m_rawData.size();
}*/

void Action::printToQDebug() {
	qDebug() << "= Action ===================";
	qDebug() << "@actionType:" << this->toString();
	//qDebug() << "@timestamp:" << m_timestamp;
	qDebug() << "@content:" << m_content;
	//qDebug() << "@rawData size:" << m_rawData.size();
	qDebug() << "============================";
}

}  // namespace bonjour
