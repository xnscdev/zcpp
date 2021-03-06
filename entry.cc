/*************************************************************************
 * entry.cc - This file is part of zcpp.                                 *
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

#include <sstream>
#include "zcpp.hh"

std::stack <std::unique_ptr <zcpp::translation_unit>> zcpp::filestack;

int
zcpp::next_char (void)
{
  int c = zcpp::filestack.top ()->file.get ();
  if (c == '\n')
    zcpp::filestack.top ()->line++;
  return c;
}

void
zcpp::change_line (unsigned long line, std::string *filename)
{
  zcpp::filestack.top ()->line = line;
  if (filename != nullptr)
    zcpp::filestack.top ()->filename = std::string (*filename);
}

std::string
zcpp::preprocess (std::string filename, std::istream &file)
{
  zcpp::filestack.push (std::make_unique <zcpp::translation_unit> (filename,
								   file));
  zcpp::filestack.top ()->output = zcpp::stamp_file ();
  zcpp::filestack.top ()->output += zcpp::replace_comments_escapes ();

  std::string stage2 = zcpp::parse_directives ();
  zcpp::filestack.top ()->output = std::string (stage2);

  std::string result = zcpp::filestack.top ()->output;
  zcpp::filestack.pop ();
  return result;
}
