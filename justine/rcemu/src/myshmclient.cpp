/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient.cpp
 * @author  Norbert Bátfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright(C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 * Robocar City Emulator and Robocar World Championship
 *
 * desc
 *
 */

#include <myshmclient.hpp>

char data[524288];

std::vector<justine::sampleclient::MyShmClient::Gangster>
justine::sampleclient::MyShmClient::AcquireGangstersFromServer(
    boost::asio::ip::tcp::socket &socket,
    int id,
    osmium::unsigned_object_id_type cop)
{
  boost::system::error_code err;

  size_t length = std::sprintf(data, "<gangsters ");
  length += std::sprintf(data + length, "%d>", id);

  socket.send(boost::asio::buffer(data, length));

  length = socket.read_some(boost::asio::buffer(data), err);

  if(err == boost::asio::error::eof)
  {
    // TODO
  }
  else if(err)
  {
    throw boost::system::system_error(err);
  }

  /* reading all gangsters into a vector */
  int idd {0};
  unsigned f, t, s;
  int n {0};
  int nn {0};
  std::vector<Gangster> gangsters;

  while(std::sscanf(data+nn, "<OK %d %u %u %u>%n", &idd, &f, &t, &s, &n) == 4)
  {
    nn += n;
    gangsters.emplace_back(Gangster {idd, f, t, s});
  }

  std::sort(gangsters.begin(), gangsters.end(), [this, cop](Gangster x, Gangster y)
  {
    return dst(cop, x.to) < dst(cop, y.to);
  });

  std::cout.write(data, length);
  std::cout << "Command GANGSTER sent." << std::endl;

  return gangsters;
}

//std::vector<justine::sampleclient::MyShmClient::Cop>
size_t
justine::sampleclient::MyShmClient::InitializeCops(
    boost::asio::ip::tcp::socket & socket,
    unsigned cop_count)
{
  boost::system::error_code err;

  this->num_cops_ = cop_count;


  size_t length = std::sprintf(data, "<init guided %s %d c>",
                               m_team_name_.c_str(), num_cops_);

  socket.send(boost::asio::buffer(data, length));

  length = socket.read_some(boost::asio::buffer(data), err);

  if(err == boost::asio::error::eof)
  {
      // TODO
  }
  else if(err)
  {
      throw boost::system::system_error(err);
  }

  int cop_car_id {0};
  int f, t;
  char c;
  int n {0};
  int nn {0};

  while(std::sscanf(data+nn, "<OK %d %d/%d %c>%n", &cop_car_id, &f, &t, &c, &n) == 4)
  {
    nn += n;
    cops_.push_back(cop_car_id);
  }

  std::cout.write(data, length);
  std::cout << "Command INIT sent." << std::endl;

  return cops_.size();

  //return cops;
}

/*
void justine::sampleclient::MyShmClient::pos(boost::asio::ip::tcp::socket & socket, int id)
{
  boost::system::error_code err;

  size_t length = std::sprintf(data, "<pos ");
  length += std::sprintf(data+length, "%d %u %u>", id, 2969934868u, 651365957u);

  socket.send(boost::asio::buffer(data, length));

  length = socket.read_some(boost::asio::buffer(data), err);

  if(err == boost::asio::error::eof)
  {
    // TODO
  }
  else if(err)
  {
    throw boost::system::system_error(err);
  }

  std::cout.write(data, length);
  std::cout << "Command POS sent." << std::endl;
}*/

void justine::sampleclient::MyShmClient::car(
  boost::asio::ip::tcp::socket & socket,
  int id, unsigned *f, unsigned *t, unsigned* s)
{
  boost::system::error_code err;

  size_t length = std::sprintf(data, "<car ");
  length += std::sprintf(data+length, "%d>", id);

  socket.send(boost::asio::buffer(data, length));

  length = socket.read_some(boost::asio::buffer(data), err);

  if(err == boost::asio::error::eof)
  {
    // TODO
  }
  else if(err)
  {
    throw boost::system::system_error(err);
  }

  int idd {0};
  std::sscanf(data, "<OK %d %u %u %u", &idd, f, t, s);

  std::cout.write(data, length);
  std::cout << "Command CAR sent." << std::endl;
}

void justine::sampleclient::MyShmClient::route(
  boost::asio::ip::tcp::socket & socket,
  int id,
  std::vector<osmium::unsigned_object_id_type> & path
)
{
  boost::system::error_code err;

  size_t length = std::sprintf(data,
                                 "<route %zu %d", path.size(), id);

  for(auto ui: path)
    length += std::sprintf(data+length, " %lu", ui);

  length += std::sprintf(data+length, ">");

  socket.send(boost::asio::buffer(data, length));

  length = socket.read_some(boost::asio::buffer(data), err);

  if(err == boost::asio::error::eof)
  {
    // TODO
  }
  else if(err)
  {
    throw boost::system::system_error(err);
  }

  std::cout.write(data, length);
  std::cout << "Command ROUTE sent." << std::endl;
}


void justine::sampleclient::MyShmClient::SimulateCarsLoop(
    boost::asio::io_service& io_service,
    const char * port)
{
  #ifdef DEBUG
  foo();
  #endif

  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "localhost", port);
  boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

  boost::asio::ip::tcp::socket socket(io_service);
  boost::asio::connect(socket, iterator);

  //std::vector<Cop> cops = InitializeCops(socket, num_cops_);

  unsigned int g {0u};
  unsigned int f {0u};
  unsigned int t {0u};
  unsigned int s {0u};

  std::vector<Gangster> gngstrs;

  for(;;)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    for(auto cop:cops_)
    {
      car(socket, cop, &f, &t, &s);

      gngstrs = AcquireGangstersFromServer(socket, cop, t);

      if(gngstrs.size() > 0)
      {
        g = gngstrs[0].to;
      }
      else
      {
        g = 0;
      }

      if(g > 0)
      {
        std::vector<osmium::unsigned_object_id_type> path = DetermineDijkstraPath(t, g);

        if(path.size() > 1)
        {
          std::copy(path.begin(), path.end(),
                      std::ostream_iterator<osmium::unsigned_object_id_type>(std::cout, " -> "));

          route(socket, cop, path);
        }
      }
    }
  }
}
