/*
 * Action.h
 *
 *  Created on: Feb 8, 2014
 *      Author: rodrigopex
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <QObject>
#include <QVariantMap>
#ifdef __QNXNTO__
#include <bb/cascades/QmlDocument>
#endif
#include "Serializable.hpp"

namespace network {

class Action: public Serializable {
	Q_OBJECT
	Q_ENUMS(ActionType)
	//Q_PROPERTY( qint64 timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)
	Q_PROPERTY( ActionType action READ action WRITE setAction NOTIFY actionChanged)
	Q_PROPERTY( QVariantMap content READ content WRITE setContent NOTIFY contentChanged)
//	Q_PROPERTY( QByteArray rawData READ rawData WRITE setRawData NOTIFY rawDataChanged)
public:
	enum ActionType {
		UNDEFINED, SYNC, DELAY_REQUEST, DELAY_REPONSE, INFORMATION, COMMAND,
		ERROR, RESPONSE, REQUEST, MULTI_PACKAGE, USER_INFO
	};
	Action(QObject *parent = 0);
	Action(const Action &action);
	virtual ~Action();
	QString toString();

	/*qint64 timestamp();
	void setTimestamp(qint64 timestamp);
	Q_SIGNAL void timestampChanged(qint64 timestamp);
	 */
	ActionType action();
	void setAction(ActionType action);
	Q_SIGNAL void actionChanged(qint64 timestamp);

	QVariantMap content();
	void setContent(QVariantMap content);
	Q_SIGNAL void contentChanged(QVariantMap content);

	/*QByteArray rawData() const;
	void setRawData(QByteArray rawData);
	Q_SIGNAL void rawDataChanged(QByteArray rawData);
	quint64 rawDataSize() const;*/

	void printToQDebug();

#ifdef __QNXNTO__
	static void registerQmlTypes() {
		qmlRegisterType<network::Action>("wisland.network", 1, 0, "Action");
		qmlRegisterUncreatableType<network::Action>("wisland.network", 1, 0, "ActionType", "");
	}
#endif

private:
	//qint64 m_timestamp;
	ActionType m_action;
	QVariantMap m_content;
	//QByteArray m_rawData;
};

}  // namespace bonjour

Q_DECLARE_METATYPE(network::Action);

#endif /* ACTION_H_ */
