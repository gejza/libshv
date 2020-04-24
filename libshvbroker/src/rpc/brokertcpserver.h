#pragma once

#include <shv/iotqt/rpc/tcpserver.h>

namespace shv {
namespace broker {
namespace rpc {

class ServerConnectionBroker;

class BrokerTcpServer : public shv::iotqt::rpc::TcpServer
{
	Q_OBJECT
	using Super = shv::iotqt::rpc::TcpServer;
public:
	BrokerTcpServer(QObject *parent = nullptr);

	ServerConnectionBroker* connectionById(int connection_id);
protected:
	shv::iotqt::rpc::ServerConnection* createServerConnection(QTcpSocket *socket, QObject *parent) override;
};

}}}
