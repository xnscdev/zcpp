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

static void
check_valid_identifier (std::vector <std::string> &args, std::string &value)
{
  if (value.empty () || (!std::isalpha (value[0]) && value[0] != '_'))
    {
      zcpp::error ("expected a valid identifier in parameter list");
      return;
    }
  for (std::size_t i = 1; i < value.size (); i++)
    {
      if (!isalnum (value[i]) && value[i] != '_')
	{
	  zcpp::error ("expected a valid identifier in parameter list");
	  return;
	}
    }
  args.push_back (value);
}

zcpp::macro::macro (std::string value)
{
  func = false;
  sub.push_back (value);
}

zcpp::macro::macro (std::vector <std::string> args, std::string value) :
  args (args)
{
  func = true;
  std::size_t i = 0;
  while (i < value.size ())
    {
      std::string temp;
      if (isalpha (value[i]) || value[i] == '_')
        zcpp::expect_read_identifier (temp, value, i);
      else
	{
	  while (i < value.size () && !isalpha (value[i]) && value[i] != '_')
	    temp += value[i++];
	}
      sub.push_back (temp);
    }
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
      std::string temp;
      for (; i < name.size (); i++)
	{
	  if (std::isspace (name[i]))
	    continue;
	  if (name[i] == ',')
	    {
	      check_valid_identifier (args, temp);
	      temp.clear ();
	    }
	  else
	    temp += name[i];
	}
      if (temp.empty ())
	{
	  zcpp::error ("extraneous comma at end of parameter list");
	  return;
	}
      check_valid_identifier (args, temp);
      if (zcpp::exiting)
	return;
      macro = std::make_unique <zcpp::macro> (args, value);
    }
  else
    macro = std::make_unique <zcpp::macro> (value);

  if (zcpp::defines.find (realname) != zcpp::defines.end ())
    warning ("redefining macro: " + realname);
  zcpp::defines[realname] = std::move (macro);
}
