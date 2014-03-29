/*
 * Record.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: rodrigopex
 */
#include "Record.hpp"

namespace bonjour {

Record::Record() {
	d = new RecordData;
}
Record::Record(const Record& other) :
		d(other.d) {

}
Record::Record(const QString &name, const QString &regType,
		const QString &domain) {
	d = new RecordData;
	setServiceName(name);
	setRegisteredType(regType);
	setReplyDomain(domain);
}
Record::~Record() {
//		qDebug() << "~Record:Deleting record for service:" << serviceName;
}

Record::Record(QVariantMap recordMap) {
	d = new RecordData;
	setServiceName(recordMap.value("serviceName").value<QString>());
	setRegisteredType(recordMap.value("registeredType").value<QString>());
	setReplyDomain(recordMap.value("replyDomain").value<QString>());
}

Record::Record(const char *name, const char *regType, const char *domain) {
	d = new RecordData;
	setServiceName(QString::fromUtf8(name));
	setRegisteredType(QString::fromUtf8(regType));
	setReplyDomain(QString::fromUtf8(domain));
}

bool Record::operator<(const bonjour::Record other) const {
	return (serviceName() < other.serviceName());
}

void Record::setServiceName(QLatin1String name) {
	setServiceName(name);
}

QVariantMap Record::toMap() const {
	QVariantMap ret;
	ret["serviceName"] = this->serviceName();
	ret["registeredType"] = this->registeredType();
	ret["replyDomain"] = this->replyDomain();
	return ret;
}

bool Record::operator==(const Record &other) const {
	return serviceName() == other.serviceName()
			&& registeredType() == other.registeredType()
			&& replyDomain() == other.replyDomain();
}

void Record::setServiceName(QString serviceName) {
	d->serviceName = serviceName;
}
const QString &Record::serviceName() const {
	return d->serviceName;
}
void Record::setRegisteredType(QString registeredType) {
	d->registeredType = registeredType;
}
const QString &Record::registeredType() const {
	return d->registeredType;
}
void Record::setReplyDomain(QString replyDomain) {
	d->replyDomain = replyDomain;
}
const QString &Record::replyDomain() const {
	return d->replyDomain;
}

//int const * const &Record::desafio(int const * const & a) const{
//	//*a = 10;
//	return a;
//}

}  // namespace bonjour
