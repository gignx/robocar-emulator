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


#include <smartcity.hpp>
#include <shmclient.hpp>

namespace justine
{
namespace sampleclient
{
using Path = std::vector<osmium::unsigned_object_id_type>;

class MyShmClient : public ShmClient
{
public:

  MyShmClient(
    const char *shm_segment,
    std::string team_name = "Police",
    const char *port      = "10007",
    int num_cops          = 10,
    bool verbose_mode     = false):
      ShmClient(shm_segment, port), num_cops_(num_cops), verbose_mode_(verbose_mode),
      port_(port), m_team_name_(team_name)
  {}

  /**
   * @brief Destructor
   *
   */
  ~MyShmClient(){}

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

  void SimulateCarsLoop(void);


private:
  using Gangster = Server::Gangster;
  using Cop = Server::Cop;


  int num_cops_;
  bool verbose_mode_;
  const char *port_;

  std::string m_team_name_;
};

}
}