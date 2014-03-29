/*
 * Serializable.cpp
 *
 *  Created on: Feb 8, 2014
 *      Author: rodrigopex
 */

#include "Serializable.hpp"

namespace network {

Serializable::Serializable(QObject *parent) :
		QObject(parent) {
}

Serializable::~Serializable() {
}

QDataStream &operator<<(QDataStream &ds, const Serializable &obj) {
	for (int i = 0; i < obj.metaObject()->propertyCount(); ++i) {
		if (obj.metaObject()->property(i).isStored(&obj)) {
			ds << obj.metaObject()->property(i).read(&obj);
		}
	}
	return ds;
}
QDataStream &operator>>(QDataStream &ds, Serializable &obj) {
	QVariant var;
	for (int i = 0; i < obj.metaObject()->propertyCount(); ++i) {
		if (obj.metaObject()->property(i).isStored(&obj)) {
			ds >> var;
			obj.metaObject()->property(i).write(&obj, var);
		}
	}
	return ds;
}

}  // namespace bonjour
