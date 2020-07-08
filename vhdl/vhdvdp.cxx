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


#include "vhdvdp.h"

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

class vhdebugop {
  map<int ,int> oplist;
public:
  vhdebugop(int);
  ~vhdebugop();
  void addop(int bitsize);
  void outopreport(vhdreport*,string);
};

//-------------------------------------------------------------

vhdebugop::vhdebugop(int bitsize) {oplist[bitsize] = 1;}
vhdebugop::~vhdebugop() {}
void vhdebugop::addop(int bitsize) {oplist[bitsize]++;}
void vhdebugop::outopreport(vhdreport* report,string name) {
  for (map<int, int>::iterator i = oplist.begin(); i != oplist.end(); ++i)
    if ((i->second))
	report->outop(i->second,i->first,name);
}

//-------------------------------------------------------------


vhdvdp::vhdvdp(string _name, _dptype tp) : name(_name), dptype(tp) {
  RegInstrEmpty = true;
  CmbInstrEmpty = true;
  controller = NULL;
}

vhdvdp::~vhdvdp() {
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
  for (map<symid, vhdlut *>::iterator i = lutlist.begin(); i != lutlist.end(); ++i)
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

void vhdvdp::addctl(vhdctl* ctl) {
  controller = ctl;
}

bool vhdvdp::isclonedp() {
  return false;
}

void vhdvdp::addgeneric(string p, string t, string d) {
  // format: generic dpname port type default_value
  // example: generic user_logic C_DWIDTH integer 32
  generic_port.push_back(p);
  generic_type.push_back(t);
  generic_default.push_back(d);
}

void vhdvdp::addattributeuse(string p, string t, string d) {
  attribute_use_attribute.push_back(p);
  attribute_use_signal.push_back(t);
  attribute_use_value.push_back(d);
}

void vhdvdp::addattributedef(string p, string t) {
  attribute_def_attribute.push_back(p);
  attribute_def_type.push_back(t);
}

void vhdvdp::addSfg(symid id, string name) {
  vhdsfg* t = new vhdsfg(id, name);
  sfglist[id] = t;
}

void vhdvdp::adddp(symid v, vhdvalldp* dp) {
  vhdportmap* t = new vhdportmap(dp);
  maplist[dp->getname()] = t;
}

void vhdvdp::addlut(symid v, vhdlut* lut) {
  lutlist[v] = lut;
}

void vhdvdp::addportmapentry(string dpnm,vhdvar* portv, vhdvar* targetv) {
  vhdmapentry* t = new vhdmapentry (portv,targetv);
  maplist[dpnm]->addoneentry(t);
  if(portv->isout())
    targetv->setio(1);
}
  
void vhdvdp::addbinInstr(symid v, symid sfg, vhdbinop* t) {
  sfglist[sfg]->addbinInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdvdp::addunInstr(symid v, symid sfg, vhdunop* t) {
  sfglist[sfg]->addunInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdvdp::addterInstr(symid v, symid sfg, vhdterop* t) {
  sfglist[sfg]->addterInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdvdp::addmultterInstr(symid v,symid sfg,vhdterop* t) {
  sfglist[sfg]->addmultterInstr(v,t);
//  oplist[v] = t;
}  

void vhdvdp::addassignInstr(symid v, symid sfg, vhdassign* t, bool isreg) {
  sfglist[sfg]->addassignInstr(v,t,isreg);
  cur_sfg = sfg;
  if (isreg)
    RegInstrEmpty = false;
  else
    CmbInstrEmpty = false;
  oplist[v] = t;
}  

void vhdvdp::addlutInstr(symid v,symid sfg,vhdlutop* t) {
  sfglist[sfg]->addlutInstr(v,t);
  cur_sfg = sfg;
  CmbInstrEmpty = false;
  oplist[v] = t;
}

void vhdvdp::addsfgvar(symid id, vhdvar* variable) {
  sfgvarlist[id] = variable;
}

void vhdvdp::addvar(symid id, vhdvar* variable) {
  varlist[id] = variable;
}

void vhdvdp::addbit(symid id, vhdvar* variable) {
  bitlist[id] = variable;
}

void vhdvdp::addportvar(symid id, vhdvar* variable) {
  portlist[id] = variable;
}

void vhdvdp::addcondvar(vhdctlstep * step){
  vector<vhdvar *> tsteplist;

  tsteplist = step->getcondvar();
  for (vector<vhdvar *>::iterator i = tsteplist.begin(); i != tsteplist.end(); ++i) 
    condvar.push_back(*i);
}

string vhdvdp::getname() {
  return name;
}

vhdlut* vhdvdp::getlut(symid id) {
  return lutlist[id];
}

void vhdvdp::setoport_tr(vhdvar *p, vhdvar *tr) {
  oport_tr[p] = tr;
}

vhdvar *vhdvdp::getoport_tr(vhdvar *p) {
  if (oport_tr.find(p) != oport_tr.end()) {    
    return oport_tr[p];
  } else 
    return NULL;
}

vhdvar* vhdvdp::getvar(symid id) {
  if (varlist[id])
    return varlist[id];
  else if (sfgvarlist[id])
    return sfgvarlist[id];
  else if (bitlist[id])
    return bitlist[id];
  else 
    return NULL;
}

vhdvar* vhdvdp::getport(symid id) {
  if (portlist[id])
    return portlist[id];
  else 
    return NULL;
}

vhdvar* vhdvdp::getvarport(symid id) {
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
  
string vhdvdp::getsfgname (symid id) {
  return sfglist[id]->getname();
}

vhdstate* vhdvdp::getstate(symid id) {
  return (controller->getstate(id));
}

vector<vhdmapentry *> vhdvdp::getportmap(vector<vhdnet *> netlist) {
  vector<vhdmapentry *> list;
  vhdmapentry* v;
  vhdvar* org;
 
  bool found;

  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i) {
    if (i->second) {
      found = false;
      org = new vhdvar(i->second->getportname(),1,i->second->gettc(),i->second->getwl(),true,i->second->getio());
      for (vector<vhdnet *>::iterator k = netlist.begin(); k != netlist.end(); ++k) {
	//cout<<"input: "<<(*k)->getinput()<<" Output: "<<(*k)->getoutput()<<endl;
	if(((*k)->getinput() == i->first) || ((*k)->getoutput() == i->first) ) {
	  (*k)->getvar()->updateparameter((*k)->getvar()->getname(),org->gettc(),vmax(org->getwl(),(*k)->getvar()->getwl()),1,false,false);
	  v = new vhdmapentry(org,(*k)->getvar());
	  found = true;
	  break;			
	}
      }
      if(found == false)
	v = new vhdmapentry(org,NULL);
      list.push_back(v);
    }
  }	     
  return list;
}

string vhdvdp::getdptype() {
  switch (dptype) {
  case dp:      return "dp";
  case dp_ram:  return "ram";
  case dp_ip:   return "ip";
  }
  return NULL;
}

string vhdvdp::getoptype(symid v) {
  if (oplist[v])
    return oplist[v]->gettype();
  else
    return "none";
}

vhdop* vhdvdp::getop(symid v) {
	return oplist[v];
}

bool vhdvdp::gethasclk() {
  return hasclk;
}

void vhdvdp::resetcount() {  count = 0;  }

void vhdvdp::resetbitcount() { bitcount = 0; }

int vhdvdp::getbitcount() {
  return bitcount++;
}

string vhdvdp::vautoname() {
  ostringstream strstrm;
  strstrm << "sig_" <<count++;
  return strstrm.str();
}

string vhdvdp::vautoname(string base,int num) {
  ostringstream out;
  out <<base<<"_"<<num;
  return out.str();
}

void vhdvdp::setramwl(string t) {
  gval *y = make_gval(t.c_str());

  ramwl = y->toulong();
  return;
}

void vhdvdp::setramsize(string t) {
  gval *y = make_gval(t.c_str());

  ramsize = y->toulong();
  return;
}

void vhdvdp::sethasclk(bool clkset) {
  hasclk = clkset;
}

void vhdvdp::addtableentry(vhdtableentry* t) {
  table.push_back(t);
  controller->addctlcmd( t->getcount(),t->getname() );
}

void vhdvdp::mergeentry() {
  
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

void vhdvdp::dumptable(ostream & os) {
  os  <<"table:"<<endl;
  for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i) 
    (*i)->dump(os);
  os  <<endl;  
}


void vhdvdp::preprocess() {
  vector<string> tmp;
  if (controller) {
    if (controller->gettype() == "fsm") {
      //       dumptable(cout);
      mergeentry();
      //       dumptable(cout);
    }
    if(controller->gettype() == "fsm") 
	hasclk = true;
    else if( !RegInstrEmpty ) 
	hasclk = true;
    else
	hasclk = false;
  }

  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i) {
    if ((i->second)) 
      if ( i->second->isout() ) 
	rmdeadcode(i->second);
  }
  
  for (vector<vhdvar *>::iterator i = condvar.begin(); i != condvar.end(); ++i) 
    rmdeadcode((*i));
  
  vector<vhdvar *> tarlist;
  for (map<string, vhdportmap *>::iterator i = maplist.begin(); i != maplist.end(); ++i)  {
    if (i->second) {
      tarlist = i->second->gettargetlist();
      for (vector<vhdvar *>::iterator j = tarlist.begin(); j != tarlist.end(); ++j) 
	rmdeadcode((*j));
	i->second->updatehasclk();
	if (i->second->gethasclk())
		hasclk = true;
    }
  }
}

void vhdvdp::rmdeadcode(vhdvar * v) {
  vector<vhdterop *> tmpterlist;
  if (v->isindex())
    rmdeadcode(v->getbasevar());
  else
    for (map<symid, vhdop *>::iterator i = oplist.begin(); i != oplist.end(); ++i) {
      if (i->second) 
	if (i->second->geteq() == v) {
	  if (i->second->isvalid())
	    break;
	  else if (i->second->getoperator() == "binop"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	    rmdeadcode(i->second->getright());
	  }
	  else if (i->second->getoperator() == "dunop"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	  }
	  else if (i->second->getoperator() == "terop"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	    rmdeadcode(i->second->getmiddle());
	    rmdeadcode(i->second->getright());
	  }
	  else if (i->second->getoperator() == "multerop"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	    rmdeadcode(i->second->getmiddle());
	    rmdeadcode(i->second->getright());
	    
	    tmpterlist = i->second->getteroplist();
	    for (vector<vhdterop *>::iterator j = (tmpterlist.end()-1); j != (tmpterlist.begin()-1); --j) {
	      rmdeadcode((*j)->getleft());
	      rmdeadcode((*j)->getmiddle());
	    }
	  }
	  else if (i->second->getoperator() == "assign"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	  }
	  else if (i->second->getoperator() == "lutop"){
	    i->second->setvalid();
	    rmdeadcode(i->second->getleft());
	  }	  
        }
    }
}

void vhdvdp::dumpdp(string clkname, string rstname,int synclk,int resetval) {
  string vhd = ".vhd";
  string filename = name + vhd;
  
  vector<symid> id;
  bool   first_fsm = true;
  bool   first_dp = true;
  ostringstream strstrm_fsm;
  ostringstream strstrm_dp;
  int count;
  
  vhdout* vhdlout = new vhdout(filename);
  
  cout<<"Generate file: "<<filename<<endl;
  
  /**********************************************************************************************/
  
  vhdlout->library("ieee");
  vhdlout->lib_use("IEEE.std_logic_1164.all");
  vhdlout->lib_use("IEEE.std_logic_arith.all");
  vhdlout->library("work");
  vhdlout->lib_use("work.std_logic_arithext.all");
  
  count = 0;
  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i) {
    if ((i->second)) {
      if ( !(i->second->isconst()) && !(i->second->isindex()) ) {
	if (!( (i->second->isport()) && (i->second->getio()) )) {
	  if (first_dp) 
	    first_dp = false;	
	  else
	    strstrm_dp << ",";
	  strstrm_dp <<i->second->getname();
	  count ++;
	  if (count == 10) {
	    strstrm_dp <<endl;
	    count = 0;
	  }
	  
	}
      } 
    }
  }
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i) {
    if ((i->second)) {
      if (!( i->second->isout() )) {
	if (first_dp) 
	  first_dp = false;	
	else
	  strstrm_dp << ",";
	if (count == 10) {
	  strstrm_dp <<i->second->getname()<<endl;
	  count = 0;
	}
	else	
	  strstrm_dp <<i->second->getname();
	count ++;
	if (count == 10) {
	  strstrm_dp <<endl;
	  count = 0;
	}
      }      
    }
  }
  
  count = 0;
  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i) {
    if ((i->second)) {
      if ( !(i->second->isconst()) && !(i->second->isindex()) ) {
	if (!( (i->second->isport()) && (i->second->getio()) )) {
	  if (first_fsm) 
	    first_fsm = false;	
	  else
	    strstrm_fsm << ",";
	  // fsmCMB uses only reg outputs
	  //		if (i->second->isreg())
	  //			strstrm_fsm <<i->second->getregname();	
	  //		else
	  strstrm_fsm <<i->second->getname();
	  count ++;
	  if (count == 10) {
	    strstrm_fsm <<endl;
	    count = 0;
	  }
	}
      } 
    }
  }
  for (map<symid, vhdvar *>::iterator i = sfgvarlist.begin(); i != sfgvarlist.end(); ++i) {
    if ((i->second)) {
      if ( !(i->second->isconst()) && !(i->second->isindex()) ) {
	if (!( (i->second->isport()) && (i->second->getio()) )) {
	  if (first_fsm)
	    first_fsm = false;
	  else
	    strstrm_fsm << ",";
	  // fsmCMB uses only reg outputs
	  //                if (i->second->isreg())
	  //                     strstrm_fsm <<i->second->getregname();
	  //               else
	  strstrm_fsm <<i->second->getname();
	  count ++;
	  if (count == 10) {
	    strstrm_fsm <<endl;
	    count = 0;
	  }
	}
      }
    }
  }
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i) {
    if ((i->second)) {
      if (!( i->second->isout() )) {
	if (first_fsm) 
	  first_fsm = false;	
	else
	  strstrm_fsm << ",";
	strstrm_fsm <<i->second->getname();
	count ++;
	if (count == 10) {
	  strstrm_fsm <<endl;
	  count = 0;
	}
      }      
    }
  }
  
  vhdlout->nextline(2);
  vhdlout->comment("datapath entity");
  vhdlout->beginentity(name);
  if (generic_port.size() > 0) {
    // has generic parameters
    vhdlout->beginentitygeneric();

    for (unsigned i=0; i<generic_port.size(); i++) {
      vhdlout->dumpgeneric(generic_port[i], 
			   generic_type[i],
			   generic_default[i]);
    }

    vhdlout->endentitygeneric();
  }
  vhdlout->beginentityport();
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
    if ((i->second))
      vhdlout->dumpportvar(i->second);

  vhdlout->endports(name,hasclk,clkname,rstname);

  // attributes
  for (unsigned i=0; i<attribute_def_attribute.size(); i++) 
    vhdlout->dumpattrdef(attribute_def_attribute[i], 
			 attribute_def_type[i]);

  for (unsigned i=0; i<attribute_use_attribute.size(); i++) 
    vhdlout->dumpattruse(attribute_use_attribute[i], 
			 attribute_use_signal[i],
			 attribute_use_value[i]);

  vhdlout->endentity(name);

  vhdlout->nextline(2);
  vhdlout->comment("signal declaration");
  vhdlout->architecture("RTL",name);
  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
    if ((i->second)) 
      if ( !(i->second->isconst()) && !(i->second->isindex()) )
	vhdlout->dumpvardeclare(i->second);
  for (map<symid, vhdvar *>::iterator i = sfgvarlist.begin(); i != sfgvarlist.end(); ++i)
    if ((i->second)) 
      if ( !(i->second->isconst()) && !(i->second->isindex()) )
	vhdlout->dumpvardeclare(i->second);
  for (map<symid, vhdvar *>::iterator i = bitlist.begin(); i != bitlist.end(); ++i)
    if ((i->second))
      vhdlout->dumpvardeclare(i->second);
  
  if(!lutlist.empty()) {
    vhdlout->nextline(2);
    vhdlout->comment("lookup table declaration");
    int lutcount = 0;
    string luttypename = "rom_table";
    for (map<symid, vhdlut *>::iterator i = lutlist.begin(); i != lutlist.end(); ++i)
      if ((i->second)) {
	vhdlout->lutdeclare(vautoname(luttypename,lutcount),i->second->getwlen(),i->second->getsize());
	vhdlout->luttable(i->second->getname(),vautoname(luttypename,lutcount++),i->second->getwlen(),i->second->getcontent(),i->second->getsize());	  
      }
  }
  
  map<string, bool> clonecomponentlist;
  
  if(!maplist.empty()) {
    vhdlout->nextline(2);
    vhdlout->comment("component map declaration");
    for (map<string, vhdportmap*>::iterator i = maplist.begin(); i != maplist.end(); ++i) {
      if (i->second) {
	if (!(i->second->isclone())) {
	  if (!clonecomponentlist[i->first]) {
	    vhdlout->begincomponent(i->first);
	    i->second->dumpportvar(vhdlout);
	    vhdlout->endcomponent(i->first,i->second->gethasclk(),clkname,rstname);
	    clonecomponentlist[i->first] = true;
	  }
	}
	else {
	  if(!clonecomponentlist[i->second->getparentnm()]) {
	    vhdlout->begincomponent(i->second->getparentnm());
	    i->second->dumpportvar(vhdlout);
	    vhdlout->endcomponent(i->second->getparentnm(),i->second->gethasclk(),clkname,rstname);
	    clonecomponentlist[i->second->getparentnm()] = true;			
	  }
	}
      }
    } 
  }
  

  if(controller) {
    if(controller->gettype() == "fsm") {

	if(first_fsm) 
           strstrm_fsm <<"cmd,STATE";
	else 
	   strstrm_fsm <<",cmd,STATE";
	if(first_dp) 
           strstrm_dp <<"cmd,STATE";
	else 
	   strstrm_dp <<",cmd,STATE";

	controller->vhdoutdeclaration(vhdlout);
        vhdlout->outputchar("type CONTROL is (");
	first_dp = true;
	for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i)  {
	  if(first_dp) {
		vhdlout->outputchar((*i)->getname());
		vhdlout->nextline(1);
		first_dp = false;
	  }
	  else {
		vhdlout->outputchar(", "+(*i)->getname());
		vhdlout->nextline(1);
	  }
	}
	vhdlout->outputchar(");");
	vhdlout->nextline(1);
    	vhdlout->outputline("signal cmd : CONTROL;");
		

       // PS - 'begin' after architecture is unconditional
       //	if( !RegInstrEmpty || !CmbInstrEmpty || !maplist.empty()) {
		vhdlout->nextline(2);
		vhdlout->begin();
		vhdlout->increaseindent();
       //	}

        if( !maplist.empty() ) {
	    vhdlout->nextline(2);
	    vhdlout->comment("portmap");
	    for (map<string, vhdportmap*>::iterator i = maplist.begin(); i != maplist.end(); ++i)
	 	if (i->second) {
		    if (i->second->isclone())
			    vhdlout->beginportmap(i->first,i->second->getparentnm());
		    else
			    vhdlout->beginportmap(i->first,i->first);
		    i->second->dumpmapvar(vhdlout);
		      vhdlout->endportmap(i->second->gethasclk(),clkname,rstname,clkname,rstname);
		}
        }

        if( !RegInstrEmpty ) {
	  vhdlout->comment("register updates");
	  vhdlout->process("dpREG",clkname + "," + rstname);
	  vhdlout->increaseindent();
	  vhdlout->begin();
	  vhdlout->increaseindent();
	  if(synclk) {
	    vhdlout->ifclk(clkname);
	    vhdlout->increaseindent();
	    vhdlout->ifresethigh(rstname, resetval);
	    vhdlout->increaseindent();
	    for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	      if ((i->second))
		if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()) )
		  vhdlout->defaultzero(i->second,true);
	    vhdlout->decreaseindent();
	    vhdlout->outputline("else");
	    vhdlout->increaseindent();
	    for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	      if ((i->second))
		if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()) )
		  vhdlout->assignreg(i->second);
	    vhdlout->nextline(1);
	    vhdlout->decreaseindent();
	    vhdlout->endif();
	    vhdlout->decreaseindent();
	    vhdlout->endif();
	  }
	  else {
	    vhdlout->ifresethigh(rstname, resetval);
	    vhdlout->increaseindent();
	    for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	      if ((i->second))
		if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()) )
		  vhdlout->defaultzero(i->second,true);
	    vhdlout->decreaseindent();
	    vhdlout->elseclk(clkname);
	    vhdlout->increaseindent();
	    for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	      if ((i->second))
		if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()) )
		  vhdlout->assignreg(i->second);
	    
	    vhdlout->nextline(1);
	    vhdlout->decreaseindent();
	    vhdlout->endif();
	  }
	  vhdlout->decreaseindent();
	  vhdlout->endprocess("dpREG");
	  vhdlout->decreaseindent();
	}
	if( !CmbInstrEmpty) {
	  vhdlout->nextline(2);
	  vhdlout->comment("combinational logics");
	  vhdlout->process("dpCMB",strstrm_dp.str());
	  
	  vhdlout->increaseindent();
	  vhdlout->begin();
	  vhdlout->increaseindent();
	  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	    if ((i->second)) {
	      if (i->second->isreg() && !( i->second->isconst() ) && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->assigndefaultreg(i->second);
	      else {
		if (!( i->second->isconst() ) && !(i->second->isindex()) && !(i->second->isout()))
		  vhdlout->defaultzero(i->second,true);
	      }
	    }
	  for (map<symid, vhdvar *>::iterator i = bitlist.begin(); i != bitlist.end(); ++i)
	    if ((i->second))
	      vhdlout->dumpbitstream(i->second);
	  
	  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
	    if ((i->second))
	      if ( i->second->isout() )
		vhdlout->defaultzero(i->second,true);
	  
	  vhdlout->nextline(1);
	  
	  vhdlout->nextline(2);
	  vhdlout->casestr("cmd");
	  vhdlout->increaseindent();
	  for (vector<vhdtableentry *>::iterator i = table.begin(); i != table.end(); ++i) {
	    vhdlout->when( (*i)->getname() );
	    id = (*i)->getsfgid(); 
	    vhdlout->increaseindent();
	    for (vector<symid>::iterator i = id.begin(); i != id.end(); ++i) {
	      sfglist[(*i)]->vhdoutcmb(vhdlout);      
	      sfglist[(*i)]->vhdoutreg(vhdlout);
	    }
	    vhdlout->decreaseindent();
	  }
	  vhdlout->outputline("when others=>");
	  vhdlout->decreaseindent();
	  vhdlout->endcase();
	  vhdlout->decreaseindent();
	  vhdlout->endprocess("dpCMB");
	  vhdlout->decreaseindent();
	}
	
	vhdlout->nextline(2);
	vhdlout->comment("controller reg");
	vhdlout->process("fsmREG",clkname + "," + rstname);
	//vhdlout->process("fsmREG",clkname + "," + rstname + "," + strstrm_fsm.str());
	vhdlout->increaseindent();
	vhdlout->begin();
	vhdlout->increaseindent();
	if(synclk) {
	  vhdlout->ifclk(clkname);
	  vhdlout->increaseindent();
	  vhdlout->ifresethigh(rstname, resetval);
	  vhdlout->increaseindent();
  	  vhdlout->outputline("STATE <= " + controller->getinitial() + ";");
	  vhdlout->decreaseindent();
	  vhdlout->outputline("else");
	  vhdlout->increaseindent();
	  vhdlout->outputline("STATE <= STATE;");
	  // MOVED cmd selection logic to CMB process
	  //		vhdlout->outputline("cmd  <= cmd; ");
	  vhdlout->casestr("STATE");
	  vhdlout->increaseindent();
	  controller->vhdoutstatetrans(vhdlout,true);
	  vhdlout->outputline("when others=>");
	  vhdlout->decreaseindent();
	  vhdlout->endcase();
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	  
	  // CMD selection logic
	  //vhdlout->outputline("cmd <= " + controller->getinitialcmd() + ";");
	  //vhdlout->casestr("STATE");
	  //vhdlout->increaseindent();
	  //controller->vhdoutstatetrans(vhdlout,false);
	  //vhdlout->outputline("when others=>");
	  //vhdlout->endcase();
	  //vhdlout->decreaseindent();
	  
	}
	else {
	  vhdlout->ifresethigh(rstname, resetval);
	  vhdlout->increaseindent();
	  // MOVED cmd selection logic to CMB process
	  //	        vhdlout->outputline("cmd <= " + controller->getinitialcmd() + ";");
	  vhdlout->outputline("STATE <= " + controller->getinitial() + ";");
	  vhdlout->decreaseindent();
	  vhdlout->elseclk(clkname);
	  vhdlout->increaseindent();
	  vhdlout->outputline("STATE <= STATE;");
	  // MOVED cmd selection logic to CMB process
	  //		vhdlout->outputline("cmd  <= cmd; ");
	  vhdlout->casestr("STATE");
	  vhdlout->increaseindent();
	  controller->vhdoutstatetrans(vhdlout,true);
	  vhdlout->outputline("when others=>");
	  vhdlout->decreaseindent();
	  vhdlout->endcase();
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	  
          // CMD selection logic
	  //vhdlout->outputline("cmd <= " + controller->getinitialcmd() + ";");
	  //vhdlout->casestr("STATE");
	  //vhdlout->increaseindent();
	  //controller->vhdoutstatetrans(vhdlout,false);
	  //vhdlout->outputline("when others=>");
	  //vhdlout->endcase();
	  //vhdlout->decreaseindent();

	}
	vhdlout->decreaseindent();
	vhdlout->endprocess("fsmREG");
	vhdlout->decreaseindent();

	vhdlout->nextline(2);
	vhdlout->comment("controller cmb");
	vhdlout->process("fsmCMB",strstrm_fsm.str());
	vhdlout->increaseindent();
	vhdlout->begin();


	for (map<symid, vhdvar *>::iterator i = sfgvarlist.begin(); i != sfgvarlist.end(); ++i)
	  if ((i->second))
             if (!( i->second->isconst() ) && !(i->second->isindex()) && !(i->second->isout()))
	         vhdlout->defaultzero(i->second,true);

	for (map<symid, vhdsfg *>::iterator i = sfglist.begin(); i != sfglist.end(); ++i)
	  if ((i->second))
	      if ( i->second->getname() == "fsm")
		  i->second->vhdoutfsmcmb(vhdlout);      

          // CMD selection logic
	vhdlout->outputline("cmd <= " + controller->getinitialcmd() + ";");
	vhdlout->casestr("STATE");
	vhdlout->increaseindent();
	controller->vhdoutstatetrans(vhdlout,false);
	vhdlout->outputline("when others=>");
	vhdlout->endcase();
	vhdlout->decreaseindent();
	                 
	vhdlout->decreaseindent();
	vhdlout->endprocess("fsmCMB");
    }
    else if(controller->gettype() == "hardwire") {
      
      
      //       if(first_dp)
      //           strstrm_dp <<clkname;
      
      // PS - 'begin' after architecture is unconditional
      //       if( !RegInstrEmpty || !CmbInstrEmpty || !maplist.empty()) {
      vhdlout->nextline(2);
      vhdlout->begin();
      vhdlout->increaseindent();
      //       }
      
      if( !maplist.empty() ) {
	vhdlout->nextline(2);
	vhdlout->comment("portmap");
	for (map<string, vhdportmap*>::iterator i = maplist.begin(); i != maplist.end(); ++i)
	  if (i->second) {
	    if (i->second->isclone())
	      vhdlout->beginportmap(i->first,i->second->getparentnm());
	    else
	      vhdlout->beginportmap(i->first,i->first);
	    i->second->dumpmapvar(vhdlout);
	    vhdlout->endportmap(i->second->gethasclk(),clkname,rstname,clkname,rstname);
	  }
      }
      
      if( !RegInstrEmpty ) {
	vhdlout->comment("register updates");
	vhdlout->process("dpREG",clkname + "," + rstname);
	vhdlout->increaseindent();
	vhdlout->begin();
	vhdlout->increaseindent();
	if(synclk) {
	  vhdlout->ifclk(clkname);
	  vhdlout->increaseindent();
	  vhdlout->ifresethigh(rstname, resetval);
	  vhdlout->increaseindent();
	  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	    if ((i->second))
	      if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->defaultzero(i->second,true);
	  vhdlout->decreaseindent();
	  vhdlout->outputline("else");
	  vhdlout->increaseindent();
	  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	    if ((i->second))
	      if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->assignreg(i->second);
	  vhdlout->nextline(1);
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	}
	else {  
	  vhdlout->ifresethigh(rstname,resetval);
	  vhdlout->increaseindent();
	  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	    if ((i->second))
	      if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->defaultzero(i->second,true);
	  vhdlout->decreaseindent();
	  vhdlout->elseclk(clkname);
	  vhdlout->increaseindent();
	  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	    if ((i->second))
	      if ( i->second->isreg() && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->assignreg(i->second);
	  vhdlout->nextline(1);
	  vhdlout->decreaseindent();
	  vhdlout->endif();
	}
	vhdlout->decreaseindent();
	vhdlout->endprocess("dpREG");
	vhdlout->decreaseindent();
      }
      
      if( !CmbInstrEmpty ) {
	vhdlout->nextline(2);
	vhdlout->comment("combinational logics");
	vhdlout->process("dpCMB",strstrm_dp.str());
	
	vhdlout->increaseindent();
	vhdlout->begin();
	vhdlout->increaseindent();
	for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
	  if ((i->second)) {
	    if (i->second->isreg()&& !( i->second->isconst() ) && !(i->second->isindex()) && !(i->second->isout()))
	      vhdlout->assigndefaultreg(i->second);
	    else {
	      if (!( i->second->isconst() ) && !(i->second->isindex()) && !(i->second->isout()))
		vhdlout->defaultzero(i->second,true);
	    }
	  }
	for (map<symid, vhdvar *>::iterator i = bitlist.begin(); i != bitlist.end(); ++i)
	  if ((i->second))
	    vhdlout->dumpbitstream(i->second);
	
	for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
	  if ((i->second))
	    if ( i->second->isout() )
	      vhdlout->defaultzero(i->second,true);
	
	vhdlout->nextline(1);
	sfglist[controller->getsfgexe()]->vhdoutcmb(vhdlout);      
	sfglist[controller->getsfgexe()]->vhdoutreg(vhdlout);      
	
	vhdlout->decreaseindent();
	vhdlout->endprocess("dpCMB");
	vhdlout->decreaseindent();
      }  
      
    }
    else
      cout<<"VHD ERROR: sequencer controller not supported yet"<<endl;
  }
  else
    vhdlout->begin();
  
  vhdlout->decreaseindent();
  vhdlout->outputline("end RTL;");
  
  vhdlout->close();
  delete vhdlout;
  vhdlout = NULL;
}

void vhdvdp::dumpram(string clkname, string rstname,int synclk) {
  string vhd = ".vhd";
  string filename = name + vhd;


  vhdout* vhdlout = new vhdout(filename);

  cout<<"Generate file: "<<filename<<endl;

/**********************************************************************************************/

  vhdlout->library("ieee");
  vhdlout->lib_use("IEEE.std_logic_1164.all");
  vhdlout->lib_use("IEEE.std_logic_arith.all");
  vhdlout->library("work");
  vhdlout->lib_use("work.std_logic_arithext.all");

  vhdlout->nextline(2);
  vhdlout->comment("datapath entity");
  vhdlout->beginentity(name);
  vhdlout->beginentityport();
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
    if ((i->second))
	vhdlout->dumpportvar(i->second);

  vhdlout->endports(name,hasclk,clkname,rstname);
  vhdlout->endentity(name);


  vhdlout->nextline(2);
  vhdlout->comment("signal declaration");
  vhdlout->architecture("RTL",name);

  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
    if ((i->second)){
	ramaddrwl = i->second->getwl();
	break;
    } 
    

  vhdlout->outputram(ramsize,ramwl,ramaddrwl,clkname);
  vhdlout->outputline("end RTL;");

  vhdlout->close();
  delete vhdlout;
  vhdlout = NULL;
}


void vhdvdp::dumpip(string clkname, string rstname, int synclk) {
  string vhd = ".vhd";
  string filename = name + vhd;


  vhdout* vhdlout = new vhdout(filename);

  cout<<"Generate file: "<<filename<<endl;

/**********************************************************************************************/

  vhdlout->library("ieee");
  vhdlout->lib_use("IEEE.std_logic_1164.all");
  vhdlout->lib_use("IEEE.std_logic_arith.all");
  vhdlout->library("work");
  vhdlout->lib_use("work.std_logic_arithext.all");

  vhdlout->nextline(2);
  vhdlout->comment("datapath entity");
  vhdlout->beginentity(name);
  vhdlout->beginentityport();
  for (map<symid, vhdvar *>::iterator i = portlist.begin(); i != portlist.end(); ++i)
    if ((i->second))
	vhdlout->dumpportvar(i->second);
  vhdlout->endports(name,hasclk,clkname,rstname);
  vhdlout->endentity(name);

  vhdlout->nextline(2);
  vhdlout->comment("signal declaration");
  vhdlout->architecture("RTL",name);

  vhdlout->outputline("begin");

  vhdlout->outputline("   -- VHDL view of ipblock");

  vhdlout->outputline("end RTL;");

  vhdlout->close();
  delete vhdlout;
  vhdlout = NULL;
}

void vhdvdp::dump(string clkname, string rstname,int synclk, int resetval) {
  switch (dptype) {
  case dp:       dumpdp(clkname,rstname,synclk,resetval);  break;
  case dp_ram:   dumpram(clkname,rstname,synclk); break;
  case dp_ip:    dumpip(clkname,rstname,synclk);  break;
  }
}

void vhdvdp::debugreport(vhdreport* report){
  map<int,   int> regcount;   //bit size, number of registers
  map<int,   int> lutcount;   //bit size, number of lut
  map<string,int> opcount;    //op type, number of operators
  map<string,vhdebugop*> debugoplist;
  map<symid,vhdop*> tmplist;
  
  //  ostringstream strstrm;
  
  report->outputline("");
  report->outputline("datapath : "+getname());
  report->outputline("--------------------------------------");
  report->increaseindent();
  for (map<symid, vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
    if ((i->second))
      if (i->second->isreg())
	regcount[i->second->getwl()] ++;

  for (map<symid, vhdlut *>::iterator i = lutlist.begin(); i != lutlist.end(); ++i)
    if ((i->second))
	lutcount[i->second->getwlen()] ++;

  for (map<symid, vhdsfg *>::iterator i = sfglist.begin(); i != sfglist.end(); ++i)
    if ((i->second)){
        tmplist = i->second->getcinlist();
	  for (map<symid, vhdop *>::iterator j = tmplist.begin(); j != tmplist.end(); ++j)
	    if ((j->second)){
	      if(j->second->gettype() != "notuse") {
	          if(debugoplist[j->second->gettype()])
		  	debugoplist[j->second->gettype()]->addop(j->second->getleft()->getwl());
		  else {
			debugoplist[j->second->gettype()] = new vhdebugop(j->second->getleft()->getwl());
		  }
	      }
            }
    }

  //output reg count, lut count, op count
  for (map<int, int>::iterator i = regcount.begin(); i != regcount.end(); ++i)
     if ((i->second))
	report->outregister(i->second,i->first);
  for (map<int, int>::iterator i = lutcount.begin(); i != lutcount.end(); ++i)
     if ((i->second))
	report->outlut(i->second,i->first);
  for (map<string, vhdebugop*>::iterator i = debugoplist.begin(); i != debugoplist.end(); ++i)
     if ((i->second))
	i->second->outopreport(report,i->first);

  //output FSM info
  if(controller)
     controller->debugreportstate(report);

  for (map<string, vhdportmap*>::iterator i = maplist.begin(); i != maplist.end(); ++i)
 	if (i->second) {
		report->outputline("datapath used: "+ i->first);
        } 

  report->decreaseindent();
  report->outputline("--------------------------------------");
  report->outputline("");
}

