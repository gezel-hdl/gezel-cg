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


#ifndef VGEN_H
#define VGEN_H

#include "fdlcode.h"
#include "vsupport.h"

#include "vhdvar.h"
#include "vhdassign.h"
#include "vhdlut.h"

#include "vhdop.h"
#include "vhdunop.h"
#include "vhdbinop.h"
#include "vhdterop.h"

#include "vhdsfg.h"
#include "vhdvalldp.h"
#include "vhdvdp.h"
#include "vhdclonedp.h"
#include "vhdsys.h"
#include "vhdportnet.h"

#include "vhdctl.h"

#include "vhdout.h"
#include "vhdreport.h"


#include <map>
#include <vector>

using namespace std;

// vhdl code generator
class vgen : public acodegen {
  map<symid, vhdvalldp  *>   dplist;
  map<symid, vhdctl     *>   ctllist;
  
  vhdsys * sys;
  vhdctlstep*  cur_step;

  symid cur_dp;
  symid cur_sfg;
  symid cur_ter;

  bool multterop; 
  bool asignconst;
  vector<symid> asignidlist;

  vector<string> generic_dpname;
  vector<string> generic_pname;
  vector<string> generic_ptype;
  vector<string> generic_pdefault;
 
  vector<string> attribute_def_dpname;
  vector<string> attribute_def_attribute;
  vector<string> attribute_def_type;

  vector<string> attribute_use_dpname;
  vector<string> attribute_use_attribute;
  vector<string> attribute_use_signal;
  vector<string> attribute_use_value;

 public:
  vgen();
  ~vgen();
  // CONTROLLER/ SYSTEM
  void cghwire_ctl  (symid v, symid dp);
  void cgseq_ctl    (symid v, symid dp);
  void cgctlstep    (symid v, symid ctl, int n, symid condition);
  void cgexec       (symid ctl, symid dp, symid sfg);
  void cgfile       (symid v, char *filename);
  void cgcon        (symid pin1, symid pin2);
  void cgdisp       (symid disp, symid sfg, symid dp);
  void cgdispvar    (symid disp, symid var);
  void cgdispstr    (symid disp, char *str);
  void cgdispcycle  (symid disp);
  void cgdispbase   (symid disp, int v);
  void cgfsm_ctl    (symid v, symid dp);
  void cgfsm_state  (symid v, symid c, char *str);
  void cgfsm_initial(symid v, symid c, char *str);
  void cgfsm_trans  (symid v, symid c, symid from, symid to);
  void cgctltrue    (symid v, symid c, symid e, symid p);
  void cgctlfalse   (symid v, symid c, symid e, symid p);
  void cgsysdp      (symid v);
  void cgdpc_arg    (symid parentdp, symid parentpin, symid childdp, symid childpin);
  void cgdpc_dp     (symid v, symid parentdp);
  void cgdpc_ipblock(symid v, symid parentdp);
  void cgoption     (symid v, char *option);

  // SFG/ SIGNAL
  void cgdp         (symid v, char *n);
  void cgdpclone    (symid v, char *n, symid p);
  void cgipblock    (symid v, char * instancename, char *);
  void cgipout      (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgipparm     (symid v, char *parmname, symid _dp);
  void cgsfg        (symid v, char*);
  void cginput      (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgoutput     (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgreg        (symid v, int wlen, int sign, symid _dp, char*);
  void cgsig        (symid v, int wlen, int sign, symid _dp, char*);
  void cgconst      (symid v, char *str);
  void cgassign     (symid v, symid sfg, symid lhs, symid rhs);
  void cgindex      (symid v, symid basevar, int idx, int len);
  void cglutable    (symid v, int wlen, int sign, vector<char *> &content, symid _dp,char*);
  void cgluop       (symid v, symid L, symid table);

  // OPERATIONS
  void cgadd        (symid v, symid L, symid R);
  void cgsub        (symid v, symid L, symid R);
  void cgconcat     (symid v, symid L, symid R);
  void cgior        (symid v, symid L, symid R);
  void cgxor        (symid v, symid L, symid R);
  void cgand        (symid v, symid L, symid R);
  void cgshr        (symid v, symid L, symid R);
  void cgshl        (symid v, symid L, symid R);
  void cgmul        (symid v, symid L, symid R);
  void cgmod        (symid v, symid L, symid R);
  void cgeq         (symid v, symid L, symid R);
  void cgne         (symid v, symid L, symid R);
  void cggrt        (symid v, symid L, symid R);
  void cgsmt        (symid v, symid L, symid R);
  void cggrteq      (symid v, symid L, symid R);
  void cgsmteq      (symid v, symid L, symid R);
  void cgcast       (symid v, symid L, int wlen, int sign);
  void cgnot        (symid v, symid L);
  void cgneg        (symid v, symid L);
  void cgsel        (symid v, symid L, symid M, symid R);


  // dump
  void preprocess();
  void dump(string,string,int,int);
  void debugreport();
};


#endif
