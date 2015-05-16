#ifndef ROBOCAR_CARLEXER_HPP
#define ROBOCAR_CARLEXER_HPP

/**
 * @brief Justine - this is a rapid prototype for development of Robocar City Emulator
 *
 * @file carlexer.hpp
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

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <vector>

namespace justine
{
namespace robocar
{

class CarLexer : public yyFlexLexer
{
public:

  virtual int yylex ();

  friend std::ostream & operator<< (std::ostream & os, CarLexer & cl)
  {
    os << cl.team_name_
       << " "
       << cl.car_role_
       << std::endl;

    return os;
  }

  char* get_team_name()
  {
    return team_name_;
  }
  char get_car_role() const
  {
    return car_role_;
  }
  int get_num() const
  {
    return num_;
  }
  int get_errnumber() const
  {
    return errnumber_;
  }
  bool get_is_guided() const
  {
    return is_guided_;
  }
  int get_command_id() const
  {
    return command_id_;
  }
  int get_auth_code() const
  {
    return auth_code_;
  }
  int get_car_id() const
  {
    return car_id_;
  }
  std::vector<long unsigned int> & get_route ( void )
  {
    return route_;
  }
  unsigned int get_from() const
  {
    return from_;
  }
  unsigned int get_to() const
  {
    return to_;
  }

private:
  int command_id_ {0};
  char team_name_[128];
  int num_ {0};
  char car_role_;
  int errnumber_ {0};
  bool is_guided_ {false};
  std::vector<long unsigned int> route_;
  int car_id_ {0};
  int auth_code_ {0};
  unsigned int from_ {0u};
  unsigned int to_ {0u};
};

}
} // justine::robocar::

#endif
