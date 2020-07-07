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

#include <iostream>
#include "vhdterop.h"
#include "vhdvar.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdterop::vhdterop(vhdvar* lhs, vhdvar* mid, vhdvar* rhs, _optype op) {
  left = lhs; 
  middle = mid;
  right = rhs;
  type = op;
  valid = false;
}
 
vhdterop::~vhdterop() {
  for (vector<vhdterop *>::iterator i = terlist.begin(); i != terlist.end(); ++i)
    delete (*i);
}

string vhdterop::gettype() {
  switch (type) {
  case sel_op:     return "select"; break;
  }
  return "notuse";
}

void vhdterop::addseteq(vhdvar* t) {
      	seteq = t;
}

void vhdterop::addter(vhdterop* t) {
  terlist.push_back(t);
}

vhdvar* vhdterop::getleft() {
  return left;
}

vhdvar* vhdterop::getright() {
  return right;
}

vhdvar* vhdterop::getmiddle() {
  return middle;
}

string vhdterop::getoperator() {
  if(terlist.empty())
    return "terop";
  else
    return "multerop";
}

vector<vhdterop *> vhdterop::getteroplist() {
  return terlist;
}

vhdvar* vhdterop::geteq() {
  return seteq;
}

bool vhdterop::isvalid() {
  return valid;
}

void vhdterop::setvalid() {
  valid = true;
}
 
void vhdterop::vhdoutop(vhdout* os, bool reg, bool isfsmcmb) {
  bool first = true;
  string leftnm, rightnm, middlenm;
  
  if(terlist.empty()){
     if (left->isreg()&& isfsmcmb)
        leftnm = left->getregname();
     else
        leftnm = left->getname();
     if (right->isreg()&& isfsmcmb)
        rightnm = right->getregname();
     else
        rightnm = right->getname();
     if (middle->isreg()&& isfsmcmb)
        middlenm = middle->getregname();
     else
       middlenm = middle->getname();
     os->teropsel(seteq->getname(),
		  seteq->getwl(),
		  leftnm,
		  left->gettc(),
		  left->getwl(),
		  middlenm,
		  middle->isconst(),
		  middle->getwl(),
		  middle->gettc(),
		  middle->isbitstring(),
		  rightnm,
		  right->isconst(),
		  right->getwl(),
		  right->gettc(),
		  right->isbitstring());
  }
  else {
     for (vector<vhdterop *>::iterator i = (terlist.end()-1); i != (terlist.begin()-1); --i) {
       if ((*i)->getleft()->isreg()&& isfsmcmb)
	 leftnm = (*i)->getleft()->getregname();
       else
	 leftnm = (*i)->getleft()->getname();
       if ((*i)->getmiddle()->isreg()&& isfsmcmb)
	 middlenm = (*i)->getmiddle()->getregname();
       else
	 middlenm = (*i)->getmiddle()->getname();
       os->teropmultsel(first,
			seteq->getname(),
			seteq->getwl(),
			leftnm,
			(*i)->getleft()->gettc(),
			(*i)->getleft()->getwl(),
			middlenm,
			(*i)->getmiddle()->isconst(),
			(*i)->getmiddle()->getwl(),
			(*i)->getmiddle()->gettc(),
			(*i)->getmiddle()->isbitstring());
       if(first) first = false;		
     }
     if (left->isreg()&& isfsmcmb)
       leftnm = left->getregname();
     else
       leftnm = left->getname();
     if (right->isreg()&& isfsmcmb)
       rightnm = right->getregname();
     else
       rightnm = right->getname();
     if (middle->isreg()&& isfsmcmb)
       middlenm = middle->getregname();
     else
       middlenm = middle->getname();
     os->teropendsel(seteq->getname(),
		     seteq->getwl(),
		     leftnm,
		     left->gettc(),
		     left->getwl(),
		     middlenm,
		     middle->isconst(),
		     middle->getwl(),
		     middle->gettc(),
		     middle->isbitstring(),
		     rightnm,
		     right->isconst(),
		     right->getwl(),
		     right->gettc(),
		     right->isbitstring());
  }
}
