/*************************************************************************
 * directive.cc - This file is part of zcpp.                             *
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

#define input zcpp::filestack.top ()->output

static void
parse_line (const std::string &content)
{
  unsigned long line = 0;
  std::size_t i = 0;
  while (i < content.size () && std::isdigit (content[i]))
    {
      line *= 10;
      line += content[i++] - '0';
    }
  if (i >= content.size ())
    {
      zcpp::change_line (line, nullptr);
      return;
    }
  if (!std::isspace (content[i]))
    {
      zcpp::error ("bad line number in #line directive");
      return;
    }
  while (i < content.size () && std::isspace (content[i]))
    i++;
  if (i >= content.size ())
    {
      zcpp::change_line (line, nullptr);
      return;
    }

  std::string filename;
  zcpp::expect_read_string (filename, content, i);
  zcpp::change_line (line, &filename);
}

static void
parse_directive (std::string &result, const std::string &name,
		 const std::string &content)
{
  if (name == "error")
    zcpp::error ("#error " + content);
  else if (name == "line")
    parse_line (content);
  else if (name == "warning")
    zcpp::warning ("#warning " + content);
  else
    {
      unsigned long line;
      for (char c : name)
	{
	  if (!std::isdigit (c))
	    goto err;
	}
      line = std::stoul (name);
      if (content.empty ())
	{
	  zcpp::change_line (line, nullptr);
	  result += "# " + std::to_string (line) + '\n';
	}
      else
	{
	  std::string filename;
	  std::size_t i = 0;
	  zcpp::expect_read_string (filename, content, i);
	  zcpp::change_line (line, &filename);
	  result += "# " + std::to_string (line) + " \"" + filename + '"';
	}
      return;

    err:
      zcpp::error ("unrecognized preprocessor directive: #" + name);
    }
}

std::string
zcpp::parse_directives (void)
{
  std::string result;
  int last = '\n'; /* Begin accepting directives */
  for (std::size_t pos = 0; pos < input.size (); pos++)
    {
      if (last == '\n' && input[pos] == '#')
	{
	  std::string name;
	  do
	    pos++;
	  while (std::isspace (input[pos]) && input[pos] != '\n');
	  if (input[pos] == '\n')
	    goto end; /* Null directive */
	  expect_read_identifier (name, input, pos, true);

	  std::string content;
	  while (pos < input.size () && input[pos] != '\n')
	    content += input[pos++];

	  parse_directive (result, name, content);
	}
    end:
      result += input[pos];

      if (!std::isspace (input[pos]) || input[pos] == '\n')
	last = input[pos];
    }
  return result;
}
