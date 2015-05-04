#ifndef ROBOCAR_MYSHMCLIENT_HPP
#define ROBOCAR_MYSHMCLIENT_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient.hpp
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

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <smartcity.hpp>
#include <car.hpp>

#include <cstdlib>
#include <iterator>

#include <boost/asio.hpp>

#include <limits>

#include <memory>

#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

#include <shmclient.hpp>

#include <algorithm>

#ifdef DEBUG
#include <iostream>
#include <chrono>
#endif

#include <boost/graph/graphviz.hpp>
#include <fstream>
#include <string.h>

namespace justine
{
namespace sampleclient
{

// 512 KiB, should be enough for all messages received and sent
constexpr int kMaxBufferLen = 524288;

using NodeRefGraph    =
        boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
          boost::property<boost::vertex_name_t, osmium::unsigned_object_id_type>,
          boost::property<boost::edge_weight_t, int>>;

using NRGVertex       =
        boost::graph_traits<NodeRefGraph>::vertex_descriptor;

using NRGVertexIter   =
        boost::graph_traits<NodeRefGraph>::vertex_iterator;

using NRGEdge         =
        boost::graph_traits<NodeRefGraph>::edge_descriptor;

using NRGEdgeIter     =
        boost::graph_traits<NodeRefGraph>::edge_iterator;


using VertexNameMap   =
        boost::property_map<NodeRefGraph, boost::vertex_name_t>::type;

using VertexIndexMap  =
        boost::property_map<NodeRefGraph, boost::vertex_index_t>::type;

using PredecessorMap  =
        boost::iterator_property_map <NRGVertex*, VertexIndexMap,
          NRGVertex, NRGVertex&>;

using DistanceMap     =
        boost::iterator_property_map <int*, VertexIndexMap, int, int&>;

using EdgeWeightMap   =
        boost::property_map<NodeRefGraph, boost::edge_weight_t>::type;


/**
 * @brief A sample class used for testing the routing algorithms.
 *
 * This sample class shows how client agents can create BGL graph from data can be found in the shared memory.
 *
 * @author Norbert Bátfai
 * @date Dec. 7, 2014
 */
class MyShmClient : public ShmClient
{
public:

  /**
   * @brief This constructor creates the BGL graph from the map graph.
   * @param shm_segment the shared memory object name
   *
   * This constructor creates the BGL graph from the map graph that
   * is placed in the shared memory segment.
   */

  MyShmClient(
    const char *shm_segment,
    std::string team_name = "Police",
    const char *port      = "10007",
    int num_cops          = 10,
    bool verbose_mode     = false):
      ShmClient(shm_segment), num_cops_(num_cops),
      auth_code_(0), is_authenticated_(false),
      verbose_mode_(verbose_mode),
      port_(port), m_team_name_(team_name)
  {
    BuildGraph();

    #ifdef DEBUG
    PrintVertices(10);
    PrintEdges(10);
    std::fstream graph_log(team_name+".dot" , std::ios_base::out);
    boost::write_graphviz(graph_log, *nr_graph_);
    #endif
  }

  /**
   * @brief Destructor
   *
   */
  ~MyShmClient()
  {
    delete nr_graph_;
  }

  // Getters
  int get_num_cops(void)
  {
    return this->num_cops_;
  }

  bool get_verbose_mode(void)
  {
    return this->verbose_mode_;
  }

  std::string get_team_name(void)
  {
    return this->m_team_name_;
  }

  const char* get_port(void)
  {
    return this->port_;
  }

  /**
   * @brief This function starts the client.
   * @param io_service
   * @param port the TCP port of the traffic server
   *
   * This method does the following: retrieves a value from shared memory,
   * then establishes a connection with the traffic server, finally
   * sends some client commands.
   */
  void SimulateCarsLoop(void);

  // The following functions serve testing purposes

  /**
   * @brief This function counts the number of vertices and number of edges in the map graph.
   * @param [out] sum_edges the number of edges
   * @return the number of vertices
   *
   * This function counts the number of vertices and number of edges in the map graph that
   * is placed in the shared memory segment.
   */
   int get_num_vertices(int &sum_edges);

  /**
   * @brief This function prints the edges of the map graph.
   * @param more the maximum number of printed items
   *
   */
  void PrintEdges(unsigned more);

  /**
   * @brief This function prints the vertices of the map graph.
   * @param more the maximum number of printed items
   *
   */
  void PrintVertices(unsigned more);

private:
  struct SmartCar
  {
    int id;
    unsigned from;
    unsigned to;
    unsigned step;
  };

  using Gangster = SmartCar;
  using Cop = int;

  int num_cops_;
  int auth_code_;
  bool is_authenticated_;
  bool verbose_mode_;
  const char *port_;

  std::string m_team_name_;

  NodeRefGraph *nr_graph_;

  std::vector<Cop> cops_;

  /**
   * Helper structure to create the BGL graph.
   */
  std::map<osmium::unsigned_object_id_type, NRGVertex> nr2v;


  // For server responses
  void LogMessage(const char *command, char *response_buffer);
  // For internal messages
  void LogMessage(std::string &&msg);

  /**
   * @brief This function creates the BGL graph.
   * @return he pointer of the created BGL graph.
   *
   */
  void BuildGraph(void);

  /**
   * @brief This function solves the shortest path problem using Dijkstra algorithm.
   * @param source the source node
   * @param target the target node
   * @return the shortest path between nodes source and target
   *
   * This function determines the shortest path from the source node to the target node.
   */
  std::vector<osmium::unsigned_object_id_type> DetermineDijkstraPath(
    osmium::unsigned_object_id_type from,
    osmium::unsigned_object_id_type to);

  int InitializeCops(
    boost::asio::ip::tcp::socket & socket);

  /*void pos(
    boost::asio::ip::tcp::socket & socket, int id);*/

  void AcquireCarDataFromServer(
    boost::asio::ip::tcp::socket & socket, int id,
    unsigned *f, unsigned *t, unsigned* s);

  std::vector<Gangster> AcquireGangstersFromServer(
    boost::asio::ip::tcp::socket & socket,
    int id, osmium::unsigned_object_id_type cop);

  void Authenticate(boost::asio::ip::tcp::socket &socket);

  void SendRouteToServer(
    boost::asio::ip::tcp::socket & socket, int id,
    std::vector<osmium::unsigned_object_id_type> &);
};

}
} // justine::sampleclient::

#endif // ROBOCAR_SHMCLIENT_HPP
