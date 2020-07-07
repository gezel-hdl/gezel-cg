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

#include "veriprint.h"
#include <fstream>

veriprint::veriprint(string filename) : outfile(filename.c_str()) { 
  indent_offset = 0;
}

veriprint::~veriprint() { outfile.close(); }

void veriprint::keyword(verikeywordtype N) {
  switch (N) {
  case MODULE:    outfile << "module ";    break;
  case INPUT:     outfile << "input ";     break;
  case OUTPUT:    outfile << "output ";    break;
  case WIRE:      outfile << "wire ";      break;
  case REG:       outfile << "reg ";       break;
  case SIGNED:    outfile << "signed ";    break;
  case ENDMODULE: outfile << "endmodule "; break;
  }
}

void veriprint::token(veritokentype N) {
  switch (N) {
  case BRACKET_L:   outfile << "("; break;
  case BRACKET_R:   outfile << ")"; break;
  case COMMA:       outfile << ","; break;
  case SEMICOLON:   outfile << ";"; break;
  case COLON:       outfile << ":"; break;
  case SQBRACKET_L: outfile << "["; break;
  case SQBRACKET_R: outfile << "]"; break;
  }
}

void veriprint::identifier(string N) {
  static string keywords = " always end ifnone or rpmos tranif1 and endcase initial output rtran tri assign endmodule inout parameter rtranif0 tri0 begin endfunction input pmos rtranif1 tri1 buf endprimitive integer posedge scalared triand bufif0 endspecify join primitive small trior bufif1 endtable large pull0 specify trireg case endtask macromodule pull1 specparam vectored casex event medium pullup strong0 wait casez for module pulldown strong1 wand cmos force nand rcmos supply0 weak0 deassign forever negedge real supply1 weak1 default for nmos realtime table while defparam function nor reg task wire disable highz0 not release time wor edge highz1 notif0 repeat tran xnor else if notif1 rnmos tranif0 xor ";

  if (keywords.find(string(" ") + N + string(" ")) != string::npos) {
    N = N + "_user";
  }

  outfile << N;
}

void veriprint::newline() {
  outfile << "\n";
}

void veriprint::indentinc() {
  indent_offset += 4;
}

void veriprint::indentdec() {
  indent_offset -= 4;
}

void veriprint::indent() {
  for (int i=0; i<indent_offset; i++)
    outfile << " ";
}

void veriprint::blank() {
  outfile << " ";
}

void veriprint::number(int i) {
  outfile << i;
}
