/*************************************************************************
 * console.cc - This file is part of zcpp.                               *
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

#include <iostream>
#include <unistd.h>
#include "zcpp.hh"

static bool tty;

bool zcpp::exiting;

void
zcpp::init_console (void)
{
  tty = isatty (STDERR_FILENO);
}

std::string
zcpp::bold (std::string s)
{
  if (tty)
    return "\033[1m" + s + "\033[0m";
  return '\'' + s + '\'';
}

void
zcpp::warning (std::string msg)
{
  if (tty)
    std::cerr << "\033[35;1mwarning:\033[37m ";
  else
    std::cerr << "warning: ";
  std::cerr << zcpp::filestack.top ()->filename << ':'
	    << zcpp::filestack.top ()->line << ':';
  if (tty)
    std::cerr << "\033[0m ";
  else
    std::cerr << ' ';
  std::cerr << msg << std::endl;
}

void
zcpp::error (std::string msg)
{
  if (tty)
    std::cerr << "\033[31;1merror:\033[37m ";
  else
    std::cerr << "error: ";
  std::cerr << zcpp::filestack.top ()->filename << ':'
	    << zcpp::filestack.top ()->line << ':';
  if (tty)
    std::cerr << "\033[0m ";
  else
    std::cerr << ' ';
  std::cerr << msg << std::endl;
  zcpp::exiting = true;
}
