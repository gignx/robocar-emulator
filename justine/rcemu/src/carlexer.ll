/*
* @brief Justine - this is a rapid prototype for development of Robocar City Emulator
*
* @file carlexer.ll
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

%option c++
%option noyywrap

%{
#define YY_DECL int justine::robocar::CarLexer::yylex()
#include "carlexer.hpp"
#include <cstdio>
#include <limits>
%}

AUTH  "<auth"
INIT	"<init"
INITG	"<init guided"
STOP "<stop"
WS	[ \t]*
WORD	[^-:\n \t()]{2,}
INT	[0123456789]+
FLOAT	[-.0123456789]+
ROUTE	"<route"
CAR	"<car"
POS	"<pos"
GANGSTERS	"<gangsters"
STAT	"<stat"
DISP	"<disp>"
%%
{DISP}					{
						command_id_ = 0;
}
{AUTH}{WS}{WORD} {
						std::sscanf(yytext, "<auth %s", team_name_);
						command_id_ = 1;
}
{POS}{WS}{INT}{WS}{INT}{WS}{INT}{WS}{INT}	{
					  std::sscanf(yytext, "<pos %d %d %u %u", &auth_code_, &car_id_, &from_, &to_);
						command_id_ = 6;
					}
{CAR}{WS}{INT}{WS}{INT}	{
					  std::sscanf(yytext, "<car %d %d", &auth_code_, &car_id_);
						command_id_ = 3;
					}
{STAT}{WS}{INT}				{
					  std::sscanf(yytext, "<stat %d", &auth_code_);
						command_id_ = 7;
					}
{GANGSTERS}{WS}{INT}			{
					  std::sscanf(yytext, "<gangsters %d", &auth_code_);
						command_id_ = 4;
					}
{ROUTE}{WS}{INT}{WS}{INT}({WS}{INT})*	{
				  int size{0};
				  int ss{0};
				  int sn{0};

				  std::sscanf(yytext, "<route %d %d %d%n", &size, &auth_code_, &car_id_, &sn);
				  ss += sn;
				  for (int i {0}; i < size; ++i)
				  {
				    unsigned int u{0u};
				    std::sscanf(yytext+ss, "%u%n", &u, &sn);
				    route_.push_back(u);
				    ss += sn;
				  }
					command_id_ = 5;
				}
{STOP}{WS}{INT}{WS}{INT} {
					std::sscanf(yytext, "<stop %d %d>", &auth_code_, &car_id_);
					command_id_ = 8;
}
{INIT}{WS}{INT}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init %d %c>", &auth_code_, &car_role_);
				  num_ = 1;
					command_id_ = 2;
				}
{INIT}{WS}{INT}{WS}{INT}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init %d %d %c>", &auth_code_, &num_, &car_role_);
				  if (num_ > 200)
				  {
				    errnumber_ = 1;
				    num_ = 200;
				  }
					command_id_ = 2;
				}
{INITG}{WS}{INT}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init guided %d %c>", &auth_code_, &car_role_);
				  num_ = 1;
				  is_guided_ = true;
					command_id_ = 2;
				}
{INITG}{WS}{INT}{WS}{INT}{WS}("c"|"g")	{
				  std::sscanf(yytext, "<init guided %d %d %c>", &auth_code_, &num_, &car_role_);
				  if (num_ > 200)
				  {
				    errnumber_ = 1;
				    num_ = 200;
				  }
				  is_guided_ = true;
					command_id_ = 2;
				}
.				{;}
%%

int yyFlexLexer::yylex(){return -1;}
