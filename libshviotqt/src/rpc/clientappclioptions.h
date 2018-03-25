#pragma once

#include <shv/coreqt/utils/clioptions.h>

#include "../shviotqtglobal.h"

namespace shv {
namespace iotqt {
namespace rpc {

class SHVIOTQT_DECL_EXPORT ClientAppCliOptions : public shv::coreqt::utils::ConfigCLIOptions
{
	Q_OBJECT
private:
	using Super = shv::coreqt::utils::ConfigCLIOptions;
public:
	ClientAppCliOptions(QObject *parent = NULL);
	~ClientAppCliOptions() Q_DECL_OVERRIDE {}

	//CLIOPTION_GETTER_SETTER(QString, l, setL, ocale)
	CLIOPTION_GETTER_SETTER2(QString, "user.name", u, setU, serName)
	CLIOPTION_GETTER_SETTER2(QString, "server.host", s, setS, erverHost)
	CLIOPTION_GETTER_SETTER2(int, "server.port", s, setS, erverPort)
	CLIOPTION_GETTER_SETTER2(int, "rpc.timeout", r, setR, pcTimeout)
	CLIOPTION_GETTER_SETTER2(bool, "rpc.metaTypeExplicit", is, set, MetaTypeExplicit)
};

} // namespace client
} // namespace iot
} // namespace shv
