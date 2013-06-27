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

#include <boost/filesystem.hpp>
#include <exception>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <stdio.h>
#include "config.hpp"

static inline int ToKey(int c)  {
  switch(c) {
    case '\\':
      return '/';
    default:
      return tolower(c);
  }
}

class Includes
{
  typedef std::map<std::string, std::string> IncludeMap;
  typedef std::pair<std::string, std::string> IncludePair;
 
  const boost::filesystem::path root;
  IncludeMap includeMap;
  
  Includes(){}
  
public:
  
  typedef IncludeMap::iterator iterator;
  typedef IncludeMap::const_iterator const_iterator;
  
  iterator begin() {
    return includeMap.begin(); 
  }
  const_iterator begin() const { 
    return includeMap.begin();
  }
  iterator end() {
    return includeMap.end(); 
  }
  const_iterator end() const 	{
    return includeMap.end(); 
  }
  iterator find(const std::string &_key) {
    std::string key = _key;
    std::transform(key.begin(), key.end(), key.begin(), ToKey );
    return includeMap.find(key); 
  }
  const_iterator find(const std::string &_key) const {
    std::string key = _key;
    std::transform(key.begin(), key.end(), key.begin(), ToKey );
    return includeMap.find(key); 
  }
  
  class OpenDirException : public std::exception {public: const char * what() const throw() { return "OpenDirException"; } };
  
  Includes(const boost::filesystem::path &path)
    :	root(path)
  {
    Scan(path);
  }
  
  Includes& operator+=(const Includes &that) {
   
    for(IncludeMap::const_iterator itor = that.includeMap.begin(); itor != that.includeMap.end(); itor++)
      this->includeMap[itor->first] = itor->second;
    
    return *this;
  }
  
  Includes operator+(const Includes &that) const {
   
    Includes includes;
    includes += *this;
    includes += that;
    return includes;
  }
  
  static Includes &SearchPathIncludes() {
    
    static bool initialised = false;
    static Includes includes;
    
    if(initialised)
      return includes;
    
    for(std::vector<std::string>::const_iterator itor = args.includeSearchPath.begin(); itor != args.includeSearchPath.end(); itor++)
      includes += Includes( *itor );
    
    initialised = true;
    
    return includes;
  }
    
private:
   
  void Add(const std::string &_include) {
    
    int trim = root.string().length();
    if(*root.string().rbegin() != '/' && *root.string().rbegin() != '\\')
      trim++;
   
    std::string include( _include.begin() + trim,  _include.end() );
    std::string key = include;
    std::transform(key.begin(), key.end(), key.begin(), ToKey );
    
    if(args.verbose)
      printf("%s -> %s\n",key.c_str(),include.c_str());
    
    includeMap[key] = include;
  }
  
  void Scan(const boost::filesystem::path & path) {
    
    if(args.verbose)
      printf("scanning %s\n", path.c_str());
    
    if(!boost::filesystem::exists(path))
	throw OpenDirException();
      
    if(!boost::filesystem::is_directory(path))
      throw OpenDirException();
    
    // dont parse SVN
    if(path.has_extension() && (strcasecmp(path.extension().c_str(), ".svn")==0))
      return;
    
    boost::filesystem::directory_iterator end;
    for( boost::filesystem::directory_iterator itor = boost::filesystem::directory_iterator( path ); itor != end; itor++)
      if(boost::filesystem::is_regular_file( itor->status() ))
	if( !itor->path().has_extension() || ((strcasecmp(itor->path().extension().c_str(), ".h")==0) || (strcasecmp(itor->path().extension().c_str(), ".hpp")==0)) )
	  Add( itor->path().string() );
    
    if( boost::filesystem::is_symlink(path) )
      return; // FIXME: better way to deal with gphoto's recursive symlinks?
      
    for( boost::filesystem::directory_iterator itor = boost::filesystem::directory_iterator( path ); itor != end; itor++)  
      if(boost::filesystem::is_directory( itor->status() ))
	  Scan( itor->path() );
  }
};

