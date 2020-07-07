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

#include "verigen.h"
#include "verimod.h"
#include "gval.h"

void verigen::dumpcode() {

  for (map <symid, verimod *>::iterator i = modules.begin();
       i != modules.end();
       i++) {
    // add reset and clock port to each module
    verivar *v1 = new verivar(1, 0, "clk", verivar::input);
    i->second->addio(NOSYMBOL, v1);

    verivar *v2 = new verivar(1, 0, "rst", verivar::input);
    i->second->addio(NOSYMBOL, v2);
  }

  // print all modules
  for (map <symid, verimod *>::iterator i = modules.begin();
       i != modules.end();
       i++) {
    // for module abc, create file "abc.v"
    veriprint V(i->second->getname() + string(".v"));
    i->second->printverilog(&V);
  }

  // print all constants
  veriprint V(string("constants.v"));
  for (vericonstants_it i = vericonstants.begin();
       i != vericonstants.end();
       i++) {
    i->second->printverilog(&V);
    V.newline();
  }


}

void verigen::cghwire_ctl  (symid v, symid dp) {  }

void verigen::cgseq_ctl    (symid v, symid dp) {  }

void verigen::cgctlstep    (symid v, symid ctl, int n, symid condition) {  }

void verigen::cgexec       (symid ctl, symid dp, symid sfg) {  }

void verigen::cgtracer     (symid ctl) {  }

void verigen::cgtracesig   (symid v, symid thesig, char *filename) {  }

void verigen::cgfile       (symid v, char *filename) {  }

void verigen::cgoption     (symid v, char *option) {  }

void verigen::cgcon        (symid pin1, symid pin2) {  }

void verigen::cgdisp       (symid disp, symid sfg, symid dp) {  }

void verigen::cgfinish     (symid disp, symid sfg, symid dp) {  }

void verigen::cgdispvar    (symid disp, symid var) {  }

void verigen::cgdispdp     (symid disp) {  }

void verigen::cgdispstr    (symid disp, char *str) {  }

void verigen::cgdispcycle  (symid disp) {  }

void verigen::cgdisptoggle (symid disp) {  }

void verigen::cgdispones   (symid disp) {  }

void verigen::cgdispzeroes (symid disp) {  }

void verigen::cgdispbase   (symid disp, int v) {  }

void verigen::cgfsm_ctl    (symid v, symid dp) {  }

void verigen::cgfsm_state  (symid v, symid c, char *str) {  }

void verigen::cgfsm_initial(symid v, symid c, char *str) {  }

void verigen::cgfsm_trans  (symid v, symid c, symid from, symid to) {  }

void verigen::cgctltrue    (symid v, symid c, symid e, symid p) {  }

void verigen::cgctlfalse   (symid v, symid c, symid e, symid p) {  }

void verigen::cgsysdp      (symid v) {  }

void verigen::cgdpc_arg    (symid parentdp, symid parentpin, symid childdp, symid childpin) {  }

void verigen::cgdpc_dp     (symid v, symid parentdp) {  }

void verigen::cgdpc_ipblock(symid v, symid parentdp) {  }

void verigen::cgdp         (symid v, char *n) {  
  modules[v] = new verimod(string(n));
}

void verigen::cgdpclone    (symid v, char *, symid p) {  }

void verigen::cgipblock    (symid v, char * instancename, char *) {  }

void verigen::cgipout      (symid v, int wlen, int sign, symid _dp, char *portname) {  }

void verigen::cgipparm     (symid v, char *parmname, symid _dp) {  }

void verigen::cgsfg        (symid v, char *n) {  }

void verigen::cginput      (symid v, int wlen, int sign, symid _dp, char *portname) {
  verivar *vv = new verivar(wlen, sign, string(portname), verivar::input);
  modules[_dp]->addio(v, vv);
}

void verigen::cgoutput     (symid v, int wlen, int sign, symid _dp, char *portname) {  
  verivar *vv = new verivar(wlen, sign, string(portname), verivar::output);
  modules[_dp]->addio(v, vv);
}

void verigen::cgreg        (symid v, int wlen, int sign, symid _dp, char *n) {  
  verivar *vv = new verivar(wlen, sign, string(n), verivar::reg);
  modules[_dp]->addreg(v, vv);
}

void verigen::cgsig        (symid v, int wlen, int sign, symid _dp, char *n) {  
  verivar *vv = new verivar(wlen, sign, string(n), verivar::wire);
  modules[_dp]->addwire(v, vv);
}

void verigen::cgconst      (symid v, char *str) {
  string value;
  int wl;
  if (str[0] == '0' && str[1] == 'x') {
    value = string(str);
    value.replace(0,2,"'h");
  } else if (str[0] == '0' && str[1] == 'x') {
    value = string(str);
    value.replace(0,2,"'b");
  } else {
    value = string(str);
    value.insert(0,"'d");
  }

  gval q1(str); // make sure we have same wordlength as gezel for decimals
  wl = q1.getwordlength();

  verivar *vv = new verivar(wl, 0 /* unsigned */, value, verivar::constant);
  vericonstants[v] = vv;
}

void verigen::cgassign     (symid v, symid sfg, symid lhs, symid rhs) {
}

void verigen::cgindex      (symid v, symid basevar, int idx, int len) {  }

void verigen::cglutable    (symid v, int wlen, int sign, vector<char *> &content, symid _dp, char *) {  }

void verigen::cgluop       (symid v, symid L, symid table) {  }

void verigen::cgadd        (symid v, symid L, symid R) {  }

void verigen::cgconcat     (symid v, symid L, symid R) {  }

void verigen::cgsub        (symid v, symid L, symid R) {  }

void verigen::cgior        (symid v, symid L, symid R) {  }

void verigen::cgxor        (symid v, symid L, symid R) {  }

void verigen::cgand        (symid v, symid L, symid R) {  }

void verigen::cgshr        (symid v, symid L, symid R) {  }

void verigen::cgshl        (symid v, symid L, symid R) {  }

void verigen::cgmul        (symid v, symid L, symid R) {  }

void verigen::cgmod        (symid v, symid L, symid R) {  }

void verigen::cgeq         (symid v, symid L, symid R) {  }

void verigen::cgne         (symid v, symid L, symid R) {  }

void verigen::cggrt        (symid v, symid L, symid R) {  }

void verigen::cgsmt        (symid v, symid L, symid R) {  }

void verigen::cggrteq      (symid v, symid L, symid R) {  }

void verigen::cgsmteq      (symid v, symid L, symid R) {  }

void verigen::cgcast       (symid v, symid L, int wlen, int sign) {  }

void verigen::cgnot        (symid v, symid L) {  }

void verigen::cgneg        (symid v, symid L) {  }

void verigen::cgsel        (symid v, symid L, symid M, symid R) {  }

