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
#ifndef VERIPRINT_H
#define VERIPRINT_H

using namespace std;
#include <string>
#include <fstream>

class veriprint {
  ofstream outfile;
  int indent_offset;
 public:
  enum verikeywordtype {MODULE, INPUT, OUTPUT, WIRE, REG, SIGNED, ENDMODULE};
  enum veritokentype   {BRACKET_L, BRACKET_R, COMMA, SEMICOLON, SQBRACKET_L, SQBRACKET_R, COLON};
 veriprint   (string filename );
  ~veriprint  ();
  void keyword(verikeywordtype );
  void token  (veritokentype   );
  void identifier(string       );
  void newline();
  void indentinc();
  void indentdec();
  void indent();
  void blank();
  void number(int);
};


#endif
