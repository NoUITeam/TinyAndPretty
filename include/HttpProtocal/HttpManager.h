#ifndef __HTTP_MANAGER__
#define __HTTP_MANAGER__

#include <common.h>
#include <Network/ManagerBase.h>
#include <HttpProtocal/HttpException.h>
#include <HttpProtocal/HttpProtocal.h>
#include <HttpProtocal/HttpAdapter.h>

////HttpTask center manager
class HttpManager : public ManagerBase
{
	//specialize http io through adapter
	std::unique_ptr<HttpAdapter> wrapper = std::make_unique<HttpAdapter>(sock);
public:
	HttpResponse *dispatchException(const HttpException &e);
	HttpResponseBase *taskExecute(Connection* conn, std::shared_ptr<uint8_t> raw, size_t len);

	virtual bool protocalConfirm() override;
	virtual void createTask(Connection* conn) override;
};

#endif