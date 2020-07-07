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
#ifndef VERIVAR_H
#define VERIVAR_H

#include "veriprint.h"

using namespace std;
#include <string>

// verilog variable (GEZEL input, output, wire, reg)
class verivar {
 public:
  enum verivartype {input, output, wire, reg, constant};
 private:
  int         wlen;
  int         sign;
  string      name;
  string      value;  // constant
  verivartype type;
 public:
  verivar(int _wlen, int _sign, string _name, verivartype _type);

  void printverilog(veriprint *V);
  void printverilog_declaration(veriprint *V);
  bool is_io();
};

#endif
