/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file traffic-main.cpp
 * @author  Norbert B�tfai <nbatfai@gmail.com>
 * @version 0.0.10
 *
 * @section LICENSE
 *
 * Copyright(C) 2014 Norbert B�tfai, batfai.norbert@inf.unideb.hu
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

#include <traffic.hpp>
#include <boost/program_options.hpp>

int main(int argc, char* argv[])
{
  int t = std::time(nullptr);

  #ifdef DEBUG
  std::cout << "srand init =  " << t << std::endl;
  #endif

  std::srand(t);

  boost::program_options::options_description desc("Options");
  desc.add_options()
 ("version",      "produce version message")
 ("help,h",       "produce help message")
 ("verbose,v",    boost::program_options::bool_switch(), "verbose mode")
 ("shm,s",        boost::program_options::value<std::string>()->default_value("JustineSharedMemory"), "shared memory segment name")
 ("port,p",       boost::program_options::value<std::string>()->default_value("10007"), "the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation, the default value is 10007")
 ("cars,c",       boost::program_options::value<int>()->default_value(100), "number of the random cars")
 ("delay,d",      boost::program_options::value<int>()->default_value(200), "sleep duration between calculations")
 ("minutes,m",    boost::program_options::value<int>()->default_value(10), "how long does the traffic simulation run for? (-1 means infinite)")
 ("catchdist",    boost::program_options::value<double>()->default_value(15.5), "the catch distance of cop cars")
 ("traffictype",  boost::program_options::value<std::string>()->default_value("NORMAL"), "traffic type = NORMAL|ANTS|ANTS_RND|ANTS_RERND|ANTS_MRERND");

  std::string traffic_type;

  boost::program_options::variables_map vm;

  try
  {
    boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);

    boost::program_options::notify(vm);

    std::string enabled_traffic_types[] =
      { "NORMAL", "ANTS", "ANTS_RND", "ANTS_RERND", "ANTS_MRERND" };

    traffic_type.assign(vm["traffictype"].as<std::string>());

    if (std::end(enabled_traffic_types) ==
        std::find(std::begin(enabled_traffic_types), std::end(enabled_traffic_types), traffic_type))
    {
      throw boost::program_options::validation_error(
        boost::program_options::validation_error::invalid_option_value);
    }
  }
  catch (boost::program_options::error &e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl
              << desc << std::endl;

    return -1;
  }

  if(vm.count("version"))
  {
    std::cout << "Robocar City Emulator and Robocar World Championship, Traffic Server" << std::endl
              << "Copyright(C) 2014, 2015 Norbert B�tfai\n" << std::endl
              << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
              << "This is free software: you are free to change and redistribute it." << std::endl
              << "There is NO WARRANTY, to the extent permitted by law." << std::endl;

    return 0;
  }

  if(vm.count("help"))
  {
    std::cout << "Robocar City Emulator and Robocar World Championship home page: https://code.google.com/p/robocar-emulator/" << std::endl
              << desc << std::endl
              << "Please report bugs to: nbatfai@gmail.com" << std::endl;
    return 0;
  }

  std::string shm   = vm["shm"].as<std::string>();

  std::string port  = vm["port"].as<std::string>();

  int num_cars      = vm["cars"].as<int>();

  int delay         = vm["delay"].as<int>();

  int minutes       = vm["minutes"].as<int>();

  double catchdist  = vm["catchdist"].as<double>();

  bool verbose_mode = vm["verbose"].as<bool>();

  justine::robocar::TrafficType type;

  if(traffic_type == "ANTS_RND")
    type = justine::robocar::TrafficType::ANT_RND;
  else if(traffic_type == "ANTS_RERND")
    type = justine::robocar::TrafficType::ANT_RERND;
  else if(traffic_type == "ANTS_MRERND")
    type = justine::robocar::TrafficType::ANT_MRERND;
  else if(traffic_type == "ANTS")
    type = justine::robocar::TrafficType::ANT;
  else
    type = justine::robocar::TrafficType::NORMAL;

  justine::robocar::Traffic traffic {num_cars,
                                     shm.c_str(),
                                     catchdist,
                                     type,
                                     delay,
                                     minutes,
                                     verbose_mode};

  try
  {
    boost::asio::io_service io_service;

    traffic.start_server(io_service, std::atoi(port.c_str()));
  }
  catch(std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
