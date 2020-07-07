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

#ifndef VHDPORTNET_H
#define VHDPORTNET_H

#include "asymbol.h"
#include "vhdvalldp.h"

#include <vector>
using std::vector;

class vhdvar;
class vhdout;

class vhdnet {
  symid input;
  symid output;
  vhdvar* var;
  
 public:
  vhdnet(symid, symid);
  ~vhdnet();
  
  vhdvar* getvar();
  symid getinput();
  symid getoutput();
  void addvar(vhdvar*);
};

class vhdmapentry {
  vhdvar* portv;
  vhdvar* targetv;
 public:
  vhdmapentry(vhdvar*, vhdvar*);
  ~vhdmapentry();
  
  vhdvar* getportv();
  vhdvar* gettargetv();
  void settargetv(vhdvar*);
};

class vhdportmap {
  vector<vhdmapentry *> entry;
  vhdvalldp* dp;
  string parentnm;
  bool clone;
  bool hasclk;
  
  public:
  vhdportmap(vhdvalldp*);
  ~vhdportmap(); 
  
  void addentry(vector<vhdmapentry *>);
  void addoneentry(vhdmapentry*);
  string getparentnm();
  vector<vhdvar *> gettargetlist();	
  bool gethasclk();
  void updatehasclk();

  vhdvalldp *getdp() { return dp; }

  bool isclone();
  void dumpportvar(vhdout*);
  void dumpmapvar(vhdout*);
};

#endif
