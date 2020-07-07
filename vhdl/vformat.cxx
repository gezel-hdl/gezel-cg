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

#include "vformat.h"

#include <time.h>

#include <iostream>
#include <fstream>
#include <string>
using std::string;

//--- vindent
static int indentlevel = 0;
void vindent(ostream &os) {
  int i = indentlevel;
  while (i--)
    os << " ";
}

void vindentinc() {
  indentlevel += 2;
}

void vindentdec() {
  indentlevel -= 2;
  if (indentlevel < 0)
    indentlevel = 0;
}

void vcommenttime(ostream &os, const char *a) {
  time_t currenttime;
  time(&currenttime);
  vindent(os);
  os << "-- " << a << " " << ctime(&currenttime);
}

//--- vcomment
void vcomment(ostream &os, const char *a, const char *b) {
  vindent(os);
  os << "-- " << a << b << "\n";
}

void vcomment(ostream &os, const char *a) {
  vindent(os);
  os << "-- " << a << "\n";
}

void vcomment(ostream &os) {
  vindent(os);
  os << "-- \n";
}

void vdumptype(ostream &os, int wlen) {
  if (wlen == 1)
    os << "std_logic";
  else
    os << "std_logic_vector(" << wlen-1 << " downto 0)";
}

void vseparator(ostream &os) {
  os << ";\n";
}

void vline(ostream &os, const char *s) {
  vindent(os);
  os << s << "\n";
}
void vline(ostream &os, const char *s, const char *s2) {
  vindent(os);
  os << s << s2 << "\n";
}
void vline(ostream &os, const char *s, const char *s2, const char *s3) {
  vindent(os);
  os << s << s2 << s3 << "\n";
}
void vline(ostream &os) {
  os << "\n";
}

void vraw(ostream &os, const char *s) {
  os << s;
}





