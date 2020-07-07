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

#include "vhdvalldp.h"

#include "vhdctl.h"
#include "vhdvar.h"
#include "vhdbinop.h"
#include "vhdunop.h"
#include "vhdterop.h"
#include "vhdassign.h"
#include "vhdlut.h"
#include "vhdctl.h"
#include "vhdreport.h"
#include "vhdportnet.h"


int vmax(int first, int second) {
    if (first > second)
	return first;
    else
	return second;
}    

//--------------------------------------------------------------
vhdvalldp::vhdvalldp() {}

vhdvalldp::~vhdvalldp() {}

bool vhdvalldp::isclonedp() {
  return false;
}

void vhdvalldp::addctl(vhdctl* f){}

void vhdvalldp::addSfg(symid v, string f){}

void vhdvalldp::adddp(symid v, vhdvalldp* f){}

void vhdvalldp::addlut(symid v, vhdlut* f){}

void vhdvalldp::addportmapentry(string n, vhdvar* v,vhdvar* vv){}

void vhdvalldp::addbinInstr(symid v, symid vv, vhdbinop* op){}

void vhdvalldp::addunInstr(symid v, symid vv, vhdunop* op){}

void vhdvalldp::addterInstr(symid v, symid vv, vhdterop* op){}

void vhdvalldp::addmultterInstr(symid v,symid cur_sfg,vhdterop* s){}

void vhdvalldp::addassignInstr(symid v, symid vv, vhdassign* op, bool t){}

void vhdvalldp::addlutInstr(symid v, symid vv,vhdlutop* l){}

void vhdvalldp::addsfgvar(symid id, vhdvar* v){}

void vhdvalldp::addvar(symid id, vhdvar* v){}

void vhdvalldp::addbit(symid id, vhdvar* v){}

void vhdvalldp::addportvar(symid v, vhdvar* d){}

void vhdvalldp::addcondvar(vhdctlstep * step){}

void vhdvalldp::addgeneric(string, string, string) {}

void vhdvalldp::addattributedef(string, string) {}

void vhdvalldp::addattributeuse(string, string, string) {}

string vhdvalldp::getname() {
  return 0;
}

string vhdvalldp::getparentname() {
  return 0;
}

vhdlut* vhdvalldp::getlut(symid v) {
  return 0;
}

vhdvar* vhdvalldp::getvar(symid v) {
  return 0;
}

vhdvar* vhdvalldp::getport(symid v) {
  return 0;
}

vhdvar* vhdvalldp::getoport_tr(vhdvar *) {
  return 0;
}

void    vhdvalldp::setoport_tr(vhdvar *, vhdvar *) {}

vhdvar* vhdvalldp::getvarport(symid v) {
  return 0;
}

string  vhdvalldp::getsfgname(symid v) {
  return 0;
}

vhdstate* vhdvalldp::getstate(symid v) {
  return 0;
}

vector<vhdmapentry *> vhdvalldp::getportmap(vector<vhdnet *> nt) {
  vector<vhdmapentry *> n;
  return n;
}

string vhdvalldp::getdptype() {
  return 0;
}

string vhdvalldp::getoptype(symid v) {
  return 0;
}

vhdop* vhdvalldp::getop(symid v) {
  return 0;
}

bool vhdvalldp::gethasclk() {return 0;}

void vhdvalldp::resetcount(){}

void vhdvalldp::resetbitcount(){}

int vhdvalldp::getbitcount(){
  return 0;
}

string vhdvalldp::vautoname() {
  return 0;
}

string vhdvalldp::vautoname(string base, int num) {
  return 0;
}

void vhdvalldp::setramwl(string){}

void vhdvalldp::setramsize(string){}

void vhdvalldp::sethasclk(bool){}

void vhdvalldp::addtableentry(vhdtableentry* t){}

void vhdvalldp::mergeentry(){}

void vhdvalldp::dumptable(ostream & os){}

void vhdvalldp::preprocess(){}

void vhdvalldp::rmdeadcode(vhdvar * v) {}

void vhdvalldp::dump(string clkname,string rstname,int synclk, int resetval){}

void vhdvalldp::debugreport(vhdreport* report) {}
