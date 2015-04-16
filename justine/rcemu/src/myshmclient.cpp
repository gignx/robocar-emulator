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
#include <stdint.h>

std::vector<justine::sampleclient::MyShmClient::Gangster>
justine::sampleclient::MyShmClient::AcquireGangstersFromServer(
    boost::asio::ip::tcp::socket &socket,
    int id,
    osmium::unsigned_object_id_type cop)
{
  boost::system::error_code error_code;

  char buffer[kMaxBufferLen];

  size_t msg_length = std::sprintf(buffer, "<gangsters ");
  msg_length += std::sprintf(buffer + msg_length, "%d>", id);

  socket.send(boost::asio::buffer(buffer,msg_length));

  msg_length = socket.read_some(boost::asio::buffer(buffer), error_code);

  if(error_code == boost::asio::error::eof)
  {
    // TODO
  }
  else if(error_code)
  {
    throw boost::system::system_error(error_code);
  }

  /* reading all gangsters into a vector */
  int gangster_car_id {0};

  int bytes_read      {0};
  int seek_pointer    {0};

  unsigned from_node, to_node, step;

  std::vector<Gangster> gangsters;

  while(std::sscanf(buffer+seek_pointer, "<OK %d %u %u %u>%n",
                    &gangster_car_id, &from_node, &to_node, &step, &bytes_read) == 4)
  {
    seek_pointer += bytes_read;
    gangsters.emplace_back(Gangster {gangster_car_id, from_node, to_node, step});
  }

  std::sort(gangsters.begin(), gangsters.end(), [this, cop](Gangster x, Gangster y)
  {
    return dst(cop, x.to) < dst(cop, y.to);
  });

  std::cout.write(buffer, msg_length);
  std::cout << "Command GANGSTER sent." << std::endl;

  return gangsters;
}

//std::vector<justine::sampleclient::MyShmClient::Cop>
int
justine::sampleclient::MyShmClient::InitializeCops(
    boost::asio::ip::tcp::socket & socket,
    unsigned cop_count)
{
  boost::system::error_code error_code;

  char buffer[kMaxBufferLen];

  this->num_cops_ = cop_count;

  size_t msg_length = std::sprintf(buffer, "<init guided %s %d c>",
                                   m_team_name_.c_str(), num_cops_);

  socket.send(boost::asio::buffer(buffer, msg_length));

  msg_length = socket.read_some(boost::asio::buffer(buffer), error_code);

  if(error_code == boost::asio::error::eof)
  {
      // TODO
  }
  else if(error_code)
  {
      throw boost::system::system_error(error_code);
  }

  int cop_car_id   {0};
  int bytes_read   {0};
  int seek_pointer {0};

  while(std::sscanf(buffer+seek_pointer, "<OK %d %*d/%*d %*c>%n", &cop_car_id, &bytes_read) == 1)
  {
    seek_pointer += bytes_read;
    cops_.push_back(cop_car_id);
  }

  std::cout.write(buffer, msg_length);
  std::cout << "Command INIT sent." << std::endl;

  return cops_.size();

  //return cops;
}

/*
void justine::sampleclient::MyShmClient::pos(boost::asio::ip::tcp::socket & socket, int id)
{
  boost::system::error_code err;

  size_t length = std::sprintf(buffer, "<pos ");
  length += std::sprintf(buffer+length, "%d %u %u>", id, 2969934868u, 651365957u);

  socket.send(boost::asio::buffer(buffer, length));

  length = socket.read_some(boost::asio::buffer(buffer), err);

  if(err == boost::asio::error::eof)
  {
    // TODO
  }
  else if(err)
  {
    throw boost::system::system_error(err);
  }

  std::cout.write(buffer, length);
  std::cout << "Command POS sent." << std::endl;
}*/

void justine::sampleclient::MyShmClient::car(
  boost::asio::ip::tcp::socket & socket,
  int id, unsigned *f, unsigned *t, unsigned* s)
{
  boost::system::error_code error_code;

  char buffer[kMaxBufferLen];

  size_t msg_length = std::sprintf(buffer, "<car ");
  msg_length += std::sprintf(buffer + msg_length, "%d>", id);

  socket.send(boost::asio::buffer(buffer, msg_length));

  msg_length = socket.read_some(boost::asio::buffer(buffer), error_code);

  if(error_code == boost::asio::error::eof)
  {
    // TODO
  }
  else if(error_code)
  {
    throw boost::system::system_error(error_code);
  }

  std::sscanf(buffer, "<OK %*d %u %u %u", f, t, s);

  std::cout.write(buffer, msg_length);
  std::cout << "Command CAR sent." << std::endl;
}

void justine::sampleclient::MyShmClient::route(
  boost::asio::ip::tcp::socket & socket,
  int id,
  std::vector<osmium::unsigned_object_id_type> & path
)
{
  boost::system::error_code error_code;

  char buffer[kMaxBufferLen];

  size_t msg_length = std::sprintf(buffer,
                                   "<route %zu %d", path.size(), id);

  for(auto ui: path)
  {
    msg_length += std::sprintf(buffer + msg_length, " %lu", ui);
  }
  msg_length += std::sprintf(buffer + msg_length, ">");

  socket.send(boost::asio::buffer(buffer, msg_length));

  msg_length = socket.read_some(boost::asio::buffer(buffer), error_code);

  if(error_code == boost::asio::error::eof)
  {
    // TODO
  }
  else if(error_code)
  {
    throw boost::system::system_error(error_code);
  }

  std::cout.write(buffer, msg_length);
  std::cout << "Command ROUTE sent." << std::endl;
}


void justine::sampleclient::MyShmClient::SimulateCarsLoop(void)
{
  #ifdef DEBUG
  foo();
  #endif

  boost::asio::io_service io_service;

  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "localhost", port_);
  boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

  boost::asio::ip::tcp::socket socket(io_service);
  boost::asio::connect(socket, iterator);

  int cops_initialized = InitializeCops(socket, num_cops_);

  if (cops_initialized < num_cops_)
  {
    std::cout << "WARNING: Failed to initialize the number of cops requested: " << std::endl
              << cops_initialized << " out of " << num_cops_  <<  "initialized" << std::endl;
  }

  unsigned from_node     {0u};
  unsigned to_node       {0u};
  unsigned step          {0u};

  std::vector<Gangster> gangsters;

  for(;;)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    for(auto cop:cops_)
    {
      car(socket, cop, &from_node, &to_node, &step);

      gangsters = AcquireGangstersFromServer(socket, cop, to_node);

      if(gangsters.size() > 0)
      {
        std::vector<osmium::unsigned_object_id_type> path =
          DetermineDijkstraPath(to_node, gangsters[0].to);

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
