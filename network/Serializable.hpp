/*
 * Serializable.h
 *
 *  Created on: Feb 8, 2014
 *      Author: rodrigopex
 */

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include <QObject>
#include <QMetaProperty>

namespace network {

class Serializable: public QObject {
	Q_OBJECT
public:
	Serializable(QObject *parent = 0);
	Serializable(const Serializable &other): QObject(0) {
		Q_UNUSED(other);
	}
	virtual ~Serializable();

};
QDataStream &operator<<(QDataStream &ds, const Serializable &obj);
QDataStream &operator>>(QDataStream &ds, Serializable &obj);

}  // namespace bonjour

Q_DECLARE_METATYPE(network::Serializable);

#endif /* SERIALIZABLE_H_ */
