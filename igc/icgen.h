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

#ifndef ICGEN_H
#define ICGEN_H

#include "fdlcode.h"
#include "gval.h"
#include <fstream>
#include <set>

class icgen : public acodegen {

  char *modulename;

  // for each symid, typemap collects the type:
  //    ops_t -> is an intermediate signal or signal of 'gval' type
  //  const_t -> is a constant
  //    reg_t -> is a register
  //     in_t -> is a datapath input
  //    out_t -> is a datapath output
  enum gval_t {sig_t, const_t, reg_t, lut_t, inp_t, out_t};
  map <symid, gval_t> symkind;
  typedef map <symid, gval_t>::iterator symkind_it;
 
  // collects operation type
  enum optype_t {op_assign, 
		 op_index, 
		 op_add, 
		 op_concat, 
		 op_sub,
		 op_ior, 
		 op_xor, 
		 op_and, 
		 op_shr, 
		 op_shl, 
		 op_mul,
		 op_mod, 
		 op_eq, 
		 op_ne, 
		 op_grt, 
		 op_smt, 
		 op_grteq,
		 op_smteq, 
		 op_cast, 
		 op_not, 
		 op_neg, 
		 op_sel,
                 op_lu,
                 op_ipblock,
                 op_disp};

  map <symid, optype_t> opkind;
  typedef map <symid, optype_t>::iterator opkind_it;
  map <symid, symid>    opleft;
  map <symid, symid>    opright;
  map <symid, symid>    opmiddle;
  map <symid, symid>    oplutable;           // for op_lu
  map <symid, int>      opindex;             // for op_index

  vector <symid>        opip;           // included ipblock (points into glbSymboltable)
  typedef map<symid, char *> opip_typename_t;
  opip_typename_t opip_typename;        // type name of this ipblock
  typedef map<symid, vector<symid> > opip_pin_t;
  opip_pin_t            opip_pin;       // ipblock connections to parent pins 
  typedef map<symid, vector<bool>  > opip_input_t;
  opip_input_t          opip_input;
  typedef map<symid, vector<bool>  > opip_regout_t;
  opip_input_t          opip_regout;

  // collects (inferred) type information
  map <symid, gval *>           symtype;

  // contents of a lookup table
  map <symid, vector<char *> >  ludata;

  // list of items to display
  enum dispid {dispvar, dispdp, dispstr, dispcycle, dispbase};
  map <symid, vector<unsigned> > displaymap;
  unsigned dispctr; 
  bool     needscyclectr;
  map <unsigned, dispid>         displaytype;
  map <unsigned, symid>          displayvar;
  map <unsigned, char *>         displaystr;
  map <unsigned, int>            displaybase;

  // collect list of states (their names)
  map <symid, char *> states;
  symid               initialstate;
  typedef map <symid, char *>::iterator states_it;

  // controller (also collects conditional expr in 'expressions' below)
  symid controller;
  symid ctlstep;   // current control step

  map <symid, symid>          stepcond;   // map ctl step to condition
  map <symid, vector<symid> > stepsfg;    // map ctl step to sfg
  map <symid, vector<symid> > srcstep;    // map source state to ctl step
  map <symid, symid>          steptgt;    // map ctl step to target state

  map <symid, symid>          condvar;    // map condition to expression
  map <symid, bool>           condtrue;   // map condition to true/false branch
  map <symid, symid>          condparent; // map condition to parent
  map <symid, symid>          condscope;  // map sfgid recorded for fsm condexpr
                                          // to source states
  typedef map<symid, vector<symid> >::iterator stepsfg_it;

  // contains names and number of ioports
  // (direction is stored in symkind)
  map <symid, char *> portnames;
  map <symid, int>    portnumbers;  // port index 
  int portnumber;                   // port index counter

  // collect list of sfg
  map <symid, char *> sfgs;
  typedef map <symid, char *>::iterator sfgs_it;

  // for each sfg, a list of the expressions contained within it.
  symid currentsfg;
  typedef map<symid, vector<symid> > expressions_t;
  expressions_t expressions;
  expressions_t input_expressions;
  expressions_t output_expressions;
  typedef vector<symid>::iterator expr_it;

  int wlensym(symid );
  int signsym(symid );

  // output dependencies, per sfg (used by collect_output_deps)
  typedef map<symid, map<symid, set<symid> > > odeps_t;
  //          output     sfg          ops
  odeps_t odeps;
  odeps_t nodeps;  // output dependencies of independent outputs
                   // (i.e. outputs which have no comb path to input)
  set<symid> nodep_sfg; // all sfg which contain independent outputs

  // dependency analysis
  bool dependson               (symid, symid);
  void collect_dependson       (vector<symid>, symid, set<symid> &);
  void sort_sfg_expressions    (expressions_t &, symid sfgid);
  void init_odeps              ();
  void collapse_sfg            ();
  void partition_expressions   ();
  void insert_hierarchy_ops    ();
  void find_hierarchy_regout   ();

  void generate_indent         (ostream &, int);
  void generate_cxx            ();
  void generate_header         ();
  void generate_ipblock        ();
  void generate_constructor    (ostream &);
  void generate_setparm        (ostream &);
  void generate_sfgs           (ostream &);
  void generate_ctlconditions  (ostream &, int );

  void generate_sfg_nodep_ops  (ostream &, int, symid /*sfgid*/);
  void generate_sfg_ops        (ostream &, int, symid /*sfgid*/);
  void generate_sfg_oneop      (ostream &, int, symid /*opid*/ );
  void generate_update         (ostream &);
  typedef void (icgen::*generate_action_t)(ostream &, int, symid);
  void generate_action         (ostream &, int, symid);
  void generate_out_action     (ostream &, int, symid);
  void generate_condition_scope(ostream &, generate_action_t , int, symid scope, vector<symid> &src);
  void generate_source_state   (ostream &, generate_action_t, symid);
  void generate_run            (ostream &);
  void generate_out_run        (ostream &);
  void generate_checkterminal  (ostream &);

  void debug_resolve_hierarchy ();
  void debug_print_action      (int, symid);
  void debug_print_condition_scope(int, symid scope, vector<symid> &src);
  void debug_print_source_state(symid);
  void debug_print_output_deps ();
  void debug_print             ();

  const char *symname(symid v); // name used by the code generator
  const char *lexname(symid v); // name written by the user in GEZEL code
  const char *lhsname(symid v); // name used when this symbol is assigned to
  const char *rhsname(symid v); // name used when this symbol is read from

 public:
  icgen(); // collect info for this dp
  virtual ~icgen();
  
  void generate();

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
