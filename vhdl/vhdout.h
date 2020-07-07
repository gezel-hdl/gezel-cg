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

#ifndef VHDOUT_H
#define VHDOUT_H

#include "vhdvar.h"
#include "gval.h"
#include "vsupport.h"

#include <string>
using std::string;

#include <fstream>
using std::ofstream;

#include <vector>
using std::vector;

class vhdout {
  ofstream os;
  int	indent;  
  
  bool first;
  
 public:
  vhdout(string);
  ~vhdout();
  
  void comment(string);
  void library(string);
  void lib_use(string);
  
  void increaseindent();
  void decreaseindent();
  void doindent();
  
  void outputline(string);
  void outputchar(string);
  void nextline(int);
  
  //for entity
  void beginentity(string);
  void beginentitygeneric();
  void endentitygeneric();
  void beginentityport();
  void dumpgeneric(string p, string t, string d);
  void dumpportvar(vhdvar*);
  void endports(string,bool,string clk,string rst);
  void endentity(string);
  void begincomponent(string);
  void endcomponent(string,bool,string clk, string rst);
  void dumpattrdef(string, string);
  void dumpattruse(string, string, string);

  //for architect
  void architecture(string, string);
  void dumpvardeclare(vhdvar*);
  void beginstatedeclare();
  void eachstate(string);
  void endstatedeclare();
  void lutdeclare(string,int,int);  
  void luttable(string,string,int,vector<gval *>,int);  
  void dumpbitstream(vhdvar*);
  
  //for ram
  void outputram(int ramsize, int ramwl,int addrwl, string clk);
  
  //portmap
  void beginportmap(string,string);
  void dumpmapvar(vhdvar*,vhdvar*);
  void endportmap(bool,string clkpport, string rstport, string clknet, string rstnet);
  
  //for process set up
  void begin();
  void process(string, string);
  void ifresethigh(string rst, int rsthighval);
  void ifclk(string clk);
  void elseclk(string clk);
  void defaultzero(vhdvar*, bool);
  void endif();
  void endprocess(string);
  
  //for case
  void casestr(string);
  void endcase();
  void when(string);
  
  //for support of operator
  string setstringsign(string nm,int sign);
  string conv_std_vector(string nm, int twl);
  string to_std_vector(string nm, int twl,int wl);
  string setstringlen(int twl,string nm,int wl,int sign);

  //for operator
  void binoparith(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm,int lsign,bool rconst, int rwl, string rnm,int rsign);
  void binoparithlogic(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm,int lsign, bool rconst, int rwl, string rnm,int rsign);
  void binopshift(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm, int lsign, bool rconst, int rwl, string rnm);
  void binoplogic(string op, string nm, bool lconst, string lnm, int lsign,int lwl,bool rconst, string rnm,int rsign);
  void dunopnot(string nm,int wl,bool lconst,int lwl, string lnm,int lsign);
  void dunopneg(string nm,int wl,int sign,int lwl, string lnm, bool lconst,int lsign);
  void dunopcast(string nm,int wl,int sign,int lwl, string lnm, bool lconst,int lsign);
  void teropsel(string nm,int wl,string lnm,int lsign,int lwl,string mnm,bool mconst,int mwl,int msign,bool misbit,string rnm,bool rconst,int rwl,int rsign,bool risbit);
  void teropmultsel(bool first,string nm,int wl,string lnm,int lsign,int lwl,string mnm,bool mconst,int mwl,int msign,bool misbit);
  void teropendsel(string nm,int wl,string lnm,int lsign,int lwl,string mnm,bool mconst,int mwl,int msign,bool misbit,string rnm,bool rconst,int rwl,int rsign,bool risbit);
  
  void assign(string nm,int wl,bool lconst,int lwl,string lnm,int lsign,bool lisbit);
  void lutop(string,int,string,int,string,int,bool);
  
  void assignreg(vhdvar*);
  void assigndefaultreg(vhdvar*);
  
  void close();  
  
};

#endif
