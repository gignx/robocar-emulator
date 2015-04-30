#include <server.hpp>

	using Gangster = justine::Server::Gangster;
	using Cop = justine::Server::Cop;

	std::vector<Gangster> justine::Server::getGangsters()
	{
		std::vector<Gangster> gangsters;
		if(hack_id==0 || !isConnected()) return gangsters;

		boost::system::error_code error_code;
		char buffer[MAX_BUFFER_SIZE];
		size_t msg_length = std::sprintf(buffer, "<gangsters ");
		msg_length += std::sprintf(buffer + msg_length, "%d>", hack_id);
		socket->send(boost::asio::buffer(buffer,msg_length));
		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);

  		int gangster_car_id {0};
	  	int bytes_read      {0};
		int seek_pointer    {0};

		unsigned from_node, to_node, step;

		while(std::sscanf(buffer+seek_pointer, "<OK %d %u %u %u>%n",
		                  &gangster_car_id, &from_node, &to_node, &step, &bytes_read) == 4)
		{
		  seek_pointer += bytes_read;
		  gangsters.emplace_back(Gangster {gangster_car_id, from_node, to_node, step});
		}
	    return gangsters;
	}

	Cop justine::Server::getCopData(int id)
	{
		Cop r {0,0,0,0};
		if(!isConnected()) return r;
		boost::system::error_code error_code;
 		char buffer[MAX_BUFFER_SIZE];
 		size_t msg_length = std::sprintf(buffer, "<car ");
  		msg_length += std::sprintf(buffer + msg_length, "%d>", id);
  		socket->send(boost::asio::buffer(buffer, msg_length));
  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
 		r.id=id;
  		std::sscanf(buffer, "<OK %*d %u %u %u", &r.from, &r.to, &r.step);
		return r;
	}

	std::vector<Cop> justine::Server::spawnCops(std::string teamname, int num_cops)
	{
		std::vector<Cop> cops;
		if(!isConnected()) return cops;
        boost::system::error_code error_code;
  		char buffer[MAX_BUFFER_SIZE];
  		size_t msg_length = std::sprintf(buffer, "<init guided %s %d c>",teamname.c_str(), num_cops);
  		socket->send(boost::asio::buffer(buffer, msg_length));
  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);

  		int cop_car_id   {0};
  		int bytes_read   {0};
  		int seek_pointer {0};

  		std::vector<int> cop_ids;

  		while(std::sscanf(buffer+seek_pointer, "<OK %d %*d/%*d %*c>%n", &cop_car_id, &bytes_read) == 1)
  		{
    		seek_pointer += bytes_read;
    		cop_ids.push_back(cop_car_id);
    		hack_id = cop_car_id;
  		}

  		
  		for(int c:cop_ids) cops.emplace_back(getCopData(c));

   		return cops;
	}

	void justine::Server::sendRoute(int id, std::vector<osmium::unsigned_object_id_type> & path)
	{
		if(!isConnected()) return;
		boost::system::error_code error_code;
  		char buffer[MAX_BUFFER_SIZE];
  		size_t msg_length = std::sprintf(buffer, "<route %zu %d", path.size(), id);

  		for(auto ui: path){msg_length += std::sprintf(buffer + msg_length, " %lu", ui);}
		msg_length += std::sprintf(buffer + msg_length, ">");

  		socket->send(boost::asio::buffer(buffer, msg_length));

  		msg_length = socket->read_some(boost::asio::buffer(buffer), error_code);
	}

	void justine::Server::sendRoute(Cop c, std::vector<osmium::unsigned_object_id_type> & path)
	{
		justine::Server::sendRoute(c.id, path);
	}


	Cop justine::Server::getCopData(Cop c)
	{
		return justine::Server::getCopData(c.id);
	}


	Gangster justine::Server::getGangster(int id)
	{
		std::vector<Gangster> gangsters = justine::Server::getGangsters();
		return *std::find_if(gangsters.begin(), gangsters.end(), [id] (Gangster g) {return g.id==id;});
	}


	Gangster justine::Server::getGangster(Gangster g)
	{
		return justine::Server::getGangster(g.id);
	}

