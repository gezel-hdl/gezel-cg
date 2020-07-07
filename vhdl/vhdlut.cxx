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

#include "vhdlut.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdlut::vhdlut(int _wlen,int _sign,vector<char *> _content,string _name) {
  wlen = _wlen;
  sign = _sign;

  for (unsigned i=0; i< _content.size(); ++i) {
    content.push_back(make_gval(_content[i]));
  }
  for (unsigned i=0; i< _content.size(); ++i) {
    content[i]->setwordlength(wlen);
  }

  name = _name;
  size = content.size();
}
 
vhdlut::~vhdlut() {}

int vhdlut::getwlen() {
  return wlen;
}

int vhdlut::getsign() {
  return sign;
}

int vhdlut::getsize() {
  return size;
}

string vhdlut::getname() {
  return name;
}

vector<gval *> vhdlut::getcontent() {
  return content;
}

//--------------------------------------------------------------
vhdlutop::vhdlutop(vhdlut* l, vhdvar* arg) {
  lut = l; 
  left = arg;
  valid = false;
}

vhdlutop::~vhdlutop() {}

string vhdlutop::gettype() {
  return "notuse";
}

vhdvar* vhdlutop::getleft() {
  return left;
}

string vhdlutop::getoperator() {
  return "lutop";
}

vhdvar* vhdlutop::geteq() {
  return seteq;
}

void vhdlutop::addseteq(vhdvar* t) {
  seteq = t;
}

bool vhdlutop::isvalid() {
  return valid;
}

void vhdlutop::setvalid() {
  valid = true;
}
 
void vhdlutop::vhdoutop (vhdout* os,bool reg, bool isfsmcmb) {
  string leftnm;
  if (left->isreg()&& isfsmcmb)
     leftnm = left->getregname();
  else
     leftnm = left->getname();
  os->lutop(seteq->getname(),seteq->getwl(),lut->getname(),lut->getsize(),leftnm,left->getwl(),left->isconst());
}
