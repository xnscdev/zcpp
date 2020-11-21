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

static std::unique_ptr <zcpp::ifexpr::number>
parse_number (std::size_t &i, const std::string &s)
{
  std::string temp;
  do
    temp += s[i++];
  while (i < s.size () && (isdigit (s[i]) || std::tolower (s[i]) == 'x'));
  std::size_t pos;
  unsigned long long num = std::stoull (temp, &pos, 0);
  if (pos < temp.size ())
    {
      zcpp::error ("invalid integer literal in #if expression");
      return nullptr;
    }
  if (i >= s.size ())
    return std::make_unique <zcpp::ifexpr::number> (num, num <= LLONG_MAX);
  if (!std::isspace (s[i]))
    {
      zcpp::error ("invalid integer literal in #if expression");
      return nullptr;
    }
  while (i < s.size () && std::isspace (s[i]))
    i++;
  return std::make_unique <zcpp::ifexpr::number> (num, num <= LLONG_MAX);
}

static std::unique_ptr <zcpp::ifexpr::expr>
parse_expr (std::size_t &i, const std::string &s)
{
  if (s[i] == '(')
    {
      if (++i >= s.size ())
	{
	  zcpp::error ("unmatched parentheses in #if expression");
	  return nullptr;
	}
      std::unique_ptr <zcpp::ifexpr::expr> result = parse_expr (i, s);
      if (i >= s.size () || s[i] != ')')
	{
	  zcpp::error ("unmatched parentheses in #if expression");
	  return nullptr;
	}
      i++;
      return result;
    }
  else if (std::isalpha (s[i]) || s[i] == '_')
    {
      std::string name;
      zcpp::expect_read_identifier (name, s, i, false, true);
      return std::make_unique <zcpp::ifexpr::number> (0, true);
    }
  else if (std::isdigit (s[i]))
    return parse_number (i, s);

  zcpp::error ("unexpected character " + zcpp::bold (std::string (s[i], 1)) +
	       " in #if expression");
  return nullptr;
}

bool
zcpp::ifexpr::eval (const std::string &s)
{
  std::size_t i = 0;
  std::unique_ptr <zcpp::ifexpr::expr> result =
    parse_expr (i, zcpp::expand (s));
  if (result == nullptr || result->eval ().raw == 0)
    return false;
  return true;
}
