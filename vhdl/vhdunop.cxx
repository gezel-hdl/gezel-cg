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

#include "vhdunop.h"
#include "vhdvar.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdunop::vhdunop(vhdvar* lhs, _optype op) {
  left = lhs; 
  type = op;
  valid = false;
}

vhdunop::~vhdunop() {}

string vhdunop::gettype() {
  switch (type) {
	case neg_op:     return "neg"; break;
	case not_op:     return "not"; break; 
	case cast_op:    return "cast"; break; 
  }
  return "notuse";
}

vhdvar* vhdunop::getleft() {
  return left;
}

string vhdunop::getoperator() {
  return "dunop";
}

vhdvar* vhdunop::geteq() {
  return seteq;
}

void vhdunop::addseteq(vhdvar* t) {
  seteq = t;
}

bool vhdunop::isvalid() {
  return valid;
}

void vhdunop::setvalid() {
  valid = true;
}
 
void vhdunop::vhdoutop(vhdout* os, bool reg, bool isfsmcmb) {
  string leftnm;
  if (left->isreg()&& isfsmcmb)
     leftnm = left->getregname();
  else
     leftnm = left->getname();

  switch(type) {
	case not_op:
		os->dunopnot(seteq->getname(),seteq->getwl(),left->isconst(),left->getwl(),leftnm,left->gettc()); 
		break;
	case neg_op:
		os->dunopneg(seteq->getname(),seteq->getwl(),seteq->gettc(),left->getwl(),leftnm,left->isconst(),left->gettc());
		break;
        case cast_op:
		os->dunopcast(seteq->getname(),seteq->getwl(),seteq->gettc(),left->getwl(),leftnm,left->isconst(),left->gettc());
		break;
  }
}
