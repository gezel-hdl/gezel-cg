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

#ifndef VHDCTL_H
#define VHDCTL_H

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <map>
using std::map;

#include <iostream>
using std::ostream;

#include "asymbol.h"

class vhdstate;
class vhdvar;
class vhdout;
class vhdreport;

class vhdstepcond {
  vhdvar* cond;
  string condtype;
 public:
  vhdstepcond(vhdvar*,string);
  ~vhdstepcond();
  vhdvar* getcond();
  string gettype();
};

class vhdctlstep {
  vector<vhdstepcond *> condlist;
  
  vhdstate* nextstate;
  
  int   count;
  string controlname;
  
 public:
  vector<symid> sfgexe;
  
  vhdctlstep();
  ~vhdctlstep();
  
  void addsfgexe(symid );
  void addcond(vhdstepcond* );
  void addnextstate(vhdstate* );
  void addcount(int );
  void addcontrolname(string);
  
  bool havecondition();
  int  getcount();
  vhdstate* getnextstate();
  string getcontrolname();
  vector<vhdvar *> getcondvar();
  
  void vhdoutstatetrans(vhdout*, bool);
//  void vhdoutcmdsel(vhdout*);
  void vhdoutstatetranscon(vhdout*);
  void dump(ostream & os);	
};

class vhdstate {
  string name;
  vector<vhdctlstep* > steplist;
  
 public:
  vhdstate(string);
  ~vhdstate();
  
  string getname();
  string getinitialcmd();
  void addctlstep(vhdctlstep*);
  void vhdoutstatetrans(vhdout*,bool);
//  void vhdoutcmdsel(vhdout*);
  void vhdoutstatetranscon(vhdout*);
  void dump(ostream & os);
};

class vhdtableentry{
  string controlname;
  int count;
  
  public:
  vector<symid> sfgid;	
  
  vhdtableentry(int);
  ~vhdtableentry();
  
  void addsfgid(symid);
  void addname(string);
  bool samesfglist(vhdtableentry*);
  
  string getname();
  int getcount();
  vector<symid> getsfgid();
  
  void dump(ostream &);
};

class vhdctl {
  
 public:
  vhdctl();
  virtual ~vhdctl();
  
  virtual string getinitial();
  virtual	string getinitialcmd();
  
  virtual string gettype();
  virtual symid getsfgexe();
  
  virtual vhdstate* getstate(symid);
  
  virtual void addinitial(symid);
  virtual void addstate(symid, string);
  
  virtual void addctlstep(symid, vhdctlstep*, int);
  virtual void addsfgexe(symid);
  
  virtual void addctlcmd(int, string);
  
  virtual void vhdoutdeclaration(vhdout*);
  virtual void vhdoutstatetrans(vhdout*, bool);
//  virtual void vhdoutcmdsel(vhdout*);
  
  virtual void debugreportstate(vhdreport*);
  virtual void dump(ostream &);
};

class vhdfsm : public vhdctl {
  map<symid, vhdstate *> statelist;
  map<int, vhdctlstep* > steplist;
  
  symid initial;
  symid cur_state;
  string type;  
  
 public:
  vhdfsm(string );
  ~vhdfsm();
  
  string getinitial();
  string getinitialcmd();
  string gettype();
  vhdstate* getstate(symid);
  
  void addinitial(symid );
  void addstate(symid, string);
  
  void addctlstep(symid, vhdctlstep*, int);
  void addctlcmd(int, string);
  
  void vhdoutdeclaration(vhdout*);
  void vhdoutstatetrans(vhdout*, bool);
//  void vhdoutcmdsel(vhdout*);
  
  void debugreportstate(vhdreport*);
  void dump(ostream &);
};

class vhdhardwire : public vhdctl {
  symid sfgexe;
  string type;  
  
 public:
  vhdhardwire(string );
  ~vhdhardwire();
  
  string gettype();
  symid getsfgexe();
  void addsfgexe(symid);
  
  void vhdoutdeclaration(vhdout*);
  void vhdoutstatetrans(vhdout*, bool);
//  void vhdoutcmdsel(vhdout*);
  
  void debugreportstate(vhdreport*);
  void dump(ostream &);
};

class vhdsequencer : public vhdctl {
  string type;  
  
 public:
  vhdsequencer(string );
  ~vhdsequencer();
  
  string gettype();
  
  void vhdoutdeclaration(vhdout*);
  void vhdoutstatetrans(vhdout*, bool);
//  void vhdoutcmdsel(vhdout*);
  
  void debugreportstate(vhdreport*);
  void dump(ostream &);
};

#endif
