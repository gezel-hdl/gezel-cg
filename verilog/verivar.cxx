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
#include "verivar.h"

verivar::verivar(int _wlen, int _sign, string _name, verivartype _type) :
  wlen(_wlen), sign(_sign), name(_name), type(_type) { }

void verivar::printverilog(veriprint *V) {
  if (type == constant) {
    // need to print wordlength for nn'h..., nn'b..., nn'd...
    V->number(wlen);
  }
  V->identifier(name);
}

void verivar::printverilog_declaration(veriprint *V) {
  switch(type) {
  case input:
    V->keyword(veriprint::INPUT);
    break;
  case output:
    V->keyword(veriprint::OUTPUT);
    break;
  case wire:
    V->keyword(veriprint::WIRE);
    break;
  case reg:
    V->keyword(veriprint::REG);
    break;
  case constant:
    // constants are not declared
    break;
  }

  if (sign)
    V->keyword(veriprint::SIGNED);

  if (wlen >1) {
    V->token(veriprint::SQBRACKET_L);
    V->number(wlen-1);
    V->token(veriprint::COLON);
    V->number(0);
    V->token(veriprint::SQBRACKET_R);
  }

  V->blank();
  V->identifier(name);
  if (type == reg) {
    V->token(veriprint::COMMA);
    V->blank();
    V->identifier(name + "_next");    
  }
  V->token(veriprint::SEMICOLON);

}

bool verivar::is_io() {
  return ((type == input) || (type == output));
}
