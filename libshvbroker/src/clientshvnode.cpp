#include "clientshvnode.h"
#include "brokerapp.h"

#include "rpc/brokerclientserverconnection.h"
#include "rpc/masterbrokerclientconnection.h"

#include <shv/coreqt/log.h>

namespace cp = shv::chainpack;

namespace shv {
namespace broker {

//======================================================================
// ClientShvNode
//======================================================================
ClientShvNode::ClientShvNode(rpc::BrokerClientServerConnection *conn, ShvNode *parent)
	: Super(parent)
{
	shvInfo() << "Creating client node:" << this << nodeId() << "connection:" << conn->connectionId();
	addConnection(conn);
}

ClientShvNode::~ClientShvNode()
{
	shvInfo() << "Destroying client node:" << this << nodeId();// << "connections:" << [this]() { std::string s; for(auto c : m_connections) s += std::to_string(c->connectionId()) + " "; return s;}();
}

void ClientShvNode::addConnection(rpc::BrokerClientServerConnection *conn)
{
	// prefere new connections, old one might not work
	m_connections.insert(0, conn);
	connect(conn, &rpc::BrokerClientServerConnection::destroyed, [this, conn]() {removeConnection(conn);});
}

void ClientShvNode::removeConnection(rpc::BrokerClientServerConnection *conn)
{
	//shvWarning() << this << "removing connection:" << conn;
	m_connections.removeOne(conn);
	if(m_connections.isEmpty() && ownChildren().isEmpty())
		deleteLater();
}

void ClientShvNode::handleRawRpcRequest(shv::chainpack::RpcValue::MetaData &&meta, std::string &&data)
{
	rpc::BrokerClientServerConnection *conn = connection();
	if(conn)
		conn->sendRawData(std::move(meta), std::move(data));
}

shv::chainpack::RpcValue ClientShvNode::hasChildren(const StringViewList &shv_path)
{
	Q_UNUSED(shv_path)
	//shvWarning() << "ShvClientNode::hasChildren path:" << StringView::join(shv_path, '/');// << "ret:" << nullptr;
	return nullptr;
}

//======================================================================
// MasterBrokerShvNode
//======================================================================
MasterBrokerShvNode::MasterBrokerShvNode(ShvNode *parent)
	: Super(parent)
{
	shvInfo() << "Creating master broker connection node:" << this;
}

MasterBrokerShvNode::~MasterBrokerShvNode()
{
	shvInfo() << "Destroying master broker connection node:" << this;
}

}}
