#ifndef ROBOCAR_CAR_HPP
#define ROBOCAR_CAR_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file car.hpp
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
#include <osmium/osm/types.hpp>
#include <iostream>
#include <vector>

#include "src/robocar.pb.h"

#include <osmreader.hpp>
#include <algorithm>
#include <time.h> 
#include <unistd.h>

namespace justine
{
namespace robocar
{

enum class CarType: unsigned int
{
  NORMAL=0, POLICE, GANGSTER, CAUGHT, PEDESTRIAN, BUS, BUSSTOP
};

class Traffic;

class Car
{
public:

  Car ( Traffic & traffic , CarType type = CarType::NORMAL );

  virtual void init();

  virtual void step();

  osmium::unsigned_object_id_type from() const
  {
    return m_from;
  }
  osmium::unsigned_object_id_type to() const
  {
    return m_to;
  }
  osmium::unsigned_object_id_type get_step() const
  {
    return m_step;
  }
  CarType get_type() const
  {
    return m_type;
  }
  void set_type ( CarType type )
  {
    m_type = type;
  }

  osmium::unsigned_object_id_type to_node() const;
  osmium::unsigned_object_id_type get_max_steps() const;
  virtual void nextEdge ( void );
  virtual void nextSmarterEdge ( void );
//asdasdas
  virtual void setMehetValue (int value)
   {
    mehet=value;
  }
  virtual void currentTime()
  {
      std::cout<<"szar"<<std::endl;
     time(&time1);
    
    
  }
  virtual void canIGo()
  {
      time(&time2);
      std::cout<<time2<<" szar time2 "<<std::endl;
      this->seconds = difftime(time2,time1);
      std::cout<<std::fixed;
      std::cout<<seconds<<" szar seconds "<<std::endl;
  }
  virtual void setLastNode(long unsigned int node)
  {
    last_node=node;
  }
  virtual bool checkLastNode(long unsigned int node)
  {
    if (last_node==node)
    {
      return true;
    }
    else
      return false;
  }
//asdasdasdasd

  virtual void print ( std::ostream & os ) const
  {
    os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> ( get_type() );
  }

  virtual void assign(CarData *car_data, bool full)
  {
    // meghívjuk a protobuf által generált setter függvényeket
    // és belehelyezzük a CarData objektumba az autó adatait
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    // a cast biztosan lehetséges, így a static_cast is megfelelő lesz
    // (nincs runtime ellenőrzés benne)
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
  }

  friend std::ostream & operator<< ( std::ostream & os, Car & c )
  {
    c.print ( os );

    return os;
  }

public:
  int mehet;
  long unsigned int last_node=0;
  time_t time1;
  time_t time2;
  double seconds;
  Traffic & traffic;
  CarType m_type {CarType::NORMAL};
  osmium::unsigned_object_id_type m_from {3130863972};
  osmium::unsigned_object_id_type m_to {0};
  osmium::unsigned_object_id_type m_old_step {1};
  osmium::unsigned_object_id_type m_step {0};

private:
  
};

class AntCar : public Car
{
public:
  AntCar ( Traffic & traffic );

  virtual void nextSmarterEdge ( void );

  virtual void print ( std::ostream & os ) const
  {
    os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> ( get_type() );
  }



  virtual void assign(CarData *car_data, bool full)
  {
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
  }

  osmium::unsigned_object_id_type ant ( void );
  osmium::unsigned_object_id_type ant_rnd ( void );
  osmium::unsigned_object_id_type ant_rernd ( void );
  osmium::unsigned_object_id_type ant_mrernd ( void );

  static AdjacencyList alist;
  static AdjacencyList alist_evaporate;

private:
  bool rnd {true};
};




class Pedestrian : public Car
{
public:
  Pedestrian ( Traffic & traffic );

  //virtual void nextSmarterEdge ( void );

virtual void step();

  virtual void print ( std::ostream & os ) const
  {
    os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> ( get_type() );
  }



  virtual void assign(CarData *car_data, bool full)
  {
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
  }

  osmium::unsigned_object_id_type ped ( void );
  osmium::unsigned_object_id_type ped_rnd ( void );
  osmium::unsigned_object_id_type ped_rernd ( void );
  osmium::unsigned_object_id_type ped_mrernd ( void );

  static AdjacencyList plist;
  static AdjacencyList plist_evaporate;

private:
  bool rnd {true};
  int counter = 0;
};

class SmartCar : public Car
{
public:
  SmartCar ( Traffic & traffic, CarType type, bool guided , int id);

  virtual void step();
  virtual void init();
  virtual void setMehetValue(){};

  virtual void print ( std::ostream & os ) const
  {
    os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> ( get_type() );
  }

  virtual void assign(CarData *car_data, bool full)
  {
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
  }

  bool get_guided() const
  {
    return m_guided;
  }
  bool set_route ( std::vector<long unsigned int> & route );
  virtual void nextEdge ( void );
  virtual void nextGuidedEdge ( void );
  bool set_fromto ( unsigned int from, unsigned int to );

  int get_id()
  {
    return this->id_;
  }
protected:
  bool m_guided {false};
  bool m_routed {false};

protected:
  int id_;
  std::vector<long unsigned int> route;

};

class CopCar : public SmartCar
{
public:
  CopCar (Traffic & traffic, bool guided, const char *name , int id);

  virtual void print (std::ostream & os) const
  {
    os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> (get_type())
       << " "
       << num_gangsters_caught_
       << " "
       << team_name_
       << " "
       << id_;
  }

  virtual void assign(CarData *car_data, bool full)
  {
    // annyiban külonbozik a tobbi assign() függvénytől, hogy
    // a rendőrautókra jellemző adatokat is átadjuk
    // ezek a .proto fájlban "optional" értékek
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
    car_data->set_caught(num_gangsters_caught_);
    car_data->set_team(team_name_);
    car_data->set_id(id_);
    if(full){
      car_data->set_size(route.size());
      for(auto it = route.begin();it!=route.end();it++)
        car_data->add_path(*it);
    }else{
      car_data->set_size(0);
    }
  }

  std::string get_team_name() const
  {
    return team_name_;
  }

  int get_num_gangsters_caught() const
  {
    return num_gangsters_caught_;
  }

  int GangsterCaught(void)
  {
    return ++num_gangsters_caught_;
  }
protected:
  std::string team_name_;

  int num_gangsters_caught_;
};

class Bus : public SmartCar
{
public:
  Bus (Traffic & traffic, bool guided, const char *name , int id);

  void init(osmium::unsigned_object_id_type place);
  virtual void setMehetValue (int value);
  virtual void currentTime();
  virtual void canIGo();
  virtual void setLastNode(long unsigned int node);
  virtual bool checkLastNode(long unsigned int node);
  virtual void step()
  {

    //std::cout<<mehet<<" mehet "<<std::endl;
    if (mehet==0)
    {
      //std::cout<<"nem mek"<<std::endl;
    }

    
    else if (mehet==1)
    {
     //std::cout<<"1 vagyok"<<std::endl; 
    nextGuidedEdge();

    long unsigned int last;

    if (isGoingFrom)
    {
      last = routeWayFrom[routeWayFrom.size() - 2];
    }
    else
    {
      // safety not yolo
      if (routeWayTo.size() > 0)
      {
        last = routeWayTo[routeWayTo.size() - 2];
      }
    }

    if ((m_old_step == m_step) && (m_from == last))
    {
      if (isGoingFrom)
      {
        if (routeWayTo.size() > 0)
        {
          this->init(this->routeWayTo[0]);

          this->set_route(this->routeWayTo);

          isGoingFrom = false;
        }
        else
        {
          this->init(this->routeWayFrom[0]);

          this->set_route(this->routeWayFrom);

          isGoingFrom = true;
        }
      }
      else
      {
        this->init(this->routeWayFrom[0]);

        this->set_route(this->routeWayFrom);

        isGoingFrom = true;
      }
    }

    m_old_step = m_step;
    }
  }
  




  virtual void print (std::ostream & os) const
  {
    /*os << m_from
       << " "
       << to_node()
       << " "
       << get_max_steps()
       << " "
       << get_step()
       << " "
       << static_cast<unsigned int> (get_type())
       << " "
       << line_
       << " "
       << id_;*/
       std::cout<<std::fixed;
       std::cout<<std::endl;
       //os<<" "<<line_<<" ";
       os<<" "<<id_<<" ";
       os<<" "<<mehet<<" ";
       os<<" "<<time1<<" ";
       os<<" "<<time2<<" ";
       os<<" "<<seconds<<" ";
       os<<" "<<last_node<<" ";
       std::cout<<std::endl;
  } 

  virtual void assign(CarData *car_data, bool full)
  {
    car_data->set_node_from(m_from);
    car_data->set_node_to(to_node());
    car_data->set_max_step(get_max_steps());
    car_data->set_step(get_step());
    car_data->set_type(static_cast<CarData::ProtoCarType>(get_type())); // safe
    car_data->set_team(line_);
    car_data->set_id(id_);
    if(full){
      car_data->set_num_stops(busstops.size());
      for(auto it = busstops.begin();it!=busstops.end();it++)
        car_data->add_busstops(*it);
      car_data->set_size(route.size());
      for(auto it = route.begin();it!=route.end();it++)
        car_data->add_path(*it);
    }else{
      car_data->set_size(0);
    }
  }
  
  virtual void setMehetValue(bool value){
      mehet = value;
      };

  std::string get_line() const
  {
    return line_;
  }

  // TODO: Ide jönnek az adott busz megállóinak ID-i
  //       (azok amiket mi adtunk nekik) (relax, már el van küldve a JAVA-nak, csak feltölteni kell!!)

  int mehet;
  long unsigned int last_node = 0;
  time_t time1;
  time_t time2;
  double seconds;
  std::vector<int> busstops = {};
  std::vector<long unsigned int> routeWayFrom, routeWayTo;
protected:
  std::string line_;
private:
  bool isGoingFrom;
  
};

}
} // justine::robocar::


#endif // ROBOCAR_CAR_HPP
