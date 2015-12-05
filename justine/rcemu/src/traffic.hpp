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
#include "immovableObject.hpp"

#include <cstdlib>
#include <iterator>

#include <robocar.pb.h>

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

constexpr int kMaxBufferLen = 524288;

enum ClientCommand
{
  DISP = 0,
  AUTH = 1,
  INIT = 2,
  CAR = 3,
  GANGSTERS = 4,
  ROUTE = 5,
  POS = 6,
  STAT = 7,
  STOP = 8
};

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
          int port,
          double catch_distance = 15.5,
          TrafficType traffic_type = TrafficType::NORMAL,
          int delay = 200,
          int minutes = 10,
          bool verbose_mode = false,
          bool full = false):
            num_cars_(num_cars),
            num_gangsters_(0),
            port_(port),
            catch_distance_(catch_distance),
            traffic_type_(traffic_type),
            delay_(delay),
            running_time_minutes_(minutes),
            verbose_mode_(verbose_mode),
            full_logging(full)
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

    bus_stop_map_ =
      shm_segment_->find<bus_stop_map_Type>("BusStops").first;


    bus_stop_map2_ =
      shm_segment_->find<bus_stop_map2_Type>("BusStops2").first;

    bus_way_vector_ =
      shm_segment_->find<shm_bus_way_Type>("BusWays").first;

    running_time_elapsed_ = 0;

    // infinite mode
    if (running_time_minutes_ == -1)
    {
      running_time_allowed_ = (std::numeric_limits<int>::max() / 60 / 1000) - 1;
    }
    else
    {
      running_time_allowed_ = running_time_minutes_ * 60 * 1000;
    }

    InitializePedestrians();

    InitializeRoutineCars();

    InitializeBuses();

    OpenLogStream();

    is_running_ = true;

    m_cv.notify_one();

    std::cout << "The traffic server is ready." << std::endl;
  }

  ~Traffic()
  {
    is_running_ = false;

    m_thread.join();
    shm_segment_->destroy<shm_map_Type>("JustineMap");
    shm_segment_->destroy<bus_stop_map_Type>("BusStops");
    shm_segment_->destroy<bus_stop_map2_Type>("BusStops2");
    shm_segment_->destroy<shm_bus_way_Type>("BusWays");

    delete shm_segment_;
  }

  void OpenLogStream(void);

  void CloseLogStream(void);

  void InitializeRoutineCars(void);

  void InitializePedestrians(void);

  void InitializeBuses(void);

  void SimulationLoop(void);
  void Test(void);

  std::string get_title(std::string name);

  virtual osmium::unsigned_object_id_type node();

  virtual void UpdateTraffic();

  void StepCars();

  inline void CheckIfCaught(void);

  size_t nedges(osmium::unsigned_object_id_type from) const;

  osmium::unsigned_object_id_type alist(osmium::unsigned_object_id_type from, int to) const;

  int alist_inv(osmium::unsigned_object_id_type from, osmium::unsigned_object_id_type to) const;

  osmium::unsigned_object_id_type salist(osmium::unsigned_object_id_type from, int to) const;

  void set_salist(osmium::unsigned_object_id_type from, int to , osmium::unsigned_object_id_type value);

  osmium::unsigned_object_id_type palist(osmium::unsigned_object_id_type from, int to) const;

  bool hasNode(osmium::unsigned_object_id_type node);

  void StartServer(void);

  int InitCmdHandler(CarLexer &car_lexer, char *buffer);

  int RouteCmdHandler(CarLexer &car_lexer, char *buffer);

  int CarCmdHandler(CarLexer &car_lexer, char *buffer);

  int GangstersCmdHandler(CarLexer &car_lexer, char *buffer);

  int StatCmdHandler(CarLexer &car_lexer, char *buffer);

  int PosCmdHandler(CarLexer &car_lexer, char *buffer);

  int AuthCmdHandler(CarLexer &car_lexer, char *buffer);

  int StopCmdHandler(CarLexer &car_lexer, char *buffer);

  void DispCmdHandler(boost::asio::ip::tcp::socket &socket);

  inline bool IsAuthenticated(CarLexer &car_lexer);

  void CommandListener(boost::asio::ip::tcp::socket socket);

  int addSmartCar(
        justine::robocar::CarType type,
        bool is_guided,
        char *team_name);

  osmium::unsigned_object_id_type naive_node_for_nearest_gangster(
      osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step);

  double Distance(osmium::unsigned_object_id_type n1,
               osmium::unsigned_object_id_type n2) const;
  double Distance2(osmium::unsigned_object_id_type n1,
               osmium::unsigned_object_id_type n2) const;

  double Distance(double lon1, double lat1, double lon2, double lat2) const;


  void toGPS(osmium::unsigned_object_id_type from,
               osmium::unsigned_object_id_type to,
               osmium::unsigned_object_id_type step,
               double *lo, double *la) const;

  osmium::unsigned_object_id_type naive_nearest_gangster(
      osmium::unsigned_object_id_type from,
      osmium::unsigned_object_id_type to,
      osmium::unsigned_object_id_type step);

  TrafficType get_type() const;

  int get_time() const;
protected:
  boost::interprocess::managed_shared_memory *shm_segment_;
  boost::interprocess::offset_ptr<bus_stop_map_Type> bus_stop_map_;
  boost::interprocess::offset_ptr<bus_stop_map2_Type> bus_stop_map2_;
  boost::interprocess::offset_ptr<shm_map_Type> shm_map_;
  boost::interprocess::offset_ptr<shm_bus_way_Type> bus_way_vector_;
  bool is_running_;


private:
  int num_cars_;
  int num_gangsters_;
  int port_;


protected:
  double catch_distance_;
private:
  TrafficType traffic_type_;
protected:
  int delay_;
private:
  int running_time_minutes_;


  bool verbose_mode_;
  bool full_logging;
  int running_time_allowed_;  int running_time_elapsed_;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::thread m_thread { &Traffic::SimulationLoop, this };

  std::vector<std::shared_ptr<Car>> cars;
  std::vector<std::shared_ptr<SmartCar>> m_smart_cars;
  std::vector<std::shared_ptr<CopCar>> m_cop_cars;

  std::vector<std::shared_ptr<ImmovableObject> > immovableObjects;

  std::map<int, std::shared_ptr<SmartCar>> m_smart_cars_map;
  std::map<int, char*> authenticated_teams_;

  std::mutex cars_mutex;

  boost::asio::io_service io_service_;



  std::string   logfile_name_;
  std::fstream *logfile_stream_;

  friend std::ostream & operator<<(std::ostream & os, Traffic &);
};

}
} // justine::robocar::

#endif // ROBOCAR_TRAFFIC_HPP
