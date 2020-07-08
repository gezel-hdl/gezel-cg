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
#ifndef VERIGEN_H
#define VERIGEN_H

#include "fdlcode.h"
#include "verimod.h"

class verigen : public acodegen {

  map <symid, verimod *> modules;

 public:
  void dumpcode();

  // CONTROLLER/ SYSTEM
  void cghwire_ctl  (symid v, symid dp);
  void cgseq_ctl    (symid v, symid dp);
  void cgctlstep    (symid v, symid ctl, int n, symid condition);
  void cgexec       (symid ctl, symid dp, symid sfg);
  void cgtracer     (symid ctl);
  void cgtracesig   (symid v, symid thesig, char *filename);
  void cgfile       (symid v, char *filename);
  void cgoption     (symid v, char *option);
  void cgcon        (symid pin1, symid pin2);
  void cgdisp       (symid disp, symid sfg, symid dp);
  void cgfinish     (symid disp, symid sfg, symid dp);
  void cgdispvar    (symid disp, symid var);
  void cgdispdp     (symid disp);
  void cgdispstr    (symid disp, char *str);
  void cgdispcycle  (symid disp);
  void cgdisptoggle (symid disp);
  void cgdispones   (symid disp);
  void cgdispzeroes (symid disp);
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

  // SFG/ SIGNAL
  void cgdp         (symid v, char *);
  void cgdpclone    (symid v, char *, symid p);
  void cgipblock    (symid v, char * instancename, char *);
  void cgipout      (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgipparm     (symid v, char *parmname, symid _dp);
  void cgsfg        (symid v, char *);
  void cginput      (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgoutput     (symid v, int wlen, int sign, symid _dp, char *portname);
  void cgreg        (symid v, int wlen, int sign, symid _dp, char *);
  void cgsig        (symid v, int wlen, int sign, symid _dp, char *);
  void cgconst      (symid v, char *str);
  void cgassign     (symid v, symid sfg, symid lhs, symid rhs);
  void cgindex      (symid v, symid basevar, int idx, int len);
  void cglutable    (symid v, int wlen, int sign, vector<char *> &content, symid _dp, char *);
  void cgluop       (symid v, symid L, symid table);

  // OPERATIONS
  void cgadd        (symid v, symid L, symid R);
  void cgconcat     (symid v, symid L, symid R);
  void cgsub        (symid v, symid L, symid R);
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

};

#endif
