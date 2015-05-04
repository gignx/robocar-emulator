#include <server.hpp>

	int justine::sampleclient::Server::auth_code = -1;
    
	using Gangster = justine::sampleclient::Server::Gangster;
	using Cop = justine::sampleclient::Server::Cop;

	void justine::sampleclient::Server::stopCop(int id, int auth)
	{
		boost::system::error_code error_code;
		char buffer[MAX_BUFFER_SIZE];
		size_t msg_length = std::sprintf(buffer, "<stop %d %d>", auth, id);
		socket->send(boost::asio::buffer (buffer, msg_length));
		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
	}

	void justine::sampleclient::Server::stopCop(Cop c, int auth)
	{
		justine::sampleclient::Server::stopCop(c.id, auth);
	}

	int justine::sampleclient::Server::authenticate(std::string team_name)
	{
		if(!isConnected()) return auth_code;
		auth_code = -2;
		boost::system::error_code error_code;
		char buffer[MAX_BUFFER_SIZE];
		size_t msg_length = std::sprintf(buffer, "<auth %s c>", team_name.c_str());
		std::cout << "Sending: " << buffer;
		socket->send(boost::asio::buffer (buffer, msg_length));
		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
		std::cout <<" Answer: "<< buffer << std::endl;
		std::sscanf(buffer, "<OK %d>", &auth_code);
		return auth_code;
	}

	std::vector<Gangster> justine::sampleclient::Server::getGangsters(int auth)
	{
		std::vector<Gangster> gangsters;
		if(auth==-1) {std::cout << "Error: authenticate required for this command. (GANGSTERS)" << std::endl; return gangsters;}
		if(!isConnected()) return gangsters;

		boost::system::error_code error_code;
		char buffer[MAX_BUFFER_SIZE];
		size_t msg_length = std::sprintf(buffer, "<gangsters %d>", auth);
		socket->send(boost::asio::buffer(buffer,msg_length));
		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);

  		int gangster_car_id {0};
	  	int bytes_read      {0};
		int seek_pointer    {0};

		unsigned int from_node, to_node, step;

		while(std::sscanf(buffer+seek_pointer, "<OK %d %u %u %u>%n", &gangster_car_id, &from_node, &to_node, &step, &bytes_read) == 4)
		{
		  seek_pointer += bytes_read;
		  gangsters.emplace_back(Gangster {gangster_car_id, from_node, to_node, step});
		}
	    return gangsters;
	}

	Cop justine::sampleclient::Server::getCopData(int id, int auth)
	{
		Cop r {0,0,0,0};
		if(auth==-1) {std::cout << "Error: authenticate required for this command. (CAR)" << std::endl; return r;}
		if(!isConnected()) return r;
		boost::system::error_code error_code;
 		char buffer[MAX_BUFFER_SIZE];
 		size_t msg_length = std::sprintf(buffer, "<car %d %d>", auth, id);
  		socket->send(boost::asio::buffer(buffer, msg_length));
  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
  		int cop_id;
  		unsigned int f,t,s;
  		std::sscanf(buffer, "<OK %d %u %u %u>", &cop_id, &f, &t, &s);
		return Cop{cop_id, f,t,s};
	}

	std::vector<Cop> justine::sampleclient::Server::spawnCops(std::string team_name, int num_cops, int auth)
	{
		std::vector<Cop> cops;
		if(!isConnected()) return cops;
		// Automatic auth at spawn, this way in the client we dont need to worry about auth
		if(auth == -1){justine::sampleclient::Server::authenticate(team_name); return justine::sampleclient::Server::spawnCops(team_name, num_cops);}
		if(auth == -2) {std::cout << "Error: team name already taken!"; return cops;}

        boost::system::error_code error_code;
  		char buffer[MAX_BUFFER_SIZE];
  		size_t msg_length = std::sprintf(buffer, "<init guided %d %d c>", auth, num_cops);
  		socket->send(boost::asio::buffer(buffer, msg_length));
  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);

  		int cop_car_id   {0};
  		int bytes_read   {0};
  		int seek_pointer {0};

  		std::vector<int> cop_ids;

  		while(std::sscanf(buffer+seek_pointer, "<OK %d %*d/%*d %*c>%n", &cop_car_id, &bytes_read) == 1)
  		{
    		seek_pointer += bytes_read;
    		cop_ids.emplace_back(cop_car_id);
  		}
  		
  		for(int c:cop_ids) cops.emplace_back(getCopData(c, auth));
   		return cops;
	}

	void justine::sampleclient::Server::sendRoute(int id, std::vector<osmium::unsigned_object_id_type> & path, int auth)
	{
		if(!isConnected()) return;
		if(auth==-1){std::cout << "Error: authenticate required for this command. (ROUTE)" << std::endl; return;}
		boost::system::error_code error_code;
  		char buffer[MAX_BUFFER_SIZE];
  		size_t msg_length = std::sprintf(buffer, "<route %lu %d %d", path.size(), auth, id);

  		for(auto ui: path){msg_length += std::sprintf(buffer + msg_length, " %lu", ui);}
		msg_length += std::sprintf(buffer + msg_length, ">");



  		socket->send(boost::asio::buffer(buffer, msg_length));

  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
	}

	void justine::sampleclient::Server::sendRoute(Cop c, std::vector<osmium::unsigned_object_id_type> & path, int auth)
	{
		justine::sampleclient::Server::sendRoute(c.id, path, auth);
	}


	Cop justine::sampleclient::Server::getCopData(Cop c, int auth)
	{
		return justine::sampleclient::Server::getCopData(c.id, auth);
	}


	Gangster justine::sampleclient::Server::getGangster(int id,int auth)
	{
		std::vector<Gangster> gangsters = justine::sampleclient::Server::getGangsters(auth);
		return *std::find_if(gangsters.begin(), gangsters.end(), [id] (Gangster g) { return g.id==id;});
	}


	Gangster justine::sampleclient::Server::getGangster(Gangster g, int auth)
	{
		return justine::sampleclient::Server::getGangster(g.id, auth);
	}

