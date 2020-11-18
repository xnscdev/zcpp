/*************************************************************************
 * zcpp.cc - This file is part of zcpp.                                  *
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
#include <iostream>
#include "zcpp.hh"

std::istream *input = &std::cin;
std::string input_filename = "<stdin>";
std::ostream *output = &std::cout;
bool reserved[2];

static struct
{
  const char *flag;
  const char *desc;
} options[] = {
  {"-h, --help", "Show this help text and exit"},
  {"-o FILE", "Write output to FILE"},
  {"--version", "Show the version of this program and exit"}
};

static void
usage (void)
{
  std::cout << "Usage: zcpp [OPTIONS] [FILE...]" << std::endl
	    << "Options:" << std::endl;
  for (std::size_t i = 0; i < sizeof options / sizeof *options; i++)
    std::printf ("  %-24s%s\n", options[i].flag, options[i].desc);
}

static void
version (void)
{
  std::cout << "zcpp " PROJECT_VERSION << std::endl;
}

int
main (int argc, char **argv)
{
  zcpp::filestack.push
    (std::make_unique <zcpp::translation_unit> ("<command-line>", std::cin));
  zcpp::init_console ();
  bool parsing_opts = true;
  for (int i = 1; i < argc; i++)
    {
      std::string arg (argv[i]);
      if (arg.empty ())
	continue;
      else if (parsing_opts && arg[0] == '-')
	{
	  if (arg == "-h" || arg == "--help")
	    {
	      usage ();
	      std::exit (0);
	    }
	  else if (arg == "-o")
	    {
	      if (i >= argc - 1)
		{
		  zcpp::error ("option " + zcpp::bold ("-o") +
			       " requires an argument");
		  continue;
		}
	      std::string filename (argv[++i]);
	      output = new std::ofstream (filename);
	      reserved[1] = true;
	      if (output == nullptr || !output->good ())
	        zcpp::error ("failed to open output file " +
			     zcpp::bold (arg) + ": " + std::strerror (errno));
	    }
	  else if (arg == "--version")
	    {
	      version ();
	      std::exit (0);
	    }
	  else if (arg == "--")
	    parsing_opts = false;
	  else
	    zcpp::error ("unrecognized command-line option: " + arg);
	}
      else if (!reserved[0])
	{
	  input = new std::ifstream (arg);
	  input_filename = arg;
	  if (input == nullptr || !input->good ())
	    zcpp::error ("failed to open input file " + zcpp::bold (arg) +
			 ": " + std::strerror (errno));
	  reserved[0] = true;
	}
      else if (!reserved[1])
	{
	  output = new std::ofstream (arg);
	  if (output == nullptr || !output->good ())
	    zcpp::error ("failed to open output file " + zcpp::bold (arg) +
			 ": " + std::strerror (errno));
	  reserved[1] = true;
	}
      else
	zcpp::error ("too many files specified");
    }
  if (zcpp::exiting)
    std::exit (1);

  std::string result = zcpp::preprocess (input_filename, *input);
  if (zcpp::exiting)
    std::exit (1);
  *output << result;
  return 0;
}
