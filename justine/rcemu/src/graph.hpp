#include <boost/interprocess/managed_shared_memory.hpp>
#include <smartcity.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace justine{

using ID = osmium::unsigned_object_id_type;
using NodeRefGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,boost::property<boost::vertex_name_t, ID>,boost::property<boost::edge_weight_t, int>>;
using NRGVertex = boost::graph_traits<NodeRefGraph>::vertex_descriptor;
using NRGVertexIter = boost::graph_traits<NodeRefGraph>::vertex_iterator;
using NRGEdge = boost::graph_traits<NodeRefGraph>::edge_descriptor;
using NRGEdgeIter = boost::graph_traits<NodeRefGraph>::edge_iterator;
using VertexNameMap = boost::property_map<NodeRefGraph, boost::vertex_name_t>::type;
using VertexIndexMap = boost::property_map<NodeRefGraph, boost::vertex_index_t>::type;
using PredecessorMap = boost::iterator_property_map <NRGVertex*, VertexIndexMap, NRGVertex, NRGVertex&>;
using DistanceMap = boost::iterator_property_map <int*, VertexIndexMap, int, int&>;
using EdgeWeightMap = boost::property_map<NodeRefGraph, boost::edge_weight_t>::type;

class Graph{
public:
	Graph(const char* shm_segment)
	{
		segment = new boost::interprocess::managed_shared_memory (boost::interprocess::open_only, shm_segment);
     	shm_map = segment->find<justine::robocar::shm_map_Type> ("JustineMap").first;
     	BuildGraph();
	}
	~Graph()
	{
		delete nrg;	
	}

	ID palist ( ID from, int to ) const;
	void BuildGraph(void);
	std::vector<ID> DetermineDijkstraPath(ID from, ID to);

	NodeRefGraph* nrg;
	boost::interprocess::offset_ptr<justine::robocar::shm_map_Type> shm_map;
   	boost::interprocess::managed_shared_memory *segment;
	std::map<ID, NRGVertex> nr2v;
};

}