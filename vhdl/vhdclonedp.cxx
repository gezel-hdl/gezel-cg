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

#include "vhdclonedp.h"

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
#include "vhdsfg.h"
#include "vhdout.h"


//--------------------------------------------------------------
vhdclonedp::vhdclonedp(string nm,vhdvalldp* parent,_dptype tp){
  name = nm;
  parentdp = parent;
  dptype = tp;
}

vhdclonedp::~vhdclonedp() {
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid, vhdvar *>::iterator i = sfgvarlist.begin(); i != sfgvarlist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid, vhdvar *>::iterator i = bitlist.begin(); i != bitlist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid, vhdsfg *>::iterator i = sfglist.begin(); i != sfglist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (vector<vhdvar *>::iterator i = controllist.begin(); i != controllist.end(); ++i)
      delete (*i);
  for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i)
      delete (*i);
  for (map<string, vhdportmap *>::iterator i = maplist.begin(); i != maplist.end(); ++i)
    if ((i->second))
      delete ((i->second));
}

bool vhdclonedp::isclonedp() {
  return true;
}

void vhdclonedp::addctl(vhdctl* ctl) {
  controller = ctl;
}

void vhdclonedp::addSfg(symid id, string name) {
  vhdsfg* t = new vhdsfg(id, name);
  sfglist[id] = t;
}

void vhdclonedp::adddp(symid v, vhdvalldp* dp) {
  vhdportmap* t = new vhdportmap(dp);
  maplist[dp->getname()] = t;
}

void vhdclonedp::addlut(symid v,vhdlut* lut) {
  lutlist[v] = lut;
}

void vhdclonedp::addportmapentry(string dpnm,vhdvar* portv, vhdvar* targetv) {
  vhdmapentry* t = new vhdmapentry (portv,targetv);
  maplist[dpnm]->addoneentry(t);
  if(portv->isout())
    targetv->setio(1);
}
  
void vhdclonedp::addbinInstr(symid v, symid sfg, vhdbinop* t) {
  sfglist[sfg]->addbinInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdclonedp::addunInstr(symid v, symid sfg, vhdunop* t) {
  sfglist[sfg]->addunInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdclonedp::addterInstr(symid v, symid sfg, vhdterop* t) {
  sfglist[sfg]->addterInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdclonedp::addmultterInstr(symid v,symid sfg,vhdterop* t) {
  sfglist[sfg]->addmultterInstr(v,t);
  oplist[v] = t;
}  

void vhdclonedp::addassignInstr(symid v, symid sfg, vhdassign* t, bool isreg) {
  sfglist[sfg]->addassignInstr(v,t,isreg);
  cur_sfg = sfg;
  if (isreg)
	RegInstrEmpty = false;
  else
        CmbInstrEmpty = false;
  oplist[v] = t;
}  

void vhdclonedp::addlutInstr(symid v,symid sfg,vhdlutop* t) {
  sfglist[sfg]->addlutInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdclonedp::addsfgvar(symid id, vhdvar* variable) {
  sfgvarlist[id] = variable;
}

void vhdclonedp::addvar(symid id, vhdvar* variable) {
  varlist[id] = variable;
}

void vhdclonedp::addbit(symid id, vhdvar* variable) {
  bitlist[id] = variable;
}

void vhdclonedp::addportvar(symid id, vhdvar* variable) {
  portlist[id] = variable;
}

void vhdclonedp::addcondvar(vhdctlstep * step){
  vector<vhdvar *> tsteplist;

  tsteplist = step->getcondvar();
  for (vector<vhdvar *>::iterator i = tsteplist.begin(); i != tsteplist.end(); ++i) 
	condvar.push_back(*i);
}

string vhdclonedp::getname() {
  return name;
}

string vhdclonedp::getparentname() {
  return parentdp->getname();
}

vhdlut* vhdclonedp::getlut(symid id) {
  return lutlist[id];
}

vhdvar* vhdclonedp::getvar(symid id) {
  if (varlist[id])
	return varlist[id];
  else if (sfgvarlist[id])
	return sfgvarlist[id];
  else if (bitlist[id])
	return bitlist[id];
  else 
	return NULL;
}

vhdvar* vhdclonedp::getport(symid id) {
  if (portlist[id])
	return portlist[id];
  else 
	return NULL;
}

void vhdclonedp::setoport_tr(vhdvar *p, vhdvar *tr) {
  oport_tr[p] = tr;
}

vhdvar *vhdclonedp::getoport_tr(vhdvar *p) {
  if (oport_tr.find(p) != oport_tr.end()) {    
    return oport_tr[p];
  } else 
    return NULL;
}



vhdvar* vhdclonedp::getvarport(symid id) {
  vhdvar* t = getvar(id);
  if (t == NULL) {
    t = getport(id);
    if (t == NULL) {
      if(bitlist[id])
	return bitlist[id];
      else
	return NULL;
    }
  }			
  return t;
}

string vhdclonedp::getsfgname (symid id) {
  return sfglist[id]->getname();
}

vhdstate* vhdclonedp::getstate(symid id) {
  return (controller->getstate(id));
}

vector<vhdmapentry *> vhdclonedp::getportmap(vector<vhdnet *> netlist) {
  vector<vhdmapentry *> list;
  vhdmapentry* v;
  vhdvar* org;
 
  bool notfound;

  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i) {
    if (i->second) {
      notfound = false;
      org = new vhdvar(i->second->getname(),1,i->second->gettc(),i->second->getwl(),true,i->second->getio());
      for (vector<vhdnet *>::iterator k = netlist.begin(); k != netlist.end(); ++k) {
	//cout<<"input: "<<(*k)->getinput()<<" Output: "<<(*k)->getoutput()<<endl;
	if(((*k)->getinput() == i->first) || ((*k)->getoutput() == i->first) ) {
	  (*k)->getvar()->updateparameter((*k)->getvar()->getname(),org->gettc(),vmax(org->getwl(),(*k)->getvar()->getwl()),1,false,false);
	  v = new vhdmapentry(org,(*k)->getvar());
	  notfound = false;
	  break;			
	}
	else
	  notfound = true;
      }
      if(notfound)
	v = new vhdmapentry(org,NULL);
      list.push_back(v);
    }
  }	     
  return list;
}

string vhdclonedp::getdptype() {
  switch (dptype) {
     case dp:      return "dp";
     case dp_ram:  return "ram";
     case dp_ip:   return "ip";
  }
  return NULL;
}

string vhdclonedp::getoptype(symid v) {
  if (oplist[v])
     return oplist[v]->gettype();
  else
     return "none";
}

vhdop* vhdclonedp::getop(symid v) {
	return oplist[v];
}

bool vhdclonedp::gethasclk() {
  return hasclk;
}

void vhdclonedp::resetcount() {  count = 0;  }

void vhdclonedp::resetbitcount() { bitcount = 0; }

int vhdclonedp::getbitcount() {
  return bitcount++;
}

string vhdclonedp::vautoname() {
  ostrstr_t strstrm;
  strstrm << "sig_" <<count++;
  return strstrm.str();
}

string vhdclonedp::vautoname(string base,int num) {
    ostrstr_t out;
    out <<base<<"_"<<num;
    return out.str();
}

void vhdclonedp::setramwl(string t) {
  gval *y = make_gval(t.c_str());
  ramwl = y->toulong();
  return;
}

void vhdclonedp::setramsize(string t) {
  gval *y = make_gval(t.c_str());
  ramsize = y->toulong();
  return;
}

void vhdclonedp::sethasclk(bool clkset) {
  hasclk = clkset;
}

void vhdclonedp::addtableentry(vhdtableentry* t) {
   table.push_back(t);
   controller->addctlcmd( t->getcount(),t->getname() );
}

void vhdclonedp::mergeentry() {

  for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i) {
     for (vector<vhdtableentry *>::iterator j = i+1; j != table.end(); ++j) {
	if( (*i)->samesfglist(*j) ) {
           controller->addctlcmd( (*j)->getcount(),(*i)->getname() );
	   table.erase(j,j+1);
           j--;
	}	
     }
  }
} 

void vhdclonedp::dumptable(ostream & os) {
//  os  <<"table:"<<endl;
//  for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i) 
//      (*i)->dump(os);
//  os  <<endl;  
}
void vhdclonedp::preprocess(){
  sethasclk(parentdp->gethasclk());
}

void vhdclonedp::rmdeadcode(vhdvar * v) {}
void vhdclonedp::dumpdp(string clkname, string rstname, int synclk){}
void vhdclonedp::dumpram(string clkname, string rstname, int synclk){}
void vhdclonedp::dumpip(string clkname, string rstname, int synclk) {}
void vhdclonedp::dump(string clkname, string rstname, int synclk){}
void vhdclonedp::debugreport(vhdreport* report){
  report->outputline("cloned datapath "+getname()+" from datapath "+getparentname());
}
