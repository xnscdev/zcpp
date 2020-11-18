/*************************************************************************
 * zcpp.hh - This file is part of zcpp.                                  *
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

#ifndef _ZCPP_HH
#define _ZCPP_HH

#include <istream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "config.h"

namespace zcpp
{
  class translation_unit
  {
  public:
    std::string filename;
    std::string output;
    std::istream &file;
    std::size_t line;

    translation_unit (std::string filename, std::istream &file) :
      filename (std::move (filename)), file (file), line (1) {}
  };

  class macro
  {
  public:
    std::vector <std::string> args;
    bool func;
    std::vector <std::string> sub;

    explicit macro (std::string value);
    macro (std::vector <std::string> args, std::string value);
  };

  extern bool exiting;
  extern std::stack <std::unique_ptr <translation_unit>> filestack;
  extern std::map <std::string, std::unique_ptr <macro>> defines;

  std::string replace_comments_escapes (void);

  void init_console (void);
  std::string bold (std::string s);
  void warning (std::string msg);
  void error (std::string msg);

  void define (std::string name, std::string value);

  std::string parse_directives (void);

  int next_char (void);
  void change_line (unsigned long line, std::string *filename);
  std::string preprocess (std::string filename, std::istream &file);

  std::string stamp_file (void);

  void expect_read_identifier (std::string &result, const std::string &input,
			       std::size_t &pos, bool first_num = false,
			       bool end_space = false);
  void expect_read_string (std::string &result, const std::string &input,
			   std::size_t &pos);
}

#endif
