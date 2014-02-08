/*
 * Serializable.h
 *
 *  Created on: Feb 8, 2014
 *      Author: rodrigopex
 */

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include <QObject>

class Serializable: public QObject {
	Q_OBJECT
public:
	Serializable(QObject *parent = 0);
	virtual ~Serializable();

};
QDataStream &operator<<(QDataStream &ds, const Serializable &obj);
QDataStream &operator>>(QDataStream &ds, Serializable &obj);

#endif /* SERIALIZABLE_H_ */
