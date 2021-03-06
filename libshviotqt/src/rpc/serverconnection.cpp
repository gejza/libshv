﻿#include "serverconnection.h"
#include "socketrpcconnection.h"
#include "socket.h"

#include <shv/coreqt/log.h>

#include <shv/core/exception.h>
#include <shv/core/string.h>

//#include <shv/chainpack/chainpackprotocol.h>
#include <shv/chainpack/rpcmessage.h>

#include <QTcpSocket>
#include <QTimer>
#include <QCryptographicHash>
#include <QHostAddress>

namespace cp = shv::chainpack;

namespace shv {
namespace iotqt {
namespace rpc {

static int s_initPhaseTimeout = 10000;

ServerConnection::ServerConnection(Socket *socket, QObject *parent)
	: Super(parent)
{
	//socket->setParent(nullptr);
	setSocket(socket);
	connect(this, &ServerConnection::socketConnectedChanged, [this](bool is_connected) {
		if(is_connected) {
			m_helloReceived = m_loginReceived = false;
			setConnectionName(peerAddress() + ':' + shv::chainpack::Utils::toString(peerPort()));
		}
	});
	QTimer::singleShot(s_initPhaseTimeout, this, [this]() {
		if(isLoginPhase()) {
			shvWarning() << "Client should login in" << (s_initPhaseTimeout/1000) << "seconds, dropping out connection.";
			abort();
		}
	});
}

ServerConnection::~ServerConnection()
{
	//m_isDestroyPhase = true;
	shvInfo() << "Destroying Connection ID:" << connectionId() << "name:" << connectionName();
	abort();
}

bool ServerConnection::isSlaveBrokerConnection() const
{
	return m_connectionOptions.toMap().hasKey(cp::Rpc::KEY_BROKER);
}

void ServerConnection::sendMessage(const chainpack::RpcMessage &rpc_msg)
{
	sendRpcValue(rpc_msg.value());
}

void ServerConnection::onRpcDataReceived(shv::chainpack::Rpc::ProtocolType protocol_type, shv::chainpack::RpcValue::MetaData &&md, const std::string &data, size_t start_pos, size_t data_len)
{
	//shvInfo() << __FILE__ << RCV_LOG_ARROW << md.toStdString() << shv::chainpack::Utils::toHexElided(data, start_pos, 100);
	if(isLoginPhase()) {
		shv::chainpack::RpcValue rpc_val = decodeData(protocol_type, data, start_pos);
		rpc_val.setMetaData(std::move(md));
		cp::RpcMessage msg(rpc_val);
		processLoginPhase(msg);
		return;
	}
	Super::onRpcDataReceived(protocol_type, std::move(md), data, start_pos, data_len);
}

void ServerConnection::onRpcValueReceived(const chainpack::RpcValue &rpc_val)
{
	cp::RpcMessage msg(rpc_val);
	onRpcMessageReceived(msg);
}

void ServerConnection::onRpcMessageReceived(const chainpack::RpcMessage &msg)
{
	emit rpcMessageReceived(msg);
}

void ServerConnection::processLoginPhase(const chainpack::RpcMessage &msg)
{
	cp::RpcRequest rq(msg);
	try {
		if(!msg.isRequest())
			SHV_EXCEPTION("Initial message is not RPC request! Dropping client connection. " + connectionName() + " " + msg.toCpon());
			//shvInfo() << "RPC request received:" << rq.toStdString();

		if(!m_helloReceived && !m_loginReceived && rq.method() == shv::chainpack::Rpc::METH_HELLO) {
			shvInfo().nospace() << "Client hello received from: " << socket()->peerAddress().toString().toStdString() << ':' << socket()->peerPort();
			m_helloReceived = true;
			shvInfo() << "sending hello response:" << connectionName();
			m_userLoginContext.serverNounce = shv::chainpack::Utils::toString(std::rand());
			cp::RpcValue::Map params {
				{"nonce", m_userLoginContext.serverNounce}
			};
			sendResponse(rq.requestId(), params);
			return;
		}
		if(m_helloReceived && !m_loginReceived && rq.method() == shv::chainpack::Rpc::METH_LOGIN) {
			shvInfo() << "Client login received";// << profile;// << "device id::" << m.value("deviceId").toStdString();
			m_loginReceived = true;
			m_userLoginContext.loginRequest = msg;
			m_userLoginContext.connectionId = connectionId();
			cp::RpcValue::Map params = rq.params().toMap();
			m_connectionOptions = params.value(cp::Rpc::KEY_OPTIONS);
			/*
			{
				const chainpack::RpcValue::Map omap = m_connectionOptions.toMap();
				if(omap.hasKey("broker"))
					m_connectionType = ConnectionType::Broker;
				else if(omap.hasKey("device"))
					m_connectionType = ConnectionType::Device;
				else
					m_connectionType = ConnectionType::Client;
			}
			*/
			processLoginPhase();
			/*
			if(!login_resp.isValid())
				SHV_EXCEPTION("Invalid authentication for user: " + m_userLogin.user + " at: " + connectionName());
			shvInfo().nospace() << "Client logged in user: " << m_userLogin.user << " from: " << peerAddress() << ':' << peerPort();
			sendResponse(rq.requestId(), login_resp);
			*/
			return;
		}
	}
	catch(shv::core::Exception &e) {
		sendError(rq.requestId(), cp::RpcResponse::Error::create(cp::RpcResponse::Error::MethodCallException, e.message()));
	}
	shvError() << "Initial handshake error! Dropping client connection." << connectionName() << msg.toCpon();
	QTimer::singleShot(100, this, &ServerConnection::abort); // need some time to send error to client
}

void ServerConnection::processLoginPhase()
{
	m_userLogin = m_userLoginContext.userLogin();
	shvInfo() << "login - user:" << userName();// << "password:" << password_hash;
	//checkPassword();
	/*
	if(password_ok) {
		cp::RpcValue::Map login_resp;
		//login_resp[cp::Rpc::KEY_CLIENT_ID] = connectionId();
		return chainpack::RpcValue{login_resp};
	}
	return cp::RpcValue();
	*/
}

void ServerConnection::setLoginResult(const chainpack::UserLoginResult &result)
{
	m_loginOk = result.passwordOk;
	auto resp = cp::RpcResponse::forRequest(m_userLoginContext.loginRequest);
	if(result.passwordOk) {
		shvInfo().nospace() << "Client logged in user: " << m_userLogin.user << " from: " << peerAddress() << ':' << peerPort();
		resp.setResult(result.toRpcValue());
	}
	else {
		shvInfo().nospace() << "Invalid authentication for user: " << m_userLogin.user
							<< " reason: " + result.loginError
							<< " at: " + connectionName();
		resp.setError(cp::RpcResponse::Error::createMethodCallExceptionError("Invalid authentication for user: " + m_userLogin.user
																			 + " reason: " + result.loginError
																			 + " at: " + connectionName()));
	}
	sendMessage(resp);
}

}}}


