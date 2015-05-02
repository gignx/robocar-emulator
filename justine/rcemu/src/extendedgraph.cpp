#include <extendedgraph.hpp>

	using ID = osmium::unsigned_object_id_type;

	void justine::ExtendedGraph::ElliminateCircles(void)
	{
	;
	}

	std::pair<NRGVertexIter, NRGVertexIter> justine::ExtendedGraph::getVertices(void)
	{
		return boost::vertices(*nrg);
	}

	std::pair<NRGEdgeIter, NRGEdgeIter> justine::ExtendedGraph::getEdges(void)
	{
		return boost::edges(*nrg);
	}

	VertexIndexMap justine::ExtendedGraph::getVertexIndexMap(void)
	{
		return boost::get(boost::vertex_index, *nrg);
	}

	VertexNameMap justine::ExtendedGraph::getVertexNameMap(void)
	{
		return boost::get(boost::vertex_name, *nrg);
	}

	NRGVertex justine::ExtendedGraph::findObject(ID id)
	{
		return nr2v[id];
	}

	std::pair<NRGAdjacentVertexIter, NRGAdjacentVertexIter> justine::ExtendedGraph::getAdjacentVertices(NRGVertex v)
	{
		return boost::adjacent_vertices(v, *nrg);
	}

	double justine::ExtendedGraph::getDistance(ID n1, ID n2)
	{
		justine::robocar::shm_map_Type::iterator iter1=shm_map->find ( n1 );
	    justine::robocar::shm_map_Type::iterator iter2=shm_map->find ( n2 );

	    if(iter1==shm_map->end()){
	    	VertexNameMap vertexNameMap = getVertexNameMap();
	    	iter1=shm_map->find ( vertexNameMap[n1] );
	    	iter2=shm_map->find ( vertexNameMap[n2] );
	    }

	    osmium::geom::Coordinates c1 {iter1->second.lon/10000000.0, iter1->second.lat/10000000.0};
	    osmium::geom::Coordinates c2 {iter2->second.lon/10000000.0, iter2->second.lat/10000000.0};

	    return osmium::geom::haversine::distance ( c1, c2 );
	}

	double justine::ExtendedGraph::pathLength(std::vector<ID> path, int accuracy)
	{
		double distance = 0;
		if(path.size()<2) return distance;
		unsigned int step = std::ceil(100/(double)accuracy);
  		if(path.size()>step+1){
			for(unsigned int i=0;i<path.size()-step;i+=step){
				distance+=getDistance(path.at(i), path.at(i+step));
			}
		}
		return distance;
	}

	std::vector<ID> justine::ExtendedGraph::DetermineDijkstraPath(ID from, ID to)
	{
	
    std::vector<NRGVertex> parents ( boost::num_vertices ( *nrg ) );
    std::vector<int> distances ( boost::num_vertices ( *nrg ) );

    VertexIndexMap vertexIndexMap = boost::get ( boost::vertex_index, *nrg );

    PredecessorMap predecessorMap ( &parents[0], vertexIndexMap );
    DistanceMap distanceMap ( &distances[0], vertexIndexMap );

    boost::dijkstra_shortest_paths ( *nrg, nr2v[from],
                                     boost::distance_map ( distanceMap ).predecessor_map ( predecessorMap ) );

    VertexNameMap vertexNameMap = boost::get ( boost::vertex_name, *nrg );

    std::vector<osmium::unsigned_object_id_type> path;

    NRGVertex tov = nr2v[to];
    NRGVertex fromv = predecessorMap[tov];


    while ( fromv != tov )
      {

        NRGEdge edge = boost::edge ( fromv, tov, *nrg ).first;


        path.push_back ( vertexNameMap[boost::target ( edge, *nrg )] );

        tov = fromv;
        fromv = predecessorMap[tov];
      }
    path.push_back ( from );

    std::reverse ( path.begin(), path.end() );

    return path;
	}

	std::vector<ID> operator+ (std::vector<ID> lhs, std::vector<ID> rhs)
	{
		std::vector<ID> v;
		for (std::vector<ID>::iterator i = lhs.begin();i!=lhs.end();++i) { v.push_back(*i); }
		v.pop_back();
		for (std::vector<ID>::iterator i = rhs.begin();i!=rhs.end();++i) { v.push_back(*i); }
		return v;
	}