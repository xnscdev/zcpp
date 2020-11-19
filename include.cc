/*************************************************************************
 * include.cc - This file is part of zcpp.                               *
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

std::vector <zcpp::include> zcpp::includes;

const int zcpp::include::quote = 0;
const int zcpp::include::regular = 1;
const int zcpp::include::system = 2;
const int zcpp::include::dirafter = 3;

void
zcpp::add_includedir (std::string path, int type)
{
  if (!includes.empty ())
    {
      for (std::vector <zcpp::include>::iterator i = zcpp::includes.begin ();
	   i != zcpp::includes.end (); i++)
	{
	  if (i->type > type)
	    {
	      zcpp::includes.insert (i, zcpp::include (path, type));
	      return;
	    }
	}
    }
  zcpp::includes.emplace_back (path, type);
}

std::string
zcpp::stamp_file (void)
{
  std::unique_ptr <zcpp::translation_unit> &unit = zcpp::filestack.top ();
  return "# " + std::to_string (unit->line) + " \"" + unit->filename + "\"\n";
}
