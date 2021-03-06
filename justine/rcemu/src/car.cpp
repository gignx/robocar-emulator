/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file car.cpp
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

#include <car.hpp>
#include <traffic.hpp>
#include <boost/iterator/iterator_concepts.hpp>

justine::robocar::Car::Car (
  justine::robocar::Traffic & traffic,
  justine::robocar::CarType type ) :  traffic ( traffic ), m_type ( type )
{

}

void justine::robocar::Car::init()
{
  m_from = traffic.node();

  m_to = 0;
  m_step = 0;

  traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
}

//
osmium::unsigned_object_id_type justine::robocar::Car::to_node() const
{
  return traffic.alist ( m_from, m_to );
}

osmium::unsigned_object_id_type justine::robocar::Car::get_max_steps() const
{
  return traffic.palist ( m_from, m_to );
}

void justine::robocar::Car::nextSmarterEdge ( void )
{
  osmium::unsigned_object_id_type next_m_from = traffic.alist ( m_from, m_to );
  size_t nes = traffic.nedges ( next_m_from );

  if ( !nes )
    return;

  osmium::unsigned_object_id_type next_m_to = std::rand() % nes;

  if ( traffic.alist ( next_m_from, next_m_to ) == m_from )
    next_m_to = ( next_m_to + 1 ) % nes;

  if ( traffic.palist ( next_m_from, next_m_to ) >
       traffic.salist ( next_m_from, next_m_to ) )
  {
    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

    m_from = next_m_from;
    m_to = next_m_to;
    m_step = 0;

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
  }
}

void justine::robocar::Car::nextEdge ( void )
{
  osmium::unsigned_object_id_type next_m_from = traffic.alist ( m_from, m_to );
  size_t nes = traffic.nedges ( next_m_from );

  if ( !nes )
    return;

  osmium::unsigned_object_id_type next_m_to = std::rand() % nes;

  if ( traffic.palist ( next_m_from, next_m_to ) >
       traffic.salist ( next_m_from, next_m_to ) )
  {
    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

    m_from = next_m_from;
    m_to = next_m_to;
    m_step = 0;

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
  }
}

void justine::robocar::Car::step()
{
  if ( traffic.hasNode ( to_node() ) )
  {
    if ( m_step >= traffic.palist ( m_from, m_to ) )
    {
      nextSmarterEdge();
    }
    else
      ++m_step;
  }
  else
  {
      // car stopped
  }
}

justine::robocar::AdjacencyList justine::robocar::AntCar::alist;
justine::robocar::AdjacencyList justine::robocar::AntCar::alist_evaporate;

justine::robocar::AntCar::AntCar (
  justine::robocar::Traffic & traffic ) : justine::robocar::Car ( traffic )
{

}

osmium::unsigned_object_id_type justine::robocar::AntCar::ant ( void )
{
  AdjacencyList::iterator iter= AntCar::alist.find ( m_from );

  WayNodesVect::iterator i = std::max_element ( iter->second.begin(), iter->second.end() );

  osmium::unsigned_object_id_type next_m_to = std::distance ( iter->second.begin(), i );

  ++*i;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::AntCar::ant_rnd ( void )
{
  AdjacencyList::iterator iter= AntCar::alist.find ( m_from );

  int sum = std::accumulate ( iter->second.begin(), iter->second.end(), 0 );

  int rnd = std::rand() % sum;

  int sum2 = 0;

  WayNodesVect::iterator j=iter->second.begin();
  for ( ; j!= iter->second.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( iter->second.begin(), j );

  ++*j;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::AntCar::ant_rernd ( void )
{
  AdjacencyList::iterator iter= AntCar::alist.find ( m_from );

  WayNodesVect cpv = iter->second;

  int sum = std::accumulate ( cpv.begin(), cpv.end(), 0 );

  int res = ( int ) ( ( double ) sum/.75 );

  int total = sum + cpv.size() * ( res/cpv.size() );

  for ( osmium::unsigned_object_id_type& v : cpv )
    v += res/cpv.size();

  int rnd = std::rand() % total;

  int sum2 = 0;

  WayNodesVect::iterator j=cpv.begin();
  for ( ; j!= cpv.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( cpv.begin(), j );

  ++*j;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::AntCar::ant_mrernd ( void )
{
  AdjacencyList::iterator iter = AntCar::alist.find ( m_from );
  AdjacencyList::iterator iter2 = AntCar::alist_evaporate.find ( m_from );

  for ( WayNodesVect::iterator j=iter->second.begin(); j!= iter->second.end(); ++j )
  {
      int del = traffic.get_time() -
                iter2->second[std::distance ( iter->second.begin(), j )];

      int pheromone = *j - del;

      if ( pheromone > 1 )
        *j = pheromone;
      else
        *j = 1;
  }

  WayNodesVect cpv = iter->second;

  int sum = std::accumulate ( cpv.begin(), cpv.end(), 0 );

  int res = ( int ) ( ( double ) sum/.6 );

  int total = sum + cpv.size() * ( res/cpv.size() );

  for ( osmium::unsigned_object_id_type& v : cpv )
    v += res/cpv.size();

  int rnd = std::rand() % total;

  int sum2 = 0;

  WayNodesVect::iterator j=cpv.begin();
  for ( ; j!= cpv.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( cpv.begin(), j );

  ++*j;
  iter2->second[std::distance (cpv.begin(), j )] = traffic.get_time();

  return next_m_to;
}

void justine::robocar::AntCar::nextSmarterEdge ( void )
{
  osmium::unsigned_object_id_type next_m_from = traffic.alist ( m_from, m_to );
  size_t nes = traffic.nedges ( next_m_from );
  if ( !nes )
    return;

  osmium::unsigned_object_id_type next_m_to;

  switch (traffic.get_type())
  {
    case TrafficType::ANT:
            next_m_to = ant();
            break;
    case TrafficType::ANT_RND:
            next_m_to = ant_rnd();
            break;
    case TrafficType::ANT_RERND:
            next_m_to = ant_rernd();
            break;
    default:
            next_m_to = ant_mrernd();
            break;
  }

  /*
  if ( traffic.get_type() == TrafficType::ANT )
  {
    next_m_to = ant();
  }
  else if ( traffic.get_type() == TrafficType::ANT_RND )
  {
    next_m_to = ant_rnd();
  }
  else if ( traffic.get_type() == TrafficType::ANT_RERND )
  {
    next_m_to = ant_rernd();
  }
  else
  {
    next_m_to = ant_mrernd();
  }
  */

  if ( traffic.alist ( next_m_from, next_m_to ) == m_from )
    next_m_to = ( next_m_to + 1 ) % nes;

  if ( traffic.palist ( next_m_from, next_m_to ) >
       traffic.salist ( next_m_from, next_m_to ) )
  {
    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

    m_from = next_m_from;
    m_to = next_m_to;
    m_step = 0;

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
  }
}

justine::robocar::AdjacencyList justine::robocar::Pedestrian::plist;
justine::robocar::AdjacencyList justine::robocar::Pedestrian::plist_evaporate;

justine::robocar::Pedestrian::Pedestrian (
  justine::robocar::Traffic & traffic ) : justine::robocar::Car ( traffic )
{

}

osmium::unsigned_object_id_type justine::robocar::Pedestrian::ped ( void )
{
  AdjacencyList::iterator iter= Pedestrian::plist.find ( m_from );

  WayNodesVect::iterator i = std::max_element ( iter->second.begin(), iter->second.end() );

  osmium::unsigned_object_id_type next_m_to = std::distance ( iter->second.begin(), i );

  ++*i;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::Pedestrian::ped_rnd ( void )
{
  AdjacencyList::iterator iter= Pedestrian::plist.find ( m_from );

  int sum = std::accumulate ( iter->second.begin(), iter->second.end(), 0 );

  int rnd = std::rand() % sum;

  int sum2 = 0;

  WayNodesVect::iterator j=iter->second.begin();
  for ( ; j!= iter->second.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( iter->second.begin(), j );

  ++*j;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::Pedestrian::ped_rernd ( void )
{
  AdjacencyList::iterator iter= Pedestrian::plist.find ( m_from );

  WayNodesVect cpv = iter->second;

  int sum = std::accumulate ( cpv.begin(), cpv.end(), 0 );

  int res = ( int ) ( ( double ) sum/.75 );

  int total = sum + cpv.size() * ( res/cpv.size() );

  for ( osmium::unsigned_object_id_type& v : cpv )
    v += res/cpv.size();

  int rnd = std::rand() % total;

  int sum2 = 0;

  WayNodesVect::iterator j=cpv.begin();
  for ( ; j!= cpv.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( cpv.begin(), j );

  ++*j;

  return next_m_to;
}

osmium::unsigned_object_id_type justine::robocar::Pedestrian::ped_mrernd ( void )
{
  AdjacencyList::iterator iter = Pedestrian::plist.find ( m_from );
  AdjacencyList::iterator iter2 = Pedestrian::plist_evaporate.find ( m_from );

  for ( WayNodesVect::iterator j=iter->second.begin(); j!= iter->second.end(); ++j )
  {
      int del = traffic.get_time() -
                iter2->second[std::distance ( iter->second.begin(), j )];

      int pheromone = *j - del;

      if ( pheromone > 1 )
        *j = pheromone;
      else
        *j = 1;
  }

  WayNodesVect cpv = iter->second;

  int sum = std::accumulate ( cpv.begin(), cpv.end(), 0 );

  int res = ( int ) ( ( double ) sum/.6 );

  int total = sum + cpv.size() * ( res/cpv.size() );

  for ( osmium::unsigned_object_id_type& v : cpv )
    v += res/cpv.size();

  int rnd = std::rand() % total;

  int sum2 = 0;

  WayNodesVect::iterator j=cpv.begin();
  for ( ; j!= cpv.end(); ++j )
  {
    sum2 += *j;

    if ( sum2 >= rnd )
      break;
  }

  osmium::unsigned_object_id_type next_m_to = std::distance ( cpv.begin(), j );

  ++*j;
  iter2->second[std::distance (cpv.begin(), j )] = traffic.get_time();

  return next_m_to;
}

void justine::robocar::Pedestrian::step() {
  if (counter == 5) {
    Car::step();
    counter = 0;
  } else {
    counter++;
  }
}

justine::robocar::SmartCar::SmartCar (
  justine::robocar::Traffic & traffic,
  justine::robocar::CarType type,
  bool guided,
  int id ) : justine::robocar::Car ( traffic, type ), m_guided ( guided ), id_(id)
{

}

void justine::robocar::SmartCar::init()
{
  if ( m_guided )
  {
      /*
            osmium::unsigned_object_id_type ini {2969934868};

            if ( traffic.hasNode ( ini ) )
              {
                m_from = ini;
              }
            else
              {
                m_from = traffic.node();
              }
      */

    m_from = traffic.node();

    m_to = 0;
    m_step = 0;

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
  }
  else
    Car::init();
}

void justine::robocar::Bus::init(osmium::unsigned_object_id_type place)
{
  if ( m_guided )
  {

    m_from = place;

    m_to = 0;
    m_step = 0;

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
  }
  else
    Car::init();
}

void justine::robocar::Bus::setMehetValue (int value)
  {
    //std::cout<<"lefutottam value"<<std::endl;
      busgoing=value;
      //std::cout<<mehet<<std::endl;
  }
void justine::robocar::Bus::currentTime()
  {

     time(&time1);
    
     //std::cout<<time1<<" time1 "<<std::endl;
    
  }
void justine::robocar::Bus::canIGo()
  {

      time(&time2);
      //std::cout<<time2<<" time2 "<<std::endl;
      //this->seconds = difftime(time2,time1);
      //std::cout<<std::fixed;
      seconds = difftime(time2,time1);
      //std::cout<<seconds<<"seconds "<<std::endl;
      if (seconds>=10.0)
      {
        //std::cout<<"mehet = 1 canigoba"<<std::endl;
        busgoing=1;
      }
      else
        busgoing=0;
    
    
  }  
  void justine::robocar::Bus::setLastNode(long unsigned int node)
  {
    last_node=node;
  }
  bool justine::robocar::Bus::checkLastNode(long unsigned int node)
  {
    if (last_node==node)
    {
      return true;
    }
    else
      return false;
  }
void justine::robocar::SmartCar::nextEdge ( void )
{
  if ( traffic.hasNode ( to_node() ) )
  {
    if ( m_step >= traffic.palist ( m_from, m_to ) )
    {
      osmium::unsigned_object_id_type next_m_from
          = traffic.alist ( m_from, m_to );
      osmium::unsigned_object_id_type next_m_to
          = traffic.naive_node_for_nearest_gangster ( m_from, m_to, m_step );

      if ( traffic.palist ( next_m_from, next_m_to ) >
           traffic.salist ( next_m_from, next_m_to ) )
      {
        traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

        m_from = next_m_from;
        m_to = next_m_to;
        m_step = 0;

        traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
      }

    }
    else
      ++m_step;
  }
  else
  {
      // car stopped
  }
}
/*
if ( traffic.hasNode ( to_node() ) )
  {
      std::cout << "lol" << std::endl;
    if ( m_step >= traffic.palist ( m_from, m_to ) )
    {
      std::vector<long unsigned int>::iterator i
          = std::find ( route.begin(), route.end(), to_node() );

          std::cout << "lolend" << std::endl;

      if ( i == route.end() )
        return;
*/

void justine::robocar::SmartCar::nextGuidedEdge ( void )
{
  if ( traffic.hasNode ( to_node() ) )
  {
    if ( m_step >= traffic.palist ( m_from, m_to ) )
    {
      //std::cout << "trace" << to_node() << std::endl;
      std::vector<long unsigned int>::iterator i
          = std::find ( route.begin(), route.end(), to_node() );

      if ( i == route.end() )
        return;

      osmium::unsigned_object_id_type next_m_to;
      osmium::unsigned_object_id_type next_m_from;

      if ( static_cast<unsigned int>(std::distance ( route.begin(), i ) )== route.size() )
      {
        next_m_to = 0;
        next_m_from = to_node();
      }
      else
      {
        next_m_to;
        osmium::unsigned_object_id_type inv = traffic.alist_inv ( to_node(), * ( i+1 ) );
        //osmium::unsigned_object_id_type inv = *(i+1);
        //std::cout << "inv: "<<  inv << std::endl;
        if ( inv != -1 )
        {
          next_m_to = inv;
        }
        else
        {
          return;
        }

        next_m_from = to_node();
      }

      if ( traffic.palist ( next_m_from, next_m_to ) >=
           traffic.salist ( next_m_from, next_m_to ) )
      {
        //std::cout << "valami if: " << std::endl;
        traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to )-1 );

        m_from = next_m_from;
        m_to = next_m_to;
        m_step = 0;

        traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );
      }
      else
      {
        // NOP
      }

    }
    else
      ++m_step;

  }
  else
  {
      // car stopped
  }
}

void justine::robocar::SmartCar::step()
{
  if ( (m_type == CarType::POLICE) || (m_type == CarType::BUS)
   )
  {
    if ( m_guided )
      nextGuidedEdge();
    else
      nextEdge();
    std::cout<<"smartcar step"<<std::endl;
  }
  else if ( m_type == CarType::CAUGHT )
  {
    return;
  }
  else
    Car::step();

  if (m_type == CarType::BUS)
  {
    /*
    if ((m_old_step == m_step) && (m_from == 3055243036))
    {
	// TODO m_from = masodik | utolso_elotti csomopont
      std::vector<osmium::unsigned_object_id_type> busway ;
      busway.push_back(3055243036);
      busway.push_back(3055243037);
      busway.push_back(3055243034);
      busway.push_back(3055242832);
      busway.push_back(3055242829);
      busway.push_back(3055242830);
      busway.push_back(3055242831);
      busway.push_back(3055243033);
      busway.push_back(3055243038);
      busway.push_back(3055243039);
      busway.push_back(3055243040);
      busway.push_back(3055243041);
      busway.push_back(3055243043);
      busway.push_back(3055243045);
      busway.push_back(3055243044);
      busway.push_back(3055243042);
      busway.push_back(3039407854);
      busway.push_back(3039407853);
      busway.push_back(3039407852);
      busway.push_back(3039407851);
      busway.push_back(3039407850);
      this->set_route(busway);
    }
	if ((m_old_step == m_step) && (m_from == 3039407851))
	// TODO m_from = masodik | utolso_elotti csomopont
    {
      std::vector<osmium::unsigned_object_id_type> busway ;
	  busway.push_back(3039407851);
	  busway.push_back(3039407852);
	  busway.push_back(3039407853);
	  busway.push_back(3039407854);
	  busway.push_back(3055243042);
	  busway.push_back(3055243044);
	  busway.push_back(3055243045);
	  busway.push_back(3055243043);
	  busway.push_back(3055243041);
	  busway.push_back(3055243040);
	  busway.push_back(3055243039);
	  busway.push_back(3055243038);
	  busway.push_back(3055243033);
	  busway.push_back(3055242831);
	  busway.push_back(3055242830);
	  busway.push_back(3055242829);
	  busway.push_back(3055242832);
	  busway.push_back(3055243034);
	  busway.push_back(3055243037);
	  busway.push_back(3055243036);
	  busway.push_back(3055243035);
      this->set_route(busway);
    }

    m_old_step = m_step;*/
  }
}

bool justine::robocar::SmartCar::set_route ( std::vector<long unsigned int> & route )
{
  if ( route.size() < 2 )
    return false;

  this->route = route;
  m_routed = true;

  if ( m_from == route[0] )
  {
    int next_m_to = traffic.alist_inv ( m_from, route[1] );

    if ( next_m_to != m_to )
    {
      m_to = next_m_to;
      m_step = 0;
    }
  }

  return true;
}

bool justine::robocar::SmartCar::set_fromto ( unsigned int from, unsigned int to )
{
  if ( m_from == from && traffic.hasNode ( to ) )
  {
    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) -1 );

    m_to = traffic.alist_inv ( m_from, to );

    traffic.set_salist ( m_from, m_to, traffic.salist ( m_from, m_to ) +1 );

    return true;
  }

  return false;
}

justine::robocar::CopCar::CopCar (
  justine::robocar::Traffic & traffic,
  bool guided, const char *name, int id) :
    justine::robocar::SmartCar(traffic, CarType::POLICE, guided, id), team_name_(name),
    num_gangsters_caught_(0)
{

}

justine::robocar::Bus::Bus (
  justine::robocar::Traffic & traffic,
  bool guided, const char *name, int id) :
    justine::robocar::SmartCar(traffic, CarType::BUS, guided, id), line_(name),
    isGoingFrom(true)
{

}
