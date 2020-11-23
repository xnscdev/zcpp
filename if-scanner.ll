/*************************************************************************
 * if-scanner.ll - This file is part of zcpp.                            *
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

IS	[a-zA-Z_]
IE	[a-zA-Z0-9_]
D	[0-9]

%{

#include <string>
#include "if-parser.hh"
#include "zcpp.hh"

#define SAVE (yylval.string = new std::string (yytext, yyleng))
#define END yyterminate ()

void yyerror (const char *msg);
extern "C" int yywrap (void);

%}

%option nounput

%%

[ \t]		;
"+"		return PLUS;
"-"		return MINUS;
"*"		return MULTIPLY;
"/"		return DIVIDE;
"("		return LPAREN;
")"		return RPAREN;
"defined"	return DEFINED;
{IS}{IE}*       SAVE; return IDENTIFIER;
{D}+		SAVE; return NUMBER;
.		yyerror ("unexpected character in #if directive"); END;

%%

void
yyerror (const char *msg)
{
  zcpp::error (msg);
}

extern "C" int
yywrap (void)
{
  return 1;
}
