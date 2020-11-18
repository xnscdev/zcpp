/*************************************************************************
 * comments.cc - This file is part of zcpp.                              *
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

std::string
zcpp::replace_comments_escapes (void)
{
  std::string result;
  int c = zcpp::next_char ();
  int last = '\0';
  int realign = 0;
  while (c != EOF)
    {
      if (c == '/')
	{
	  c = zcpp::next_char ();
	  if (c == '*')
	    {
	      result += ' ';
	      while (last != '*' || c != '/')
		{
		  if (c == EOF)
		    {
		      zcpp::error ("unterminated comment");
		      return result;
		    }
		  else if (c == '\n')
		    result += '\n';
		  else
		    result += ' ';
		  last = c;
		  c = zcpp::next_char ();
		}
	      result += ' ';
	      last = '\0';
	    }
	  else
	    {
	      result += '/';
	      continue;
	    }
	}
      else if (c == '\\')
	{
	  std::string fail;
	  do
	    {
	      c = zcpp::next_char ();
	      fail += c;
	    }
	  while (std::isspace (c) && c != '\n');
	  if (c == EOF)
	    {
	      zcpp::warning ("ignoring backslash at end of file");
	      return result;
	    }
	  if (c == '\n')
	    {
	      c = zcpp::next_char ();
	      realign++;
	      continue;
	    }
	  else
	    result += '\\' + fail;
	}
      else
	{
	  result += c;
	  last = c;
	  if (c == '\n' && realign > 0)
	    {
	      result += zcpp::stamp_file ();
	      realign = 0;
	    }
	}
      c = zcpp::next_char ();
    }
  return result;
}
