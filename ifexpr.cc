/*************************************************************************
 * ifexpr.cc - This file is part of zcpp.                                *
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

#include <climits>
#include "zcpp.hh"

const int zcpp::ifexpr::binary::plus = 0;
const int zcpp::ifexpr::binary::minus = 1;
const int zcpp::ifexpr::binary::multiply = 2;
const int zcpp::ifexpr::binary::divide = 3;
const int zcpp::ifexpr::binary::bitwise_and = 4;
const int zcpp::ifexpr::binary::bitwise_or = 5;
const int zcpp::ifexpr::binary::bitwise_xor = 6;
const int zcpp::ifexpr::binary::left_shift = 7;
const int zcpp::ifexpr::binary::right_shift = 8;
const int zcpp::ifexpr::binary::logical_and = 9;
const int zcpp::ifexpr::binary::logical_or = 10;
const int zcpp::ifexpr::binary::equal = 11;
const int zcpp::ifexpr::binary::not_equal = 12;
const int zcpp::ifexpr::binary::less = 13;
const int zcpp::ifexpr::binary::less_equal = 14;
const int zcpp::ifexpr::binary::greater = 15;
const int zcpp::ifexpr::binary::greater_equal = 16;

const int zcpp::ifexpr::unary::plus = 32;
const int zcpp::ifexpr::unary::minus = 33;
const int zcpp::ifexpr::unary::bitwise_not = 34;
const int zcpp::ifexpr::unary::logical_not = 35;

static std::map <int, int> operator_precedences = {
  {zcpp::ifexpr::binary::logical_or, 1},
  {zcpp::ifexpr::binary::logical_and, 2},
  {zcpp::ifexpr::binary::bitwise_or, 3},
  {zcpp::ifexpr::binary::bitwise_xor, 4},
  {zcpp::ifexpr::binary::bitwise_and, 5},
  {zcpp::ifexpr::binary::equal, 6},
  {zcpp::ifexpr::binary::not_equal, 6},
  {zcpp::ifexpr::binary::less, 7},
  {zcpp::ifexpr::binary::less_equal, 7},
  {zcpp::ifexpr::binary::greater, 7},
  {zcpp::ifexpr::binary::greater_equal, 7},
  {zcpp::ifexpr::binary::left_shift, 8},
  {zcpp::ifexpr::binary::right_shift, 8},
  {zcpp::ifexpr::binary::plus, 9},
  {zcpp::ifexpr::binary::minus, 9},
  {zcpp::ifexpr::binary::multiply, 10},
  {zcpp::ifexpr::binary::divide, 10},
  {zcpp::ifexpr::unary::plus, 11},
  {zcpp::ifexpr::unary::minus, 11},
  {zcpp::ifexpr::unary::bitwise_not, 11},
  {zcpp::ifexpr::unary::logical_not, 11}
};

static unsigned long long numval;
static std::string strval;

enum class token
  {
    null,
    integer,
    identifier,
    operation,
    misc
  };

static token
next_token (std::size_t &i, const std::string &s)
{
  while (i < s.size () && std::isspace (s[i]))
    i++;
  if (i >= s.size ())
    return token::null;

  if (std::isdigit (s[i]))
    {
      numval = 0;
      do
	{
	  numval *= 10;
	  numval += s[i++] - '0';
	}
      while (i < s.size () && std::isdigit (s[i]));
      return token::integer;
    }
  else if (std::isalpha (s[i]) || s[i] == '_')
    {
      strval.clear ();
      do
	strval += s[i++];
      while (i < s.size () && (std::isalnum (s[i]) || s[i] == '_'));
      return token::identifier;
    }

  switch (s[i])
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
    case '~':
      strval = std::string (1, s[i++]);
      return token::operation;
    case '<':
    case '>':
      strval = std::string (1, s[i++]);
      if (i < s.size ())
	{
	  if (s[i] == '=')
	    {
	      strval += '=';
	      i++;
	    }
	  else if (s[i] == s[i - 1])
	    strval += s[i++];
	}
      return token::operation;
    case '&':
    case '|':
      strval = std::string (1, s[i++]);
      if (i < s.size () && s[i] == s[i - 1])
	strval += s[i++];
      return token::operation;
    case '=':
    case '!':
      strval = std::string (1, s[i++]);
      if (i < s.size () && s[i] == '=')
	{
	  strval += '=';
	  i++;
	}
      return token::operation;
    default:
      strval = std::string (1, s[i++]);
      return token::misc;
    }
}

static std::unique_ptr <zcpp::ifexpr::defined>
parse_defined (std::size_t &i, const std::string &s)
{
  switch (next_token (i, s))
    {
    case token::identifier:
      return std::make_unique <zcpp::ifexpr::defined> (strval);
    case token::misc:
      if (strval == "(")
	{
	  if (next_token (i, s) != token::identifier)
	    {
	      zcpp::error ("expected an identifier expression after " +
			   zcpp::bold ("defined"));
	      return nullptr;
	    }
	  std::unique_ptr <zcpp::ifexpr::defined> result =
	    std::make_unique <zcpp::ifexpr::defined> (strval);
	  if (next_token (i, s), strval != ")")
	    {
	      zcpp::error ("unmatched parentheses after " +
			   zcpp::bold ("defined"));
	      return nullptr;
	    }
	  return result;
	}
    default:
      zcpp::error ("expected an identifier expression after " +
		   zcpp::bold ("defined"));
      return nullptr;
    }
}

static std::unique_ptr <zcpp::ifexpr::expr>
parse_primary (std::size_t &i, const std::string &s)
{
  switch (next_token (i, s))
    {
    case token::null:
      return nullptr;
    case token::integer:
      return std::make_unique <zcpp::ifexpr::number> (numval,
						      numval <= LLONG_MAX);
    case token::identifier:
      if (strval == "defined")
	return parse_defined (i, s);
      return std::make_unique <zcpp::ifexpr::number> (0, true);
    default:
      zcpp::error ("unexpected character " + zcpp::bold (strval) +
		   " in #if expression");
      return nullptr;
    }
}

static std::unique_ptr <zcpp::ifexpr::expr>
parse_expr (std::size_t &i, const std::string &s)
{
  return parse_primary (i, s);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator+ (const zcpp::ifexpr::number &rhs)
{
  bool result_unsigned = !is_signed || !rhs.is_signed;
  if (result_unsigned)
    return zcpp::ifexpr::number (raw + rhs.raw, false);
  return zcpp::ifexpr::number ((long long) raw + (long long) rhs.raw, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator- (const zcpp::ifexpr::number &rhs)
{
  bool result_unsigned = !is_signed || !rhs.is_signed;
  if (result_unsigned)
    return zcpp::ifexpr::number (raw - rhs.raw, false);
  return zcpp::ifexpr::number ((long long) raw - (long long) rhs.raw, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator* (const zcpp::ifexpr::number &rhs)
{
  bool result_unsigned = !is_signed || !rhs.is_signed;
  if (result_unsigned)
    return zcpp::ifexpr::number (raw * rhs.raw, false);
  return zcpp::ifexpr::number ((long long) raw * (long long) rhs.raw, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator/ (const zcpp::ifexpr::number &rhs)
{
  bool result_unsigned = !is_signed || !rhs.is_signed;
  if (result_unsigned)
    return zcpp::ifexpr::number (raw / rhs.raw, false);
  return zcpp::ifexpr::number ((long long) raw / (long long) rhs.raw, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator& (const zcpp::ifexpr::number &rhs)
{
  return zcpp::ifexpr::number (raw & rhs.raw, is_signed && rhs.is_signed);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator| (const zcpp::ifexpr::number &rhs)
{
  return zcpp::ifexpr::number (raw | rhs.raw, is_signed && rhs.is_signed);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator^ (const zcpp::ifexpr::number &rhs)
{
  return zcpp::ifexpr::number (raw ^ rhs.raw, is_signed && rhs.is_signed);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator<< (const zcpp::ifexpr::number &rhs)
{
  return zcpp::ifexpr::number (raw << rhs.raw, is_signed && rhs.is_signed);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator>> (const zcpp::ifexpr::number &rhs)
{
  return zcpp::ifexpr::number (raw >> rhs.raw, is_signed && rhs.is_signed);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator&& (const zcpp::ifexpr::number &rhs)
{
  if (raw != 0 && rhs.raw != 0)
    return zcpp::ifexpr::number (1, true);
  return zcpp::ifexpr::number (0, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::operator|| (const zcpp::ifexpr::number &rhs)
{
  if (raw != 0 || rhs.raw != 0)
    return zcpp::ifexpr::number (1, true);
  return zcpp::ifexpr::number (0, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::expr::eval (void)
{
  return zcpp::ifexpr::number (0, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::binary::eval (void)
{
  switch (op)
    {
    case zcpp::ifexpr::binary::plus:
      return lhs.eval () + rhs.eval ();
    case zcpp::ifexpr::binary::minus:
      return lhs.eval () - rhs.eval ();
    case zcpp::ifexpr::binary::multiply:
      return lhs.eval () * rhs.eval ();
    case zcpp::ifexpr::binary::divide:
      return lhs.eval () / rhs.eval ();
    case zcpp::ifexpr::binary::bitwise_and:
      return lhs.eval () & rhs.eval ();
    case zcpp::ifexpr::binary::bitwise_or:
      return lhs.eval () | rhs.eval ();
    case zcpp::ifexpr::binary::bitwise_xor:
      return lhs.eval () ^ rhs.eval ();
    case zcpp::ifexpr::binary::left_shift:
      return lhs.eval () << rhs.eval ();
    case zcpp::ifexpr::binary::right_shift:
      return lhs.eval () >> rhs.eval ();
    case zcpp::ifexpr::binary::logical_and:
      return lhs.eval () && rhs.eval ();
    case zcpp::ifexpr::binary::logical_or:
      return lhs.eval () || rhs.eval ();
    default:
      return zcpp::ifexpr::number (0, true);
    }
}

zcpp::ifexpr::number
zcpp::ifexpr::defined::eval (void)
{
  for (const auto &pair : zcpp::defines)
    {
      if (pair.first == macro)
	return zcpp::ifexpr::number (1, true);
    }
  return zcpp::ifexpr::number (0, true);
}

zcpp::ifexpr::number
zcpp::ifexpr::number::eval (void)
{
  return *this;
}

bool
zcpp::ifexpr::eval (const std::string &s)
{
  std::vector <std::string> reserved = {"defined"};
  std::size_t pos = s.find ("defined", 0);
  while (pos != std::string::npos)
    {
      while (pos < s.size () && !std::isspace (s[pos]))
	pos++;
      while (pos < s.size () && std::isspace (s[pos]))
	pos++;
      if (pos >= s.size ())
	break;
      if (s[pos] == '(')
	pos++;

      std::string temp;
      zcpp::expect_read_identifier (temp, s, pos);
      reserved.push_back (temp);
      pos = s.find ("defined", pos + 1);
    }

  std::size_t i = 0;
  std::unique_ptr <zcpp::ifexpr::expr> result =
    parse_expr (i, zcpp::expand (s, &reserved));
  if (result == nullptr || result->eval ().raw == 0)
    return false;
  return true;
}
