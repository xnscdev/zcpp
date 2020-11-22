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

#include <cstring>
#include <fstream>
#include "zcpp.hh"

#define input zcpp::filestack.top ()->output

static std::stack <bool> ifstack;
static std::string ifdef_names[] = {"#ifndef", "#ifdef"};

static void
parse_define (const std::string &content)
{
  std::string name;
  std::size_t i = 0;
  if (!ifstack.top ())
    return;
  if (content.empty ())
    {
      zcpp::error ("#define directive missing macro name");
      return;
    }
  zcpp::expect_read_identifier (name, content, i);
  if (i >= content.size ())
    {
      zcpp::define (name, std::string ());
      return;
    }

  if (content[i] == '(')
    {
      i++;
      name += ' ';
      while (i < content.size () && content[i] != ')')
	name += content[i++];
      if (content[i] != ')')
	{
	  zcpp::error ("unterminated function macro parameter list");
	  return;
	}
      i++;
    }
  if (!std::isspace (content[i]))
    {
      zcpp::error ("macro name must be a valid identifier");
      return;
    }
  while (i < content.size () && std::isspace (content[i]))
    i++;
  zcpp::define (name, content.substr (i));
}

static void
parse_else (const std::string &content)
{
  if (!content.empty ())
    zcpp::warning ("ignoring extra tokens at end of #else directive");
  if (ifstack.size () > 1)
    ifstack.top () ^= true;
  else
    zcpp::error ("unmatched #else directive");
}

static void
parse_endif (const std::string &content)
{
  if (!content.empty ())
    zcpp::warning ("ignoring extra tokens at end of #endif directive");
  if (ifstack.size () > 1)
    ifstack.pop ();
  else
    zcpp::error ("unmatched #endif directive");
}

static void
parse_if (const std::string &content)
{
  if (!ifstack.top ())
    return;
  if (content.empty ())
    zcpp::error ("expected expression after #if directive");
  else
    ifstack.push (zcpp::ifexpr::eval (content));
}

static void
parse_ifdef (bool truth, const std::string &content)
{
  std::string name;
  std::size_t i = 0;
  if (!ifstack.top ())
    return;
  zcpp::expect_read_identifier (name, content, i, false, true);
  if (i < content.size ())
    zcpp::warning (std::string ("ignoring extra tokens at end of ") +
		   ifdef_names[(int) truth] + " directive");
  ifstack.push (truth == (zcpp::defines.find (name) != zcpp::defines.end ()));
}

static void
parse_include (std::string &result, const std::string &content)
{
  if (content.empty () || (content[0] != '<' && content[0] != '"'))
    {
      zcpp::error ("expected " + zcpp::bold ("<filename>") + " or " +
		   zcpp::bold ("\"filename\"") + " after #include directive");
      return;
    }
  char end = content[0] == '<' ? '>' : '"';

  std::size_t i;
  std::string path;
  for (i = 1; i < content.size () && content[i] != end; i++)
    path += content[i];
  if (i++ >= content.size ())
    {
      zcpp::error ("#include directive missing terminating " +
		   zcpp::bold (std::string (end, 1)) + " character");
      return;
    }

  while (i < content.size () && std::isspace (content[i]))
    i++;
  if (i < content.size ())
    zcpp::warning ("ignoring extra tokens at end of #include directive");

  for (const zcpp::include &dir : zcpp::includes)
    {
      if (end == '>' && dir.type == zcpp::include::quote)
	continue;
      std::string filename = dir.path + '/' + path;
      std::ifstream file (filename);
      if (!file.good ())
	continue;
      std::string output = zcpp::preprocess (filename, file);
      if (zcpp::exiting)
	return;
      result += output;
      result += zcpp::stamp_file ();
      return;
    }
  zcpp::error ("failed to find " + zcpp::bold (path) + ": " +
	       std::strerror (errno));
}

static void
parse_line (const std::string &content)
{
  unsigned long line = 0;
  std::size_t i = 0;
  if (!ifstack.top ())
    return;
  while (i < content.size () && std::isdigit (content[i]))
    {
      line *= 10;
      line += content[i++] - '0';
    }
  line--;
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
parse_undef (const std::string &content)
{
  std::string name;
  std::size_t i = 0;
  if (!ifstack.top ())
    return;
  if (content.empty ())
    {
      zcpp::error ("#undef directive missing macro name");
      return;
    }
  zcpp::expect_read_identifier (name, content, i);
  zcpp::defines.erase (name);

  while (i < content.size () && std::isspace (content[i]))
    i++;
  if (i < content.size ())
    zcpp::warning ("ignoring extra tokens at end of #undef directive");
}

static void
parse_directive (std::string &result, const std::string &name,
		 const std::string &content)
{
  if (name == "define")
    parse_define (content);
  else if (name == "else")
    parse_else (content);
  else if (name == "endif")
    parse_endif (content);
  else if (name == "error")
    zcpp::error ("#error " + content);
  else if (name == "if")
    parse_if (content);
  else if (name == "ifdef" || name == "ifndef")
    parse_ifdef (name == "ifdef", content);
  else if (name == "include")
    parse_include (result, content);
  else if (name == "line")
    parse_line (content);
  else if (name == "undef")
    parse_undef (content);
  else if (name == "warning")
    zcpp::warning ("#warning " + content);
  else
    {
      if (!ifstack.top ())
	return;
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
      zcpp::filestack.top ()->line--;
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
  zcpp::filestack.top ()->line = 0; /* Reset line numbering */
  ifstack.push (true);
  for (std::size_t pos = 0; pos < input.size (); pos++)
    {
      if (last == '\n' && input[pos] == '#')
	{
	  std::string name;
	  do
	    pos++;
	  while (std::isspace (input[pos]) && input[pos] != '\n');
	  if (input[pos] == '\n') /* Null directive */
	    {
	      result += '\n';
	      goto end;
	    }
	  expect_read_identifier (name, input, pos, true);
	  if (pos < input.size ())
	    {
	      if (!std::isspace (input[pos]))
		{
		  error ("expected an identifier in directive name");
		  while (pos < input.size () && input[pos] != '\n')
		    pos++;
		  continue;
		}
	      while (pos < input.size () && std::isspace (input[pos])
		     && input[pos] != '\n')
		pos++;
	    }

	  std::string content;
	  while (pos < input.size () && input[pos] != '\n')
	    content += input[pos++];

	  parse_directive (result, name, content);
	  result += '\n';
	}
      else if (ifstack.top ())
	{
	  std::string temp;
	  while (pos < input.size () && input[pos] != '\n')
	    temp += input[pos++];
	  result += zcpp::expand (temp) + '\n';
	}

    end:
      if (input[pos] == '\n')
	zcpp::filestack.top ()->line++;
      if (!std::isspace (input[pos]) || input[pos] == '\n')
	last = input[pos];
    }
  if (ifstack.size () > 1)
    zcpp::error ("unmatched #if, expected " + zcpp::bold ("#endif"));
  return result;
}
