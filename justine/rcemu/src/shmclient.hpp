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
 namespace justine
 {
  namespace sampleclient
  {

    using Cop = Server::Cop;
    using Gangster = Server::Gangster;
    using GraphNodeID = osmium::unsigned_object_id_type;
    using NRGVertex = ExtendedGraph::NRGVertex;

    class ShmClient
    {
    public:
     Server* server;
     ExtendedGraph* graph;

     ShmClient ( const char * shm_segment, const char* port)
     {
       server = new Server (port);
       graph = new ExtendedGraph (shm_segment);
     }

     ~ShmClient()
     {
      delete server;
      delete graph;
    }

    void sortByDistance(std::vector<Gangster> &gangsters, Cop c);
    void sortByDistance(std::vector<Gangster> &gangsters, GraphNodeID t);
    NRGVertex getAdjacentCrossroad(GraphNodeID g, bool direction);
  };

}
}
