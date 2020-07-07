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

#ifndef VHDSYS_H
#define VHDSYS_H

#include <map>
using std::map;

#include <vector>
using std::vector;

#include "asymbol.h"
class vhdvalldp;
class vhdportmap;
class vhdnet;
class vhdvar;
class vhdreport;

class vhdsys {
  bool sysempty;
  
  map<symid, vhdvalldp  *> dplist;
  map<string, vhdportmap  *> maplist;
  vector <vhdnet *> netlist;
  
  vector<vhdvar  *> varlist;
  int count;
  bool hasclk;
  
 public:
  
  vhdsys();
  ~vhdsys();
  
  void adddp(symid,vhdvalldp*);
  void addnet(symid, symid);
  
  void resetcount();
  string vautoname();
  
  void preprocess();	
  void dump(string, string, int);
  void debugreport(vhdreport*);
};


#endif
