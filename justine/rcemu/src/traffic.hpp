#ifndef ROBOCAR_TRAFFIC_HPP
#define ROBOCAR_TRAFFIC_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file smartcity.hpp
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

#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <carlexer.hpp>
#include <smartcity.hpp>
#include <car.hpp>

#include <cstdlib>
#include <iterator>

#include <boost/asio.hpp>

#include <limits>
#include <memory>

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

namespace justine
{

namespace robocar
{

enum class TrafficType: unsigned int
{
  NORMAL = 0, ANT, ANT_RND, ANT_RERND, ANT_MRERND
};

class Traffic
{
  // I find the following initialization and so on very disgusting
  // Usage of a configuration file should be considered
public:
  Traffic(int num_cars,
          const char *shm_segment_name,
          double catch_distance = 15.5,
          TrafficType traffic_type = TrafficType::NORMAL,
          int delay = 200,
          int minutes = 10,
          bool verbose_mode = false):
            num_cars_(num_cars),
            catch_distance_(catch_distance),
            traffic_type_(traffic_type),
            delay_(delay),
            running_time_minutes_(minutes),
            verbose_mode_(verbose_mode)
  {
    #ifdef DEBUG
    std::cout << "Attaching shared memory segment called "
              << shm_segment_name
              << "... " << std::endl;
    #endif

    shm_segment_ = new boost::interprocess::managed_shared_memory(
      boost::interprocess::open_only,
      shm_segment_name);

    shm_map_ =
      shm_segment_->find<shm_map_Type>("JustineMap").first;

    running_time_allowed_ = running_time_minutes_ * 60 * 1000;

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

    boost::posix_time::ptime now =
      boost::posix_time::second_clock::universal_time();

    logfile = boost::posix_time::to_simple_string(now);
    logFile = new std::fstream(logfile.c_str() , std::ios_base::out);

    m_cv.notify_one();

    std::cout << "The traffic server is ready." << std::endl;
  }

  ~Traffic()
  {
    is_running_ = false;
    m_thread.join();
    shm_segment_->destroy<shm_map_Type>("JustineMap");

    delete shm_segment_;
  }

  void SimulationLoop()
  {

  }

  void processes()
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
        traffic_run();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_));
      }
    }

    std::cout << "The traffic simulation is over." << std::endl;

    for(auto c:m_cop_cars)
      *logFile  << *c << std::endl;

    logFile->close();

    boost::filesystem::rename(
      boost::filesystem::path(logfile),
      boost::filesystem::path(get_title(logfile))
     );
  }

  std::string get_title(std::string name)
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

  osmium::unsigned_object_id_type virtual node()
  {
    shm_map_Type::iterator iter=shm_map_->begin();
    std::advance(iter, std::rand() % shm_map_->size());

    return iter->first;
  }

  virtual void traffic_run()
  {
    pursuit();

    steps();
  }

  void steps()
  {
    std::lock_guard<std::mutex> lock(cars_mutex);

    *logFile <<
             running_time_elapsed_ <<
             " "    <<
             running_time_minutes_  <<
             " "    <<
             cars.size() << std::endl;

    for(auto car:cars)
    {
      car->step();

      *logFile << *car
               <<  " " << std::endl;
    }
  }

  inline void pursuit(void)
  {
    for(auto car1:m_cop_cars)
    {
      double lon1 {0.0}, lat1 {0.0};
      toGPS(car1->from(), car1->to() , car1->get_step(), &lon1, &lat1);

      double lon2 {0.0}, lat2 {0.0};
      for(auto car:m_smart_cars)
      {
        if(car->get_type() == CarType::GANGSTER)
        {
          toGPS(car->from(), car->to() , car->get_step(), &lon2, &lat2);
          double d = dst(lon1, lat1, lon2, lat2);

          if(d < catch_distance_)
          {
            car1->captured_gangster();
            car->set_type(CarType::CAUGHT);
          }
        }
      } // for - smart cars
    } // for -cop cars
  }

  size_t nedges(osmium::unsigned_object_id_type from) const
  {
    shm_map_Type::iterator iter=shm_map_->find(from);
    return iter->second.m_alist.size();
  }

  osmium::unsigned_object_id_type alist(osmium::unsigned_object_id_type from, int to) const
  {
    shm_map_Type::iterator iter=shm_map_->find(from);
    return iter->second.m_alist[to];
  }

  int alist_inv(osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to) const
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

  osmium::unsigned_object_id_type salist(osmium::unsigned_object_id_type from, int to) const
  {
    shm_map_Type::iterator iter=shm_map_->find(from);
    return iter->second.m_salist[to];
  }

  void set_salist(osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value)
  {
    shm_map_Type::iterator iter=shm_map_->find(from);
    iter->second.m_salist[to] = value;
  }

  osmium::unsigned_object_id_type palist(osmium::unsigned_object_id_type from, int to) const
  {
    shm_map_Type::iterator iter=shm_map_->find(from);
    return iter->second.m_palist[to];
  }

  bool hasNode(osmium::unsigned_object_id_type node)
  {
    shm_map_Type::iterator iter=shm_map_->find(node);
    return !(iter == shm_map_->end());
  }

  void start_server(boost::asio::io_service& io_service, unsigned short port);

  void cmd_session(boost::asio::ip::tcp::socket sock);

  friend std::ostream & operator<<(std::ostream & os, Traffic & t)
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

  osmium::unsigned_object_id_type naive_node_for_nearest_gangster(
      osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step);

  double dst(osmium::unsigned_object_id_type n1,
               osmium::unsigned_object_id_type n2) const;

  double dst(double lon1, double lat1, double lon2, double lat2) const;


  void toGPS(osmium::unsigned_object_id_type from,
               osmium::unsigned_object_id_type to,
               osmium::unsigned_object_id_type step,
               double *lo, double *la) const;

  osmium::unsigned_object_id_type naive_nearest_gangster(
      osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step);

  TrafficType get_type() const
  {
    return traffic_type_;
  }

  int get_time() const
  {
    return running_time_elapsed_;
  }

protected:
  boost::interprocess::managed_shared_memory *shm_segment_;
  boost::interprocess::offset_ptr<shm_map_Type> shm_map_;

  int delay_;
  bool is_running_;
  double catch_distance_;

private:
  int addCop(CarLexer& cl);
  int addGangster(CarLexer& cl);

  int num_cars_;
  int running_time_elapsed_;
  int running_time_minutes_;
  int running_time_allowed_;

  bool verbose_mode_;

  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_thread {&Traffic::processes, this};

  std::vector<std::shared_ptr<Car>> cars;
  std::vector<std::shared_ptr<SmartCar>> m_smart_cars;
  std::vector<std::shared_ptr<CopCar>> m_cop_cars;

  std::map<int, std::shared_ptr<SmartCar>> m_smart_cars_map;

  std::mutex cars_mutex;

  TrafficType traffic_type_ {TrafficType::NORMAL};

  std::fstream* logFile;
  std::string logfile;
};

}
} // justine::robocar::

#endif // ROBOCAR_TRAFFIC_HPP
