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


#include "vhdbinop.h"
#include "vhdvar.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdbinop::vhdbinop(vhdvar* lhs, vhdvar* rhs, _optype op) {
  left = lhs; 
  right = rhs;
  type = op;
  valid = false;

}

vhdbinop::~vhdbinop() {}

string vhdbinop::gettype() {
  switch (type) {
	case ior_op:     return "or"; break;
	case xor_op:     return "xor"; break;
	case and_op:     return "and"; break;
	case eq_op:      return "eq"; break;
	case ne_op:      return "ne"; break;
	case smt_op:     return "smt"; break; 
	case grt_op:     return "grt"; break; 
	case smteq_op:   return "smteq"; break;
	case grteq_op:   return "grteq"; break;
	case shl_op:     return "shl"; break; 
	case shr_op:     return "shr"; break; 
	case add_op:     return "add"; break; 
	case sub_op:     return "sub"; break; 
	case concat_op:  return "concat"; break; 
	case mul_op:     return "mul"; break; 
	case mod_op:     return "mod"; break; 
	case cast_op:    return "cast"; break; 
  }
  return "notuse";
}

vhdvar* vhdbinop::getleft() {
  return left;
}

vhdvar* vhdbinop::getright() {
  return right;
}

string vhdbinop::getoperator() {
  return "binop";
}

vhdvar* vhdbinop::geteq() {
  return seteq;
}

void vhdbinop::addseteq(vhdvar* t) {
  seteq = t;
}

bool vhdbinop::isvalid() {
  return valid;
}

void vhdbinop::setvalid() {
  valid = true;
}
 
void vhdbinop::vhdoutop(vhdout* os,
			bool reg,
			bool isfsmcmb) {
  string leftnm,
    rightnm;

  if (left->isreg() && isfsmcmb)
     leftnm = left->getregname();
  else
     leftnm = left->getname();

  if (right->isreg()&& isfsmcmb)
     rightnm = right->getregname();
  else
     rightnm = right->getname();
  
  switch (type) {
  case ior_op: 
    os->binoparithlogic(" or ",
			seteq->gettc(),
			seteq->getwl(),
			seteq->getname(),
			left->isconst(),
			left->getwl(),
			leftnm,
			left->gettc(),
			right->isconst(),
			right->getwl(),
			rightnm,
			right->gettc());
    break;
  case xor_op: 
    os->binoparithlogic(" xor ",
			seteq->gettc(),
			seteq->getwl(),
			seteq->getname(),
			left->isconst(),
			left->getwl(),
			leftnm,
			left->gettc(),
			right->isconst(),
			right->getwl(),
			rightnm,
			right->gettc());
    break;
  case and_op: 
    os->binoparithlogic(" and ",
			seteq->gettc(),
			seteq->getwl(),
			seteq->getname(),
			left->isconst(),
			left->getwl(),
			leftnm,
			left->gettc(),
			right->isconst(),
			right->getwl(),
			rightnm,
			right->gettc());
    break;
  case shl_op: 
    os->binopshift(" << ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm);
    break;
  case shr_op:
    os->binopshift(" >> ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm);
    break;
  case add_op: 
    os->binoparith(" + ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
  case sub_op: 
    os->binoparith(" - ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
  case concat_op: 
    os->binoparith(" & ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
  case mul_op: 
    os->binoparith(" * ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
  case mod_op: 
    os->binoparith(" % ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
  case cast_op: 
    os->binoparith(" cast ",
		   seteq->gettc(),
		   seteq->getwl(),
		   seteq->getname(),
		   left->isconst(),
		   left->getwl(),
		   leftnm,
		   left->gettc(),
		   right->isconst(),
		   right->getwl(),
		   rightnm,
		   right->gettc());
    break;
    
  case eq_op: 
    os->binoplogic (" = ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  case ne_op:
    os->binoplogic (" /= ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  case smt_op: 
    os->binoplogic (" < ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  case grt_op: 
    os->binoplogic (" > ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  case smteq_op: 
    os->binoplogic (" <= ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  case grteq_op: 
    os->binoplogic (" >= ",
		    seteq->getname(),
		    left->isconst(),
		    leftnm,
		    left->gettc(),
		    left->getwl(),
		    right->isconst(),
		    rightnm,
		    right->gettc());
    break;
  }
}

