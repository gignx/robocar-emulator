#ifndef ROBOCAR_SMARTCITY_HPP
#define ROBOCAR_SMARTCITY_HPP

/**
* @brief Justine - this is a rapid prototype for development of Robocar City Emulator
*
* @file smartcity.hpp
* @author  Norbert Bátfai <nbatfai@gmail.com>
* @version 0.0.10
*
* @section LICENSE
*
* Copyright (C) 2014 Norbert Bátfai, batfai.norbert@inf.unideb.hu
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
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

#include <osmreader.hpp>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <exception>
#include <stdexcept>

#include <iomanip>

#include "smartcity_defs.hpp"

namespace justine
{
namespace robocar
{
  class SharedData
  {
  public:
    uint_vector m_alist;
    uint_vector m_salist;
    uint_vector m_palist;

    int lon;
    int lat;

    SharedData ( const void_allocator &void_alloc )
    :  m_alist ( void_alloc ), m_salist ( void_alloc ), m_palist ( void_alloc )
    {}

  };

  struct SharedBusWay
  {
    char_string ref;

    uint_vector nodesTo;
    uint_vector nodesFrom;

    SharedBusWay(std::string r, const void_allocator &void_alloc):
      ref(r.c_str(), void_alloc),
      nodesTo(void_alloc),
      nodesFrom(void_alloc)
    {}
  };

  typedef std::pair<const unsigned int, SharedData> map_pair_Type;
  typedef std::pair<const unsigned int, char_string > bus_stop_Type;

  typedef boost::interprocess::allocator<map_pair_Type, segment_manager_Type> map_pair_Type_allocator;
  typedef boost::interprocess::allocator<bus_stop_Type, segment_manager_Type> bus_stop_Type_allocator;
  typedef boost::interprocess::allocator<SharedBusWay, segment_manager_Type> bus_way_Type_allocator;
  typedef boost::interprocess::map< unsigned int, SharedData, std::less<unsigned int>,
  map_pair_Type_allocator> shm_map_Type;

  typedef boost::interprocess::vector<SharedBusWay, bus_way_Type_allocator> shm_bus_way_Type;

  typedef boost::interprocess::map< unsigned int, char_string, std::less<unsigned int>,
                                    bus_stop_Type_allocator> bus_stop_map_Type;

  class SmartCity
  {
  public:
    SmartCity ( const char * osm_file, const char * shm_segment, const char * map_file ) : SmartCity ( osm_file, shm_segment )
    {
      std::fstream gpsFile ( map_file, std::ios_base::out );

      for ( auto loc: m_waynode_locations )
      {
        gpsFile << loc.first
                << " " << loc.second.lat()
                <<  " " << loc.second.lon() << std::endl;
      }

      for  (auto loc : m_busStopNodesMap)
      {
        gpsFile << loc.first
                << " " << loc.second.lat()
                << " " << loc.second.lon()
                << std::endl;
      }

      gpsFile.close ();
    }

    SmartCity ( const char * osm_file, const char * shm_segment )
    {
      AdjacencyList alist, palist;

      std::size_t estimated_size;

      try
      {
        #ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
        #endif

        OSMReader osm_reader ( osm_file, alist, palist,
          m_waynode_locations,
          m_busWayNodesMap,
          m_way2nodes,
          m_busStopNodesMap,
          m_busstops,
          m_busWayVector);
          estimated_size = 7*3*osm_reader.get_estimated_memory(); //20*3


          #ifdef DEBUG

          for (const auto& obw : m_busWayVector)
          {
            std::cout << obw.ref << std::endl;
            std::cout << "\tTo: " << obw.nodesTo.size()
                      << " From: " << obw.nodesFrom.size()
                      << std::endl;
          }

          std::cout << "m_busWayNodesMap" << std::endl;
          for(std::map<std::string, std::vector<osmium::unsigned_object_id_type>>::iterator it = m_busWayNodesMap.begin(); it != m_busWayNodesMap.end(); ++it)
          {
              std::cout << (*it).first << std::endl;
              std::vector<osmium::unsigned_object_id_type> inVect = (*it).second;
              for (unsigned j=0; j<inVect.size(); j++)
                  std::cout << inVect[j] << " - ";

                  std::cout << std::endl;
                  std::cout << "-------------------------------------------------------------------" << std::endl;
          }

          std::cout << "m_way2nodes" << std::endl;
          for(std::map<osmium::unsigned_object_id_type, std::vector<osmium::unsigned_object_id_type>>::iterator it = m_way2nodes.begin(); it != m_way2nodes.end(); ++it)
          {
              std::cout << (*it).first << std::endl;
              std::vector<osmium::unsigned_object_id_type> inVect = (*it).second;
              for (unsigned j=0; j<inVect.size(); j++)
                  std::cout << inVect[j] << " - ";

                  std::cout << std::endl;
                  std::cout << "-------------------------------------------------------------------" << std::endl;
          }

        #endif

        #ifdef DEBUG

          for  (auto bstp : m_busstops)
          {
            std::cout << "idk.push_back(" << bstp.first << ");" << std::endl;
          }

          for  (auto bstp : m_busstops)
          {
            std::cout << "nevek.push_back(\"" << bstp.second << "\");" << std::endl;
          }

        #endif


        #ifdef DEBUG
        std::cout << " Processing OSM: "
                  << std::chrono::duration_cast<std::chrono::milliseconds> (
                      std::chrono::high_resolution_clock::now() - start ).count()
                  << " ms " << std::endl;
        #endif
      }
      catch ( std::exception &err )
      {
        m_cv.notify_one();

        m_run = false;
        m_thread.join();

        throw;
      }

      google::protobuf::ShutdownProtobufLibrary();

      m_remover = new shm_remove ( shm_segment );

      segment = new boost::interprocess::managed_shared_memory (
        boost::interprocess::create_only,
        shm_segment,
        estimated_size );

      void_allocator  alloc_obj ( segment->get_segment_manager() );

      shm_map_Type* shm_map_n =
      segment->construct<shm_map_Type>
      ( "JustineMap" ) ( std::less<unsigned int>(), alloc_obj );


      bus_stop_map_Type* bus_stop_map_bs =
      segment->construct<bus_stop_map_Type>
      ( "BusStops" ) ( std::less<unsigned int>(), alloc_obj );

      shm_bus_way_Type* bus_way_vector =
      segment->construct<shm_bus_way_Type>
      ( "BusWays" ) (alloc_obj);

      try
      {
        for ( BusWayVector::iterator iter = m_busWayVector.begin();
              iter != m_busWayVector.end(); ++iter)
        {
          SharedBusWay sbw(iter->ref, alloc_obj);

          std::vector<osmium::unsigned_object_id_type> temp;

          for (std::size_t i = 0; i < iter->nodesFrom.size(); ++i)
          {
            // igen, másolatot akarunk
            std::vector<osmium::unsigned_object_id_type> vec = m_way2nodes[iter->nodesFrom[i]];

            if (vec.size() == 0)
            {
              continue;
            }

            if (temp.size() != 0)
            {
              if (i == 1)
              {
                if ((temp[0] == vec[0]) || (temp[0] == vec.back()))
                {
                  std::reverse(temp.begin(), temp.end());
                }
              }

              if (temp.back() != vec[0])
              {
                std::reverse(vec.begin(), vec.end());
              }
            }

            for (std::size_t j = 0; j < vec.size(); ++j)
            {
                temp.push_back(vec[j]);
            }
          }

          if (temp.size() > 0)
          {
            std::unique(temp.begin(), temp.end());

            for (std::size_t i = 0; i < temp.size(); ++i)
            {
              sbw.nodesFrom.push_back(temp[i]);
            }
          }

          temp.clear();


          for (std::size_t i = 0; i < iter->nodesTo.size(); ++i)
          {
            // újabb másolat
            std::vector<osmium::unsigned_object_id_type> vec = m_way2nodes[iter->nodesTo[i]];

            if (vec.size() == 0)
            {
              continue;
            }

            if (temp.size() != 0)
            {
              if (temp.back() != vec[0])
              {
                std::reverse(vec.begin(), vec.end());
              }
            }

            for (std::size_t j = 0; j < vec.size(); ++j)
            {
                temp.push_back(vec[j]);
            }
          }

          if (temp.size() > 0)
          {
            std::unique(temp.begin(), temp.end());

            for (std::size_t i = 0; i < temp.size(); ++i)
            {
              sbw.nodesTo.push_back(temp[i]);
            }
          }

          #ifdef DEBUG
          std::cout << sbw.ref << " | " << sbw.nodesFrom.size()
                    << " - " << sbw.nodesTo.size() << std::endl;
          #endif

          bus_way_vector->push_back(sbw);
        }

        for ( AdjacencyList::iterator iter=alist.begin();
              iter!=alist.end(); ++iter )
        {

          SharedData v ( alloc_obj );

          /*
          v.lon = m_waynode_locations.get ( iter->first ).x();
          v.lat = m_waynode_locations.get ( iter->first ).y();
          */

          v.lon = m_waynode_locations[ iter->first ].x();
          v.lat = m_waynode_locations[ iter->first ].y();

          for ( WayNodesVect::iterator noderefi = iter->second.begin();
                noderefi!= iter->second.end(); ++noderefi )
          {
            v.m_alist.push_back ( *noderefi );
            v.m_salist.push_back ( 0u );
            v.m_palist.push_back (
              palist[iter->first][std::distance ( iter->second.begin(), noderefi )]+1 );
          }

          map_pair_Type p ( iter->first, v );
          shm_map_n->insert ( p );
        }

        for  (auto bstp : m_busstops)
        {
            bus_stop_Type p(bstp.first, char_string(bstp.second.c_str(), alloc_obj));
            bus_stop_map_bs->insert(p);
        }

        #ifdef DEBUG
        std::cout << " alist.size = " << alist.size() << " (deg- >= 1)"<< std::endl;
        std::cout << " SHM/alist.size = " << shm_map_n->size() << std::endl;
        #endif


      }
      catch ( boost::interprocess::bad_alloc e )
      {
        std::cerr << " Out of shared memory..." << std::cerr;
        std::cout << e.what() <<std::endl;

        std::cerr
          << " Shared memory usage: "
          << segment->get_free_memory() /1024.0/1024.0 << " Mbytes "
          << std::setprecision ( 2 )
          << 100.0- ( 100.0*segment->get_free_memory() ) /segment->get_size()
          << "% is free"
          << std::endl;

        m_cv.notify_one();

        m_run = false;
        m_thread.join();

        throw e;
      }

      #ifdef DEBUG
      std::streamsize p = std::cout.precision();

      std::cout
        << " Shared memory usage: "
        << segment->get_free_memory() /1024.0/1024.0 << " Mbytes "
        << std::setprecision ( 2 )
        << 100.0- ( 100.0*segment->get_free_memory() ) /segment->get_size()
        << "% is free"
        << std::setprecision ( p )
        << std::endl;
      #endif

      shm_map = segment->find<shm_map_Type> ( "JustineMap" ).first;
      bus_stop_map = segment->find<bus_stop_map_Type> ("BusStops").first;

      m_cv.notify_one();
    }

    ~SmartCity()
    {
      m_run = false;
      m_thread.join();

      delete segment;
      delete m_remover;
    }

    void processes ( )
    {
      std::unique_lock<std::mutex> lk ( m_mutex );
      m_cv.wait ( lk );

      for ( ; m_run; )
      {
        std::this_thread::sleep_for ( std::chrono::milliseconds ( m_delay ) );
        city_run();
      }
    }

    friend std::ostream & operator<< ( std::ostream & os, SmartCity & t )
    {
      for ( shm_map_Type::iterator iter=t.shm_map->begin();
            iter!=t.shm_map->end(); ++iter )
      {
        std::cout
          << iter->first
          << " "
          << iter->second.lon
          << " "
          << iter->second.lat
          << " "
          << iter->second.m_alist.size()
          << " ";

        for ( auto noderef : iter->second.m_alist )
        {
          std::cout
            << noderef
            << " ";
        }

        for ( auto noderef : iter->second.m_salist )
        {
          std::cout
            << noderef
            << " ";
        }

        for ( auto noderef : iter->second.m_palist )
        {
          std::cout
          << noderef
          << " ";
        }

        std::cout << std::endl;
      }

      return os;
    }

    virtual void city_run()
    {
      // activities that may occur in the city

      // std::cout << *this;

    }

    double busWayLength ( bool verbose );

  protected:
    boost::interprocess::managed_shared_memory *segment;
    boost::interprocess::offset_ptr<shm_map_Type> shm_map;
    boost::interprocess::offset_ptr<bus_stop_map_Type> bus_stop_map;

    int m_delay {5000};
    bool m_run {true};

  private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_thread {&SmartCity::processes, this};

    WaynodeLocations m_waynode_locations;
    WayNodesMap m_busWayNodesMap;
    Way2Nodes m_way2nodes;
    NodesMap m_busStopNodesMap;
    BusStops m_busstops;
    BusWayVector m_busWayVector;

    struct shm_remove
    {
      const char * name;
      shm_remove ( const char * name )
      {
        boost::interprocess::shared_memory_object::remove ( name );
      }
      ~shm_remove()
      {
        boost::interprocess::shared_memory_object::remove ( name );
      }
    } * m_remover;
  };

}
} // justine::robocar::


#endif // ROBOCAR_SMARTCITY_HPP
