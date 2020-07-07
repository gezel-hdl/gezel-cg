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

#include "velements.h"
#include "rterror.h"
#include "vformat.h"

#include <iostream>
#include <fstream>
#include <string>
using std::string;

#include <algorithm>

//--------------------------------------------------------------
venumtype::venumtype(const char *name) : enumtypename(name) {}

void venumtype::addcase(const char *casename) {
  enumcases.push_back(casename);
}

void venumtype::dumpDeclaration(ostream &os) {
  vline(os, "type ", enumtypename, " is (");
  vindentinc();
  for (vector<const char *>::iterator i = enumcases.begin();
       i != enumcases.end();
       /* INC IN BODY */) {
    vindent(os);
    vraw(os, *i);
    i++;
    if (i != enumcases.end()) {
      vraw(os, ",");
    }
  }
  vindentdec();
  vline(os, ");");
}

void venumtype::vdumptype (ostream &os) {
  os << enumtypename;
}

//--------------------------------------------------------------
vaport::vaport(int _dir, char *_name) : direction(_dir), name(_name) {}

void vaport::dumpDeclaration(ostream &os) {
  fdlerror(ERROR_INTERNAL_ABSTRACT_PORT);
}

vstdport::vstdport(int _dir, char *_name, int _wlen) :
  vaport(_dir, _name), wlen(_wlen) {}

void vstdport::dumpDeclaration(ostream &os) {
  vindent(os);
  vraw(os, name);
  vraw(os, " : ");
  if (direction == input) {
    vraw(os, "in ");
  } else if (direction == output) {
    vraw(os,"out ");
  } else
    fdlerror(ERROR_PORT_DIRECTION_UNKNOWN, name);
  vdumptype(os, wlen);
}

//----

venumport::venumport(int _dir, char *_name, venumtype *_t) :
  vaport(_dir, _name), t(_t) {}

void venumport::dumpDeclaration(ostream &os) {
  vindent(os);
  vraw(os, name);
  vraw(os, " : ");
  if (direction == input) {
    vraw(os, "in ");
  } else if (direction == output) {
    vraw(os,"out ");
  } else
    fdlerror(ERROR_PORT_DIRECTION_UNKNOWN, name);
  if (t)
    t->vdumptype(os);
  else
    fdlerror(ERROR_PORT_ENUM_TYPE_UNKNOWN);
}

//--------------------------------------------------------
vinstruction::vinstruction() {}

void vinstruction::addsfg(symid s) {
  ins.push_back(s);
};

void vinstruction::dumpInsName(ostream &os) {
  for (vector<symid>::iterator i = ins.begin();
       i != ins.end();
       i++) {
    if (i != ins.begin())
      vraw(os,"_");
    glbSymboltable.getsymbol(*i)->content()->show(os);
  }
}

const char *vinstruction::makeInsName() {
  string k;
  for (vector<symid>::iterator i = ins.begin();
       i != ins.end();
       i++) {
    string n = glbSymboltable.getsymbol(*i)->content()->getname();
    string j = "_";
    if (i != ins.begin())
      k = k + n + j;
    else
      k = k + n;
  }
  return k.c_str();
}

bool vinstruction::isdup(vinstruction *p) {
  // compares two instructions
  // does preceise comparison; if the instructions are
  // {3, 4, 3} and {3, 4} it will return false
  // but for {3, 4, 3} and {3, 3, 4} it will return true

  if (!p)
    return false;

  if (ins.size() != p->ins.size())
    return false;

  for (vector<symid>::iterator i = ins.begin();
       i != ins.end();
       i++) {
    // should find it in p
    int occurence = 0;
    for (vector<symid>::iterator j = p->ins.begin();
	 j != p->ins.end();
	 j++) {
      if (*i == *j)
	occurence += 1;
    }
    if (occurence != 1)
      return false;
  }

  return true;
}

//--------------------------------------------------------

void vactl::addstep(symid cond) {
  inslist.push_back(new vinstruction);
  cndlist.push_back(cond);
}

void vactl::addsfg(symid ctl, symid s) {
  if ((inslist.size() > 0) && (inslist.back()))
    inslist.back()->addsfg(s);
  else
    fdlerror(ERROR_INTERNAL_MISSING_INSLIST, ctl);;
}

void vactl::findduplicates() {
  // identify which instructions are the same as others
  // this is done by filling out the duplicate array
  //
  // example
  //
  //  index      0   1   2   3  4   5
  //  inslist    3   4   4   5  4   3
  //  duplicate  N   N   1   N  1   0  (N = null)
  //
  duplicate.push_back(0); // first is never a duplicate
  for (vector<vinstruction *>::iterator i = inslist.begin() + 1;
       i != inslist.end();
       i++) {
    bool found = false;
    for (vector<vinstruction *>::iterator j = inslist.begin();
	 j != i;
	 j++) {
      //      cout << "compare ";
      //     (*i)->dumpInsName(cout);
      //     cout << " and ";
      //      (*j)->dumpInsName(cout);
      //      cout << "\n";
      if ((*i)->isdup(*j)) {
	duplicate.push_back(*j);
	found = true;
	//	cout << "duplicate\n";
	break;
      }
    }
    if (! found)
      duplicate.push_back(0);
  }
}

void vactl::showinstructions(ostream &os) {
  findduplicates();
  vector<vinstruction *>::iterator d = duplicate.begin();
  for (vector<vinstruction *>::iterator i = inslist.begin();
       i != inslist.end();
       i++) {
    vraw(os,"-- ");
    if (*i) {
      (*i)->dumpInsName(os);
    } else vraw(os, "<unknown>");
    if (*d++)
      vraw(os, " <duplicate>");
    vraw(os, "\n");
  }
}

void vactl::defineInsType(venumtype *V) {
  findduplicates();
  vector<vinstruction *>::iterator d = duplicate.begin();
  for (vector<vinstruction *>::iterator i = inslist.begin();
       i != inslist.end();
       i++) {
    if (V) {
      if (*d++ == 0) { // do not add 'duplicates'
	V->addcase((*i)->makeInsName());
      } 
    } else fdlerror(ERROR_VENUMTYPE_IS_NULL);
  }
}

void vactl::collectcondops(vector<symid> &ops) {
  for (vector<symid >::iterator i = cndlist.begin();
       i != cndlist.end();
       ++i) {
    vector<symid> i_ops;
    glbSymboltable.findoperands(*i, i_ops);
    for (vector<symid >::iterator j = i_ops.begin();
	 j != i_ops.end();
	 j++) {
      // add if not yet in ops
      vector<symid>::iterator ins = find(ops.begin(), ops.end(), *j);
      if (ins == ops.end())
	ops.push_back(*j);	
    }
  }
}



//--------------------------------------------------------

//--- vhdl datapath toplevel
vdp::vdp(char *_name) : name(_name) {
}

vdp::~vdp() {
  for (vector<vaport *>::iterator i = ports.begin(); i != ports.end(); ++i)
    delete (*i);
};

void vdp::addctl(vactl *v) {
  ctl = v;
}

void vdp::addinput(char *portname, int wlen) {
  vaport *k = new vstdport(vaport::input, portname, wlen);
  ports.push_back(k);
}

void vdp::addoutput(char *portname, int wlen) {
  vaport *k = new vstdport(vaport::output, portname, wlen);
  ports.push_back(k);
}

void vdp::dumpTopLevel(ostream &os) {
  vcomment(os, "Toplevel for ", name);
  
  vline(os, "library ieee;");
  vline(os, "use ieee.std_logic_1164.all;");
  vline(os, "use work.opspack.all;");
  vline(os);

  vline(os, "entity ", name, " is");
  vline(os, "port (");
  vindentinc();
  for (vector<vaport *>::iterator i=ports.begin(); i != ports.end(); /* INC IN BODY */) {
    (*i)->dumpDeclaration(os);
    i++;
    if (i != ports.end())
      vseparator(os);
  }
  vindentdec();
  vline(os, ");");
  vline(os, "end;");

}

void vdp::dumpTopLevelArchitecture(ostream &os) {

  vline(os);
  vline(os, "architecture RTL of " , name , " is");
  vline(os);

  vindentinc();

  // declare instruction bus
  
  // declare conditions

  // controller
 
  vline(os, "end RTL;");
  vindentdec();
}

void vdp::dump() {
  string vhd = ".vhd";
  string base(name);
  string filename = base + vhd;

  ofstream F(filename.c_str());

  if (F.fail()) {
    fdlerror(ERROR_CANNOT_OPEN_VHD_FILE, filename.c_str());
  }

  vcomment(F, "--------------------------------------------------------------");
  vcomment(F, "GEZEL VHDL Code Generated for ", name);
  vcomment(F, "(c) 2001-2003 University of California at Los Angeles");
  vcommenttime(F, "Generated");
  vcomment(F);
  vcomment(F, "Please report bugs to schaum@ee.ucla.edu");
  vcomment(F, "--------------------------------------------------------------");

  dumpTopLevel(F);

  string ins1 = "_instype";
  string ins2 = base + ins1;
  venumtype *inslisttype = new venumtype(ins2.c_str());

  if (ctl) {

    vector<symid> ops;
    ctl->showinstructions(F);

    ctl->collectcondops(ops);
    vraw(F, "-- Condition Variables: ");
    for (vector<symid>::iterator i = ops.begin();
	 i != ops.end();
	 ++i) {
      glbSymboltable.getsymbol(*i)->content()->show(F);
      vraw(F, " ");
    }
    vraw(F, "\n");

    ctl->defineInsType(inslisttype);
    inslisttype->dumpDeclaration(F);
    vraw(F, "\n");
    inslisttype->vdumptype(F);
    vraw(F, "\n");

  }

  F.close();
}

