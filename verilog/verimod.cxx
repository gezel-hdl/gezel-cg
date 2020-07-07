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
#include "verimod.h"
#include "veriprint.h"

map<symid, verivar *> vericonstants;

verimod::verimod   (string _name) : name   (_name) { }

void verimod::addio (symid n, verivar *v) {
  iolist.push_back(v);
  variables[n] = v;
}

void verimod::addreg (symid n, verivar *v) {
  reglist.push_back(v);
  variables[n] = v;
}

void verimod::addwire (symid n, verivar *v) {
  wirelist.push_back(v);
  variables[n] = v;
}

void verimod::printverilog(veriprint *V) {

  // module header
  V->keyword   (veriprint::MODULE);
  V->identifier(name);
  V->token     (veriprint::BRACKET_L);
  for (iolist_it i = iolist.begin(); i != iolist.end(); i++) {
    if (i != iolist.begin())
      V->token(veriprint::COMMA);
    (*i)->printverilog(V);
  }
  V->token     (veriprint::BRACKET_R);
  V->token     (veriprint::SEMICOLON);
  V->newline();

  V->indentinc();

  // list of input/output signals
  for (iolist_it i = iolist.begin(); i != iolist.end(); i++) {
    V->indent();
    (*i)->printverilog_declaration(V);
    V->newline();
  }


  // list of internal signals
  for (wirelist_it i = wirelist.begin(); i != wirelist.end(); i++) {
      V->indent();
      (*i)->printverilog_declaration(V);
      V->newline();
  }

  for (wirelist_it i = reglist.begin(); i != reglist.end(); i++) {
      V->indent();
      (*i)->printverilog_declaration(V);
      V->newline();
  }

  V->indentdec();
  V->keyword (veriprint::ENDMODULE);
  V->newline ();
}
