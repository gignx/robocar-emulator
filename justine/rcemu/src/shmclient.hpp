/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file shmclient.hpp
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
#include <server.hpp>
#include <extendedgraph.hpp>
#include <smartcity.hpp>
#include <car.hpp>
namespace justine
{
namespace sampleclient
{

 class ShmClient
 {
 public:
   Server* server;
   ExtendedGraph* graph;

   ShmClient ( const char * shm_segment, const char* port)
   {
     server = new Server (port);
     graph = new ExtendedGraph (shm_segment);

#ifdef DEBUG
     using NRGVertexIter = boost::graph_traits<NodeRefGraph>::vertex_iterator;
     NRGVertexIter i, end;
     std::tie(i,end) = graph->getVertices();
     for(;i!=end;++i) std::cout << *i << std::endl;
#endif
  }

   ~ShmClient()
   {
    delete server;
    delete graph;
   }

   

   osmium::unsigned_object_id_type virtual get_random_node ( void )
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->begin();
     std::advance ( iter, std::rand() % graph->shm_map->size() );

     return iter->first;
   }

   size_t num_edges ( osmium::unsigned_object_id_type from ) const
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( from );

     return iter->second.m_alist.size();
   }

   osmium::unsigned_object_id_type alist ( osmium::unsigned_object_id_type from, int to ) const
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( from );

     return iter->second.m_alist[to];
   }

   int alist_inv ( osmium::unsigned_object_id_type from,
                   osmium::unsigned_object_id_type to ) const
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( from );

     int ret = -1;

     for ( justine::robocar::uint_vector::iterator noderefi = iter->second.m_alist.begin();
          noderefi!=iter->second.m_alist.end();
          ++noderefi )
     {
       if ( to == *noderefi )
       {
         ret = std::distance ( iter->second.m_alist.begin(), noderefi );
         break;
       }
     }

     return ret;
   }

   osmium::unsigned_object_id_type salist ( osmium::unsigned_object_id_type from, int to ) const
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( from );
     return iter->second.m_salist[to];

   }

   void set_salist ( osmium::unsigned_object_id_type from, int to ,
                     osmium::unsigned_object_id_type value )
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( from );

     iter->second.m_salist[to] = value;
   }

   bool hasNode ( osmium::unsigned_object_id_type node )
   {
     justine::robocar::shm_map_Type::iterator iter=graph->shm_map->find ( node );

     return ! ( iter == graph->shm_map->end() );
   }

   double dst ( osmium::unsigned_object_id_type n1, osmium::unsigned_object_id_type n2 ) const
   {
    return 0;
   }

   double dst ( double lon1, double lat1, double lon2, double lat2 ) const
   {
     osmium::geom::Coordinates c1 {lon1, lat1};
     osmium::geom::Coordinates c2 {lon2, lat2};

     return osmium::geom::haversine::distance ( c1, c2 );
   }

   void toGPS ( osmium::unsigned_object_id_type from, double *lo, double *la ) const
   {
     justine::robocar::shm_map_Type::iterator iter1=graph->shm_map->find ( from );
     double lon1 {iter1->second.lon/10000000.0}, lat1 {iter1->second.lat/10000000.0};

     *lo = lon1;
     *la = lat1;
   }

   void toGPS ( osmium::unsigned_object_id_type from,
                osmium::unsigned_object_id_type to,
                osmium::unsigned_object_id_type step, double *lo, double *la ) const
   {
     justine::robocar::shm_map_Type::iterator iter1=graph->shm_map->find ( from );
     double lon1 {iter1->second.lon/10000000.0}, lat1 {iter1->second.lat/10000000.0};

     justine::robocar::shm_map_Type::iterator iter2=graph->shm_map->find ( alist ( from, to ) );
     double lon2 {iter2->second.lon/10000000.0}, lat2 {iter2->second.lat/10000000.0};

     osmium::unsigned_object_id_type maxstep = graph->palist ( from, to );

     if ( maxstep == 0 )
     {
       maxstep = 1;
     }

     lat1 += step * ( ( lat2 - lat1 ) / maxstep );
     lon1 += step * ( ( lon2 - lon1 ) / maxstep );

     *lo = lon1;
     *la = lat1;
   }

 };

}
}
