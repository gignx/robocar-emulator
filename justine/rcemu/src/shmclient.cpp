/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file shmclient.cpp
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

#include <shmclient.hpp>
//#include <trafficlexer.hpp>

 using GraphNodeID = osmium::unsigned_object_id_type;
 using NRGVertex = justine::sampleclient::ExtendedGraph::NRGVertex;    	
 using NRGAdjacentVertexIter = justine::sampleclient::ExtendedGraph::NRGAdjacentVertexIter;
 using NRGVertex = justine::sampleclient::ExtendedGraph::NRGVertex;

     void justine::sampleclient::ShmClient::sortByDistance(std::vector<Gangster> &gangsters, Cop c)
    {
      sortByDistance(gangsters, c.to);
    }

    void justine::sampleclient::ShmClient::sortByDistance(std::vector<Gangster> &gangsters, GraphNodeID t){
        std::sort ( gangsters.begin(), gangsters.end(), 
                  [this, t] ( Gangster x, Gangster y )
                  {return graph->getDistance( t, x.to ) < graph->getDistance ( t, y.to );} 
        );
    }

    NRGVertex justine::sampleclient::ShmClient::getAdjacentCrossroad(GraphNodeID g, bool direction){
    	int db;
    	NRGVertex before, v;
    	std::vector<NRGVertex> circle;
    	v = before = graph->findObject(g);
    	std::pair<NRGAdjacentVertexIter, NRGAdjacentVertexIter> adjacent;
    	db = 0;
    	adjacent = graph->getAdjacentVertices(v);

    	for(;adjacent.first!=adjacent.second;++adjacent.first) {db++;if(db>2){ return v;}}
    		if(db == 2) {
    			NRGAdjacentVertexIter it = adjacent.first;
    			if(direction) before = *(it);
    			else before = *(++it);
    		}

    		do{
    			adjacent = graph->getAdjacentVertices(v);
    			db = 0;
    			for(NRGAdjacentVertexIter item = adjacent.first;item!=adjacent.second;++item) {db++;}
    				if(db==1){before = v; v=*(adjacent.first);}
    			if(db==2){
    				if(*(adjacent.first) == before){before = v;++adjacent.first; v = *adjacent.first; }
    				else{before = v; v = *adjacent.first;}
    			}
    			if(v==before){return v;}

    			//checking for possible circles in the graph (aka infinite loops)
    			for(auto it = circle.begin();it!=circle.end();it++) if(v==*it) {return v;}
    				circle.push_back(v);
    		}while(db<3);
    		return v;  
    	}