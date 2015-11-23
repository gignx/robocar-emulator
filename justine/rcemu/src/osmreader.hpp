#ifndef ROBOCAR_OSMREADER_HPP
#define ROBOCAR_OSMREADER_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file osmreader.hpp
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

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/index/map/sparse_mem_table.hpp>
#include <osmium/index/map/sparse_mem_map.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/geom/coordinates.hpp>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <google/protobuf/stubs/common.h>

#include <exception>
#include <stdexcept>

#include "smartcity_defs.hpp"
#include "osmDataTypes.hpp"

namespace justine
{
namespace robocar
{

typedef osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, osmium::Location> OSMLocations;
typedef std::vector<osmium::unsigned_object_id_type> WayNodesVect;
typedef std::map<std::string, WayNodesVect> WayNodesMap;
typedef std::map<osmium::unsigned_object_id_type, osmium::Location> NodesMap;
//typedef osmium::index::map::StlMap<osmium::unsigned_object_id_type, osmium::Location> WaynodeLocations;
typedef std::map<osmium::unsigned_object_id_type, osmium::Location> WaynodeLocations;
typedef std::map<osmium::unsigned_object_id_type, WayNodesVect> Way2Nodes;

typedef std::map<osmium::unsigned_object_id_type, WayNodesVect> AdjacencyList;
typedef osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, int > Vertices;

//new busstops
typedef std::map<osmium::unsigned_object_id_type, std::string> BusStops;

typedef std::vector<OSMBusWay> BusWayVector;

class OSMReader : public osmium::handler::Handler
{
public:
  std::vector<std::string> jarat_vektor;

  OSMReader ( const char * osm_file,
              AdjacencyList & alist,
              AdjacencyList & palist,
              WaynodeLocations & waynode_locations,
              WayNodesMap & busWayNodesMap,
              Way2Nodes & way2nodes,
              NodesMap & bsnm,
              BusStops & busstops,
              BusWayVector & busWayVector) : alist ( alist ),
    palist ( palist ),
    waynode_locations ( waynode_locations ),
    busWayNodesMap ( busWayNodesMap ),
    way2nodes ( way2nodes ),
    busStopNodesMap( bsnm ),
    busstops( busstops ),
    busWayVector(busWayVector)
  {

    try
      {

#ifdef DEBUG
        std::cout << "\n OSMReader is running... " << std::endl;
#endif

        osmium::io::File infile ( osm_file );
        osmium::io::Reader reader ( infile, osmium::osm_entity_bits::all );

        using SparseLocations = osmium::index::map::SparseMemMap<osmium::unsigned_object_id_type, osmium::Location>;
        osmium::handler::NodeLocationsForWays<SparseLocations> node_locations ( locations );

        osmium::apply ( reader, node_locations, *this );
        reader.close();

#ifdef DEBUG
        std::cout << " #OSM Nodes: " << nOSM_nodes << "\n";
        std::cout << " #OSM Highways: " << nOSM_ways << "\n";
        std::cout << " #Kms (Total length of highways) = " << sum_highhway_length/1000.0 << std::endl;
        std::cout << " #OSM Relations: " << nOSM_relations << "\n";
        std::cout << " #Highway Nodes: " << sum_highhway_nodes << "\n";
        std::cout << " #Unique Highway Nodes: " << sum_unique_highhway_nodes << "\n";
        std::cout << " E= (#Highway Nodes - #OSM Highways): " << sum_highhway_nodes - nOSM_ways << "\n";
        std::cout << " V= (#Unique Highway Nodes):" << sum_unique_highhway_nodes << "\n";
        std::cout << " V^2/E= "
                  <<
                  ( ( long ) sum_unique_highhway_nodes * ( long ) sum_unique_highhway_nodes )
                  / ( double ) ( sum_highhway_nodes - nOSM_ways )
                  << "\n";
        std::cout << " Edge_multiplicity: " << edge_multiplicity << std::endl;
        std::cout << " max edle length: " << max_edge_length << std::endl;
        std::cout << " edle length mean: " << mean_edge_length/cedges << std::endl;
        std::cout << " cedges: " << cedges << std::endl;

        std::cout << " #Buses = " << busWayNodesMap.size() << std::endl;

        std::cout << " Node locations " << locations.used_memory() /1024.0/1024.0 << " Mbytes" << std::endl;
        //std::cout << " Waynode locations " << waynode_locations.used_memory() /1024.0/1024.0 << " Mbytes" << std::endl;
        std::cout << " Vertices " << vert.used_memory() /1024.0/1024.0 << " Mbytes" << std::endl;
        std::cout << " way2nodes " << way2nodes.size() << "" << std::endl;

        std::set<osmium::unsigned_object_id_type> sum_vertices;
        std::map<osmium::unsigned_object_id_type, size_t>  word_map;
        int sum_edges {0};
        for ( auto busit = begin ( alist );
              busit != end ( alist ); ++busit )
          {

            sum_vertices.insert ( busit->first );
            sum_edges+=busit->second.size();

            for ( const auto &v : busit->second )
              {
                sum_vertices.insert ( v );
              }

          }
        std::cout << " #citymap edges = "<< sum_edges<< std::endl;
        std::cout << " #citymap vertices = "<< sum_vertices.size() << std::endl;
        std::cout << " #citymap vertices (deg- >= 1) = "<< alist.size() << std::endl;
        std::cout << " #onewayc = "<< onewayc<< std::endl;

#endif

        m_estimator *= 8;

      }
    catch ( std::exception &err )
      {

        google::protobuf::ShutdownProtobufLibrary();
        throw;
      }

  }

  ~OSMReader()
  {

    google::protobuf::ShutdownProtobufLibrary();

  }

  std::size_t get_estimated_memory() const
  {
    return m_estimator;
  }

  inline bool edge ( osmium::unsigned_object_id_type v1, osmium::unsigned_object_id_type v2 )
  {
    return ( std::find ( alist[v1].begin(), alist[v1].end(), v2 ) != alist[v1].end() );
  }


  void bus_stop_name(osmium::Node& node)
  {
      const char *b_s_n;
      int nodeid;

        for (const osmium::Tag& tag : node.tags()) {

            if (strcmp(tag.key(), "name") == 0)
            {
              nodeid = node.id();
              b_s_n = tag.value();

                busstops[node.id()] = std::string(b_s_n);

                #ifdef DEBUG

                std::cout << "Bus Stop Node: " << nodeid << " Bus Stop Name: " << b_s_n << std::endl;

                #endif
            }
        }
  }

  void node(osmium::Node& node)
  {
    ++nOSM_nodes;

    for (const osmium::Tag& tag : node.tags()) {

          if ( (strcmp(tag.key(), "highway") == 0) && (strcmp(tag.value(), "bus_stop") == 0) )
          {
              bus_stop_name(node);
          }

      }

      const char *highway = node.tags()["highway"];

   if (!highway)
    {
      return;
    }

   if (strcmp(highway, "bus_stop") != 0)
    {
      return;
    }
    busStopNodesMap[node.id()] = node.location();
  }

  int onewayc {0};
  int onewayf {false};

  void way ( osmium::Way& way )
  {
    const char* highway = way.tags() ["highway"]; //ebbe kerul bele, hogy milyen fajta az utunk
    if ( !highway )
      return;
    // http://wiki.openstreetmap.org/wiki/Key:highway
    if ( !strcmp ( highway, "footway" )
         || !strcmp ( highway, "cycleway" )
         || !strcmp ( highway, "bridleway" )
         || !strcmp ( highway, "steps" )
         || !strcmp ( highway, "path" )
         || !strcmp ( highway, "construction" ) )
      return;

    onewayf = false;
    const char* oneway = way.tags() ["oneway"];
    if ( oneway )
      {
        onewayf = true;
        ++onewayc;
      }

    ++nOSM_ways;

    double way_length = osmium::geom::haversine::distance ( way.nodes() );
    sum_highhway_length += way_length;

    int node_counter {0};
    int unique_node_counter {0};
    osmium::Location from_loc;

    osmium::unsigned_object_id_type vertex_old;

    for ( const osmium::NodeRef& nr : way.nodes() )
      {

        osmium::unsigned_object_id_type vertex = nr.positive_ref();

        #ifdef DEBUG
        std::cout << "way2nodes.map< " << way.id() << ", " << vertex << ">;" << std::endl;
        #endif

        way2nodes[way.id()].push_back ( vertex ); //map<way.id (elozo mapben a ref vektor egyik tagja), egy vektor, ami tartalmazza a ref nodejait>

        try
          {

            vert.get ( vertex );

          }
        catch ( std::exception& e )
          {

            vert.set ( vertex, 1 );

            ++unique_node_counter;

            //waynode_locations.set ( vertex, nr.location() );
            waynode_locations[vertex] = nr.location();

          }

        if ( node_counter > 0 )
          {

            if ( !edge ( vertex_old, vertex ) )
              {

                alist[vertex_old].push_back ( vertex );

                double edge_length = distance ( vertex_old, vertex );

                palist[vertex_old].push_back ( edge_length / 3.0 );

                if ( edge_length>max_edge_length )
                  max_edge_length = edge_length;

                mean_edge_length += edge_length;

                ++m_estimator;
                ++cedges;


              }
            else
              ++edge_multiplicity;

            if ( !onewayf )
              {

                if ( !edge ( vertex, vertex_old ) )
                  {

                    alist[vertex].push_back ( vertex_old );

                    double edge_length = distance ( vertex_old, vertex );

                    palist[vertex].push_back ( edge_length / 3.0 );

                    if ( edge_length>max_edge_length )
                      max_edge_length = edge_length;

                    mean_edge_length += edge_length;

                    ++m_estimator;
                    ++cedges;


                  }
                else
                  ++edge_multiplicity;

              }

          }

        vertex_old = vertex;

        ++node_counter;
      }

    sum_highhway_nodes += node_counter;
    sum_unique_highhway_nodes  += unique_node_counter;

  }

  void bus_ways_read(osmium::Relation& relat)
  {
    int szamlalo=0;
        for (const osmium::Tag& tag : relat.tags()) {

              if (strcmp(tag.key(), "ref") == 0)
              {
                szamlalo=0;
                for(int i=0;i<jarat_vektor.size();i++)
                {
                  if(tag.value() == jarat_vektor[i])
                    szamlalo++;
                }
                if(szamlalo == 0)
                  jarat_vektor.push_back(tag.value());

              }

        }

        osmium::RelationMemberList& rml = relat.members();
        for (osmium::RelationMember& rm : rml) {

          if (strcmp(rm.role(), "stop") == 0)
          {
              //TODO stop kezelese
                //std::cout << "stop vagyok, a nodeom: " << rm.ref() << std::endl;
          }
          else
          {

              //TODO Matyi talad ki hogy kell e csinalni itt valamit

          }

        }

  }


  void relation (osmium::Relation& rel)
  {


    for (const osmium::Tag& tag : rel.tags()) {

        if ((strcmp(tag.key(),"route") == 0) && (strcmp(tag.value(),"bus") == 0))
        {
            //FIXME na hat igen, mondta hogy nincs meghivva!!!
            //bus_ways_read(rel);
        }

    }


    ++nOSM_relations;

    const char *bus = rel.tags()["route"];

    if (bus && !strcmp(bus, "bus")) // ha buszrol van szo, akkor belepunk
    {
      ++nbuses;

      bool busWayFrom = true;

      std::string ref_key;

      try
      {
        const char* ref = rel.tags() ["ref"]; //a ref valtozo tarolja a busz jaratszamat

        if (ref)//ha letzik akkor hozzafuzi a ref_key hez
        {
          ref_key.append ( ref );

          //std::cout << rel.id() << " " << ref_key << std::endl;

          auto it = std::find_if(busWayVector.begin(),
                                 busWayVector.end(),
            [ref_key](OSMBusWay obw)
            {
              return obw.ref == ref_key;
            });

          if (it == busWayVector.end())
          {
            busWayVector.emplace_back(ref_key);
          }
          else
          {
              it->timesFound++;
              //std::cout << it->timesFound << std::endl;
          }
        }
        else
        {
          ref_key.append ( "Not specified" );
        }
      }
      catch ( std::exception& e )//ha az osmbol hianyoznak a buszok
      {
        std::cout << "There is no bus number."<< e.what() << std::endl;
      }

      const osmium::RelationMemberList& members = rel.members();

      auto it = std::find_if(busWayVector.begin(),
                             busWayVector.end(),
        [ref_key](OSMBusWay obw)
        {
          return obw.ref == ref_key;
        });

      for (const auto& m : members )
      {
        if (m.type() == osmium::item_type::way) //map<jaratszam, hozza tartozo ref vektor> //a ref vektor tagjai, hivatkozasok egy way-re, amibol ki tudjuk majd nyerni a hozzajuk tartozo nodeokat
        {

          //std::cout << "busWayNodesMap<" << ref_key << ", " << m.ref() << ">;" << std::endl;


          if (it != busWayVector.end())
          {
            if (it->timesFound == 1)
            {
              it->nodesFrom.push_back(m.ref());
              //std::cout << "push " << ref_key << " s " << it->nodesFrom.size() << std::endl;
            }
            else if (it->timesFound == 2)
            {
              it->nodesTo.push_back(m.ref());
            }
          }

          busWayNodesMap[ref_key].push_back (m.ref());
        }
      }
    }
  }

protected:

  Vertices vert;
  int nOSM_nodes {0};
  int nOSM_ways {0};
  int nOSM_relations {0};
  int sum_unique_highhway_nodes {0};
  int sum_highhway_nodes {0};
  int sum_highhway_length {0};
  int edge_multiplicity = 0;
  int nbuses {0};
  double max_edge_length {0.0};
  double mean_edge_length {0.0};
  int cedges {0};
  OSMLocations  locations;

private:

  inline double distance ( osmium::unsigned_object_id_type vertexa, osmium::unsigned_object_id_type vertexb )
  {

    osmium::Location A = locations.get ( vertexa );
    osmium::Location B = locations.get ( vertexb );
    osmium::geom::Coordinates ac {A};
    osmium::geom::Coordinates ab {B};

    return osmium::geom::haversine::distance ( ac, ab );
  }

  std::size_t m_estimator {0u};
  AdjacencyList & alist, & palist;
  WaynodeLocations & waynode_locations;
  WayNodesMap & busWayNodesMap;
  Way2Nodes & way2nodes;
  NodesMap & busStopNodesMap;
  BusStops & busstops;
  BusWayVector & busWayVector;

};

}
} // justine::robocar::

#endif // ROBOCAR_OSMREADER_HPP
