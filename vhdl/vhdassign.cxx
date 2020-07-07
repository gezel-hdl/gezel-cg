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

#include "vhdassign.h"
#include "vhdvar.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdassign::vhdassign(vhdvar* lhs, vhdvar* eq) {
  left = lhs; 
  seteq = eq;
  valid = false;
}

vhdassign::~vhdassign() {}

string vhdassign::gettype() {
  return "assign";
}

vhdvar* vhdassign::getleft() {
  return left;
}

string vhdassign::getoperator() {
  return "assign";
}

vhdvar* vhdassign::geteq() {
  return seteq;
}

void vhdassign::addseteq(vhdvar* t) {
  seteq = t;
}

bool vhdassign::isvalid() {
  return valid;
}

void vhdassign::setvalid() {
  valid = true;
}
 
void vhdassign::vhdoutop (vhdout* os,
			  bool reg,
			  bool isfsmcmb) {
  
  if(seteq->isreg())
    os->assign(seteq->getregname(), 
	       seteq->getwl(),
	       left->isconst(),
	       left->getwl(),
	       left->getname(),
	       left->gettc(),
	       left->isbitstring());
  else
    os->assign(seteq->getportname(), // portname does not add "_int"; cfr getname()
	       seteq->getwl(),
	       left->isconst(),
	       left->getwl(),
	       left->getname(),
	       left->gettc(),
	       left->isbitstring());
  
}


