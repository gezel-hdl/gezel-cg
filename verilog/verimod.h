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
#ifndef VERIMOD_H
#define VERIMOD_H

#include "verivar.h"
#include "fdlcode.h"

using namespace std;

#include <string>
#include <vector>
#include <map>

// all constants over all modules
extern map<symid, verivar *> vericonstants;
typedef map<symid, verivar *>::iterator vericonstants_it;

// verilog module (equivalent to GEZEL dp)
class verimod {
  vector <verivar *>        iolist;
  typedef vector<verivar *>::iterator iolist_it;

  vector <verivar *> reglist;
  typedef vector<verivar *>::iterator reglist_it;

  vector <verivar *> wirelist;
  typedef vector<verivar *>::iterator wirelist_it;

  map    <symid, verivar *> variables;
  typedef map<symid, verivar *>::iterator variables_it;

  string name;
 public:
  verimod        (string name);
  void   addio   (symid, verivar *);
  void   addreg  (symid, verivar *);
  void   addwire (symid, verivar *);
  string getname ()   { return name; }
  void   printverilog(veriprint *V);
};

#endif
