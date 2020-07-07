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

#include "fdlparse.h"
#include "fdlsym.h"
#include "verigen.h"

#include "ipblock.h"
#include "ipconfig.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <string>

void usage(char *p) {
}

int main(int argc, char **argv) {
  verigen V;

  char *filearg;
  int floc = 1;

  glbIpblockcreate = stdipblockcreate;

  if (argc < 2) {
    usage(argv[0]);
    exit(0);
  }

  if ((floc+1) == argc) {
    filearg = argv[floc];
  } else {
    filearg = "stdin";
  }

  if (call_parser(filearg)) {
    cout << "Could not open " << argv[floc] << "\n";
    exit(0);
  }

  ofstream F("fdl.symbols");
  glbSymboltable.show(F);
  F.close();

  verigen S;
  glbSymboltable.createcode(&S);

  S.dumpcode();

  return 0;

}
