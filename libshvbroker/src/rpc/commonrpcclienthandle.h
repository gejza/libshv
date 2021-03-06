#pragma once

#include <shv/chainpack/rpcmessage.h>

namespace shv { namespace core { class StringView; }}

namespace shv {
namespace broker {
namespace rpc {

class CommonRpcClientHandle
{
public:
	struct Subscription
	{
		std::string absolutePath;
		std::string relativePath;
		std::string method;

		Subscription() {}
		Subscription(const std::string &ap, const std::string &rp, const std::string &m);

		std::string toRelativePath(const std::string &abs_path) const;

		//bool operator<(const Subscription &o) const;
		bool operator==(const Subscription &o) const;
		bool match(const shv::core::StringView &shv_path, const shv::core::StringView &shv_method) const;
		std::string toString() const {return absolutePath + ':' + method;}
	};
	/*
	struct SubsKeyLess
	{
		bool operator()(const SubsKey &k1, const SubsKey &k2) const
		{
			if(k1.pathPattern == k2.pathPattern)
				return k1.method < k2.method;
			return k1.pathPattern < k2.pathPattern;
		}
	};
	*/
public:
	CommonRpcClientHandle();
	virtual ~CommonRpcClientHandle();

	virtual int connectionId() const = 0;
	virtual bool isConnectedAndLoggedIn() const = 0;

	virtual unsigned addSubscription(const std::string &rel_path, const std::string &method) = 0;
	unsigned addSubscription(const Subscription &subs);
	virtual bool removeSubscription(const std::string &rel_path, const std::string &method) = 0;
	bool removeSubscription(const Subscription &subs);
	int isSubscribed(const std::string &shv_path, const std::string &method) const;
	virtual std::string toSubscribedPath(const Subscription &subs, const std::string &abs_path) const;
	size_t subscriptionCount() const {return m_subscriptions.size();}
	const Subscription& subscriptionAt(size_t ix) const {return m_subscriptions.at(ix);}
	bool rejectNotSubscribedSignal(const std::string &path, const std::string &method);

	virtual std::string loggedUserName() = 0;
	virtual bool isSlaveBrokerConnection() const = 0;
	virtual bool isMasterBrokerConnection() const = 0;

	virtual void sendRawData(const shv::chainpack::RpcValue::MetaData &meta_data, std::string &&data) = 0;
	virtual void sendMessage(const shv::chainpack::RpcMessage &rpc_msg) = 0;
protected:
	std::vector<Subscription> m_subscriptions;
};

}}}

