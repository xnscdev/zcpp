/*************************************************************************
 * if-parser.yy - This file is part of zcpp.                             *
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

%{

#include <string>
#include "if-parser.hh"
#include "zcpp.hh"

int yylex (void);
void yyerror (const char *msg);

bool if_result;

%}

%define parse.lac full
%define parse.error detailed

%union
{
  long long number;
  std::string *string;
}

%token	<string>	NUMBER "number"
%token	<string>	IDENTIFIER "identifier"
%token DEFINED "defined"
%token PLUS "+"
%token MINUS "-"
%token MULTIPLY "*"
%token DIVIDE "/"
%token MODULUS "%"
%token LSHIFT "<<"
%token RSHIFT ">>"
%token LE "<="
%token LT "<"
%token GE ">="
%token GT ">"
%token EQ "=="
%token NE "!="
%token LOG_AND "&&"
%token LOG_OR "||"
%token LOG_NOT "!"
%token BIT_AND "&"
%token BIT_XOR "^"
%token BIT_OR "|"
%token BIT_NOT "~"
%token LPAREN "("
%token RPAREN ")"
%token YYEOF "end of expression"

%type	<number>	expression defined_expression number

%left "||"
%left "&&"
%left "|"
%left "^"
%left "&"
%left "==" "!="
%left "<" "<=" ">" ">="
%left "<<" ">>"
%left "+" "-"
%left "*" "/" "%"
%precedence UNARY

%start input

%%

input:		expression { if_result = $1 != 0; }
	;

expression:	number
	|	"identifier" { $$ = 0; delete $1; }
	|	defined_expression
	|	expression "+" expression { $$ = $1 + $3; }
	|	expression "-" expression { $$ = $1 - $3; }
	|	expression "*" expression { $$ = $1 * $3; }
	|	expression "/" expression { $$ = $1 / $3; }
	|	expression "%" expression { $$ = $1 % $3; }
	|	expression "<<" expression { $$ = $1 << $3; }
	|	expression ">>" expression { $$ = $1 >> $3; }
	|	expression "<=" expression { $$ = $1 <= $3; }
	|	expression "<" expression { $$ = $1 < $3; }
	|	expression ">=" expression { $$ = $1 >= $3; }
	|	expression ">" expression { $$ = $1 > $3; }
	|	expression "==" expression { $$ = $1 == $3; }
	|	expression "!=" expression { $$ = $1 != $3; }
	|	expression "&&" expression { $$ = $1 && $3; }
	|	expression "||" expression { $$ = $1 || $3; }
	|	expression "&" expression { $$ = $1 & $3; }
	|	expression "^" expression { $$ = $1 ^ $3; }
	|	expression "|" expression { $$ = $1 | $3; }
	|	"(" expression ")" { $$ = $2; }
	|	%prec UNARY "-" expression { $$ = -$2; }
	|	%prec UNARY "!" expression { $$ = !$2; }
	|	%prec UNARY "~" expression { $$ = ~$2; }
	;

number:		"number"
		{
		  $$ = std::strtoll ($1->c_str (), nullptr, 10);
		  if (errno == ERANGE)
		    {
		      zcpp::error ("integer literal " + zcpp::bold (*$1) +
				   " is out of range");
		      delete $1;
		      YYERROR;
		    }
		  delete $1;
		}
	;

defined_expression:
		"defined" "identifier"
		{
		  $$ = zcpp::defines.find (*$2) != zcpp::defines.end ();
		  delete $2;
		}
	|	"defined" "(" "identifier" ")"
		{
		  $$ = zcpp::defines.find (*$3) != zcpp::defines.end ();
		  delete $3;
		}
	;

%%
