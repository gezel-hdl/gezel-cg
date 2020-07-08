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

#include "vhdctl.h"
#include "vhdvar.h"
#include "vhdout.h"
#include "vhdreport.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
using std::string;
using std::ostringstream;

//--------------------------------------------------------------
vhdstepcond::vhdstepcond(vhdvar* _cond,string _condtype) : cond(_cond), condtype(_condtype) {}

vhdstepcond::~vhdstepcond() {}

vhdvar* vhdstepcond::getcond() {
   return cond;
}

string vhdstepcond::gettype() {
   return condtype;
}

//--------------------------------------------------------------

vhdctlstep::vhdctlstep() {}

vhdctlstep::~vhdctlstep() {
  for (vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i)
    delete (*i);
}

void vhdctlstep::addsfgexe (symid sfg) {
  sfgexe.push_back(sfg);
}

void vhdctlstep::addcond (vhdstepcond* c) {
  condlist.push_back(c);
}

void vhdctlstep::addnextstate (vhdstate* nstate) {
  nextstate = nstate;
}

void vhdctlstep::addcount (int n) {
  count = n;
}

void vhdctlstep::addcontrolname (string nm) {
  controlname = nm;
}

bool vhdctlstep::havecondition() {
  if (condlist.empty())
	return false;
  else
	return true;
}

int vhdctlstep::getcount () {
  return count;
}

vhdstate* vhdctlstep::getnextstate() {
  return nextstate;
}

string vhdctlstep::getcontrolname() {
  return controlname;
}

vector<vhdvar *> vhdctlstep::getcondvar() {
  vector<vhdvar *> tvarlist;
  for(vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i) 
     tvarlist.push_back((*i)->getcond());
  return tvarlist;
}
/*
void vhdctlstep::vhdoutcmdsel(vhdout* os) {
  static  unsigned int	endifcount = 0;
  static  vector<int>   endiflist;

  bool iflogic = false;
  bool haselselogic = false;
  if (condlist.empty()) {
    nextstate->vhdoutstatetranscon(os);
  }
  else {
    for (vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i) {
      if((*i)->gettype() == "1") {  //if logic
	    if(haselselogic)
	      os->increaseindent();
  	    if((*i)->getcond()->getwl() > 1)
	      if((*i)->getcond()->gettc())
	         os->outputline("if(signed(" + (*i)->getcond()->getname() + ") > 0) then");
	      else
	         os->outputline("if(unsigned(" + (*i)->getcond()->getname() + ") > 0) then");
	    else
	      os->outputline("if(" + (*i)->getcond()->getname() + " = '" + (*i)->gettype() + "') then");
	    os->increaseindent();
	    iflogic = true;
      }
      else {  //else logic
	    os->decreaseindent();
	    os->outputline("else");
	    iflogic = false;
	    haselselogic = true;
      }
    }
    if (iflogic) { 
      nextstate->vhdoutstatetranscon(os);
      if (haselselogic) {
	  endiflist.push_back(condlist.size());   
	  endifcount = condlist.size()-1;
      }
    }
    else {
      os->increaseindent();	
      nextstate->vhdoutstatetranscon(os);
      os->decreaseindent();
      os->outputline("end if;");
    }  
    if((endifcount == condlist.size()) && (endifcount != 0)) {
      for (vector<int>::iterator i = endiflist.begin(); i != endiflist.end(); ++i) {
	  os->decreaseindent();
	  os->outputline("end if;");	
      }
      endiflist.clear();
      endifcount = 0;
    }   
  }
}
*/

void vhdctlstep::vhdoutstatetrans(vhdout* os, bool isstate) {
  static  unsigned int	endifcount = 0;
//  static  vector<int>   endiflist;

  static bool overelseif = false;
  bool iflogic = false;
  bool haselselogic = false;

  if (condlist.empty()) {
    if(isstate)
	os->outputline("     STATE <= " + nextstate->getname() + ";");
    else
	os->outputline("     cmd <= " + controlname + ";");
    //	nextstate->vhdoutstatetranscon(os);
    //	os->outputline("     cmd <= " + controlname + ";");
  } else {
    for (vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i) {
      if((*i)->gettype() == "1") {
	if(haselselogic) {
	  overelseif = true;
	  os->increaseindent();
	}
	if((*i)->getcond()->getwl() > 1)
	  if((*i)->getcond()->gettc())
	    os->outputline("if (signed(" + (*i)->getcond()->getname() + ") > 0) then");
	  else
	    os->outputline("if (unsigned(" + (*i)->getcond()->getname() + ") > 0) then");
	else
	  os->outputline("if (" + (*i)->getcond()->getname() + " = '" + (*i)->gettype() + "') then");
	os->increaseindent();
	iflogic = true;
	endifcount ++;
	haselselogic = false;
      } else {
	os->decreaseindent();
	os->outputline("else");
	iflogic = false;
	haselselogic = true;
      }
    }
    if (iflogic) {
      if (isstate)
	os->outputline("     STATE <= " + nextstate->getname() + ";");
      else
	os->outputline("     cmd <= " + controlname + ";");
    } else {
      os->increaseindent();
      if (isstate) {
  	os->outputline("     STATE <= " + nextstate->getname() + ";");
	while (endifcount) {
	  os->decreaseindent();
	  os->outputline("end if;");
	  endifcount--;
	}
      }
      else {
	os->outputline("     cmd <= " + controlname + ";");
	while (endifcount) {
	  os->decreaseindent();
	  os->outputline("end if;");
	  endifcount--;
	}
      }
    }
    //    if(overelseif && endifcount == 1) {
    //	os->decreaseindent();
    //	os->outputline("end if;");
    //	overelseif = false;
    //	endifcount = 0;
    //    }
  }
}

void vhdctlstep::vhdoutstatetranscon(vhdout* os) {
  static  unsigned int	endifcount = 0;
//  static  vector<int>   endiflist;

  static bool overelseif = false;
  bool iflogic = false;
  bool haselselogic = false;

  if (condlist.empty()) {
    os->outputline("     cmd <= " + controlname + ";");
  }
  else {
    for (vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i) {
      if((*i)->gettype() == "1") {
	if(haselselogic) {
	  os->increaseindent();
	  overelseif = true;
	}
	if((*i)->getcond()->getwl() > 1)
	  if((*i)->getcond()->gettc())
	    // State transition condition can only use reg outputs
	    //		     if((*i)->getcond()->isreg())
	    //	     	       os->outputline("if(signed(" + (*i)->getcond()->getregname() + ") > 0) then");
	    //		     else
	    os->outputline("if(signed(" + (*i)->getcond()->getname() + ") > 0) then");
	  else
	    // State transition condition can only use reg outputs
	    //		     if((*i)->getcond()->isreg())
	    //      	  	       os->outputline("if(unsigned(" + (*i)->getcond()->getregname() + ") > 0) then");
	    //		     else
	    os->outputline("if(unsigned(" + (*i)->getcond()->getname() + ") > 0) then");
	else
	  // State transition condition can only use reg outputs
	  //		  if((*i)->getcond()->isreg())
	  //     		     os->outputline("if(" + (*i)->getcond()->getregname() + " = '" + (*i)->gettype() + "') then");
	  //		  else
	  os->outputline("if(" + (*i)->getcond()->getname() + " = '" + (*i)->gettype() + "') then");
	os->increaseindent();
	iflogic = true;
	endifcount ++;
      }
      else {
	os->decreaseindent();
	os->outputline("else");
	iflogic = false;
	haselselogic = true;
      }
    }
    if (iflogic) {
      os->outputline("     cmd <= " + controlname + ";");
    }
    else {
      os->increaseindent();	
      os->outputline("     cmd <= " + controlname + ";");
      os->decreaseindent();
      os->outputline("**3** end if;");
	endifcount --;
    }  
    if(overelseif && endifcount == 1) {
	os->decreaseindent();
	os->outputline("end if;");
	overelseif = false;
	endifcount = 0;
    }
  }
}

void vhdctlstep::dump(ostream & os) {
   os <<endl;int count = 0;
   for (vector<vhdstepcond *>::iterator i = condlist.begin(); i != condlist.end(); ++i) 
     os <<"ctlstep: count "<<count++<<" "<<(*i)->getcond()->getname()<<" ";
   os <<endl;
}

//--------------------------------------------------------------

vhdstate::vhdstate(string _name) : name(_name) {}

vhdstate::~vhdstate() {}

string vhdstate::getname() {
  return name;
}

string vhdstate::getinitialcmd() {
  return steplist.front()->getcontrolname();
}

void vhdstate::addctlstep(vhdctlstep* step) {
  steplist.push_back(step);
}

void vhdstate::vhdoutstatetrans(vhdout* os,bool isstate) {
  os->outputline("when " + name + " => ");
  os->increaseindent();
  for (vector<vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i) 
    (*i)->vhdoutstatetrans(os,isstate);       
  os->decreaseindent();
}
/*
void vhdstate::vhdoutcmdsel(vhdout* os) {
  os->outputline("when " + name + " => ");
  os->increaseindent();
  for (vector<vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i) 
    (*i)->vhdoutcmdsel(os);       
  os->decreaseindent();
}
*/
void vhdstate::vhdoutstatetranscon(vhdout* os) {
        os->increaseindent();
	for (vector<vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i) 
		(*i)->vhdoutstatetranscon(os);       
        os->decreaseindent();
}

void vhdstate::dump(ostream & os) {
  os <<endl<<endl;
  os << "case state: "<<name<<endl;
  for (vector<vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i)
	(*i)->dump(os);
}

//--------------------------------------------------------------

vhdtableentry::vhdtableentry(int _count) : count(_count){}

vhdtableentry::~vhdtableentry() {}

void vhdtableentry::addsfgid(symid id) {
  sfgid.push_back(id);
}

void vhdtableentry::addname(string nm) {
  controlname += nm;
}

bool vhdtableentry::samesfglist(vhdtableentry* rhs) {
  bool same = true;
  if (sfgid.size() != rhs->sfgid.size())
    same = false;
  else {	
    for (vector<symid>::iterator i = sfgid.begin(); same && (i != sfgid.end()); ++i) {
      for (vector<symid>::iterator j = rhs->sfgid.begin(); j != rhs->sfgid.end(); ++j) {
	if ( (*i) == (*j) ) {
	  same = true;
	  break;
	}
	else
	  same = false;
      }
    }
  }
  return same;
}

string vhdtableentry::getname() {
  return controlname;
}

int vhdtableentry::getcount() {
  return count;
}

vector<symid> vhdtableentry::getsfgid() {
  return sfgid;
}

void vhdtableentry::dump(ostream& os) {
  os <<"entry: ";
  os << controlname<<" ";
  for (vector<symid>::iterator i = sfgid.begin(); i != sfgid.end(); ++i) 
    os <<(*i)<<"     ";
  os <<count<<endl;
}

//--------------------------------------------------------------

vhdctl::vhdctl() {}

vhdctl::~vhdctl() {}

string vhdctl::getinitial() {return "\0";}

string vhdctl::getinitialcmd() {return "\0";}

string vhdctl::gettype() {return "\0";}

symid vhdctl::getsfgexe() {return 0;}

vhdstate* vhdctl::getstate(symid id) { return NULL;}

void  vhdctl::addinitial(symid v) {}

void  vhdctl::addstate(symid id,string state) {}

void  vhdctl::addctlstep(symid id, vhdctlstep* step, int ) {}

void  vhdctl::addctlcmd(int, string) {}

void  vhdctl::addsfgexe(symid sfg) {}

void  vhdctl::vhdoutdeclaration(vhdout* os) {}

void  vhdctl::vhdoutstatetrans(vhdout* os, bool isstate) {}

//void  vhdctl::vhdoutcmdsel(vhdout* os) {}

void  vhdctl::debugreportstate(vhdreport* os) {}

void  vhdctl::dump(ostream & os) {}


//--------------------------------------------------------------

vhdfsm::vhdfsm(string _type) : type(_type) {}

vhdfsm::~vhdfsm() {
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<int, vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i)
    if ((i->second))
      delete ((i->second));
}

string vhdfsm::getinitial() {
  return statelist[initial]->getname();
}

string vhdfsm::getinitialcmd() {
  return statelist[initial]->getinitialcmd();
}

string vhdfsm::gettype() {
  return type;
}

vhdstate* vhdfsm::getstate(symid id) {
  return statelist[id];
}

void vhdfsm::addinitial(symid v) {
  initial = v;
}

void vhdfsm::addstate(symid id, string name) {
  vhdstate* t = new vhdstate(name);
  statelist[id]= t;
}

void vhdfsm::addctlstep(symid from, vhdctlstep* step, int count) {
  steplist[count] = step;
  statelist[from]->addctlstep(step);
}

void vhdfsm::addctlcmd(int count, string name) {
  steplist[count]->addcontrolname(name);
}

void vhdfsm::vhdoutdeclaration(vhdout* os) {
  os->beginstatedeclare();
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i) 
      if ((i->second)) 	
          os->eachstate( (i->second)->getname() );
  os->endstatedeclare();
}

void  vhdfsm::vhdoutstatetrans(vhdout* os,bool isstate) {
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i) 
      if ((i->second)) 	
	  (i->second)->vhdoutstatetrans(os,isstate);}
/*
void  vhdfsm::vhdoutcmdsel(vhdout* os) {
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i) 
      if ((i->second)) 	
	  (i->second)->vhdoutcmdsel(os);}
*/
void vhdfsm::debugreportstate(vhdreport* report) {
  int statecount = 0;
  int transcount = 0;
  ostringstream strstrm;

  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i) 
      if ((i->second)) 	
	statecount ++; 
  for (map<int, vhdctlstep *>::iterator i = steplist.begin(); i != steplist.end(); ++i) 
      if ((i->second))
	transcount ++;      
  strstrm << "FSM has "<<statecount<<" states, and "<<transcount<<" state transition";
  report->outputline(strstrm.str());
}

void vhdfsm::dump(ostream & os) {
  os << "inital state: "<<getinitial()<<endl;
  os << "states: ";
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i)
      if ((i->second)) 	
          os <<(i->second)->getname()<<" ";

  os << endl<<endl;
  os << "dump control step list"<<endl;
  for (map<symid, vhdstate *>::iterator i = statelist.begin(); i != statelist.end(); ++i)
      if ((i->second)) 
	  i->second->dump(os);  
}  



//--------------------------------------------------------------

vhdhardwire::vhdhardwire(string _type) : type(_type) {}

vhdhardwire::~vhdhardwire() {}

string vhdhardwire::gettype() {
  return type;
}

symid vhdhardwire::getsfgexe() {
  return sfgexe;
}

void vhdhardwire::addsfgexe(symid sfg) {
  sfgexe = sfg;
}

void vhdhardwire::vhdoutdeclaration(vhdout* os) {}

void vhdhardwire::vhdoutstatetrans(vhdout* os, bool isstate) {}

//void vhdhardwire::vhdoutcmdsel(vhdout* os) {}

void vhdhardwire::debugreportstate(vhdreport* report) {
  report->outputline("No FSM, hardwire logic is used");
}

void vhdhardwire::dump(ostream & os) {
  os << "hardwire control:"<<endl;
  os << "sfg = "<<sfgexe<<endl;
}

//--------------------------------------------------------------

vhdsequencer::vhdsequencer(string _type) : type(_type) {}

vhdsequencer::~vhdsequencer() {}

string vhdsequencer::gettype() {
  return type;
}

void vhdsequencer::vhdoutdeclaration(vhdout* os) {}

void vhdsequencer::vhdoutstatetrans(vhdout* os, bool isstate) {}

//void vhdsequencer::vhdoutcmdsel(vhdout* os) {}

void vhdsequencer::debugreportstate(vhdreport* report) {
  report->outputline("No FSM, sequencer logic is used");
}

void vhdsequencer::dump(ostream & os) {
 	os << "sequencer not supported yet"<<endl;
}
