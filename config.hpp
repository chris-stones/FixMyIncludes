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


#pragma once

#include<string>
#include<vector>

struct arguments
{
  arguments()
    :	pretend(false),
	ask(false),
	verbose(false),
	debug(false),
	quiet(false),
	sourceRoot(".")
  {}
  
  bool pretend;
  bool ask;
  bool verbose;
  bool debug;
  bool quiet;
  std::vector<std::string> includeSearchPath;
  std::string sourceRoot;
};

extern arguments args;

struct arguments read_args(int argc, char ** argv );

