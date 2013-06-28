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

#include "Includes.hpp"

#include <boost/filesystem.hpp>
#include <exception>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <streambuf>

#include <stdio.h>
#include "config.hpp"

static inline int FixSlashes(int c)  {
  switch(c) {
    case '\\':
      return '/';
    default:
      return(c);
  }
}

class Token {
  
  typedef enum {
    
    RAW,
    RELATIVE_INCLUDE,
    PATH_INCLUDE,
  } token_enum_t;
  
  Token(const std::string & tokenText, token_enum_t tokenType)
    :	tokenText(tokenText),
	tokenType(tokenType)
  {
  }

  token_enum_t tokenType;
  std::string tokenText;
  
public:
  
  bool IsRelativeInclude() const { return tokenType == RELATIVE_INCLUDE; }
  bool IsSearchPathInclude() const { return tokenType == PATH_INCLUDE; }
  bool IsAnInclude() const { return tokenType != RAW; }
  
  std::string IncludeString() const {
   
    if(IsRelativeInclude()) 
      return std::string("#include \"") + tokenText + "\"";
    else if(IsSearchPathInclude()) 
      return std::string("#include <") + tokenText + ">";
    else
      return "";
  }
  
  const std::string &GetText() const {
   
    return tokenText;
  }
  
  void SetText(const std::string &text) {
   
    tokenText = text;
  }
  
  void SetRelativeInclude() {
   
    tokenType = RELATIVE_INCLUDE;
  }
  
  void SwapIncludeType() {
   
    if(IsRelativeInclude()) 
      SetSearchPathInclude();
    else if(IsSearchPathInclude()) 
      SetRelativeInclude();
  }
  
  void SetSearchPathInclude() {
   
    tokenType = PATH_INCLUDE;
  }
 
  static Token Raw(const std::string &tokenText) {
   
    return Token(tokenText, RAW);
  }
  
  static Token PathInclude(const std::string &tokenText) {
   
    return Token(tokenText, PATH_INCLUDE);
  }
  
  static Token RelativeInclude(const std::string &tokenText) {
   
    return Token(tokenText, RELATIVE_INCLUDE);
  }
};

class SourceFile
{
  
  const boost::filesystem::path root;
  const std::string file;
  
  typedef std::vector<Token> Tokens;
  
  Tokens tokens;
  
  const Includes &relativeIncludes;

public:
  
  class ParseSourceFileException : public std::exception {public: const char * what() const throw() { return "ParseSourceFileException"; } };
  
  SourceFile(const boost::filesystem::path &root, const std::string &file, const Includes &relativeIncludes)
    :	root(root), file(file), relativeIncludes(relativeIncludes)
  {
    if(args.verbose)
      printf("Parsing source file %s\n", file.c_str() );
    ParseSourceFile();
    
    boost::filesystem::path filepath( file.c_str() );
    
    FixIncludes(filepath.filename().c_str());
  }
  
  void Write() {
   
    FILE * f = fopen( file.c_str(), "w+" );
    
    for(Tokens::const_iterator itor = tokens.begin(); itor != tokens.end(); itor++) {
     
      if(itor->IsRelativeInclude())
	fprintf(f, "#include \"%s\"", itor->GetText().c_str());
      else if(itor->IsSearchPathInclude()) 
	fprintf(f, "#include <%s>", itor->GetText().c_str());
      else
	fprintf(f, "%s",itor->GetText().c_str());
    }
    
    fclose(f);
  }
  
private:
  
  void FixIncludes( const char * filename, bool swapType, Includes::const_iterator include_itor, Token &token ) {
    
    // HACK: kluding back in lost <> / "" info in include lookup table.
    Token dummyToToken( token );
    dummyToToken.SetText( include_itor->second );
    if(swapType)
      dummyToToken.SwapIncludeType();
    ///////////////////////////////////////////////////////////////////
    
    char fix = '\0';
    
    if(args.ask)
      while(fix != 'y' && fix != 'Y' && fix != 'n' && fix != 'N' ) {
	printf("%s: replace %s with %s ? (y/n): ", filename, token.IncludeString().c_str(), dummyToToken.IncludeString().c_str() );
	fix = getchar();
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
      }
    else fix = 'y';
      
    if(fix == 'y' || fix == 'Y') {
      if(!args.ask && args.verbose)
	printf("%s: replaced %s with %s\n", filename, token.IncludeString().c_str(), dummyToToken.IncludeString().c_str() );
      
      token.SetText( include_itor->second );
      if(swapType)
	token.SwapIncludeType();
    }
  }
  
  void FixIncludes( const char * filename, const Includes & primary_includes, const Includes & secondary_includes, Token & token ) {
          
    Includes::const_iterator include_itor;
      
    if( (include_itor = primary_includes.find( token.GetText() ) ) != primary_includes.end() ) {
      
      if( token.GetText() == include_itor->second ) {
	
	// its good!
      }
      else {

	// wrong, but fixable
	FixIncludes( filename, false, include_itor, token );	
      }
    }
    else if( (include_itor = secondary_includes.find( token.GetText() ) ) != secondary_includes.end() ) {
      
      // wrong, but fixable with different include type <> vs ""
      FixIncludes( filename, true, include_itor, token );	
      
    }
    else {
      
      /* Couldnt find a matching entry in any of the given includes.
       * Just fix the back-slashes is we can? */
      
      if(!args.quiet)
	printf("WARNING: I cant find a match for %s\n"
		"\tDo you need to add another search path \'I\' ?\n", token.IncludeString().c_str());
      
      std::string fixedSlashes = token.GetText();
      std::transform(fixedSlashes.begin(), fixedSlashes.end(), fixedSlashes.begin(), FixSlashes );

      if( fixedSlashes != token.GetText() ) {
	
	// HACK: kludge up a fake include map to update the slashes.
	Includes::IncludeMap dummyMap;
	dummyMap[token.GetText()] = fixedSlashes;
	FixIncludes( filename, false, dummyMap.find(token.GetText()), token );
	
      }
      else {
	// Nothing I can do with this!
      }
      
      // Prompt the user to add more '-I /paths/' args ?
    } 
  }
  
  void FixIncludes(const char * filename) {
   
    Includes & searchPathIncludes = Includes::SearchPathIncludes();
//  Includes   relativeIncludes(root);
    
    for(Tokens::iterator token_itor = tokens.begin(); token_itor != tokens.end(); token_itor++) {
     
      if(token_itor->IsRelativeInclude())
	FixIncludes(filename, relativeIncludes, searchPathIncludes, *token_itor);
      else if(token_itor->IsSearchPathInclude())
	FixIncludes(filename, searchPathIncludes, relativeIncludes, *token_itor);
    }
  }
  
  void ParseSourceFile() {
    
      std::string str;
      std::ifstream ifstream(file.c_str());
      if(!ifstream.good())
	throw ParseSourceFileException();
      
      ifstream.seekg(0, std::ios::end);
      str.reserve(ifstream.tellg());
      ifstream.seekg(0, std::ios::beg);
      str.assign((std::istreambuf_iterator<char>(ifstream)),
		  std::istreambuf_iterator<char>(        ));
    
      size_t pos = 0;
      size_t lastpos = 0;
      
      do{
	
	pos = str.find("#include", lastpos);
	
	if(pos != std::string::npos) {
	  
	  tokens.push_back( Token::Raw( std::string( str.begin() + lastpos, str.begin() + pos ) ) );
	  
	  size_t start = str.find_first_of("<\"", pos);
	  size_t end   = str.find_first_of(">\"", start+1);
	  
	  if(str.at(start) == '<')
	    tokens.push_back( Token::PathInclude( std::string(str.begin()+start+1, str.begin()+end) ) );
	  else
	    tokens.push_back( Token::RelativeInclude( std::string(str.begin()+start+1, str.begin()+end) ) );

	  lastpos = end+1;
	}
	else {
	  tokens.push_back( Token::Raw( std::string( str.begin() + lastpos, str.end() ) ) );
	  pos = std::string::npos;
	}
	
      } while( pos != std::string::npos );
  }
};

