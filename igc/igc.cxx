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
#include "rtsim.h"
#include "rtopt.h"
#include "icgen.h"
#include "ipblock.h"   // glbIpblockcreate
#include "ipconfig.h"  // stdipblockcreate

using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

void usage(char *p) {
  cout << "Conversion of GEZEL FSMD into GEZEL/C++ IPBLOCK\n";
  cout << "Usage: " << p << " [-d] <ifile> <fsmd>\n";
  cout << "    -h           print this message\n";
  cout << "    <ifile>      input GEZEL file name\n";
  cout << "    <fsmd>       FSMD Module to convert\n";
  cout << "                 C++ file is in <fsmd>.{cxx, h}\n";
  cout << "                 ipblock is in <fsmd>.fdl\n";
}

int main(int argc, char **argv) {
  int floc = 1;
  char *ifile;
  char *dpname;

  glbIpblockcreate = stdipblockcreate;

  if (argc < 2) {
    usage(argv[0]);
    exit(0);
  }
    
  if(strcmp(argv[1], "-h") == 0) {
    floc++;
    usage(argv[0]);
    exit(0);
  }

  if (argc != 3) {
    usage(argv[0]);
    exit(0);
  }

  ifile  = argv[1];
  dpname = argv[2];
  
  if (call_parser(ifile)) {
    cout << "Failed to parse " << ifile << "\n";
    exit(0);
  }

  icgen S;
  glbSymboltable.createcode_dp(&S, dpname);

  S.generate();

  return 0;
}
