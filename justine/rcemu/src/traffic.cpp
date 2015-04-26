/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file traffic.cpp
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

#include <traffic.hpp>

void justine::robocar::Traffic::OpenLogStream(void)
{
  boost::posix_time::ptime now =
    boost::posix_time::second_clock::universal_time();

  logfile_name_   = boost::posix_time::to_simple_string(now);
  logfile_stream_ = new std::fstream(logfile_name_.c_str() , std::ios_base::out);
}

void justine::robocar::Traffic::CloseLogStream(void)
{
  for(auto c:m_cop_cars)
    *logfile_stream_  << *c << std::endl;

  logfile_stream_->close();

  boost::filesystem::rename(
    boost::filesystem::path(logfile_name_),
    boost::filesystem::path(get_title(logfile_name_)));
}

void justine::robocar::Traffic::InitializeRoutineCars(void)
{
  #ifdef DEBUG
  std::cout << "Initializing routine cars ... " << std::endl;
  #endif

  if (traffic_type_ != TrafficType::NORMAL)
  {
    for (shm_map_Type::iterator iter=shm_map_->begin();
         iter!=shm_map_->end();
         ++iter)
    {
      for (auto noderef : iter->second.m_alist)
      {
        AntCar::alist[iter->first].push_back(1);
        AntCar::alist_evaporate[iter->first].push_back(1);
      }
    }
  }

  for (int i {0}; i < num_cars_; ++i)
  {
    if (traffic_type_ == TrafficType::NORMAL)
    {
      std::shared_ptr<Car> car(new Car {*this});

      car->init();
      cars.push_back(car);
    }
    else
    {
      std::shared_ptr<AntCar> car(new AntCar {*this});

      car->init();
      cars.push_back(car);
    }
  }

  #ifdef DEBUG
  std::cout << "All routine cars initialized." <<"\n";
  #endif
}

void justine::robocar::Traffic::SimulationLoop(void)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  m_cv.wait(lock);

  #ifdef DEBUG
  std::cout << "Traffic simulation started." << std::endl;
  #endif

  while(is_running_)
  {
    if (++running_time_elapsed_ > (running_time_allowed_ / delay_))
    {
      is_running_ = false;

      break;
    }
    else
    {
      UpdateTraffic();

      std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
    }
  }

  std::cout << "The traffic simulation is over." << std::endl;

  CloseLogStream();
}

std::string justine::robocar::Traffic::get_title(std::string name)
{
  std::map <std::string, int> gangsters_per_team;
  for(auto c:m_cop_cars)
  {
    gangsters_per_team[c->get_team_name()] += c->get_num_gangsters_caught();
  }

  std::ostringstream ss;

  for(auto pair : gangsters_per_team)
    ss << pair.first << " " << pair.second << " ";

  ss << name << ".txt";

  return ss.str();
}

osmium::unsigned_object_id_type justine::robocar::Traffic::node()
{
  shm_map_Type::iterator iter=shm_map_->begin();
  std::advance(iter, std::rand() % shm_map_->size());

  return iter->first;
}

void justine::robocar::Traffic::UpdateTraffic()
{
  CheckIfCaught();

  StepCars();
}

void justine::robocar::Traffic::StepCars()
{
  std::lock_guard<std::mutex> lock(cars_mutex);

  *logfile_stream_ << running_time_elapsed_
                   << " "
                   << running_time_minutes_
                   << " "
                   << cars.size() << std::endl;

  for(auto car:cars)
  {
    car->step();

    *logfile_stream_ << *car
                     <<  " " << std::endl;
  }
}

void justine::robocar::Traffic::CheckIfCaught(void)
{
  for(auto cop_car:m_cop_cars)
  {
    double lon1 {0.0}, lat1 {0.0};
    toGPS(cop_car->from(), cop_car->to() , cop_car->get_step(), &lon1, &lat1);

    double lon2 {0.0}, lat2 {0.0};
    for(auto smart_car:m_smart_cars) //gangsters?
    {
      if(smart_car->get_type() == CarType::GANGSTER)
      {
        toGPS(smart_car->from(), smart_car->to() , smart_car->get_step(), &lon2, &lat2);
        double d = Distance(lon1, lat1, lon2, lat2);

        if(d < catch_distance_)
        {
          cop_car->GangsterCaught();
          smart_car->set_type(CarType::CAUGHT);

          this->num_gangsters_ -= 1;
        }
      }
    } // for - smart cars
  } // for -cop cars
}

size_t justine::robocar::Traffic::nedges(osmium::unsigned_object_id_type from) const
{
  shm_map_Type::iterator iter=shm_map_->find(from);
  return iter->second.m_alist.size();
}

osmium::unsigned_object_id_type justine::robocar::Traffic::alist(osmium::unsigned_object_id_type from, int to) const
{
  shm_map_Type::iterator iter=shm_map_->find(from);
  return iter->second.m_alist[to];
}

int justine::robocar::Traffic::alist_inv(osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to) const
{
  shm_map_Type::iterator iter=shm_map_->find(from);

  int ret = -1;

  for(uint_vector::iterator noderefi = iter->second.m_alist.begin();
        noderefi!=iter->second.m_alist.end();
        ++noderefi)
  {
    if(to == *noderefi)
    {
      ret = std::distance(iter->second.m_alist.begin(), noderefi);
      break;
    }
  }

  return ret;
}

osmium::unsigned_object_id_type justine::robocar::Traffic::salist(osmium::unsigned_object_id_type from, int to) const
{
  shm_map_Type::iterator iter=shm_map_->find(from);
  return iter->second.m_salist[to];
}

void justine::robocar::Traffic::set_salist(osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value)
{
  shm_map_Type::iterator iter=shm_map_->find(from);
  iter->second.m_salist[to] = value;
}

osmium::unsigned_object_id_type justine::robocar::Traffic::palist(osmium::unsigned_object_id_type from, int to) const
{
  shm_map_Type::iterator iter=shm_map_->find(from);
  return iter->second.m_palist[to];
}

bool justine::robocar::Traffic::hasNode(osmium::unsigned_object_id_type node)
{
  shm_map_Type::iterator iter=shm_map_->find(node);
  return !(iter == shm_map_->end());
}

std::ostream & justine::robocar::operator<<(std::ostream & os, Traffic & t)
{
  os << t.running_time_elapsed_ <<
     " " <<
     t.shm_map_->size()
     << std::endl;

  for(shm_map_Type::iterator iter=t.shm_map_->begin();
        iter!=t.shm_map_->end(); ++iter)
  {
    os  << iter->first
        << " "
        << iter->second.lon
        << " "
        << iter->second.lat
        << " "
        << iter->second.m_alist.size()
        << " ";

    for(auto noderef : iter->second.m_alist)
    {
      os  << noderef
          << " ";
    }

    for(auto noderef : iter->second.m_salist)
    {
      os  << noderef
          << " ";
    }

    for(auto noderef : iter->second.m_palist)
    {
      os  << noderef
          << " ";
    }

    os << std::endl;
  }

  return os;
}

justine::robocar::TrafficType
justine::robocar::Traffic::get_type() const
{
  return traffic_type_;
}

int justine::robocar::Traffic::get_time() const
{
  return running_time_elapsed_;
}

int justine::robocar::Traffic::addSmartCar(
      justine::robocar::CarType type,
      bool is_guided,
      char *team_name) // nullptr if gangster
{
  int id = 0;

  do
  {
    id = std::rand();
  }
  while(m_smart_cars_map.find(id) != m_smart_cars_map.end());

  std::shared_ptr<SmartCar> car;

  if (type == CarType::GANGSTER)
  {
    car = std::make_shared<SmartCar>(*this, CarType::GANGSTER, is_guided, id);

    m_smart_cars.push_back(car);
  }
  else if (type == CarType::POLICE)
  {
    car = std::make_shared<CopCar>(*this, is_guided, team_name, id);

    // maybe dynamic_pointer_cast should be used, but in this situation
    // we can rely on the static version too and get rid of the
    // runtime overhead of the dynamic cast
    m_cop_cars.push_back(std::static_pointer_cast<CopCar>(car));
  }

  cars.push_back(car);

  m_smart_cars_map[id] = car;

  car->init();

  return id;
}

// returns the msg length
int justine::robocar::Traffic::InitCmdHandler(CarLexer &car_lexer, char *buffer)
{
  std::lock_guard<std::mutex> lock(cars_mutex);

  int msg_length = 0;

  int num_cars_to_initialize = car_lexer.get_num();

  int car_id;

  char role = car_lexer.get_role();

  if (role == 'g')
    num_gangsters_ += num_cars_to_initialize;

  for(int i = 0; i < num_cars_to_initialize; ++i)
  {
    if (role == 'c')
    {
      car_id = addSmartCar(CarType::POLICE, car_lexer.get_guided(), car_lexer.get_name());
    }
    else
    {
      car_id = addSmartCar(CarType::GANGSTER, car_lexer.get_guided(), nullptr);
    }

    if(!car_lexer.get_errnumber())
    {
      msg_length += std::sprintf(buffer + msg_length,
                                 "<OK %d %d/%d %c>",
                                 car_id,
                                 num_cars_to_initialize,
                                 i + 1,
                                 car_lexer.get_role());
    }
    else
    {
      msg_length += std::sprintf(buffer + msg_length,
                                 "<WARN %d %d/%d %c>",
                                 car_id,
                                 num_cars_to_initialize,
                                 i + 1,
                                 car_lexer.get_role());
    }
  }

  return msg_length;
}

// ID REQUIRED
int justine::robocar::Traffic::RouteCmdHandler(CarLexer &car_lexer, char *buffer)
{
  int car_id = car_lexer.get_id();

  auto iterator = m_smart_cars_map.find(car_id);

  if (iterator == m_smart_cars_map.end())
    return std::sprintf(buffer, "<ERR unknown car id>");

  std::shared_ptr<SmartCar> car = iterator->second;

  if(car->set_route(car_lexer.get_route()))
      return std::sprintf(buffer, "<OK %d>", car_id);
  else
      return std::sprintf(buffer, "<ERR bad routing vector>");
}

// ID REQUIRED
int justine::robocar::Traffic::CarCmdHandler(CarLexer &car_lexer, char *buffer)
{
  int car_id = car_lexer.get_id();

  auto iterator = m_smart_cars_map.find(car_id);

  if (iterator == m_smart_cars_map.end())
    return std::sprintf(buffer, "<ERR unknown car id>");

  std::shared_ptr<SmartCar> car = iterator->second;

  return std::sprintf(buffer,
                      "<OK %d %lu %lu %lu>",
                      car_id,
                      car->from(),
                      car->to_node(),
                      car->get_step());
}

// ID REQUIRED
int justine::robocar::Traffic::GangsterCmdHandler(CarLexer &car_lexer, char *buffer)
{
  std::lock_guard<std::mutex> lock(cars_mutex);

  int msg_length = 0;

  int car_id = car_lexer.get_id();

  auto iterator = m_smart_cars_map.find(car_id);

  if (iterator == m_smart_cars_map.end())
    return std::sprintf(buffer, "<ERR unknown car id>");

  if (num_gangsters_ == 0)
    return std::sprintf(buffer, "<WARN there is no gangsters>");

  for(auto car:m_smart_cars)
  {
    if(car->get_type() == CarType::GANGSTER)
    {
      msg_length += std::sprintf(buffer + msg_length,
                                 "<OK %d %lu %lu %lu>",
                                 car->get_id(),
                                 car->from(),
                                 car->to_node(),
                                 car->get_step());

      if(msg_length > justine::robocar::kMaxBufferLen - 512)
      {
        msg_length += std::sprintf(buffer + msg_length,
                        "<WARN too many gangsters to send through this implementation...>");
        break;
      }
    }
  }

  return msg_length;
}

// ID REQUIRED
int justine::robocar::Traffic::StatCmdHandler(CarLexer &car_lexer, char *buffer)
{
  std::lock_guard<std::mutex> lock(cars_mutex);

  int msg_length = 0;

  int car_id = car_lexer.get_id();

  auto iterator = m_smart_cars_map.find(car_id);

  if (iterator == m_smart_cars_map.end())
    return std::sprintf(buffer, "<ERR unknown car id>");

  if (m_cop_cars.size() == 0)
    return std::sprintf(buffer, "<WARN there is no cops>");

  for(auto car:m_cop_cars)
  {
    msg_length += std::sprintf(buffer + msg_length,
                               "<OK %d %lu %lu %lu %d>",
                               car_id,
                               car->from(),
                               car->to_node(),
                               car->get_step(),
                               car->get_num_gangsters_caught());

    if(msg_length > justine::robocar::kMaxBufferLen - 512)
    {
      msg_length += std::sprintf(buffer + msg_length,
                                 "<WARN too many cops to send through this implementation...>");
      break;
    }
  }

  return msg_length;
}

// ID REQUIRED
int justine::robocar::Traffic::PosCmdHandler(CarLexer &car_lexer, char *buffer)
{
  int car_id = car_lexer.get_id();

  auto iterator = m_smart_cars_map.find(car_id);

  if (iterator == m_smart_cars_map.end())
    return std::sprintf(buffer, "<ERR unknown car id>");

  std::shared_ptr<SmartCar> car = iterator->second;

  if(car->set_fromto(car_lexer.get_from(), car_lexer.get_to()))
    return std::sprintf(buffer, "<OK %d>", car_lexer.get_id());
  else
    return std::sprintf(buffer, "<ERR cannot set>");
}

void justine::robocar::Traffic::CommandListener(boost::asio::ip::tcp::socket client_socket)
{
  char buffer[justine::robocar::kMaxBufferLen]; // TODO buffered write...

  try
  {
    for(;;)
    {
      CarLexer car_lexer;

      boost::system::error_code error_code;
      size_t length = client_socket.read_some(boost::asio::buffer(buffer), error_code);

      if(error_code == boost::asio::error::eof)
      {
        break;
      }
      else if(error_code)
      {
        throw boost::system::system_error(error_code);
      }

      std::istringstream pbuffer(buffer);

      car_lexer.switch_streams(&pbuffer);
      car_lexer.yylex();

      length = 0;

      int resp_code = car_lexer.get_errnumber();
      int num = car_lexer.get_num();
      int id {0};

      int command_id = car_lexer.get_cmd();

      if(car_lexer.get_cmd() == 0) // DISP handler
      {
        for(;;)
        {
          std::vector<std::shared_ptr<Car>> cars_copy;
          {
            std::lock_guard<std::mutex> lock(cars_mutex);
            cars_copy = cars;
          }

          std::stringstream ss;

          ss  <<
              running_time_elapsed_ <<
              " " <<
              running_time_minutes_ <<
              " " <<
              cars_copy.size() << std::endl;

          for(auto car:cars_copy)
          {
            car->step();

            ss << *car
               <<  " " << std::endl;

          }

          boost::asio::write(client_socket, boost::asio::buffer(buffer, length));

          length = std::sprintf(buffer,
                                  "%s", ss.str().c_str());

          boost::asio::write(client_socket, boost::asio::buffer(buffer, length));

          std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
        }

      }
      else if(car_lexer.get_cmd() < 100) // INIT Handler
      {
        length = InitCmdHandler(car_lexer, buffer);
      } // cmd 100
      else if(car_lexer.get_cmd() == 101) // ROUTE handler
      {
        length = RouteCmdHandler(car_lexer, buffer);
      }
      else if(car_lexer.get_cmd() == 1001) // CAR handler
      {
        length = CarCmdHandler(car_lexer, buffer);
      }
      else if(car_lexer.get_cmd() == 1002) // GANGSTERS handler
      {
        length = GangsterCmdHandler(car_lexer, buffer);
      }
      else if(car_lexer.get_cmd() == 1003) // STAT handler
      {
        length = StatCmdHandler(car_lexer, buffer);
      }
      else if(car_lexer.get_cmd() == 10001) // POS handler
      {
        length = PosCmdHandler(car_lexer, buffer);
      }
      else
        length = std::sprintf(buffer, "<ERR unknown proto command>");

      boost::asio::write(client_socket, boost::asio::buffer(buffer, length));
    }
  }
  catch(std::exception& e)
  {
    std::cerr << "Ooops: " << e.what() << std::endl;
  }
}

void justine::robocar::Traffic::StartServer(void)
{
  boost::asio::ip::tcp::acceptor acceptor(
      io_service_,
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_));

  for(;;)
  {
    boost::asio::ip::tcp::socket socket(io_service_);
    acceptor.accept(socket);

    std::thread t { &justine::robocar::Traffic::CommandListener, this, std::move(socket) };
    t.detach();
  }
}

double justine::robocar::Traffic::Distance(
  osmium::unsigned_object_id_type n1,
  osmium::unsigned_object_id_type n2) const
{

  shm_map_Type::iterator iter1=shm_map_->find(n1);
  shm_map_Type::iterator iter2=shm_map_->find(n2);

  osmium::geom::Coordinates c1 {iter1->second.lon/10000000.0, iter1->second.lat/10000000.0};
  osmium::geom::Coordinates c2 {iter2->second.lon/10000000.0, iter2->second.lat/10000000.0};

  return osmium::geom::haversine::distance(c1, c2);
}

double justine::robocar::Traffic::Distance(
  double lon1, double lat1, double lon2, double lat2) const
{
  osmium::geom::Coordinates c1 {lon1, lat1};
  osmium::geom::Coordinates c2 {lon2, lat2};

  return osmium::geom::haversine::distance(c1, c2);
}

void justine::robocar::Traffic::toGPS(
  osmium::unsigned_object_id_type from,
  osmium::unsigned_object_id_type to,
  osmium::unsigned_object_id_type step, double *lo, double *la) const
{
  shm_map_Type::iterator iter1=shm_map_->find(from);
  double lon1 {iter1->second.lon/10000000.0}, lat1 {iter1->second.lat/10000000.0};

  shm_map_Type::iterator iter2=shm_map_->find(alist(from, to));
  double lon2 {iter2->second.lon/10000000.0}, lat2 {iter2->second.lat/10000000.0};

  osmium::unsigned_object_id_type maxstep = palist(from, to);

  if(maxstep == 0)
  {
    maxstep = 1;
  }

  lat1 += step *((lat2 - lat1) / maxstep);
  lon1 += step *((lon2 - lon1) / maxstep);

  *lo = lon1;
  *la = lat1;
}

osmium::unsigned_object_id_type justine::robocar::Traffic::naive_nearest_gangster(
  osmium::unsigned_object_id_type from,
  osmium::unsigned_object_id_type to,
  osmium::unsigned_object_id_type step)
{
  osmium::unsigned_object_id_type ret = from;

  double lon1 {0.0}, lat1 {0.0};
  toGPS(from, to, step, &lon1, &lat1);

  double maxd = std::numeric_limits<double>::max();
  double lon2 {0.0}, lat2 {0.0};

  for(auto car:m_smart_cars)
  {
    if(car->get_type() == CarType::GANGSTER)
    {
      toGPS(car->from(), car->to() , car->get_step(), &lon2, &lat2);

      double d = Distance(lon1, lat1, lon2, lat2);

      if(d < maxd)
      {
        maxd = d;
        ret = car->to_node();
      }
    }

  }

  return ret;
}

osmium::unsigned_object_id_type justine::robocar::Traffic::naive_node_for_nearest_gangster(
  osmium::unsigned_object_id_type from,
  osmium::unsigned_object_id_type to,
  osmium::unsigned_object_id_type step)
{
  osmium::unsigned_object_id_type ret = 0;

  osmium::unsigned_object_id_type car = naive_nearest_gangster(from, to , step);

  shm_map_Type::iterator iter=shm_map_->find(from);

  double maxd = std::numeric_limits<double>::max();

  for(uint_vector::iterator noderefi = iter->second.m_alist.begin();
        noderefi!=iter->second.m_alist.end(); ++noderefi)
  {
    double d = Distance(car, *noderefi);

    if(d < maxd)
    {
      maxd = d;
      ret = std::distance(iter->second.m_alist.begin(), noderefi);
    }
  }

  return ret;
}
