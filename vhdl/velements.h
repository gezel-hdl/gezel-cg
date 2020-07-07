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


#ifndef VELEMENTS_H
#define VELEMENTS_H

#include "fdlsym.h"

#include <vector>
using namespace std;

//------------------------------------------------------------------
// enumerated type
class venumtype {
  vector<const char *> enumcases;
  const char * enumtypename;
 public:
  venumtype(const char *name);
  void addcase(const char *casename);
  void dumpDeclaration(ostream &os);
  void vdumptype(ostream &os);
};

//------------------------------------------------------------------
// port
class vaport {
 protected:
  int direction;
  char *name;
 public:
  enum {input, output};
  vaport(int _dir, char *_name);
  virtual ~vaport() {}
  virtual void dumpDeclaration(ostream &os);
};

class vstdport : public vaport {
  int wlen;
 public:
  vstdport(int _dir, char *_name, int _wlen);
  virtual ~vstdport() {}
  void dumpDeclaration(ostream &os);
};

class venumport : public vaport {
  venumtype *t;
 public:
  venumport(int _dir, char *_name, venumtype *_t);
  virtual ~venumport() {}
  void dumpDeclaration(ostream &os);
};

//------------------------------------------------------------------
class vinstruction {
  vector<symid> ins;
 public:
  vinstruction();
  void addsfg(symid s);
  void dumpInsName(ostream &os);
  bool isdup(vinstruction *);
  const char *makeInsName();
};

class vactl {
  vector<vinstruction *> inslist;
  vector<symid> cndlist;
  vector<vinstruction *> duplicate;
 public:
  vactl() {}
  ~vactl() {}
  void addstep(symid cond);
  void addsfg (symid ctl, symid sfg);
  void showinstructions(ostream &os);
  void collectcondops(vector<symid> &ops);
  void findduplicates  ();
  void defineInsType(venumtype *V);
};

//------------------------------------------------------------------
// top-level structure, represents one datapath
class vdp {
  char *name;
  vector<vaport *>  ports; // io
  vactl *ctl;            // associated control structure

  // helpers for dump();
  void dumpTopLevel(ostream &os);
  void dumpTopLevelArchitecture(ostream &os);

 public:
  vdp(char *);
  ~vdp();

  void dump();
  void addinput  (char *portname, int wlen);
  void addoutput (char *portname, int wlen);
  void addctl    (vactl *v);
  void addctlstep(symid condition);
  void showconditions(ostream &os);

};

#endif
