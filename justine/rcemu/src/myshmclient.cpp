/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient.cpp
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

#include <myshmclient.hpp>
#include <algorithm>



void justine::sampleclient::MyShmClient::SimulateCarsLoop(void)
{
  std::vector<Cop> cops = server->spawnCops(m_team_name_, num_cops_);
  std::vector<Gangster> gangsters;

  for(;;)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    gangsters = server->getGangsters();
    for(auto cop:cops)
    {
      cop = server->getCopData(cop);


      if(gangsters.size()>0){
        sortByDistance(gangsters, cop);

        NRGVertex v = getAdjacentCrossroad(gangsters[0].to, true);
        NRGVertex v2 = getAdjacentCrossroad(gangsters[0].to, false);

        if(graph->getDistance(gangsters[0].from, graph->getVertexNameMap()[v2]) > graph->getDistance(gangsters[0].to, graph->getVertexNameMap()[v2])) v=v2;

        Path path = graph->DetermineDijkstraPath(cop.to, graph->getVertexNameMap()[v]);

        if(path.size() > 1)
        {
          server->sendRoute(cop, path);
        }
      }

    }
  }
}
