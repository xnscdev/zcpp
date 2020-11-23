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
#include <filesystem>
#include <fstream>
#include <iostream>
#include "zcpp.hh"

#define GUARD_REQUIRED_ARG if (i >= argc - 1)		\
    {							\
      zcpp::error ("option " + zcpp::bold (arg) +	\
		   " requires an argument");		\
      continue;						\
    }

std::istream *input = &std::cin;
std::string input_filename = "<stdin>";
std::ostream *output = &std::cout;
bool reserved[2];

bool zcpp::verbose;

static std::map <std::string, int> includetypes = {
  {"-idirafter", zcpp::include::dirafter},
  {"-iquote", zcpp::include::quote},
  {"-isystem", zcpp::include::system}
};

static struct
{
  const char *flag;
  const char *desc;
} options[] = {
  {"-h, --help", "Show this help text and exit"},
  {"-idirafter DIR", "Add DIR to the end of the include search path"},
  {"-iquote DIR", "Add DIR to the quoted-only include search path"},
  {"-isystem DIR", "Add DIR to the system include search path"},
  {"-I DIR", "Add DIR to the default include search path"},
  {"-o FILE, --output=FILE", "Write output to FILE"},
  {"-v, --verbose", "Write verbose output"},
  {"--version", "Show the version of this program and exit"}
};

static void
usage (void)
{
  std::cout << "Usage: zcpp [OPTIONS] [FILE...]" << std::endl
	    << "Options:" << std::endl;
  for (std::size_t i = 0; i < sizeof options / sizeof *options; i++)
    std::printf ("  %-24s %s\n", options[i].flag, options[i].desc);
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
  zcpp::includes.emplace_back (".", zcpp::include::quote);

  /* Define default macros */
  zcpp::define ("__STDC__", "1");
  zcpp::define ("__STDC_HOSTED__", "1");
  zcpp::define ("__STDC_VERSION__", "199409L");
  zcpp::define ("__CHAR_BIT__", "8");

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
	  if (arg == "-idirafter" || arg == "-iquote" || arg == "-isystem")
	    {
	      GUARD_REQUIRED_ARG;
	      std::string path (argv[++i]);
	      if (path.empty ())
		{
		  zcpp::error ("argument of " + zcpp::bold (arg) +
			       " must be a valid path");
		  continue;
		}
	      zcpp::add_includedir (path, includetypes[arg]);
	      continue;
	    }
	  if (arg.rfind ("-I", 0) == 0)
	    {
	      std::string path;
	      if (arg.size () < 3)
		{
		  GUARD_REQUIRED_ARG;
		  path = std::string (argv[++i]);
		  if (path.empty ())
		    {
		      zcpp::error ("argument of " + zcpp::bold (arg) +
				   " must be a valid path");
		      continue;
		    }
		}
	      else
		path = arg.substr (2);
	      zcpp::add_includedir (path, zcpp::include::regular);
	      continue;
	    }
	  if (arg == "-o" || arg == "--output")
	    {
	      GUARD_REQUIRED_ARG;
	      std::string filename (argv[++i]);
	      if (filename.empty ())
		{
		  zcpp::error ("output filename cannot be empty");
		  continue;
		}
	      output = new std::ofstream (filename);
	      reserved[1] = true;
	      if (output == nullptr || !output->good ())
	        zcpp::error ("failed to open output file " +
			     zcpp::bold (arg) + ": " +
			     std::strerror (errno));
	      continue;
	    }
	  if (arg.rfind ("--output=", 0) == 0)
	    {
	      arg.erase (0, 9);
	      if (arg.empty ())
		{
		  zcpp::error ("output filename cannot be empty");
		  continue;
		}
	      output = new std::ofstream (arg);
	      reserved[1] = true;
	      if (output == nullptr || !output->good ())
		zcpp::error ("failed to open output file " +
			     zcpp::bold (arg) + ": " +
			     std::strerror (errno));
	      continue;
	    }
	  if (arg == "-v" || arg == "--verbose")
	    {
	      zcpp::verbose = true;
	      continue;
	    }
	  if (arg == "--version")
	    {
	      version ();
	      std::exit (0);
	    }
	  if (arg == "--")
	    {
	      parsing_opts = false;
	      continue;
	    }
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

  zcpp::add_includedir ("/usr/local/include", zcpp::include::system);
  zcpp::add_includedir ("/usr/include", zcpp::include::system);

  std::string input_path =
    std::filesystem::path (input_filename).parent_path ();
  if (input_path.empty ())
    zcpp::includes.front ().path = ".";
  else
    zcpp::includes.front ().path = input_path;

  if (zcpp::verbose)
    {
      std::cerr << "Include search path" << std::endl;
      for (const zcpp::include &dir : zcpp::includes)
	{
	  std::cerr << "  " << dir.path;
	  if (dir.type == zcpp::include::quote)
	    std::cerr << " (quoted)";
	  std::cerr << std::endl;
	}
    }

  std::string result = zcpp::preprocess (input_filename, *input);
  if (zcpp::exiting)
    std::exit (1);
  *output << result;
  return 0;
}
