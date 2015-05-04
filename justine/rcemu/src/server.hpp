#include <smartcity.hpp>
#include <boost/asio.hpp>

#define MAX_BUFFER_SIZE 524288

namespace justine{
namespace sampleclient{

class Server{

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

	static int auth_code;

	Server(const char* port){
		
		io_service = new boost::asio::io_service;
		socket = new boost::asio::ip::tcp::socket(*io_service);
  		boost::asio::ip::tcp::resolver resolver ( *io_service );
  		boost::asio::ip::tcp::resolver::query query ( boost::asio::ip::tcp::v4(), "localhost", port );
  		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve ( query );
  		boost::asio::socket_base::keep_alive option(true);
  		boost::asio::connect ( *socket, iterator );
  		std::cout << "Connected!" << std::endl;
	}

	~Server(){
		delete socket;
		delete io_service;
	}

	bool isConnected(){return socket->is_open();}

	//functions for every server-side command

	int authenticate(std::string team_name);

	void stopCop(int id, int auth = auth_code);

	std::vector<Gangster> getGangsters(int auth = auth_code);

	Cop getCopData(int id, int auth = auth_code);

	std::vector<Cop> spawnCops(std::string teamname = "Police", int num_cops = 10, int auth = auth_code);

	void sendRoute(int id, std::vector<osmium::unsigned_object_id_type> & path, int auth = auth_code);

	//some helper functions

	void sendRoute(Cop c, std::vector<osmium::unsigned_object_id_type> & path, int auth = auth_code);

	Cop getCopData(Cop c, int auth = auth_code);

	Gangster getGangster(int id, int auth = auth_code);

	Gangster getGangster(Gangster g, int auth = auth_code);

	void stopCop(Cop c, int auth = auth_code);


private:
	char* port_;
	boost::asio::ip::tcp::socket* socket;
	boost::asio::io_service* io_service;
}; 
}
} 