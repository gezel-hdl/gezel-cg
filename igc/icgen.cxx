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

#include "icgen.h"
// #include "asymbol.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
using std::string;
using std::ostringstream;

typedef ostringstream ostrstr_t;
#else
#include <strstream.h>

typedef ostrstream ostrstr_t;
#endif

#include <stdlib.h>

icgen::icgen() {
  portnumber = 0;
  dispctr    = 0;
  needscyclectr = false;
}

icgen::~icgen() {}

static map<symid, string> symnames;

const char *icgen::symname(symid j) {
  if (symnames.find(j) == symnames.end()) {
    ostrstr_t strstrm;
    strstrm << "op_" << j;
    symnames[j] = strstrm.str();
  }
  return symnames[j].c_str();
}

const char *icgen::lexname(symid j) {
  return glbSymboltable.getsymbol(j)->content()->getname();
}

static map<symid, string> lhsnames;

const char *icgen::lhsname(symid j) {
  if (lhsnames.find(j) == lhsnames.end()) {
    ostrstr_t strstrm;
    if (symkind.find(j) != symkind.end()) {
      // is a terminal
      switch (symkind[j]) {
      case reg_t:
	strstrm << "*op_" << j << "_next";
	break;
      case sig_t:
	strstrm << "*op_" << j;
	break;
      case out_t:
	strstrm << "*ioval[" << portnumbers[j] << "]";
	break;
      case inp_t:
      case const_t:
      default:
	strstrm << "EXCEPTION_" << j;
	break;
      }
    } else {
      strstrm << "*op_" << j;
    }
    lhsnames[j] = strstrm.str();
  }
  return lhsnames[j].c_str();  
}

static map<symid, string> rhsnames;

const char *icgen::rhsname(symid j) {
  if (rhsnames.find(j) == rhsnames.end()) {
    ostrstr_t strstrm;
    if (symkind.find(j) != symkind.end()) {
      // is a terminal
      switch (symkind[j]) {
      case reg_t:
	strstrm << "*op_" << j;
	break;
      case const_t:
      case sig_t:
	strstrm << "*op_" << j;
	break;
      case inp_t:
      case out_t:
	strstrm << "*ioval[" << portnumbers[j] << "]";
	break;
      default:
	strstrm << "unknown_" << j;
      }
    } else {
      strstrm << "*op_" << j;
    }
    rhsnames[j] = strstrm.str();
  }
  return rhsnames[j].c_str();  
}

void icgen::generate_checkterminal(ostream& OF) {
  int cnt = 0;
  OF << "bool " << modulename << "::checkterminal(int n, char *tname, aipblock::iodir d) {\n";

  OF << "  switch(n) {\n";
  for (map<symid, char *>::iterator i = portnames.begin();
       i != portnames.end();
       i++) {
    OF << "  case " << cnt++ << " :\n";
    if (symkind[i->first] == inp_t) 
      OF << "    return (isinput(d)  && isname(tname, \"" << i->second << "\"));\n";
    else
      OF << "    return (isoutput(d) && isname(tname, \"" << i->second << "\"));\n";
    OF << "    break;\n";
  }
  OF << "  }\n";
  OF << "  return false;\n";
  OF << "}\n";

  OF << "\n";

  OF << "bool " << modulename << "::cannotSleepTest(){\n";
  OF << "  return false;\n";
  OF << "}\n";

  OF << "\n";

  OF << "bool " << modulename << "::needsWakeupTest(){\n";
  // need to implement a 'wake-up test'
  OF << "  return true;\n";
  OF << "}\n";
  
}

void icgen::generate_setparm(ostream &OF) {
  OF << "void " << modulename << "::setparm(char *_name) {\n";
  OF << "  gval *v = make_gval(32,0);\n";
  OF << "  if (matchparm(_name, \"regOut\", *v))\n";
  OF << "    regOutput(v->toulong());\n";
  OF << "}\n";
}

void icgen::generate_indent(ostream &OF, int nesting) {
  for (int p = 0; p < nesting; p++) OF << "  ";
}

bool icgen::dependson(symid s1, symid s2) {
  // means: the operands required to evaluate s2 are 
  // available only AFTER execution of s1.
  // Therefore, s1 should be executed before s2.

  // in case s1 is an assignment, we look at the real left-hand side of
  // this assignment (to find a match against possible operands of s2)
  if (opkind.find(s1) != opkind.end())
    if (opkind[s1] == op_assign)
      return dependson(opleft[s1], s2);
  
  // in case s1 is an ipblock, we look at the DEPENDENT outputs of the
  // ipblock (i.e. outputs that do not appear in opip_regout)
  if (opkind.find(s1) != opkind.end())
    if (opkind[s1] == op_ipblock) {
      bool retval = false;
      vector<symid>::iterator k1;
      vector<bool>::iterator k2;
      vector<bool>::iterator k3;
      for (k1 = opip_pin[s1].begin(), 
	     k2 = opip_input[s1].begin(), 
	     k3 = opip_regout[s1].begin();
	   k1 != opip_pin[s1].end();
	   k1++, k2++, k3++) 
	if (! *k2)  // is an output
	  if (! *k3) // is a dependent output 
  	    retval |= dependson(*k1, s2);
      return retval;
    }
  
  // if s2 is a literal, a dependency will be found only of s1 and s2
  // are identical and s2 is not a register (because in that case
  // execution of s1 assigns the value of the NEXT cycle only)
  if (symkind.find(s2) != symkind.end()) {
    if (symkind[s2] == reg_t)
      return false; // a register
    else
      return (s2 == s1);
  }

  // if s1 and s2 are both operations, s1 should come first
  // (because s2 can be an operand in a recursive function call)
  if ((opkind.find(s2) != opkind.end())
      && (s2 == s1))
    return true;

  // if s2 is an operation, descend into the operands
  if (opkind.find(s2) != opkind.end()) {
    switch (opkind[s2]) {
    case op_assign:
      return dependson(s1, opright[s2]);
      break;
    case op_index: 
    case op_cast: 
    case op_not: 
    case op_neg: 
      return dependson(s1, opleft[s2]);
      break;
    case op_add: 
    case op_concat: 
    case op_sub:
    case op_ior: 
    case op_xor: 
    case op_and: 
    case op_shr: 
    case op_shl: 
    case op_mul:
    case op_mod: 
    case op_eq: 
    case op_ne: 
    case op_grt: 
    case op_smt: 
    case op_grteq:
    case op_smteq: 
      return (dependson(s1, opleft[s2]) || dependson(s1, opright[s2]));
      break;
    case op_sel:
      return (dependson(s1, opleft [s2]) || 
	      dependson(s1, opright[s2]) || 
	      dependson(s1, opmiddle[s2]));
      break;
    case op_lu:
      return (dependson(s1, opleft[s2]));
      break;
    case op_ipblock:
      { // check dependency on any ipblock input
	vector<symid>::iterator i1;
	vector<bool>::iterator  i2;
	bool retval = false;
	for (i1 = opip_pin[s2].begin(), i2 = opip_input[s2].begin();
	     i1 != opip_pin[s2].end();
	     i1++, i2++) 
	  if (*i2) // is an input 
	    retval |= dependson(s1, *i1);
	return retval;
      }
      break;
    case op_disp:
	return false;
      break;
    default:
      break;
    }
  }

  // if we come here, unexpected dependency resolution
  cerr << "dependson: unexpected dependency on symbol " << s2 << "\n";
  return false;

}

void icgen::sort_sfg_expressions(expressions_t &expr, symid sfgid) {
  // make sure the order of operations in expressions is procedurally executable
  // by means of a simple bubble sort on dependencies
  bool swap;
  expr_it i1, i2;

  swap = true;
  while (swap) {
    swap = false;
    for (i1 = expr[sfgid].begin();
	 i1 != expr[sfgid].end();
	 i1++) {
      for (i2 = expr[sfgid].begin();
	   i2 != i1;
	   i2++) {

 	if (dependson(*i1, *i2)) {
	  // means: the operands required to evaluate i2 are available
	  // only after i1 has executed. thus, we should swap i1 and i2
 	  symid tmp;
 	  tmp = *i2;
 	  *i2 = *i1;
 	  *i1 = tmp;
 	  swap = true;
 	}

      }
    }
  }

  // move all display expressions to the end of the expressions vector
  // These display expressions are only valid at the end of the clock
  // cycle (after all operations).
  vector<symid> otherexpr, dispexpr;
  for (i1 = expr[sfgid].begin(); i1 != expr[sfgid].end(); i1++)
    if ((opkind.find(*i1) != opkind.end()) && (opkind[*i1] == op_disp))
      dispexpr.push_back(*i1);
    else
      otherexpr.push_back(*i1);
  expr[sfgid].clear();
  for (i1 = otherexpr.begin(); i1 != otherexpr.end(); i1++)
    expr[sfgid].push_back(*i1);
  for (i1 = dispexpr.begin(); i1 != dispexpr.end(); i1++)
    expr[sfgid].push_back(*i1);

}

void icgen::generate_sfg_oneop(ostream &OF, int nesting, symid opid) {
  generate_indent(OF, nesting);
  switch (opkind[opid]) {
  case op_assign:
    OF << lhsname(opleft[opid]);
    OF << " = ";
    OF << rhsname(opright[opid]);
    OF << " /* " << lexname(opleft[opid]) << " */;\n";
    break;
  case op_index: 
    OF << symname(opid);
    OF << "->shr_ui_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << opindex[opid];
    OF << ");\n";
    break;
  case op_add: 
    OF << symname(opid);
    OF << "->add_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_concat: 
    OF << symname(opid);
    OF << "->concat_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_sub:
    OF << symname(opid);
    OF << "->sub_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_ior: 
    OF << symname(opid);
    OF << "->ior_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_xor: 
    OF << symname(opid);
    OF << "->xor_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_and: 
    OF << symname(opid);
    OF << "->and_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_shr: 
    OF << symname(opid);
    OF << "->shr_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_shl: 
    OF << symname(opid);
    OF << "->shl_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_mul:
    OF << symname(opid);
    OF << "->mul_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_mod: 
    OF << symname(opid);
    OF << "->mod_gval(";
    OF << rhsname(opleft[opid]);
    OF << ",";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_eq: 
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " == " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_ne: 
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " != " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_grt: 
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " > " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_smt: 
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " < " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_grteq:
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " >= " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_smteq: 
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "(" << rhsname(opleft[opid]) << " <= " << rhsname(opright[opid]) << ")";
    OF << " ? *gval_one : *gval_zero);\n";
    break;
  case op_cast: 
    // cast by means of assignment
    OF << lhsname(opid) << " = " << rhsname(opleft[opid]) << ";\n";
    break;
  case op_not: 
    OF << symname(opid);
    OF << "->not_gval(";
    OF << rhsname(opleft[opid]);
    OF << ");\n";
    break;
  case op_neg: 
    OF << symname(opid);
    OF << "->neg_gval(";
    OF << rhsname(opleft[opid]);
    OF << ");\n";
    break;
  case op_sel:
    OF << "if ((" << rhsname(opleft[opid]) << ").isnz())\n    (";
    OF << rhsname(opid);
    OF << ").valuecopy(";
    OF << rhsname(opmiddle[opid]);
    OF << ");\n  else\n    (";
    OF << rhsname(opid);
    OF << ").valuecopy(";
    OF << rhsname(opright[opid]);
    OF << ");\n";
    break;
  case op_lu:
    OF << symname(opid);
    OF << "->valuecopy(";
    OF << "*" << symname(oplutable[opid]);
    OF << "[(";
    OF << rhsname(opleft[opid]);
    OF << ").toulong()]";
    OF << ");\n";
    break;
  case op_ipblock: 
    // copy parent signals to ipblock input
    {
      vector<symid>::iterator i1;
      vector<bool>::iterator  i2;
      int                     i3;
      vector<bool>::iterator  i4;
      bool first = true;
      for (i1 = opip_pin[opid].begin(), i2 = opip_input[opid].begin(), i3=0;
	   i1 != opip_pin[opid].end();
	   i1++, i2++, i3++) {
	if (*i2) { // is an input 
	  if (first) first = false; else OF << "  ";
	  OF << "(" << lhsname(opid) << ").ioval[" << i3 << "]->valuecopy(";
	  OF << rhsname(*i1)  << ");\n";
	}
      }
      if (first) first = false; else OF << "  ";
      OF << symname(opid) << "->run();\n";
      for (i1 = opip_pin[opid].begin(), 
	     i2 = opip_input[opid].begin(), 
	     i3=0,
	     i4 = opip_regout[opid].begin();
	   i1 != opip_pin[opid].end();
	   i1++, i2++, i3++, i4++) {
	if (! *i2) { // is an output
	  if (! *i4) { // is a non-registered output
	    if (first) first = false; else OF << "  ";
	    OF << "(" << lhsname(*i1)  << ").valuecopy(";
	    OF << rhsname(opid) << "->ioval[" << i3 << "]);\n";
	  }
	}
      }
    }
    break;
  case op_disp:
    { vector<unsigned>::iterator lst;
      for (lst = displaymap[opid].begin();
	   lst != displaymap[opid].end();
	   lst++) {
	if (lst != displaymap[opid].begin())
	  OF << "  ";
	switch (displaytype[*lst]) {
	case dispvar:
	  switch (symkind[displayvar[*lst]]) {
	  case reg_t:
	    OF << "cout << " <<  rhsname(displayvar[*lst]);
	    OF << " << \"/\" << " << lhsname(displayvar[*lst]) << ";\n";
	    break;
	  case sig_t:
	  case const_t:
	  case inp_t:
	  case out_t:
	    OF << "cout << " << rhsname(displayvar[*lst]) << ";\n";
	    break;
	  case lut_t:
	    break;
	  }
	  break;
	case dispdp:
	  OF << "cout << \"" << modulename << "\";\n";
	  break;
	case dispstr:
	  OF << "cout << \"" << displaystr[*lst] << "\";\n";
	  break;
	case dispcycle:
	  OF << "cout << cycle;\n";
	  break;
	case dispbase:
	  OF << "cout << \"<base_change>\";\n";
	  break;
	}
      }
      OF << "  cout << \"" << "\\" << "n" << "\";\n";
    }
    break;
  default:
    OF << "  unsupported op id " << opid;
    break;
  }
}

void icgen::partition_expressions() {

  for (stepsfg_it i3 = stepsfg.begin();
       i3 != stepsfg.end();
       i3++) {

    for (expr_it i = expressions[i3->first].begin();
	 i != expressions[i3->first].end();
	 i++) // loop over all sfg
      {
	// is expression can be found in nodeps for this sfg, it is 
	// an output expression.
	// otherwise, it is an input expression
	
	bool output_expr = false;

	for (odeps_t::iterator j = nodeps.begin();
	     j != nodeps.end();
	     j++) { // loop over all outputs
	  
	  if (j->second.find(i3->first) != j->second.end()) {
	    
	    for (set<symid>::iterator k = j->second[i3->first].begin();
		 k != j->second[i3->first].end();
		 k++) {
	      
	      if (*i == *k)
		output_expr = true;
	    }
	  }
	}

	if (output_expr)
       	  output_expressions[i3->first].push_back(*i);
	else
	  input_expressions[i3->first].push_back(*i);
      }

    // sorting guarantees procedural exeution of
    // expressions will not violate data dependencies
    sort_sfg_expressions(input_expressions,  i3->first);
    sort_sfg_expressions(output_expressions, i3->first);

  }
}

void icgen::generate_ctlconditions(ostream &OF, int nesting) {

  if (expressions.find(-1) == expressions.end())
    return;

  for (expr_it i = expressions[-1].begin();
       i != expressions[-1].end();
       i++) 
    generate_sfg_oneop(OF, nesting, *i);
}


void icgen::generate_sfg_ops(ostream &OF, int nesting, symid sfgid) {

  if (input_expressions.find(sfgid) == input_expressions.end())
    return;

  for (expr_it i = input_expressions[sfgid].begin();
       i != input_expressions[sfgid].end();
       i++) 
    generate_sfg_oneop(OF, nesting, *i);
}

void icgen::generate_sfg_nodep_ops(ostream &OF, int nesting, symid sfgid) {

  if (output_expressions.find(sfgid) == expressions.end())
    return;
  
  for (expr_it i = output_expressions[sfgid].begin();
       i != output_expressions[sfgid].end();
       i++) 
    generate_sfg_oneop(OF, nesting, *i);

}


void icgen::generate_sfgs(ostream &OF) {

  // sfgs are dumped PER CONTROL STEP there is a single sfg for each
  // control step. This is needed to allow for signal dependencies
  // between individual sfg (for these sfg it is not possible to find
  // a correct procedural execution order)

  for (stepsfg_it i3 = stepsfg.begin();
       i3 != stepsfg.end();
       i3++) {

    if (i3 != stepsfg.begin()) OF << "\n";
    OF << "void " << modulename << "::sfg_" << i3->first << "() {\n";

    for (vector<symid>::iterator i4 = i3->second.begin();
    	 i4 != i3->second.end();
    	 i4++) {
      if (i4 == i3->second.begin())
    	OF << "  // Collects operations from ";
      OF << sfgs[*i4] << " ";
    }
    OF << "\n";
    
    generate_sfg_ops(OF, 1 /*nesting level*/, i3->first);
    OF << "}\n";
    
    if (nodep_sfg.find(i3->first) != nodep_sfg.end()) {
      OF << "\n";
      OF << "void " << modulename << "::out_sfg_" << i3->first << "() {\n";
      generate_sfg_nodep_ops(OF, 1 /*nesting level*/, i3->first);
      OF << "}\n";
    }

  }
}

void icgen::generate_update(ostream &OF) {

  OF << "void " << modulename << "::update() {\n";

  symkind_it i1;
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == reg_t) {
      OF << "  " << symname(i1->first) << "->valuecopy(";
      OF << "*" << symname(i1->first) << "_next); ";
      OF << "// " << lexname(i1->first) << "\n"; 
    }
  }
  OF << "\n";

  OF << "}\n";
}

void icgen::collapse_sfg( ) {
  // collapse all sfgs in cstep into one by creating new entries
  // in expressions

  for ( map <symid, vector<symid> >::iterator q = stepsfg.begin();
	q != stepsfg.end();
	q++) 
    for (vector<symid>::iterator i = q->second.begin();
	 i != q->second.end();
	 i++) {
      for (vector<symid>::iterator j = expressions[*i].begin();
	   j != expressions[*i].end();
	   j++) {
	expressions[q->first].push_back(*j);
      }
    }

}

void icgen::insert_hierarchy_ops() {
  // in each (collapsed) sfg, we insert one 'op_ipblock' operation
  // which will be sorted with other expressions to obtain proper
  // leveling of the execution schedule.
  //
  // given an ipblock wich connects inputs to signals {i}
  // and outputs to signals {o}, then the 'operation' of this
  // op_ipblock corresponds to:
  //
  // a/ copy values of {i} to corresponding ioval[]
  // b/ execute run()
  // c/ copy values of correponding ioval[] to outputs {o}
  //
  // The operation is executed in the context of 'run' of the
  // parent block. 
  //
  // Since the declaration of an ipblock does not distinguish
  // between independent outputs and normal outputs, we have to include
  // all outputs in the output set {o} (which may cause some overhead)
  //
  // Note that the collapsed sfg id are the same as the control step id

  for ( map <symid, vector<symid> >::iterator q = stepsfg.begin();
	q != stepsfg.end();
	q++) 
    for ( vector <symid>::iterator i = opip.begin();
	  i != opip.end();
	  i++) {
      expressions[q->first].push_back(*i);
    }

}

// replicated code from ipblock
// todo - find a way to call aipblock::matchparm directly
static char * matchparm_icgen(char *orig, const char *tgt) {
  // find first char of tgt in orig
  while (((unsigned char) (*tgt)) != 0) {
    while (*orig && (*orig != *tgt))
      orig++;
    if (*orig)
      tgt++;
    else
      return 0;
  }
  // complete match found - look for '='
  while (*orig && (*orig != '='))
    orig++;
  ++orig; // skip '='
  // strip additional ws
  while (*orig && ((*orig == ' ') || (*orig == '\t')))
    orig++;
  if (*orig == 0) {
    return 0;
  }
  return orig;
}

static bool matchparm_icgen(char *orig, const char *tgt, gval &v) {
  char *val = matchparm_icgen(orig, tgt);
  if (val != 0) {
    gval *v2 = make_gval(val);
    v = *v2;
    return true;
  }
  return false;
}

void icgen::find_hierarchy_regout() {
  // Besides opip_pin, and opip_input, a third map is
  // created to indicated which output of an included
  // ipblock is independent.
  // This map is called opip_regout. Dependencies on
  // opip_regout are not propagated when leveling
  // the sfg.
  // The selection of regout outputs is done through the
  // parameter set given for the included ipblock (regOut parm)

  // loop over all ipblock
  for ( vector <symid>::iterator i = opip.begin();
	i != opip.end();
	i++) {

    // set all regout to false
    for (unsigned j = 0; j < opip_input[*i].size(); j++)
      opip_regout[*i].push_back(false);

    // check parameters for this ipblock and extract the 'regOut'
    // parameters. set the regout to true for those ports
    symid p1 = glbSymboltable.findipparm(*i, NOSYMBOL);
    while (p1 != NOSYMBOL) {
      char *parm = glbSymboltable.getsymbol(p1)->content()->getname();
      gval *v = make_gval(32,0);
      if (matchparm_icgen(parm, "regOut", *v)) 
	opip_regout[*i][v->toulong()] = true;
      p1 = glbSymboltable.findipparm(*i, p1);
    }

  }
}


void icgen::generate_action(ostream &OF, int nesting, symid s) {

  // only generate function call for non-empty action
  if (expressions.find(s) != expressions.end()) {
    generate_indent(OF, nesting);
    OF << "sfg_" << s << "();\n";
  }
  generate_indent(OF, nesting);
  OF << "cstate = " << states[steptgt[s]] << ";\n";
  generate_indent(OF, nesting);
  OF << "break;\n";
}

void icgen::generate_out_action(ostream &OF, int nesting, symid s) {

  // similar to generate_action, but will only call sfg that update
  // independent outputs (using out_sfg_name() calls)
  // Such sfg will be placed, by init_odeps(), in the set nodep_sfg
  
  if (nodep_sfg.find(s) != nodep_sfg.end()) {
    // independent_output sfg exists
    generate_indent(OF, nesting);
    OF << "out_sfg_" << s << "();\n";
  }

  generate_indent(OF, nesting);
  OF << "break;\n";
}

void icgen::generate_condition_scope(ostream &OF, 
				     generate_action_t ga, 
				     int nesting, 
				     symid scope,
				     vector<symid> &src) {
  map<symid, int> scopelist;

  scopelist.clear();

  // print conditional expressions which have this scope
  //  generate_sfg_ops(OF, nesting, scope);

  // first, see if there is a direct match for the scope
  for (vector<symid>::iterator i = src.begin();
       i != src.end();
       i++) {
    if (stepcond[*i] == scope) {
      (*this.*ga)(OF, nesting, *i);    
      return;
    } else 
      scopelist[stepcond[*i]] = 1; // we abuse a map as a set here
  }

  // now, recursively add all parents of conditions in scopelist
  // until the condition tree for this source state is complete
  bool grow = true;
  while (grow) {
    grow = false;
    for (map<symid, int>::iterator i2 = scopelist.begin();
	 i2 != scopelist.end();
	 i2++) {
      if (i2->first == -1) // root scope
	continue;
      if (scopelist.find(condparent[i2->first]) == scopelist.end()) {
	scopelist[condparent[i2->first]] = 1;
	grow = true;
      }
    }
  }

  // no matching scope found, look down one level in the scopelist
  // and locate a true and false branch, and a condition
  symid c1 = NOSYMBOL, t1 = NOSYMBOL, f1 = NOSYMBOL;

  for (map<symid, int>::iterator i6 = scopelist.begin();
       i6 != scopelist.end();
       i6++) {
    if (scope == condparent[i6->first]) {
      // found a child underneath this scope
      c1 = condvar[i6->first];
      if (condtrue[i6->first]) 
	t1 = i6->first;
      else
	f1 = i6->first;
    }
  }

  generate_indent(OF, nesting);

  OF << "if (" << symname(c1) << "->isnz()) {\n";

  generate_condition_scope(OF, ga, nesting+1, t1, src);

  generate_indent(OF, nesting);
  OF << "} else {\n";

  generate_condition_scope(OF, ga, nesting+1, f1, src);

  generate_indent(OF, nesting);
  OF << "}\n";

}

void icgen::generate_source_state(ostream &OF, generate_action_t ga, symid thestate) {
  OF << "  case " << states[thestate] << ":\n";
  generate_condition_scope(OF, ga, 2 /*nesting*/, -1 /* scope */, srcstep[thestate]);
}

void icgen::generate_run(ostream &OF) {

  // note that the run() of included ipblocks is merged into the sfgs
  // that make up run (to ensure proper data dependencies)

  OF << "void " << modulename << "::run() {\n";
  OF << "\n";
  OF << "  switch(cstate) {\n";

  for (map<symid, vector<symid> >::iterator i4 = srcstep.begin();
       i4 != srcstep.end();
       i4++) {
    generate_source_state(OF, &icgen::generate_action, i4->first);
  }

  OF << "  }\n";
  OF << "  update();\n";
  if (needscyclectr) {
    OF << "  cycle++;\n";
  }
  OF << "}\n";

}

void icgen::generate_out_run(ostream &OF) {
  // like run, but will call sfg that update independent outputs
  // (as analyzed using init_odeps())
  // this function will be called at the start of the clock cycle,
  // before run(), and does not call update()

  OF << "void " << modulename << "::out_run() {\n";
  OF << "\n";

  // conditional expressions for fsm (in sfgid -1 in expressions[])
  generate_ctlconditions(OF, 1);

  // call out_run of included ipblocks
  for ( vector <symid>::iterator i = opip.begin();
	i != opip.end();
	i++) {
    OF << "  " << symname(*i) << "->out_run();\n";
    // copy registered outputs to connection signals
    vector<symid>::iterator i1;
    vector<bool>::iterator  i2;
    int                     i3;
    vector<bool>::iterator  i4;
    for (i1 = opip_pin[*i].begin(), 
	   i2 = opip_input[*i].begin(), 
	   i3 = 0,
	   i4 = opip_regout[*i].begin();
	 i1 != opip_pin[*i].end();
	 i1++, i2++, i3++, i4++) {
      if (! *i2) { // is an output
	if (  *i4) { // is a registered output
	  OF << "  " << symname(*i1)  << "->valuecopy(";
	  OF << rhsname(*i) << "->ioval[" << i3 << "]);\n";
	}
      }
    }
    OF << "\n";
  }

  OF << "  switch(cstate) {\n";

  for (map<symid, vector<symid> >::iterator i4 = srcstep.begin();
       i4 != srcstep.end();
       i4++) {
    generate_source_state(OF, &icgen::generate_out_action, i4->first);
  }

  OF << "  }\n";

  OF << "}\n";

}

void icgen::generate_constructor(ostream &OF) {

  OF << modulename << "::" << modulename << "(char *name) : aipblock(name) {\n";

  OF << "  // registers\n";
  symkind_it i1;
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == reg_t) {
      OF << "  " << symname(i1->first) << "      = make_gval(";
      OF << symtype[i1->first]->getwordlength() << ",";
      OF << symtype[i1->first]->getsign()       << "); ";
      OF << "// " << lexname(i1->first) << "\n"; 

      OF << "  " << symname(i1->first) << "_next = make_gval(";
      OF << symtype[i1->first]->getwordlength() << ",";
      OF << symtype[i1->first]->getsign()       << ");\n";
    }
  }
  OF << "\n";

  OF << "  // signal + constant\n";
  OF << "  gval_zero = make_gval(\"0\");\n";
  OF << "  gval_one  = make_gval(\"1\");\n";
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == sig_t) {
      OF << "  " << symname(i1->first) << " = make_gval(";
      OF << symtype[i1->first]->getwordlength() << ",";
      OF << symtype[i1->first]->getsign()       << "); ";
      OF << "// " << lexname(i1->first) << "\n"; 
    } else if (i1->second == const_t) {
      OF << "  " << symname(i1->first) << " = make_gval(";
      OF << "\"" << lexname(i1->first) << "\");\n"; 
    }
  }
  OF << "\n";

  OF << "  // lookup tables \n";
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == lut_t) {
      for (vector<char *>::iterator lu_el = ludata[i1->first].begin();
	   lu_el != ludata[i1->first].end();
	   lu_el++) {
	OF << "  " << symname(i1->first);
	OF << ".push_back(make_gval(";
	OF << symtype[i1->first]->getwordlength() << ",";
	OF << symtype[i1->first]->getsign() << ",";
	OF << "\"" << *lu_el << "\"));\n";
      }
    }
  }
  OF << "\n";
  
  OF << "  // operations\n";
  for (opkind_it i2 = opkind.begin(); 
       i2 != opkind.end(); 
       i2++) {
    if ((i2->second != op_assign) && 
	(i2->second != op_ipblock) &&
	(i2->second != op_disp)) {
      OF << "  " << symname(i2->first) << " = make_gval(";
      if (symtype.find(i2->first) == symtype.end()) {
	OF << "no type info);\n";
      } else {
	OF << symtype[i2->first]->getwordlength() << ",";
	OF << symtype[i2->first]->getsign()       << ");\n";
      }
    }
  }
  OF << "\n";
  OF << "  cstate = " << states[initialstate] << ";\n";
  OF << "\n";

  if (needscyclectr) {
    OF << "  cycle = 0;\n";
    OF << "\n";
  }

  // create included (internal) ipblocks
  // call out_run of included ipblocks
  for ( vector <symid>::iterator i3 = opip.begin();
	i3 != opip.end();
	i3++) {
       OF << "  " << symname(*i3);
       OF << " = glbIpblockcreate(\"" << symname(*i3);
       OF << "\", \"" << opip_typename[*i3] << "\");\n";

    // create ioval[] for this ipblock
    // use type info of connected parent signals
    int i32;
    vector<symid>::iterator i31;
    for (i32 = 0, i31 = opip_pin[*i3].begin();
	 i31 != opip_pin[*i3].end();
	 i32++, i31++) {
      int w = glbSymboltable.getsymbol(glbSymboltable.findtype(*i31))->content()->typew();
      int s = glbSymboltable.getsymbol(glbSymboltable.findtype(*i31))->content()->typesign();
      OF << "  " << symname(*i3); 
      OF << "->ioval.push_back(make_gval(" << w << "," << s << "));\n";
    }
    
    // set parameters on ipblocks
    symid p1 = glbSymboltable.findipparm(*i3, NOSYMBOL);
    while (p1 != NOSYMBOL) {
      OF << "  " << symname(*i3);
      OF << "->setparm(\"";
      OF << glbSymboltable.getsymbol(p1)->content()->getname() << "\");\n";
      p1 = glbSymboltable.findipparm(*i3, p1);
    }

  }

  OF << "}\n";
}

void icgen::generate_cxx() {
  string fname = string(modulename) + ".cxx";
  ofstream OF(fname.c_str());

  OF << "// Incremental GEZEL Compiler - Automatically generated file\n";
  OF << "// GEZEL Source Module: " << modulename << "\n";
  OF << "\n";
  OF << "#include \"" << modulename << ".h\"";
  OF << "\n";
  OF << "extern \"C\" aipblock *create_" << modulename << "(char *instname) {\n";
  OF << "  return new " << modulename << "(instname);\n";
  OF << "}\n";
  OF << "\n";

  generate_constructor(OF);
  OF << "\n";
  generate_setparm(OF);
  OF << "\n";
  generate_sfgs(OF);
  OF << "\n";
  generate_update(OF);
  OF << "\n";
  generate_run(OF);
  OF << "\n";
  generate_out_run(OF);
  OF << "\n";
  generate_checkterminal(OF);

  OF.close();
}

void icgen::generate_ipblock() {
  string fname = string(modulename) + ".fdl";
  ofstream OF(fname.c_str());

  OF << "// Incremental GEZEL Compiler - Automatically generated ipblock file\n";
  OF << "// GEZEL Source Module: " << modulename << "\n";

  OF << "ipblock " << modulename;

  for (map<symid, char *>::iterator i = portnames.begin();
       i != portnames.end();
       i++) {

    if (i != portnames.begin())
      OF << ";\n";
    else
      OF << "(\n";
    if (symkind[i->first] == inp_t) 
      OF << " in " << i->second << " : ";
    else
      OF << " out " << i->second << " : ";
    if (symtype[i->first]->getsign()) 
      OF << "tc(";
    else
      OF << "ns(";
    OF << symtype[i->first]->getwordlength();
    OF << ")";
  }
  if (portnames.size() > 0)
    OF << ") ";
  OF << " {\n";
  OF << "  iptype \"" << modulename << "\";\n";

  // flow for the independent port parameter
  //
  //   orig fdl file
  //       --> icg --> cxx file + ipblock file
  //
  // The ipblock file indicates the registered output ports
  // with portnumbers in the 'regOut' parameter
  // The cxx file calls 'regOutput' with argument set by the
  // ipblock file
  //
  // This scheme also helps to cope with hierarchy:
  //
  // ipblock file (with regOut)
  //      --> icg --> ...
  //
  // When icg reads an ipblock with hierarchically included ipblock
  // file, registered ports are marked.
  // Then, the dependencies on this included ipblock can be
  // checked correctly:
  //
  //          inputs --> run()     --> outputs
  //                     out_run() --> registered_outputs
  //

  // output independent ports as parameters
  for (odeps_t::iterator i4 = nodeps.begin();
       i4 != nodeps.end();
       i4++) {
    OF << "  ipparm \"regOut=" << portnumbers[i4->first] << "\";\n";
  }

  OF << "}\n";
  OF.close();
}

void icgen::generate_header() {
  string fname = string(modulename) + ".h";
  ofstream OF(fname.c_str());
  
  OF << "// Incremental GEZEL Compiler - Automatically generated header file\n";
  OF << "// GEZEL Source Module: " << modulename << "\n";
  OF << "\n";
  OF << "#ifndef " << modulename << "_h\n";
  OF << "#define " << modulename << "_h\n";
  OF << "\n";
  OF << "#include \"ipblock.h\"\n";
  OF << "\n";
  OF << "extern \"C\" aipblock *create_" << modulename << "(char *instname);\n";
  OF << "\n";
  OF << "class " << modulename << " : public aipblock {\n";
  OF << "\n";
  
  // list registers
  OF << "  // registers\n";
  symkind_it i1;
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == reg_t) {
      OF << "  gval *" << symname(i1->first) << ", ";
      OF << "*" << symname(i1->first) << "_next; ";
      OF << "// " << lexname(i1->first) << "\n"; 
    }
  }
  OF << "\n";

  OF << "  // signal + constant\n";
  OF << "  gval *gval_zero, *gval_one;\n";
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if ((i1->second == sig_t) || (i1->second == const_t)) {
      OF << "  gval *" << symname(i1->first) << "; ";
      OF << "// " << lexname(i1->first) << "\n"; 
    }
  }
  OF << "\n";

  OF << "  // operations\n";
  for (opkind_it i2 = opkind.begin(); 
       i2 != opkind.end(); 
       i2++) {
    if ((i2->second != op_assign) && 
	(i2->second != op_ipblock) &&
	(i2->second != op_disp))
      OF << "  gval *" << symname(i2->first) << ";\n";
  }
  OF << "\n";

  OF << "  // included ipblocks\n";
  for (vector<symid>::iterator i22 = opip.begin();
       i22 != opip.end();
       i22++) {
    //    OF << "  " << opip_typename[*i22];
    OF << " aipblock *" << symname(*i22) << ";\n";
  }
  OF << "\n";

  OF << "  // lookup tables\n";
  for (i1 = symkind.begin(); i1 != symkind.end(); i1++) {
    if (i1->second == lut_t) {
      OF << "  vector<gval *> " << symname(i1->first) << "; ";
      OF << "// " << lexname(i1->first) << "\n";
    }
  }  

  // list sfg - there is one sfg per control step
  // this sfg is created with 'collapse_sfg()'
  // as the combination of all sfg that belong to a single
  // control step
  for (stepsfg_it i3 = stepsfg.begin();
       i3 != stepsfg.end();
       i3++) {
    OF << "  void sfg_" << i3->first << "();";
    for (vector<symid>::iterator i4 = i3->second.begin();
	 i4 != i3->second.end();
	 i4++) {
      if (i4 == i3->second.begin())
	OF << " // ";
      OF << sfgs[*i4] << " ";
    }
    OF << "\n";
    if (nodep_sfg.find(i3->first) != nodep_sfg.end()) {
      // this sfg contains independent output operations
      OF << "  void out_sfg_" << i3->first << "();\n";
    }
  }


  // other function
  OF << "  void update();\n";
  OF << "\n";

  // list of states
  OF << "  enum {";
  for (states_it i4 = states.begin(); 
       i4 != states.end();
       i4++) {
    if (i4 != states.begin()) 
      OF << ", ";
    OF << i4->second;
  }
  OF << "  };\n";
  OF << "  unsigned cstate;\n";

  if (needscyclectr) {
    OF << "\n";
    OF << "  unsigned long cycle;\n";
  }
  
  OF << "\n";
  OF << " public:\n";
  OF << "  " << modulename << "(char *name);\n";
  OF << "  void setparm(char *);\n";
  OF << "  void run();\n";
  OF << "  void out_run();\n";
  OF << "  bool checkterminal(int n, char *tname, aipblock::iodir d);\n";
  OF << "  bool cannotSleepTest();\n";
  OF << "  bool needsWakeupTest();\n";
  OF << "\n";
  OF << "};\n";
  OF << "\n";
  OF << "#endif\n";
  OF.close();
}

void icgen::debug_resolve_hierarchy() {

  for (vector<symid>::iterator i = opip.begin();
       i != opip.end();
       i++) {
    cerr << "Included ipblock " << *i;
    cerr << " Type: ";
    cerr << glbSymboltable.getsymbol(glbSymboltable.findiptype(*i))->content()->getname() << "\n";

    cerr << " Connections:\n";
    vector<bool>::iterator j1;
    vector<symid>::iterator j;
    for ( j1 = opip_input[*i].begin(), j = opip_pin[*i].begin();
	 j != opip_pin[*i].end();
	 j++, j1++) {
      cerr << "    " << lexname(*j);
      if (*j1) 
	cerr << " input\n";
      else
	cerr << " output\n";
    }

    symid p1 = glbSymboltable.findipparm(*i, NOSYMBOL);
    while (p1 != NOSYMBOL) {
      cerr << " Parameter: " << glbSymboltable.getsymbol(p1)->content()->getname() << "\n";
      p1 = glbSymboltable.findipparm(*i, p1);
    }

  }

}

void icgen::debug_print_action(int nesting, symid s) {

  for (int p = 0; p < nesting; p++) cerr << "  ";
  cerr << "execute ";
  for (vector<symid>::iterator i = stepsfg[s].begin();
       i != stepsfg[s].end();
       i++) {
    cerr << *i << " ";
  }
  cerr << "\n";
}

void icgen::debug_print_condition_scope(int nesting, symid scope, vector<symid> &src) {
  map<symid, int> scopelist;

  scopelist.clear();

  // print conditional expressions which have this scope
  generate_sfg_ops(cerr, nesting, scope);

  // first, see if there is a direct match for the scope
  for (vector<symid>::iterator i = src.begin();
       i != src.end();
       i++) {
    if (stepcond[*i] == scope) {
      debug_print_action(nesting, *i);    
      return;
    } else 
      scopelist[stepcond[*i]] = 1; // we abuse a map as a set here
  }

  // now, recursively add all parents of conditions in scopelist
  // until the condition tree for this source state is complete
  bool grow = true;
  while (grow) {
    grow = false;
    for (map<symid, int>::iterator i2 = scopelist.begin();
	 i2 != scopelist.end();
	 i2++) {
      if (i2->first == -1) // root scope
	continue;
      if (scopelist.find(condparent[i2->first]) == scopelist.end()) {
	scopelist[condparent[i2->first]] = 1;
	grow = true;
      }
    }
  }

  // no matching scope found, look down one level in the scopelist
  // and locate a true and false branch, and a condition
  symid c1 = NOSYMBOL, t1 = NOSYMBOL, f1 = NOSYMBOL;

  for (map<symid, int>::iterator i6 = scopelist.begin();
       i6 != scopelist.end();
       i6++) {
    if (scope == condparent[i6->first]) {
      // found a child underneath this scope
      c1 = condvar[i6->first];
      if (condtrue[i6->first]) 
	t1 = i6->first;
      else
	f1 = i6->first;
    }
  }

  generate_indent(cerr, nesting);
  cerr << "if (" << c1 << " ) {\n";

  debug_print_condition_scope(nesting+1, t1, src);

  generate_indent(cerr, nesting);
  cerr << "} else {\n";

  debug_print_condition_scope(nesting+1, f1, src);

  generate_indent(cerr, nesting);
  cerr << "}\n";

}

void icgen::debug_print_source_state(symid q) {
  // list of steps to consider in: srcstep[symid];
  cerr << "case " << q << ":\n";
  debug_print_condition_scope(1 /*nesting*/, -1 /*scope*/, srcstep[q]);
}

void icgen::collect_dependson(vector<symid> allops, symid op, set<symid> &deps) {
  for (vector<symid>::iterator i = allops.begin();
       i != allops.end();
       i++) {
    if ((*i != op) && dependson(*i, op)) {
      if (deps.find(*i) == deps.end()) {
	deps.insert(*i);
	collect_dependson(allops, *i, deps);
      }
    }
  }
}

void icgen::init_odeps() {

  // first, construct output dependencies
  for (stepsfg_it i1 = stepsfg.begin();
       i1 != stepsfg.end();
       i1++) {
    for (map<symid, char *>::iterator i3 = portnames.begin();
	 i3 != portnames.end();
	 i3++) {
      if (symkind[i3->first] == out_t) {
	set<symid> deps;
	collect_dependson(expressions[i1->first], i3->first, deps);
	for (set<symid>::iterator i4 = deps.begin();
	     i4 != deps.end();
	     i4++) {
	  odeps[i3->first /*outp*/][i1->first /*sfg*/].insert(*i4);
	}
      } // if
    } // for all_out
  } // for all_sfg

  // next, find independent outputs
  // use odeps and select those output/sfg that have no dependency to
  // an input

  // loop over all outputs
  for (odeps_t::iterator i = odeps.begin();
       i != odeps.end();
       i++) {
    bool combdep = false;
    // loop over all sfg for this output
    for (map<symid, set<symid> >::iterator j = i->second.begin();
	 j != i->second.end();
	 j++) {
      // loop over all ops for this sfg/output
      for (set<symid>::iterator k = j->second.begin();
	   k != j->second.end();
	   k++) {
	// loop over all inputs
	for (map<symid, char *>::iterator l = portnames.begin();
	     l != portnames.end();
	     l++) {
	  if (dependson(l->first, *k)) {
	    combdep = true;
	  }
	}
      }
    }
    if (combdep == false) {
      nodeps[i->first] = odeps[i->first];

      // save this set of sfgs in an additional member 'nodep_sfg'
      for (map<symid, set<symid> >::iterator j = i->second.begin();
	   j != i->second.end();
	   j++) {
	nodep_sfg.insert(j->first);
      }
    }
  }

}

void icgen::debug_print_output_deps() {
  for (sfgs_it i1 = sfgs.begin();
       i1 != sfgs.end();
       i1++) {
    cerr << "Output dependencies for " << i1->second << "\n";
    // list operations in this sfg
    cerr << "Ops ";
    for (vector<symid>::iterator i2 = expressions[i1->first].begin();
	 i2 != expressions[i1->first].end();
	 i2++) {
      cerr << *i2 << " ";
    }
    cerr << "\n";
   
    // loop over all outputs
    for (map<symid, char *>::iterator i3 = portnames.begin();
	 i3 != portnames.end();
	 i3++) {
      if (odeps.find(i3->first) != odeps.end())
	if (odeps[i3->first].find(i1->first) != odeps[i3->first].end()) {
	  cerr << "Output " << i3->first << " Sfg " << i1->first << ":";
	  for (set<symid>::iterator i4 = odeps[i3->first][i1->first].begin();
	       i4 != odeps[i3->first][i1->first].end();
	       i4++) {
	    cerr << *i4 << " ";
	  }
	  cerr << "\n";
	} // if
    } // for all_out
  } // for all_sfg

  cerr << "Independent outputs : ";
  for (odeps_t::iterator i4 = nodeps.begin();
       i4 != nodeps.end();
       i4++) {
    cerr << portnames[i4->first] << " ";
  }
  cerr << "\n";
}

void icgen::debug_print() {

  for (map<symid, gval_t>::iterator i = symkind.begin();
       i != symkind.end();
       i++) {
    cerr << "   "; cerr.width(4); cerr << (i->first) << " ";
    cerr.width(8);
    switch (i->second) {
    case sig_t:
      cerr << "sig   ";
      break;
    case const_t:
      cerr << "const ";
      break;
    case reg_t:
      cerr << "reg   ";
      break;
    case inp_t:
      cerr << "input ";
      break;
    case out_t:
      cerr << "output";
      break;
    case lut_t:
      cerr << "lookup";
      break;
    }
    cerr << " ";
    cerr.width(12); 
    cerr << glbSymboltable.getsymbol(i->first)->content()->getname() << " ";
    if (symtype.find(i->first) != symtype.end()) {
      cerr.width(4);  cerr << symtype[i->first]->getwordlength();
      cerr.width(4);  cerr << symtype[i->first]->getsign();
    }    
    cerr << "\n";
  }

  cerr << sfgs.size() << " sfg symbols in module:\n";
  for (sfgs_it j = sfgs.begin();
       j != sfgs.end();
       j++) {
    cerr.width(15); cerr << j->second << "\n";
  }

  cerr << stepcond.size() << " control steps\n";
  for (map<symid, symid>::iterator i2 = stepcond.begin();
       i2 != stepcond.end();
       i2++) {
    cerr << "    step "; 
    cerr.width(4); cerr << i2->first;
    cerr << " cvar ";
    cerr.width(4); cerr << i2->second;
    cerr << " tgt  ";
    cerr.width(4); cerr << steptgt[i2->first];
    cerr << " sfg ";
    for (vector<symid>::iterator i3 = stepsfg[i2->first].begin();
 	 i3 != stepsfg[i2->first].end();
 	 i3++) {
      cerr.width(4); cerr << *i3;
      cerr << " ";
    }
    cerr << "\n";
  }
  
  cerr << "Source state set\n";
  for (map<symid, vector<symid> >::iterator i4 = srcstep.begin();
       i4 != srcstep.end();
       i4++) {
    cerr << "   from "; cerr.width(4); cerr << i4->first << ":";
    for (vector<symid>::iterator i5 = i4->second.begin();
	 i5 != i4->second.end();
	 i5++) {
      cerr.width(4); cerr << *i5;
    }
    cerr << "\n";
  }

  cerr << "Condition tree\n";
  for (map<symid, symid>::iterator i6 = condvar.begin();
       i6 != condvar.end();
       i6++) {
    cerr.width(4); cerr << i6->first; cerr << " ";
    cerr.width(4); cerr << i6->second; cerr << " ";
    cerr.width(4); cerr << condtrue[i6->first]; cerr << " ";
    cerr.width(4); cerr << condparent[i6->first]; cerr << "\n";
  }
  cerr << "Composed execution tree\n";
  for (map<symid, vector<symid> >::iterator i4 = srcstep.begin();
       i4 != srcstep.end();
       i4++) {
    debug_print_source_state(i4->first);
  }

  debug_print_output_deps();
}

void icgen::generate() {
  cerr << "Converting FSMD " << modulename << ": ";
  cerr << symkind.size() << " symbols, ";
  cerr << sfgs.size()    << " sfg, ";
  cerr << stepcond.size() << " csteps.\n";

  if (symkind.size() == 0) {
    cerr << "Empty module - not code generated\n";
    return;
  }

  //  debug_resolve_hierarchy();

  collapse_sfg         ();
  insert_hierarchy_ops ();
  find_hierarchy_regout();
  init_odeps           ();
  partition_expressions();

  generate_header ();
  generate_cxx    ();
  generate_ipblock();
}

void icgen::cgtracer     (symid ctl) {}
void icgen::cgtracesig   (symid v, symid thesig, char *filename) {}
void icgen::cgfile       (symid v, char *filename) {}
void icgen::cgoption     (symid v, char *option) {}
void icgen::cgcon        (symid pin1, symid pin2) {}
void icgen::cgfinish     (symid disp, symid sfg, symid dp) {}

void icgen::cgdisp       (symid disp, symid sfg, symid dp) {
  opkind[disp] = op_disp;
  expressions[currentsfg].push_back(disp);
}

void icgen::cgdispvar    (symid disp, symid var) {
  displaytype[dispctr] = dispvar;
  displayvar[dispctr]  = var;
  displaymap[disp].push_back(dispctr++);    
}

void icgen::cgdispdp     (symid disp) {
  displaytype[dispctr] = dispdp;
  displaymap[disp].push_back(dispctr++);
}

void icgen::cgdispstr    (symid disp, char *str) {
  displaytype[dispctr] = dispstr;
  displaystr[dispctr]  = str;
  displaymap[disp].push_back(dispctr++);
}

void icgen::cgdispcycle  (symid disp) {
  displaytype[dispctr] = dispcycle;
  displaymap[disp].push_back(dispctr++);
  needscyclectr = true;
}

void icgen::cgdispbase   (symid disp, int v) {
  displaytype[dispctr] = dispbase;
  displaybase[dispctr]  = v;
  displaymap[disp].push_back(dispctr++);
}

void icgen::cghwire_ctl  (symid v, symid dp) {
  controller   = v;
  ctlstep      = v;      // generate a single (artifical) control step
  states[v]    = (char *) "s0";   // from a single, initial state s0
  initialstate = v;
  stepcond[v]  = -1;     // going unconditionally
  steptgt[v]   = v;      // to the same state s0
  srcstep[v].push_back(ctlstep); // attach all sfg under hardwired to this transition
}

void icgen::cgseq_ctl    (symid v, symid dp) {
  cerr << "icgen::sequencer construct is not supported in current implementation\n";
  exit(0);
}

void icgen::cgctlstep    (symid v, symid ctl, int n, symid condition) {
  ctlstep = v;
  stepcond[v] = condition;
}

void icgen::cgexec       (symid ctl, symid dp, symid sfg) {
   stepsfg[ctlstep].push_back(sfg);
}

void icgen::cgfsm_ctl    (symid v, symid dp) {
   controller  = v;
}

void icgen::cgfsm_state  (symid v, symid c, char *str) {
   states[v] = str;
}

void icgen::cgfsm_initial(symid v, symid c, char *str) {
  states[v]     = str;
  initialstate = v;
}

void icgen::cgfsm_trans  (symid v, symid c, symid from, symid to) {
  srcstep[from].push_back(ctlstep);
  steptgt[ctlstep] = to;
}

void icgen::cgctltrue    (symid v, symid c, symid e, symid p) {
  currentsfg = -1; // will collect all conditional expr in global ctl scope
                   // inefficient but correct
  condvar[v]    = e;
  condtrue[v]   = true;
  condparent[v] = p;
}

void icgen::cgctlfalse   (symid v, symid c, symid e, symid p) {
  currentsfg = -1; // will collect all conditional expr in global ctl scope
                   // inefficient but correct
  condvar[v]    = e;
  condtrue[v]   = false;
  condparent[v] = p;
}

void icgen::cgsysdp      (symid v) {}

// for hierarchical blocks ..
void icgen::cgdpc_arg    (symid parentdp, symid parentpin, symid childdp, symid childpin) {
  opip_pin[opip.back()].push_back(parentpin);
  if (glbSymboltable.getsymbol(childpin)->istype(symbol::dpinput))
    opip_input[opip.back()].push_back(true);
  else
    opip_input[opip.back()].push_back(false);
}

void icgen::cgdpc_dp     (symid v, symid parentdp) {
  cerr << "iccgen: Cannot convert hierarchically included datapaths\n";
  cerr << "iccgen: First, convert lower level datapaths to ipblock\n";
  exit(0);
}

void icgen::cgdpc_ipblock(symid v, symid parentdp) {
  opkind[v] = op_ipblock;
  opip.push_back(v);
  opip_typename[v] = glbSymboltable.getsymbol(glbSymboltable.findiptype(v))->content()->getname();

  // do not insert op_ipblock here, but wait until
  // collapsed sfg are available and then insert and op_ipblock
  // operation in each sfg (this is a hierarchilcally included ipblock
  // that must be run each clock cycle, thus in each sfg)

}
     
// SFG/ SIGNAL
void icgen::cgdp         (symid v, char *name) {
  modulename = name;
}

void icgen::cgdpclone    (symid v, char *n, symid p) {
  cerr << "iccgen: Cannot convert a cloned datapath\n";
  exit(0);
}

void icgen::cgipblock    (symid v, char * instancename, char *) {
  cerr << "iccgen: Cannot convert an ipblock\n";
  exit(0);
}

void icgen::cgipout      (symid v, int wlen, int sign, symid _dp, char *portname) {}

void icgen::cgipparm     (symid v, char *parmname, symid _dp) {}

void icgen::cgsfg        (symid v, char *n) {
  sfgs[v]     = n;
  currentsfg  = v;  // for adding expressions[currentsfg]
}

void icgen::cginput      (symid v, int wlen, int sign, symid _dp, char *_portname) {
  symkind[v]  = inp_t;
  symtype[v]  = make_gval(wlen, sign);
  portnumbers[v]  = portnumber++;
  portnames[v] = _portname;
}

void icgen::cgoutput     (symid v, int wlen, int sign, symid _dp, char *_portname) {
  symkind[v]  = out_t;
  symtype[v]  = make_gval(wlen, sign);
  portnumbers[v]  = portnumber++;
  portnames[v] = _portname;
}

void icgen::cgreg        (symid v, int wlen, int sign, symid _dp, char *) {
  symkind[v]  = reg_t;
  symtype[v]  = make_gval(wlen, sign);
}

void icgen::cgsig        (symid v, int wlen, int sign, symid _dp, char *) {
  symkind[v]  = sig_t;
  symtype[v]  = make_gval(wlen, sign);
}

void icgen::cgconst      (symid v, char *str) {
  // if context of this constant is lookup table, do not store it
  if (symkind.find(glbSymboltable.getsymbol(v)->context()) != symkind.end()) {
    if (symkind[glbSymboltable.getsymbol(v)->context()] == lut_t)
      return;
  }

  symkind[v]  = const_t;
  symtype[v]  = make_gval(str);
}

int icgen::wlensym(symid N) {
  if (symtype.find(N) != symtype.end()) {
    return symtype[N]->getwordlength();
  } else {
    cerr << "icgen: wlensym: Symbol " << N << " has undefined length\n";
    return 0;
  }
}

int icgen::signsym(symid N) {
  if (symtype.find(N) != symtype.end()) {
    return symtype[N]->getsign();
  } else {
    cerr << "icgen: wlensym: Symbol " << N << " has undefined length\n";
    return 0;
  }
}

//-----------------------------
// assign does not need type info (does not exist as a gval in C)
void icgen::cgassign     (symid v, symid sfg, symid lhs, symid rhs) {
  opkind[v]   = op_assign;
  opleft[v]   = lhs;
  opright[v]  = rhs;
  expressions[currentsfg].push_back(v);
}

// type of cgindex is defined by ns(len)
void icgen::cgindex   (symid v, symid basevar, int idx, int len) {
  opkind[v]   = op_index;
  opleft[v]   = basevar;
  opindex[v]  = idx; 
  symtype[v]  = make_gval(len, 0); 
  expressions[currentsfg].push_back(v);
}

void icgen::cglutable (symid v, int wlen, int sign, vector<char *> &content, symid _dp, char *) {
  symkind[v] = lut_t;
  symtype[v] = make_gval(wlen, sign);
  ludata[v]  = content;
}

void icgen::cgluop    (symid v, symid L, symid table) {
  opkind[v]  = op_lu;
  opleft[v]  = L;
  symtype[v] = symtype[table];
  oplutable[v] = table;
  expressions[currentsfg].push_back(v);
}
     
// OPERATIONS
void icgen::cgadd     (symid v, symid L, symid R) {
  opkind[v]   = op_add;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgconcat  (symid v, symid L, symid R) {
  opkind[v]   = op_concat;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(wlensym(L) + wlensym(R), 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cgsub     (symid v, symid L, symid R) {
  opkind[v]   = op_sub;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgior     (symid v, symid L, symid R) {
  opkind[v]   = op_ior;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgxor     (symid v, symid L, symid R) {
  opkind[v]   = op_xor;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgand     (symid v, symid L, symid R) {
  opkind[v]   = op_and;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgshr     (symid v, symid L, symid R) {
  opkind[v]   = op_shr;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(wlensym(L), signsym(L));
  expressions[currentsfg].push_back(v);
}

void icgen::cgshl     (symid v, symid L, symid R) {
  opkind[v]   = op_shl;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(wlensym(L) + (1 << wlensym(R)), signsym(L));
  expressions[currentsfg].push_back(v);
}

void icgen::cgmul     (symid v, symid L, symid R) {
  opkind[v]   = op_mul;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(wlensym(L) + wlensym(R), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgmod     (symid v, symid L, symid R) {
  opkind[v]   = op_mod;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(max(wlensym(L), wlensym(R)), max(signsym(L), signsym(R)));
  expressions[currentsfg].push_back(v);
}

void icgen::cgeq      (symid v, symid L, symid R) {
  opkind[v]   = op_eq;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cgne      (symid v, symid L, symid R) {
  opkind[v]   = op_ne;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cggrt     (symid v, symid L, symid R) {
  opkind[v]   = op_grt;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cgsmt     (symid v, symid L, symid R) {
  opkind[v]   = op_smt;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cggrteq   (symid v, symid L, symid R) {
  opkind[v]   = op_grteq;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cgsmteq   (symid v, symid L, symid R) {
  opkind[v]   = op_smteq;
  opleft[v]   = L;
  opright[v]  = R;
  symtype[v]  = make_gval(1, 0);
  expressions[currentsfg].push_back(v);
}

void icgen::cgcast    (symid v, symid L, int wlen, int sign) {
  opkind[v]   = op_cast;
  opleft[v]   = L;
  symtype[v]  = make_gval(wlen, sign);
  expressions[currentsfg].push_back(v);
}

void icgen::cgnot     (symid v, symid L) {
  opkind[v]   = op_not;
  opleft[v]   = L;
  symtype[v]  = make_gval(wlensym(L), signsym(L));
  expressions[currentsfg].push_back(v);
}

void icgen::cgneg     (symid v, symid L) {
  opkind[v]   = op_neg;
  opleft[v]   = L;
  symtype[v]  = make_gval(wlensym(L), signsym(L));
  expressions[currentsfg].push_back(v);
}

void icgen::cgsel     (symid v, symid L, symid M, symid R) {
  opkind[v]   = op_sel;
  opleft[v]   = L;  // condition
  opmiddle[v] = M;  // true-leg
  opright[v]  = R;  // false-leg
  symtype[v]  = make_gval(max(wlensym(M), wlensym(R)), max(signsym(M), signsym(R)));
  expressions[currentsfg].push_back(v);
}
