/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file myshmclient-main.cpp
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
 *
 *
 */

#include <myshmclient.hpp>
#include <boost/program_options.hpp>

int main (int argc, char* argv[])
{
  boost::program_options::options_description desc ("Options");
  desc.add_options()
  ("version",   "produce version message")
  ("help,h",    "produce help message")
  ("verbose,v", boost::program_options::bool_switch(), "verbose mode")
  ("shm,s",     boost::program_options::value<std::string>()->default_value("JustineSharedMemory"), "shared memory segment name, required")
  ("port,p",    boost::program_options::value<std::string>()->default_value("10007"), "the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation")
  ("team,t",    boost::program_options::value<std::string>()->default_value("Police"), "team name")
  ("cops,c",    boost::program_options::value<int>()->default_value(10), "the number of cop cars");

  boost::program_options::variables_map vm;

  try
  {
    boost::program_options::store (
      boost::program_options::parse_command_line (argc, argv, desc), vm);

    boost::program_options::notify (vm);
  }
  catch (boost::program_options::error &e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl
              << desc << std::endl;

    return -1;
  }

  if (vm.count( "version" )) {
    std::cout << "Robocar City Emulator and Robocar World Championship, Sample (My) SHM Client" << std::endl
              << "Copyright (C) 2014, 2015 Norbert Bátfai\n" << std::endl
              << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
              << "This is free software: you are free to change and redistribute it." << std::endl
              << "There is NO WARRANTY, to the extent permitted by law." << std::endl;

    return 0;
  }

  if (vm.count ("help")) {
    std::cout << "Robocar City Emulator and Robocar World Championship home page: https://code.google.com/p/robocar-emulator/" << std::endl
              << desc << std::endl
              << "Please report bugs to: nbatfai@gmail.com" << std::endl;

    return 0;
  }

  //these have default values so they're always set
  std::string shm   = vm["shm"].as<std::string>();
  std::string port  = vm["port"].as<std::string>();
  std::string team  = vm["team"].as<std::string>();

  int num_cops = vm["cops"].as<int>();

  bool isVerbose = vm["verbose"].as<bool>();

  // If you use this sample you should add your copyright information here too:
  /*
  std::cout << "This SHM Client program has been modified by <Your Name>" << std::endl
  << "Copyright (C) 2014, 2015 Norbert Bátfai" << std::endl
  << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
  */

  // Do not remove this copyright notice!
  std::cout << "Robocar City Emulator and Robocar World Championship, Sample (My) SHM Client" << std::endl
            << "Copyright (C) 2014, 2015 Norbert Bátfai" << std::endl
            << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << std::endl
            << "This is free software: you are free to change and redistribute it." << std::endl
            << "There is NO WARRANTY, to the extent permitted by law." << std::endl;

  justine::sampleclient::MyShmClient myShmClient {shm.c_str(),
                                                  team,
                                                  port.c_str(),
                                                  num_cops,
                                                  isVerbose};

  try
  {
    myShmClient.SimulateCarsLoop();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
