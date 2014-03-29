/*
 Copyright (c) 2007, Trenton Schulz

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BONJOURRECORD_H
#define BONJOURRECORD_H

#include <QVariantMap>
#include <QSharedData>
#include <QtCore/QString>
#include <QtCore/QMetaType>

#include "RecordPrivate.hpp"

namespace bonjour {

class Record {
public:
	Record();
	Record(const Record& other);
	Record(QVariantMap recordMap);
	Record(const QString &name, const QString &regType, const QString &domain);
	Record(const char *name, const char *regType, const char *domain);
	~Record();
	bool operator<(const bonjour::Record other) const;
	void setServiceName(QLatin1String name);
	QVariantMap toMap() const;
	//static Record fromMap(QVariantMap recordMap);
	bool operator==(const Record &other) const;
	void setServiceName(QString serviceName);
	const QString &serviceName() const;
	void setRegisteredType(QString registeredType);
	const QString &registeredType() const;
	void setReplyDomain(QString replyDomain);
	const QString &replyDomain() const;

	int const * const &desafio(int const * const & a) const;

private:
	QSharedDataPointer<RecordData> d;
};

}  // namespace bonjour

Q_DECLARE_METATYPE(bonjour::Record)

#endif // BONJOURRECORD_H
