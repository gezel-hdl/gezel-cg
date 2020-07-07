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

#ifndef VHDVDP_H
#define VHDVDP_H

#include "vhdvalldp.h"
class vhdsfg;
class vhdop;
class vhdportmap;

#include <map>
using std::map;

class vhdvdp : public vhdvalldp {
  public:
  enum _dptype {
    dp, dp_ram, dp_ip
  };
  
 private:
  
  string name;
  vhdctl* controller;
  symid cur_sfg;
  int count;
  int bitcount;

  int ramwl;     // may move to ram class later
  int ramsize;   // may move to ram class later
  int ramaddrwl; // may move to ram class later

  _dptype dptype;
  bool hasclk;  
  bool RegInstrEmpty;
  bool CmbInstrEmpty;
  
  map<symid, vhdvar  *> portlist;
  // To support output signals as expression inputs
  // additional signals are created, one for each output port
  // These signals are stored in oport_tr
  map<vhdvar *, vhdvar   *> oport_tr; 
  map<symid, vhdvar      *> sfgvarlist;
  map<symid, vhdvar      *> varlist;
  map<symid, vhdvar      *> bitlist;
  map<symid, vhdlut      *> lutlist;
  map<symid, vhdsfg      *> sfglist;
  map<symid, vhdop       *> oplist;
  map<string, vhdportmap *> maplist;
  vector<vhdvar          *> condvar;
  vector<vhdvar          *> controllist;
  vector<vhdtableentry   *> table;

  // allow users to specify generic ports (added for xilinx ipif interface)
  vector<string> generic_port;
  vector<string> generic_type;
  vector<string> generic_default;

  // allow users to specify attributes (added for xilinx ipif interface)
  vector<string> attribute_def_attribute;
  vector<string> attribute_def_type;

  vector<string> attribute_use_attribute;
  vector<string> attribute_use_signal;
  vector<string> attribute_use_value;

 public:
  vhdvdp(string,_dptype);
  ~vhdvdp();	
  bool isclonedp();
  void addctl(vhdctl* );
  void addSfg(symid, string );
  void adddp(symid,vhdvalldp*);
  void addlut(symid,vhdlut*);
  
  void addportmapentry(string,vhdvar*,vhdvar*);
  void addbinInstr(symid, symid, vhdbinop*);
  void addunInstr(symid, symid, vhdunop*); 
  void addterInstr(symid, symid, vhdterop*);
  void addmultterInstr(symid v,symid cur_sfg,vhdterop* s);
  void addassignInstr(symid,symid,vhdassign*, bool);
  void addlutInstr(symid,symid,vhdlutop*);
  
  void addsfgvar(symid id, vhdvar* );
  void addvar(symid id, vhdvar* );
  void addbit(symid id, vhdvar* );
  void addportvar(symid, vhdvar* );
  void addcondvar(vhdctlstep *);

  void addgeneric(string, string, string);
  void addattributedef(string, string);
  void addattributeuse(string, string, string);
  
  string getname();
  vhdlut* getlut(symid);
  vhdvar* getvar(symid);
  vhdvar* getport(symid);
  vhdvar* getoport_tr(vhdvar *);
  void    setoport_tr(vhdvar *, vhdvar *);
  vhdvar* getvarport(symid);
  string  getsfgname(symid);
  vhdstate* getstate(symid);
  vector<vhdmapentry *> getportmap(vector<vhdnet *>);
  string getdptype();
  string getoptype(symid);
  vhdop* getop(symid ); 
  bool gethasclk();
  
  void resetcount();
  void resetbitcount();
  int getbitcount();
  string vautoname();
  string vautoname(string base, int num);
  void setramwl(string);
  void setramsize(string);
  void sethasclk(bool);

  void addtableentry(vhdtableentry*);
  void mergeentry();
  void dumptable(ostream &);
  
  void rmdeadcode(vhdvar *);
  void preprocess();
  void dumpdp(string,string,int,int);
  void dumpram(string,string,int);
  void dumpip(string,string,int);
  void dump(string,string,int,int);
  void debugreport(vhdreport*);
};


#endif
