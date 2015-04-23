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
  std::map <std::string, int> res;
  for(auto c:m_cop_cars)
  {
    res[c->get_name()] += c->get_num_captured_gangsters();
  }

  std::ostringstream ss;

  for(auto r: res)
    ss << r.first << " " << res[r.first] << " ";

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
          cop_car->captured_gangster();
          smart_car->set_type(CarType::CAUGHT);
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

int justine::robocar::Traffic::addCop(CarLexer& cl)
{
  std::shared_ptr<CopCar> c;

  int id {0};
  do
  {
    id = std::rand();
  }
  while(m_smart_cars_map.find(id) != m_smart_cars_map.end());

  c = std::make_shared<CopCar>(*this, cl.get_guided(), cl.get_name(), id);

  //TODO majd ráér később inicializálni, hogy ne lassítsa a szimulációt
  c->init();

  cars.push_back(c);
  m_cop_cars.push_back(c);

  m_smart_cars_map[id] = c;

  return id;
}

int justine::robocar::Traffic::addGangster(CarLexer& cl)
{
  std::shared_ptr<SmartCar> c;

  int id {0};
  do
  {
    id = std::rand();
  }
  while(m_smart_cars_map.find(id) != m_smart_cars_map.end());

  c = std::make_shared<SmartCar>(*this, CarType::GANGSTER, cl.get_guided(), id);

  //TODO majd ráér később inicializálni, hogy ne lassítsa a szimulációt
  c->init();

  cars.push_back(c);
  m_smart_cars.push_back(c);

  m_smart_cars_map[id] = c;

  return id;
}


void justine::robocar::Traffic::cmd_session(boost::asio::ip::tcp::socket client_socket)
{
  const int network_buffer_size = 524288;
  char data[network_buffer_size]; // TODO buffered write...

  try
  {
    for(;;)
    {
      CarLexer cl;

      boost::system::error_code err;
      size_t length = client_socket.read_some(boost::asio::buffer(data), err);

      if(err == boost::asio::error::eof)
      {
        break;
      }
      else if(err)
      {
        throw boost::system::system_error(err);
      }

      std::istringstream  pdata(data);

      cl.switch_streams(&pdata);
      cl.yylex();

      length = 0;

      int resp_code = cl.get_errnumber();
      int num = cl.get_num();
      int id {0};

      if(cl.get_cmd() == 0)
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

          boost::asio::write(client_socket, boost::asio::buffer(data, length));

          length = std::sprintf(data,
                                  "%s", ss.str().c_str());

          boost::asio::write(client_socket, boost::asio::buffer(data, length));

          std::this_thread::sleep_for(std::chrono::milliseconds(200));

        }

      }
      else if(cl.get_cmd() <100)
      {
        std::lock_guard<std::mutex> lock(cars_mutex);

        for(int i {0}; i<cl.get_num(); ++i)
        {
          if(cl.get_role() =='c')
            id = addCop(cl);
          else
            id = addGangster(cl);

          if(!resp_code)
            length += std::sprintf(data+length,
                                     "<OK %d %d/%d %c>", id, num,
                                     i+1, cl.get_role());
          else
            length += std::sprintf(data+length,
                                     "<WARN %d %d/%d %c>", id, num,
                                     i+1, cl.get_role());
        }
      } // cmd 100
      else if(cl.get_cmd() == 101)
      {
        if(m_smart_cars_map.find(cl.get_id()) != m_smart_cars_map.end())
        {
          std::shared_ptr<SmartCar> c = m_smart_cars_map[cl.get_id()];

          if(c->set_route(cl.get_route()))
            length += std::sprintf(data+length, "<OK %d>", cl.get_id());
          else
            length += std::sprintf(data+length, "<ERR bad routing vector>");
        }
        else
            length += std::sprintf(data+length, "<ERR unknown car id>");

      }
      else if(cl.get_cmd() == 1001)
      {
        if(m_smart_cars_map.find(cl.get_id()) != m_smart_cars_map.end())
        {
          std::shared_ptr<SmartCar> c = m_smart_cars_map[cl.get_id()];

          length += std::sprintf(data+length,
                                   "<OK %d %lu %lu %lu>", cl.get_id(), c->from(),
                                   c->to_node(), c->get_step());
        }
        else
          length += std::sprintf(data+length, "<ERR unknown car id>");
      }
      else if(cl.get_cmd() == 1002)
      {
        std::lock_guard<std::mutex> lock(cars_mutex);

        if(m_smart_cars_map.find(cl.get_id()) != m_smart_cars_map.end())
        {
          bool hasGangsters {false};

          for(auto c:m_smart_cars)
          {
            if(c->get_type() == CarType::GANGSTER)
            {
              length += std::sprintf(data+length,
                                       "<OK %d %lu %lu %lu>", c->get_id(), c->from(),
                                       c->to_node(), c->get_step());

              if(length > network_buffer_size - 512)
              {
                length += std::sprintf(data+length,
                            "<WARN too many gangsters to send through this implementation...>");
                break;
              }

              hasGangsters = true;
            }
          }

          if(!hasGangsters)
            length += std::sprintf(data+length,
                                    "<WARN there is no gangsters>");

        }
        else
          length += std::sprintf(data+length,
                                  "<ERR unknown car id>");
      }
      else if(cl.get_cmd() == 1003)
      {
        std::lock_guard<std::mutex> lock(cars_mutex);

        if(m_smart_cars_map.find(cl.get_id()) != m_smart_cars_map.end())
        {
          bool hasCops {false};

          for(auto c:m_cop_cars)
          {
            length += std::sprintf(data+length,
                                     "<OK %d %lu %lu %lu %d>", cl.get_id(), c->from(),
                                     c->to_node(), c->get_step(),
                                     c->get_num_captured_gangsters());

            if(length > network_buffer_size - 512)
            {
              length += std::sprintf(data+length,
                                      "<WARN too many cops to send through this implementation...>");
              break;
            }

            hasCops = true;
          }

          if(!hasCops)
            length += std::sprintf(data+length,
                                     "<WARN there is no cops>");

        }
        else
          length += std::sprintf(data+length,
                                   "<ERR unknown car id>");

      }
      else if(cl.get_cmd() == 10001)
      {
        if(m_smart_cars_map.find(cl.get_id()) != m_smart_cars_map.end())
        {
          std::shared_ptr<SmartCar> c = m_smart_cars_map[cl.get_id()];

          if(c->set_fromto(cl.get_from(), cl.get_to()))
            length += std::sprintf(data+length, "<OK %d>", cl.get_id());
          else
            length += std::sprintf(data+length, "<ERR cannot set>");

        }
        else
          length += std::sprintf(data+length,
                                   "<ERR unknown car id>");
      }
      else
        length += std::sprintf(data+length,
                                 "<ERR unknown proto command>");

      boost::asio::write(client_socket, boost::asio::buffer(data, length));

    }
  }
  catch(std::exception& e)
  {
    std::cerr << "Ooops: " << e.what() << std::endl;
  }
}

void justine::robocar::Traffic::start_server(
  boost::asio::io_service& io_service,
  unsigned short port)
{
  boost::asio::ip::tcp::acceptor acceptor(
      io_service,
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

  for(;;)
  {
    boost::asio::ip::tcp::socket socket(io_service);
    acceptor.accept(socket);

    std::thread t {&justine::robocar::Traffic::cmd_session, this, std::move(socket) };
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
