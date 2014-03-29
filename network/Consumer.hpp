#ifndef CONSUMER_H
#define CONSUMER_H

#include "../bonjour/Record.hpp"
#include "../bonjour/ServiceRegister.hpp"

#include "Action.hpp"
#include "ServiceConnection.hpp"

class QHostAddress;

namespace network {

class Consumer: public QObject {
Q_OBJECT

public:
	Consumer(QLatin1String serviceType, QObject *parent = 0);
	virtual ~Consumer();
	Q_SLOT void connectToService(const bonjour::Record &record);
private slots:
	virtual void decodeAction(network::Action action) = 0;
	virtual void updateRecords(const QList<bonjour::Record> &list);
	virtual void recordAdded(const bonjour::Record &record) = 0;
	virtual void recordRemoved(const bonjour::Record &record) = 0;
	virtual void serviceSessionOpened(network::ServiceConnection *connection) = 0;
	void startServiceConnection(const QHostAddress &hostAddress, int port);
	void onServiceManagerZombie();
protected:
	bonjour::Record m_currentRecord;
	//bonjour::ServiceRegister *bonjourRegister;
};

} // namespace bonjour

#endif // CONSUMER_H
