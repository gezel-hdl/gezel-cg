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

#include "vhdsfg.h"
#include "vhdbinop.h"
#include "vhdunop.h"
#include "vhdterop.h"
#include "vhdassign.h"
#include "vhdlut.h"
#include "vhdout.h"

//--------------------------------------------------------------

vhdsfg::vhdsfg(symid _id, string _name) : id(_id), name(_name) {}

vhdsfg::~vhdsfg() {
  for (map<symid, vhdop *>::iterator i = cinlist.begin(); i != cinlist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid, vhdop *>::iterator i = rinlist.begin(); i != rinlist.end(); ++i)
    if ((i->second))
      delete ((i->second));

};

void vhdsfg::addbinInstr(symid v, vhdbinop* t) {
  cinlist[v] = t;  
}

void vhdsfg::addunInstr(symid v, vhdunop* t) {
  cinlist[v] = t; 
}

void vhdsfg::addterInstr(symid v, vhdterop* t) {
  cinlist[v] = t;  
}

void vhdsfg::addmultterInstr(symid v, vhdterop* s) {
  cinlist[v]->addter(s);
}


void vhdsfg::addassignInstr(symid v, vhdassign* t, bool isreg) {
  if(isreg) 
    rinlist[v] = t; 
  else
    cinlist[v] = t;
}

void vhdsfg::addlutInstr(symid v, vhdlutop* t) {
  cinlist[v] = t; 
}

string vhdsfg::getname() {
  return name;
}

map<symid, vhdop*> vhdsfg::getcinlist() {
  return cinlist;
}

map<symid, vhdop*> vhdsfg::getrinlist() {
  return rinlist;
}

void vhdsfg::vhdoutreg(vhdout* out) {
  for (map<symid, vhdop *>::iterator i = rinlist.begin(); i != rinlist.end(); ++i)
    if ((i->second))
	if(i->second->isvalid())
	   i->second->vhdoutop(out,true,false);	
}

void vhdsfg::vhdoutcmb(vhdout* out) {
  for (map<symid, vhdop *>::iterator i = cinlist.begin(); i != cinlist.end(); ++i)
    if ((i->second))
      if(i->second->isvalid())
	i->second->vhdoutop(out,false,false);	
}

void vhdsfg::vhdoutfsmcmb(vhdout* out) {
  for (map<symid, vhdop *>::iterator i = cinlist.begin(); i != cinlist.end(); ++i)
    if ((i->second))
	if(i->second->isvalid())
	  // PS - in process fsmCMB, only register outputs may be read, similar as in dpCMB
	  //	   i->second->vhdoutop(out,false,true);	
	   i->second->vhdoutop(out,false,false);	
}


