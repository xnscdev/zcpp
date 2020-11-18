/*************************************************************************
 * define.cc - This file is part of zcpp.                                *
 * Copyright (C) 2020 XNSC                                               *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program. If not, see <https://www.gnu.org/licenses/>. *
 *************************************************************************/

#include "zcpp.hh"

std::map <std::string, std::unique_ptr <zcpp::macro>> zcpp::defines;

/*
class macro
  {
  public:
    std::vector <std::string> args;
    bool func;
    std::vector <std::string> sub;

    explicit macro (std::string value);
    macro (std::vector <std::string> args, std::string value);
  };
*/

zcpp::macro::macro (std::string value)
{
  func = false;
  sub.push_back (value);
}

zcpp::macro::macro (std::vector <std::string> args, std::string value)
{
  func = true;
  sub.push_back (value); /* TODO Parse parameters */
}

#include <iostream>

void
zcpp::define (std::string name, std::string value)
{
  std::string realname;
  std::size_t i;
  for (i = 0; i < name.size () && name[i] != ' '; i++)
    realname += name[i];

  std::unique_ptr <zcpp::macro> macro;
  if (name[i] == ' ')
    {
      i++;
      std::vector <std::string> args;
      macro = std::make_unique <zcpp::macro> (args, value);
    }
  else
    macro = std::make_unique <zcpp::macro> (value);

  if (zcpp::defines.find (realname) != zcpp::defines.end ())
    warning ("redefining macro: " + realname);
  zcpp::defines[realname] = std::move (macro);
}
