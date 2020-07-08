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
#include "vgen2.h"

#include "ipblock.h"
#include "ipconfig.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

void usage(char *p) {
  cout << "fdlvhd - GEZEL to VHDL conversion tool\n";
  cout << "(c) 2006 Schaumont, Ching. All Rights Reserved\n";
  cout << "Usage: " << p << " [-d] [-s] [-i] [-c clock reset] [<filename>]\n";
  cout << "  -d          debug report in report.txt\n";
  cout << "  -i          specify active-low reset \n";
  cout << "  -s          specify a synchronous reset \n";
  cout << "  -c          specify clock name, reset name\n";
  cout << "  [filename]  optional file name (default=stdin)\n";
}

int main(int argc, char **argv) {

  int floc = 1;
  int vhddebug = 0;
  int vhdsyn = 0;
  int resetval = 1;
  char *filearg;
  char *clkname, *rstname;

  glbIpblockcreate = stdipblockcreate;

  //  fdldebug = 1;
  if (argc < 2) {
    usage(argv[0]);
    exit(0);
  }

  if(strcmp(argv[1], "-d") == 0) {
    vhddebug = 1;
    floc++;
  }

  if (strcmp(argv[floc], "-s") == 0) {
    floc++;
    vhdsyn = 1;
  }

  if (strcmp(argv[floc], "-i") == 0) {
    floc++;
    resetval = 0;
  }

  if (strcmp(argv[floc], "-c") == 0) {
    floc++;
  
    if ((floc + 3) == argc) {
       clkname = argv[floc++];
       rstname = argv[floc++];     
    }
    else {
	cout <<"missing arguements\n";
        exit(0);
    }
  }
  else {
    clkname = (char *) "CLK";
    rstname = (char *) "RST";
  }

  if ((floc+1) == argc) {
    filearg = argv[floc];
  } else {
    filearg = (char *) "stdin";
  }

  if (call_parser(filearg)) {
    cout << "Could not open " << argv[floc] << "\n";
    exit(0);
  }

  if (vhddebug) {
    ofstream F("fdl.symbols");
    glbSymboltable.show(F);
    F.close();
  }

  vgen S;
  glbSymboltable.createcode(&S);

  S.preprocess();
  S.dump(clkname,rstname,vhdsyn,resetval);
  if (vhddebug)
    S.debugreport();

  return 0;

}


 
