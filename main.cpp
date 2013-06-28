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

#include <iostream>
#include <boost/filesystem.hpp>

#include "Includes.hpp"
#include "SourceFile.hpp"
#include "config.hpp"

void FixMyIncludes(const boost::filesystem::path & path) {
  
  // dont parse SVN
  if(path.has_extension() && (strcasecmp(path.extension().c_str(), ".svn")==0))
    return;
  
  boost::filesystem::directory_iterator end;
  for( boost::filesystem::directory_iterator itor = boost::filesystem::directory_iterator( path ); itor != end; itor++)
  {
    if(boost::filesystem::is_regular_file( itor->status() )) 
    {
      const boost::filesystem::path p = itor->path();
      
      if( p.has_extension() ) {
	
	if( ( strcasecmp(p.extension().c_str(), ".c"  )==0) ||
	    ( strcasecmp(p.extension().c_str(), ".h"  )==0) ||
	    ( strcasecmp(p.extension().c_str(), ".cpp")==0) ||
	    ( strcasecmp(p.extension().c_str(), ".hpp")==0) ){
	 
	  SourceFile sf( path, p.c_str() );
	
	  if(!args.pretend)
	    sf.Write();
	} 
      }
    }
  }

  for( boost::filesystem::directory_iterator itor = boost::filesystem::directory_iterator( path ); itor != end; itor++)
    if(boost::filesystem::is_directory( itor->status() )) 
      FixMyIncludes( itor->path() );
}

int main(int argc, char **argv) {

  args = read_args(argc, argv);
    
  FixMyIncludes( args.sourceRoot );
  
  return 0;
}

