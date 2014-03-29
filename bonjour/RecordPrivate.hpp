#ifndef RECORD_PRIVATE
#define RECORD_PRIVATE

#include <QString>
#include <QSharedData>

namespace bonjour {

class RecordData: public QSharedData {
public:
	RecordData() {
	}
	RecordData(const RecordData &other) :
			QSharedData(other), serviceName(other.serviceName), registeredType(
					other.registeredType), replyDomain(other.replyDomain) {
	}
	~RecordData() {
	}
	QString serviceName;
	QString registeredType;
	QString replyDomain;
};

}  // namespace bonjour

#endif //RECORD_PRIVATE
