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


#include "vhdsys.h"

#include "vhdvalldp.h"
#include "vhdportnet.h"
#include "vhdvar.h"
#include "vhdreport.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdsys::vhdsys() { sysempty = true; resetcount(); hasclk = false;}

vhdsys::~vhdsys() {
  for (map<symid, vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<string, vhdportmap *>::iterator i = maplist.begin(); i != maplist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (vector<vhdnet *>::iterator i = netlist.begin(); i != netlist.end(); ++i)
    delete (*i);
  for (vector<vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
    delete (*i);
}

void vhdsys::adddp(symid v, vhdvalldp* dp) {
  dplist[v] = dp;
  sysempty = false;
}

void vhdsys::addnet(symid in, symid out) {
  vhdnet* t = new vhdnet(in, out);
  netlist.push_back(t);
}

void vhdsys::resetcount() {  count = 0;  }

string vhdsys::vautoname() {
  ostringstream strstrm;
  strstrm << "sig_" <<count++;
  return strstrm.str();
}

void vhdsys::preprocess() {
  vector<vhdmapentry *> mapentry;
  vhdvar* t; 
  map<symid, vhdvar* > outlist;

  if(!sysempty) {
    for (vector<vhdnet *>::iterator i = netlist.begin(); i != netlist.end(); ++i) {
      if (outlist[(*i)->getoutput()])
	(*i)->addvar(outlist[(*i)->getoutput()]);
      else {
	t = new vhdvar(vautoname(),0,0,0,false,0);
	varlist.push_back(t);
	(*i)->addvar(t);
	outlist[(*i)->getoutput()] = t;
      }
    }
    
    for (map<symid, vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); ++i) {
      if (i->second) {
	maplist[i->second->getname()] = new vhdportmap(i->second);
	maplist[i->second->getname()]->updatehasclk();
	if(maplist[i->second->getname()]->gethasclk())
	  hasclk = true;
	mapentry = i->second->getportmap(netlist);
	for (vector<vhdmapentry *>::iterator k = mapentry.begin(); k != mapentry.end(); ++k) {
	  if((*k)->gettargetv() == NULL) {
	    t = new vhdvar(vautoname(),1,(*k)->getportv()->gettc(),(*k)->getportv()->getwl(),false,0);
	    varlist.push_back(t);
	    (*k)->settargetv(t);
	  }
	}
	maplist[i->second->getname()]->addentry(mapentry);	
      }
    }
    
    
  }
}

void vhdsys::dump(string clkname, string rstname, int synclk) {
  if(!sysempty) {

    string vhd = ".vhd";
    string filename = "system" + vhd;

    vhdout* vhdlout = new vhdout(filename);

  cout<<"Generate file: "<<filename<<endl;

/**********************************************************************************************/

    vhdlout->library("ieee");

    vhdlout->lib_use("IEEE.std_logic_1164.all");

    vhdlout->lib_use("IEEE.std_logic_arith.all");

    vhdlout->library("work");

    vhdlout->lib_use("work.std_logic_arithext.all");


    vhdlout->nextline(2);
    vhdlout->comment("system entity");
    vhdlout->outputline("entity system is");
    vhdlout->outputline("end system;");

    vhdlout->nextline(2);
    vhdlout->architecture("RTL","system");
    for (vector<vhdvar *>::iterator i = varlist.begin(); i != varlist.end(); ++i)
        vhdlout->dumpvardeclare(*i);
    
    if(hasclk) {
    	vhdlout->outputline("signal " + rstname + " : std_logic;");
	vhdlout->outputline("signal " + clkname + " : std_logic;");
    }

  map<string, bool> clonecomponentlist;
  if(!maplist.empty()) {
    vhdlout->nextline(2);
    vhdlout->comment("component declaration");
    for (map<string, vhdportmap*>::iterator i = maplist.begin(); i != maplist.end(); ++i)
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
    vhdlout->nextline(2);
    vhdlout->begin();
    vhdlout->increaseindent();
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
    
    vhdlout->nextline(2);

    if (hasclk) {
	    vhdlout->comment("clk, reset generation");

	    // PS - clk,rst generation should be made optional using a comline flag
	    vhdlout->outputline("process");
	    vhdlout->increaseindent();
	    vhdlout->outputline("begin");
	    vhdlout->doindent();
	    vhdlout->outputchar(rstname); vhdlout->outputline(" <= '1';");
	    vhdlout->outputline("wait for 50 ns;");
	    vhdlout->doindent(); vhdlout->outputchar(rstname); vhdlout->outputline(" <= '0';");
	    vhdlout->outputline("wait;");
	    vhdlout->decreaseindent();
	    vhdlout->outputline("end process;");

	    vhdlout->nextline(1);
	    vhdlout->doindent(); vhdlout->outputchar("process(");
	    vhdlout->outputchar(clkname); vhdlout->outputline(")");
	    vhdlout->increaseindent();
	    vhdlout->outputline("begin");
	    vhdlout->doindent(); vhdlout->outputchar("if ");
	    vhdlout->outputchar(clkname);
	    vhdlout->outputchar("'event and ");
	    vhdlout->outputchar(clkname);
	    vhdlout->outputline("= '1' then ");
	    vhdlout->increaseindent();
	    vhdlout->doindent(); vhdlout->outputchar(clkname);
	    vhdlout->outputline(" <= '0' after 25 ns;");
	    vhdlout->decreaseindent();
	    vhdlout->outputline("else");
	    vhdlout->increaseindent();
	    vhdlout->doindent(); vhdlout->outputchar(clkname);
	    vhdlout->outputline(" <= '1' after 25 ns;");
	    vhdlout->decreaseindent();
	    vhdlout->outputline("end if;");
	    vhdlout->decreaseindent();
	    vhdlout->outputline("end process;");
	}
    vhdlout->nextline(1);
    vhdlout->decreaseindent();
    vhdlout->outputline("end RTL;");
    vhdlout->close();
    delete vhdlout;
    vhdlout = NULL;
  }
}

void vhdsys::debugreport(vhdreport* report) {
  report->outputline("--------------------------------------");
  report->outputline("System Details");
  report->outputline("--------------------------------------");
  report->increaseindent();
  for (map<symid, vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); ++i)
    if ((i->second))
	report->outputline("datapath used: "+ i->second->getname());
  report->decreaseindent();
  report->close();
}
 

