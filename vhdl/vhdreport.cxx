//--------------------------------------------------------------
// Copyright (C) 2003-2010 P. Schaumont 
//                                                                             
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//                                                                             
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                             
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
//--------------------------------------------------------------

#include "vhdreport.h"
#include "rterror.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
using std::string;
using std::ostringstream;

typedef ostringstream ostrstr_t;
#else
#include <strstream.h>

typedef ostrstream ostrstr_t;
#endif

//---------------------------------------------------------------------
vhdreport::vhdreport(string filename) : os(filename.c_str()) {
  //  os.open(filename.c_str());

  indent = 0;

  if (os.fail()) {
      fdlerror(ERROR_CANNOT_OPEN_VHD_FILE, filename.c_str());
  }
}

vhdreport::~vhdreport() {}

void vhdreport::comment(string str) {
  doindent();
  os <<"--"<<str<<"\n";  
}

void vhdreport::increaseindent() {
  indent++;
}

void vhdreport::decreaseindent() {
  indent--;
}

void vhdreport::doindent() {
  for (int i = 0; i < indent; i ++)
	os << "   ";
}

void vhdreport::outputline(string str) {
  doindent();
  os << str<<"\n";
}

void vhdreport::outputchar(string str) {
  os << str;
}

void vhdreport::nextline(int line) {
  for (int i = 0; i < line; i ++)
	os << "\n";
}

void vhdreport::outregister(int size, int bit) {
  ostrstr_t strstrm;
  doindent();
  strstrm <<size<<" register of size "<<bit<<" bit "<<"\n";
  os <<strstrm.str();
}

void vhdreport::outlut(int size, int bit) {
  ostrstr_t strstrm;
  doindent();
  strstrm <<size<<" LUT of size "<<bit<<" bit "<<"\n";
  os <<strstrm.str();
}

void vhdreport::outop(int size, int bit, string nm) {
  ostrstr_t strstrm;
  doindent();
  strstrm <<size<<" "<<nm<<" operator of size "<<bit<<" bit"<<"\n";
  os <<strstrm.str();
}

void vhdreport::close() {
  os.close();
}
