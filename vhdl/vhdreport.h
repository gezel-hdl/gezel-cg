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

#ifndef VHDREPORT_H
#define VHDREPORT_H

#include <string>
using std::string;

#include <fstream>
using std::ofstream;

class vhdreport {
  ofstream os;
  int	indent;  
  
  bool first;

 public:
  vhdreport(string);
  ~vhdreport();
  
  void comment(string);
  
  void increaseindent();
  void decreaseindent();
  void doindent();
  
  void outputline(string);
  void outputchar(string);
  void nextline(int);
  
  void outregister(int,int); //size,bit
  void outlut(int,int);//size,bit
  void outop(int,int,string);//size,bit,name
  void close();  
};

#endif
