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

#include <algorithm>
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
  sub.push_back (zcpp::expand (value));
}

zcpp::macro::macro (std::vector <std::string> args, std::string value) :
  args (args)
{
  func = true;
  value = zcpp::expand (value, &args);
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

std::string
zcpp::expand (const std::string &s, std::vector <std::string> *reserved)
{
  std::string result;
  std::size_t i = 0;
  while (i < s.size ())
    {
      if (isalpha (s[i]) || s[i] == '_')
	{
	  std::string name;
	  zcpp::expect_read_identifier (name, s, i);
	  if ((reserved == nullptr
	       || std::find (reserved->begin (),
			     reserved->end (), name) == reserved->end ())
	      && zcpp::defines.find (name) != zcpp::defines.end ())
	    {
	      const std::unique_ptr <zcpp::macro> &macro =
		zcpp::defines[name];
	      if (macro->func)
	        {
		  std::string temp;
		  while (i < s.size () && std::isspace (s[i]))
		    temp += s[i++];
		  if (i >= s.size () || s[i] != '(')
		    result += name + temp;
		  else
		    {
		      std::vector <std::string> args;
		      std::size_t nested_parens = 1;
		      std::string arg;
		      i++;
		      while (nested_parens > 0)
			{
			  if (s[i] == '(')
			    nested_parens++;
			  else if (s[i] == ')')
			    nested_parens--;

			  if (s[i] == ',' && nested_parens == 1)
			    {
			      args.push_back (arg);
			      arg.clear ();
			    }
			  else
			    arg += s[i];
			  if (++i >= s.size () && nested_parens > 0)
			    {
			      zcpp::error ("unmatched parenthesis in "
					   "argument list");
			      return std::string ();
			    }
			}
		      args.push_back (arg);
		      if (args.size () != macro->args.size ())
			{
			  zcpp::error ("passing " +
				       std::to_string (args.size ()) +
				       " arguments to macro " +
				       zcpp::bold (name) + " which takes " +
				       std::to_string (macro->args.size ()) +
				       " arguments");
			  return std::string ();
			}

		      std::map <std::string, std::string> map_sub;
		      for (std::size_t j = 0; j < args.size (); j++)
			map_sub[macro->args[j]] = args[j];

		      for (const std::string &text : macro->sub)
			{
			  bool matched = false;
			  if (!text.empty ()
			      && (std::isalpha (text[0]) || text[0] == '_'))
			    {
			      for (const std::string &param : macro->args)
				{
				  if (text == param)
				    {
				      result += map_sub[text];
				      matched = true;
				      break;
				    }
				}
			      if (matched)
				continue;
			    }
			  result += text;
			}
		    }
		}
	      else
	        result += macro->sub[0];
	    }
	  else
	    result += name;
	}
      else
        result += s[i++];
    }
  return result;
}
