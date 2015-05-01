#include <smartcity.hpp>
#include <boost/asio.hpp>

#define MAX_BUFFER_SIZE 524288

namespace justine
{
namespace sampleclient
{

class Server {
public:
  struct SmartCar
  {
    int id;
    unsigned from;
    unsigned to;
    unsigned step;
  };

  typedef struct SmartCar Gangster;
  typedef struct SmartCar Cop;

	Server(const char* port) {
		io_service  = new boost::asio::io_service;
		socket      = new boost::asio::ip::tcp::socket(*io_service);

  	boost::asio::ip::tcp::resolver resolver(*io_service );
  	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "localhost", port);
  	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve (query);

  	boost::asio::connect(*socket, iterator);
		std::cout<<"Connected!" << std::endl;
	}

	~Server() {
		delete socket;
		delete io_service;
	}

	bool isConnected() {
    return socket->is_open();
  }

	//functions for every server-side command

	std::vector<Gangster> getGangsters();

	Cop getCopData(int id);

	std::vector<Cop> spawnCops(std::string teamname = "Police", int num_cops = 10);

	void sendRoute(int id, std::vector<osmium::unsigned_object_id_type> & path);

	//some helper functions

	void sendRoute(Cop c, std::vector<osmium::unsigned_object_id_type> & path);

	Cop getCopData(Cop c);

	Gangster getGangster(int id);

	Gangster getGangster(Gangster g);


private:
	int hack_id = 0;
	char* port_;
	boost::asio::ip::tcp::socket* socket;
	boost::asio::io_service* io_service;
};

}
}
