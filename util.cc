/*************************************************************************
 * util.cc - This file is part of zcpp.                                  *
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

void
zcpp::expect_read_identifier (std::string &result, const std::string &input,
			      std::size_t &pos, bool first_num)
{
  if (!std::isalpha (input[pos])
      && (!first_num || !std::isdigit (input[pos])) && input[pos] != '_')
    {
      zcpp::error (std::string ("expected identifier, got bad character: ") +
		   input[pos]);
      return;
    }
  while (pos < input.size ()
	 && (std::isalnum (input[pos]) || input[pos] == '_'))
    result += input[pos++];
  if (pos >= input.size ())
    return;
  if (!std::isspace (input[pos]))
    {
      zcpp::error (std::string ("expected identifier, got bad character: ") +
		   input[pos]);
      return;
    }
  while (pos < input.size () && std::isspace (input[pos])
	 && input[pos] != '\n')
    pos++;
}

void
zcpp::expect_read_string (std::string &result, const std::string &input,
			  std::size_t &pos)
{
  if (input[pos] != '"')
    {
      zcpp::error ("expected string literal");
      return;
    }
  pos++;
  while (pos < input.size () && input[pos] != '"')
    {
      if (input[pos] == '\\')
	{
	  if (pos >= input.size () - 1)
	    {
	      error ("unterminated string literal");
	      return;
	    }
	  switch (input[++pos])
	    {
	    case 'n':
	      result += '\n';
	      break;
	    case 't':
	      result += '\t';
	      break;
	    case 'r':
	      result += '\r';
	      break;
	    case 'f':
	      result += '\f';
	      break;
	    case 'v':
	      result += '\v';
	      break;
	    case '\\':
	      result += '\\';
	      break;
	    case '"':
	      result += '"';
	      break;
	    default:
	      warning (std::string ("unrecognized escape sequence: \\") +
		       input[pos]);
	      result += input[pos]; /* Ignore escape sequence */
	    }
	}
      else
	result += input[pos];
      pos++;
    }
  if (pos >= input.size ())
    error ("unterminated string literal");
  else
    pos++;
}
