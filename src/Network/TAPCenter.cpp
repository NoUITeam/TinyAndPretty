#include <Network/TAPManager.h>
#include <Network/URLParser.h>
#include <Network/ManagerBase.h>
#include <Network/ThreadPool.h>
#include <Network/ServerBase.h>

class TAPCenter
{
private:
	TAPCenter();
	TAPCenter(const TAPCenter &) = delete;
	TAPCenter(const TAPCenter &&) = delete;
	TAPCenter &operator=(const TAPCenter &) = delete;
	TAPCenter &operator=(const TAPCenter &&) = delete;

public:
	std::shared_ptr<Socket> sock;
	std::unique_ptr<EventPool> epool;
	std::unique_ptr<ThreadPool> thpool;
	std::vector<std::unique_ptr<ManagerBase>> plugins;

	static auto getInstance()
	{
		static std::shared_ptr<TAPCenter> ptr {new TAPCenter};
		return ptr;
	}
	void start();
	void distributeTask(Connection* conn);
};

TAPCenter::TAPCenter()
{
	sock   = std::make_shared<Socket>();
	epool  = std::make_unique<EventPool>();
	thpool = std::make_unique<ThreadPool>(CONFIG_THREADS_MAXIMUM);

	epool->mountFD(sock->getFD(), EPOLLIN | EPOLLET);
}

TAPManager::TAPManager()
{
	ptr = TAPCenter::getInstance();
}
void TAPManager::start()
{
	ptr->start();
}

void TAPManager::loadSubManager(std::unique_ptr<ManagerBase> sub)
{
	sub->setSock(ptr->sock);
	ptr->plugins.emplace_back(std::move(sub));
	
}

// fd conn bug!

void TAPCenter::distributeTask(Connection* conn)
{
	IFDEBUG(std::cerr << "\n*FD IN : \t" << conn->getFD() << "\n");
	for(auto& ptr : plugins) 
	{
		thpool->enqueue([&]{ // sock also =?
			if( ptr->protocalConfirm() ){
				ptr->createTask(conn);
			}
			// near future , CURRENTLY long connection is not supported
			IFDEBUG(std::cerr << "##### Connection offline. #####\n");
		});
	}
}

void TAPCenter::start()
{
	epool->Loop([&](epoll_data_t data, int type)
				{
		if(data.fd == sock->getFD()) {
			while(true) {
				Connection* con = sock->onConnect();
				if(con == nullptr) break;

				IFDEBUG(std::cerr << "Connection : " << con->getFD() << " Handshake!\n");	
				epool->mountPtr(con , con->getFD() , EPOLLIN | EPOLLET);
			}
		}else if (type & EPOLLIN) {
			Connection* conn = static_cast<Connection *>(data.ptr) ;
			this->distributeTask( conn );
		} });
}
