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


#ifndef VHDVALLDP_H
#define VHDVALLDP_H

#include "asymbol.h"

#include <string>
using std::string;

#include <vector>
using std::vector;

class vhdctl;
class vhdvar;
class vhdop;
class vhdbinop;
class vhdunop;
class vhdterop;
class vhdassign;
class vhdlutop;
class vhdctlstep;
class vhdreport;
class vhdtableentry;
class vhdlut;
class vhdstate;
class vhdmapentry;
class vhdnet;

int vmax(int first, int second);

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
using std::string;
using std::ostringstream;

typedef ostringstream ostrstr_t;
#else
#include <strstream.h>

typedef ostrstream ostrstr_t;
#endif

class vhdvalldp {
 public:
  
  vhdvalldp();
  virtual ~vhdvalldp();	
  virtual bool isclonedp();
  virtual void addctl(vhdctl* );
  virtual void addSfg(symid, string );
  virtual void adddp(symid,vhdvalldp*);
  virtual void addlut(symid,vhdlut*);
  
  virtual void addportmapentry(string,vhdvar*,vhdvar*);
  virtual void addbinInstr(symid, symid, vhdbinop*);
  virtual void addunInstr(symid, symid, vhdunop*); 
  virtual void addterInstr(symid, symid, vhdterop*);
  virtual void addmultterInstr(symid v,symid cur_sfg,vhdterop* s);
  virtual void addassignInstr(symid,symid,vhdassign*, bool);
  virtual void addlutInstr(symid,symid,vhdlutop*);
  
  virtual void addgeneric(string, string, string);
  virtual void addattributedef(string, string);
  virtual void addattributeuse(string, string, string);
  
  virtual void addsfgvar(symid id, vhdvar* );
  virtual void addvar(symid id, vhdvar* );
  virtual void addbit(symid id, vhdvar* );
  virtual void addportvar(symid, vhdvar* );
  virtual void addcondvar(vhdctlstep *);
  
  virtual string getname();
  virtual string getparentname();
  virtual vhdlut* getlut(symid);
  virtual vhdvar* getvar(symid);
  virtual vhdvar* getport(symid);
  virtual vhdvar* getoport_tr(vhdvar *);
  virtual void    setoport_tr(vhdvar *, vhdvar *);
  virtual vhdvar* getvarport(symid);
  virtual string  getsfgname(symid);
  virtual vhdstate* getstate(symid);
  virtual vector<vhdmapentry *> getportmap(vector<vhdnet *>);
  virtual string getdptype();
  virtual string getoptype(symid);
  virtual vhdop* getop(symid ); 
  virtual bool gethasclk();
  
  virtual void resetcount();
  virtual void resetbitcount();
  virtual int getbitcount();
  virtual string vautoname();
  virtual string vautoname(string base, int num);
  virtual void setramwl(string);
  virtual void setramsize(string);

  virtual void sethasclk(bool);
  virtual void addtableentry(vhdtableentry*);
  virtual void mergeentry();
  virtual void dumptable(ostream &);
  
  virtual void rmdeadcode(vhdvar *);
  virtual void preprocess();
  virtual void dump(string,string,int,int);
  virtual void debugreport(vhdreport*);
  
};


#endif
