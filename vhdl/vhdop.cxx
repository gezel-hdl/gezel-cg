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

#include "vhdop.h"
#include "vhdvar.h"

vhdop::vhdop() {}
vhdop::~vhdop() {}

void vhdop::addseteq(vhdvar* t) {}
void vhdop::addter(vhdterop* t) {}
void vhdop::vhdoutop(vhdout*,bool reg, bool isfsmcmb) {}
vhdvar* vhdop::getleft() {return 0;}
vhdvar* vhdop::getright() {return 0;}
vhdvar* vhdop::getmiddle() {return 0;}
string vhdop::gettype() {return 0;}
string vhdop::getoperator() {return 0;}
vector<vhdterop *> vhdop::getteroplist() {
	vector<vhdterop *> n;
	return n;
}
vhdvar* vhdop::geteq() {return 0;}
bool vhdop::isvalid() {return false;}
void vhdop::setvalid() {}
