/*
    Copyright (c) 2013 Christopher Stones   christopher.stones@zoho.co.uk
    
    This file is part of FixMyIncludes.

    FixMyIncludes is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FixMyIncludes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FixMyIncludes.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "config.hpp"

#include <argp.h>
#include <string.h>

const char *argp_program_version = "FixMyIncludes 0.1";
const char *argp_program_bug_address = NULL; // "NULL";
static char doc[] = "FixMyIncludes -- a program to aid in the porting of c/c++ software from Windows to *NIX";
static char args_doc[] = "SOURCE_ROOT_PATH";

static struct argp_option options[] = {
  {"pretend",  'p', 0,      		OPTION_ARG_OPTIONAL,  "dont actually write anything" },
  {"ask",      'a', 0,      		OPTION_ARG_OPTIONAL,  "ask before making each change" },
  {"verbose",  'v', 0,      		OPTION_ARG_OPTIONAL,  "produce verbose output" },
  {"quiet",    'q', 0,      		OPTION_ARG_OPTIONAL,  "very little output" },
  {"debug",    'd', 0,      		OPTION_ARG_OPTIONAL,  "debug mode" },
  {"SEARCH",   'I', "include path", 	0,                    "include search path"},
  { 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments *)state->input;

  switch (key)
    {
     case 'p':
      arguments->pretend = 1;
      break;
     case 'a':
      arguments->ask = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'd':
      arguments->debug = 1;
      break;
    case 'I':
    {
      std::string str(arg);
      arguments->includeSearchPath.push_back( str );
    }
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 1)
	/* Too many arguments. */
        argp_usage (state);
     
      arguments->sourceRoot = arg;
      break;
      
    case ARGP_KEY_END:
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

arguments args;

struct arguments read_args(int argc, char ** argv ) {
  
  argp_parse (&argp, argc, argv, 0, 0, &args);
  
  return args;
}


