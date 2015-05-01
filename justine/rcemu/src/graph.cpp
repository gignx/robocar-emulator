#include <graph.hpp>

using GraphNodeID = osmium::unsigned_object_id_type;

GraphNodeID justine::sampleclient::Graph::palist(GraphNodeID from, int to) const
{
  justine::robocar::shm_map_Type::iterator iter=shm_map->find (from);

  return iter->second.m_palist[to];
}

void justine::sampleclient::Graph::BuildGraph(void)
{
  nrg = new NodeRefGraph();
  std::map<GraphNodeID, NRGVertex>::iterator it;
  NRGVertex f, t;

  for (justine::robocar::shm_map_Type::iterator iter = shm_map->begin();
       iter!=shm_map->end();
       ++iter)
  {
    GraphNodeID u = iter->first;
    it = nr2v.find(u);

    if (it == nr2v.end()) {
      f = boost::add_vertex(u, *nrg);
      nr2v[u] = f;
    }
    else {
      f = it->second;
    }

	  for (justine::robocar::uint_vector::iterator noderefi = iter->second.m_alist.begin();
         noderefi != iter->second.m_alist.end();
         ++noderefi)
    {
      it = nr2v.find(*noderefi);

	    if (it == nr2v.end()) {
	      t = boost::add_vertex(*noderefi, *nrg);
	      nr2v[*noderefi] = t;
      }
	    else {
        t = it->second;
      }

	    int to = std::distance(iter->second.m_alist.begin(), noderefi);
	    boost::add_edge(f, t, palist(iter->first, to), *nrg);
	  }
  }
}


std::vector<GraphNodeID> justine::sampleclient::Graph::DetermineDijkstraPath(GraphNodeID from, GraphNodeID to)
{
  	std::vector<NRGVertex> parents(boost::num_vertices(*nrg));
  	std::vector<int> distances(boost::num_vertices(*nrg));

  	VertexIndexMap vertexIndexMap = boost::get(boost::vertex_index, *nrg);

  	PredecessorMap predecessorMap(&parents[0], vertexIndexMap);

  	DistanceMap distanceMap(&distances[0], vertexIndexMap);

  	boost::dijkstra_shortest_paths(*nrg, nr2v[from],
                                   boost::distance_map(distanceMap).predecessor_map(predecessorMap));

  	VertexNameMap vertexNameMap = boost::get(boost::vertex_name, *nrg);

 	  std::vector<GraphNodeID> path;

  	NRGVertex tov = nr2v[to];
  	NRGVertex fromv = predecessorMap[tov];

  	while(fromv != tov)
  	{
    	NRGEdge edge = boost::edge(fromv, tov, *nrg).first;
    	path.push_back(vertexNameMap[boost::target(edge, *nrg)]);
    	tov = fromv;
    	fromv = predecessorMap[tov];
  	}

  	path.push_back(from);
  	std::reverse(path.begin(), path.end());

  	return path;
}
