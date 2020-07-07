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


#include "vgen2.h"
#include "rterror.h"


vgen::vgen() { sys = new vhdsys (); cur_step = NULL;}
vgen::~vgen() {
  for (map<symid,
	 vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); ++i)
    if ((i->second))
      delete ((i->second));
  for (map<symid,
	 vhdctl *>::iterator i = ctllist.begin(); i != ctllist.end(); ++i)
    if ((i->second))
      delete ((i->second));
}

void vgen::cghwire_ctl  (symid v,
			 symid dp) {
 vhdctl *ctl = new vhdhardwire ("hardwire");
 ctllist[v] = ctl;
 dplist[dp]->addctl(ctl);
 cur_dp = dp;
}

void vgen::cgseq_ctl    (symid v,
			 symid dp) { 
 vhdctl *ctl = new vhdsequencer ("sequencer");
 ctllist[v] = ctl;
 dplist[dp]->addctl(ctl);
 cur_dp = dp;
}

void vgen::cgctlstep    (symid v,
			 symid ctl,
			 int n,
			 symid condition) { 
  if (condition > 0) {
	cur_step->addcount(n);	
  }
  else
  {
        vhdctlstep* t = new vhdctlstep();
	cur_step = t;
	cur_step->addcount(n);	
  }
}

void vgen::cgexec       (symid ctl,
			 symid dp,
			 symid sfg) { 
  if(ctllist[ctl]->gettype() == "fsm") 
	cur_step->addsfgexe(sfg);  
  else if (ctllist[ctl]->gettype() == "sequencer") 
	cout <<"VHD ERROR: its sequencer, not supported yet"<<endl;
  else if (ctllist[ctl]->gettype() == "hardwire") {
	ctllist[ctl]->addsfgexe(sfg);
  }
  else
	cout <<"VHD ERROR: controller type not define"<<endl;
}

void vgen::cgfile       (symid v,
			 char *filename) { }
void vgen::cgcon        (symid pin1,
			 symid pin2) {
  sys->addnet(pin2,
	      pin1); //pin2 = out,  pin1 = in
}



void vgen::cgdisp       (symid disp,
			 symid sfg,
			 symid dp) { }
void vgen::cgdispvar    (symid disp,
			 symid var) { }
void vgen::cgdispstr    (symid disp,
			 char *str) { }
void vgen::cgdispcycle  (symid disp) { }
void vgen::cgdispbase   (symid disp,
			 int v) { }

void vgen::cgfsm_ctl    (symid v,
			 symid dp) { 
 vhdctl *fsm = new vhdfsm ("fsm");
 ctllist[v] = fsm;
 dplist[dp]->addctl(fsm);
 dplist[cur_dp]->addSfg(v,
			"fsm");
 cur_sfg = v;
 cur_dp = dp;
}

void vgen::cgfsm_state  (symid v,
			 symid c,
			 char *str) { 
 ctllist[c]->addstate(v,
		      str);
 ckrservewd(str,dplist[cur_dp]->getname());
}

void vgen::cgfsm_initial(symid v,
			 symid c,
			 char *str) {
  ctllist[c]->addinitial(v);
  ctllist[c]->addstate(v,
		       str);
 ckrservewd(str,dplist[cur_dp]->getname());
}

void vgen::cgfsm_trans  (symid v,
			 symid c,
			 symid from,
			 symid to) { 
  cur_step->addnextstate(dplist[cur_dp]->getstate(to) );

  ctllist[c]->addctlstep(from,
			 cur_step,
			 cur_step->getcount());
  dplist[cur_dp]->addcondvar(cur_step);

  vhdtableentry* row = new vhdtableentry(cur_step->getcount());
  for (vector<symid>::iterator i = cur_step->sfgexe.begin(); i != cur_step->sfgexe.end(); ++i) {
     row->addsfgid(*i); 
     row->addname( dplist[cur_dp]->getsfgname(*i) );
  }
  dplist[cur_dp]->addtableentry(row);
  cur_step = NULL;
}

void vgen::cgctltrue    (symid v,
			 symid c,
			 symid e,
			 symid p) {
	string truec = "1";
        if (cur_step == NULL) {
           vhdctlstep* t = new vhdctlstep();
           cur_step = t;
        }
        vhdstepcond* cd = new vhdstepcond(dplist[cur_dp]->getvarport(e),
					  truec);
	cur_step->addcond(cd);
}

void vgen::cgctlfalse   (symid v,
			 symid c,
			 symid e,
			 symid p) { 
	string falsec =  "0";
        vhdctlstep* t = new vhdctlstep();
	cur_step = t;
        vhdstepcond* cd = new vhdstepcond(dplist[cur_dp]->getvarport(e),
					  falsec);
	cur_step->addcond(cd);
}

void vgen::cgsysdp      (symid v) { 
  sys->adddp(v,
	     dplist[v]);
}

void vgen::cgdpc_arg    (symid parentdp,
			 symid parentpin,
			 symid childdp,
			 symid childpin) {
//  cout <<"dpc_arg: "<<parentdp<<","<<parentpin<<","<<childdp<<","<<childpin<<endl;
  vhdvar* pv = dplist[childdp]->getvarport(childpin);
  vhdvar* tv = dplist[parentdp]->getvarport(parentpin);
  if (tv -> isout())
	  tv -> setnotdefault();
  dplist[parentdp]->addportmapentry(dplist[childdp]->getname(),pv,tv);
}
void vgen::cgdpc_dp     (symid v, symid parentdp) { 
//  cout <<"dpc_dp: "<<v<<","<<parentdp<<endl;
  dplist[parentdp]->adddp(v,dplist[v]);
}
void vgen::cgdpc_ipblock(symid v, symid parentdp) { 
//  cout <<"dpc_dp: "<<v<<","<<parentdp<<endl;
  dplist[parentdp]->adddp(v,dplist[v]);
}

#ifdef ENABLEEVAL
static unsigned glbevalcount = 0;
#endif

// DATAPATH
void vgen::cgdp         (symid v, char *name) {

#ifdef ENABLEEVAL
  glbevalcount++;
  if (glbevalcount >3) {
    cerr << "*** This evaluation version of fdlvhd is limited to 3 FSMD\n"; 
    cerr << "*** Contact Patrick Schaumont (schaum@vt.edu) for access to full version\n";
    exit(0); 
  }  
#endif

  vhdvalldp* t = new vhdvdp (name, vhdvdp::dp);
  dplist[v] = t;
  t->resetcount();
  t->resetbitcount();
  cur_dp = v;
  asignconst = false;
  multterop = false;
  ckrservewd(name,dplist[cur_dp]->getname());
  
  // add generic ports if those were defined
  for (unsigned i=0; i < generic_dpname.size(); i++) {
    if (generic_dpname[i] == string(name)) {
      t->addgeneric(generic_pname[i],
		    generic_ptype[i],
		    generic_pdefault[i]);
    }
  }
  
  // add attribute definitions if those were defined
  for (unsigned i=0; i < attribute_def_dpname.size(); i++) {
    if (attribute_def_dpname[i] == string(name)) {
      t->addattributedef(attribute_def_attribute[i],
		         attribute_def_type[i]);
    }
  }
  
  // add attribute uses if those were defined
  for (unsigned i=0; i < attribute_use_dpname.size(); i++) {
    if (attribute_use_dpname[i] == string(name)) {
      t->addattributeuse(attribute_use_attribute[i],
		         attribute_use_signal[i],
		         attribute_use_value[i]);
    }
  }
}

void vgen::cgoption     (symid v, char *option) {
  std::istringstream abc(option);

  std::string first, second, third, fourth, fifth;

  abc >> first;

  if (string(first).find("generic") ==0) {
    // format: generic dpname port type default_value
    // example: generic user_logic C_DWIDTH integer 32
    string dpname, pname, ptype, pdefault;
    abc >> dpname;
    abc >> pname;
    abc >> ptype;
    abc >> pdefault;

    generic_dpname.push_back(dpname);
    generic_pname.push_back(pname);
    generic_ptype.push_back(ptype);
    generic_pdefault.push_back(pdefault);
  }

  if (string(first).find("attribute_def") ==0) {
    // format: attribute_def dpname attrname type
    // example: $option "attribute_def user_logic SIGIS string"
    string dpname, attrname, attrtype;
    abc >> dpname;
    abc >> attrname;
    abc >> attrtype;

    attribute_def_dpname.push_back(dpname);
    attribute_def_attribute.push_back(attrname);
    attribute_def_type.push_back(attrtype);    
  }

  if (string(first).find("attribute_use") ==0) {
    // format: attribute_use dpname attrname signame value
    // example: $option "attribute_use user_logic SIGIS Bus2IP_Clk   CLK"
    string dpname, attrname, signame, sigvalue;
    abc >> dpname;
    abc >> attrname;
    abc >> signame;
    abc >> sigvalue;

    attribute_use_dpname.push_back(dpname);
    attribute_use_attribute.push_back(attrname);
    attribute_use_signal.push_back(signame);
    attribute_use_value.push_back(sigvalue);

  }

}

void vgen::cgdpclone (symid v, char *name, symid p) {
//  cgdp(v,name);
  vhdvalldp* t = new vhdclonedp (name,dplist[p],vhdclonedp::dp);
  dplist[v] = t;
  t->resetcount();
  t->resetbitcount();
  cur_dp = v;
  asignconst = false;
  multterop = false;
  ckrservewd(name,dplist[cur_dp]->getname());
}

void vgen::cgipblock    (symid v,
			 char * instancename,
			 char * tname) { 
  vhdvalldp* t;
  if (strcmp(tname,
	     "ram") == 0){
    t = new vhdvdp (instancename,
		    vhdvdp::dp_ram);
    t->sethasclk(true);
  }
  else {
    t = new vhdvdp (instancename,vhdvdp::dp_ip);
    t->sethasclk(true);
  }
  dplist[v] = t;
  t->resetcount();
  t->resetbitcount();
  cur_dp = v;
  asignconst = false;
  ckrservewd(instancename,dplist[cur_dp]->getname());
}

void vgen::cgipout      (symid v,
			 int wlen,
			 int sign,
			 symid _dp,
			 char *portname) { 
  vhdvar* t = new vhdvar(portname,
			 1,
			 sign,
			 wlen,
			 true,
			 1);
  dplist[_dp]->addportvar(v,t);
  ckrservewd(portname,dplist[_dp]->getname());
}

void vgen::cgipparm     (symid v, char *parmname, symid _dp) { 
  std::istringstream abc(parmname);

  std::string first, second, third, fourth, fifth;

  abc >> first;
  
  if(dplist[_dp]->getdptype() == "ram") {
    if (first == "wl") {
      abc >> second;
      dplist[_dp]->setramwl(parmname);
    } else if (first == "size") {
      abc >> second;
      dplist[_dp]->setramsize(parmname);
    }
  }

 ckrservewd(parmname,dplist[_dp]->getname());
}

void vgen::cgsfg        (symid v, char* nm) { 
 dplist[cur_dp]->addSfg(v, nm);
 cur_sfg = v;
 ckrservewd(nm,dplist[cur_dp]->getname());
}

void vgen::cginput      (symid v, int wlen, int sign, symid _dp, char *portname) {
 vhdvar* t = new vhdvar(portname,1,sign,wlen,true,0);
 dplist[_dp]->addportvar(v,t);
 ckrservewd(portname,dplist[_dp]->getname());
}

void vgen::cgoutput     (symid v, int wlen, int sign, symid _dp, char *portname) { 
 vhdvar* t = new vhdvar(portname,1,sign,wlen,true,1);
 dplist[_dp]->addportvar(v,t);
 ckrservewd(portname,dplist[_dp]->getname());
}

void vgen::cgreg        (symid v, int wlen, int sign, symid _dp, char* nm) {
 //if local, nm = cur_sfg name + nm;
 vhdvar* t = new vhdvar(nm,0,sign,wlen,false,0);
 dplist[_dp]->addvar(v,t);
 ckrservewd(nm,dplist[_dp]->getname());
}

void vgen::cgsig        (symid v, int wlen, int sign, symid _dp, char* nm) {
 //if local, nm = cur_sfg name + nm;
 vhdvar* t = new vhdvar(nm,1,sign,wlen,false,0);
 dplist[_dp]->addvar(v,t);
 ckrservewd(nm,dplist[_dp]->getname());
}

void vgen::cgconst      (symid v, char *str) {
  vhdvar* var;
  gval *gv = make_gval(str);
  string n = gv->getbitstring();

  if(str[0]=='0' && str[1]=='x') {
    var = new vhdvar(dplist[cur_dp]->vautoname(),vhextostring(str),false);
    var = new vhdvar(dplist[cur_dp]->vautoname(),vhextostring(str),false);
    //    var->setwl((strlen(str)-2)*4);
    var->setwl(n.size());
    dplist[cur_dp]->addbit(v,var);
  }
  else if( str[0]=='0' && str[1]=='b') {
    var = new vhdvar(dplist[cur_dp]->vautoname(),vbintostring(str),false);
    //    var->setwl(strlen(str)-2);
    var->setwl(n.size());
    dplist[cur_dp]->addbit(v,var);
  }
  else {
     var = new vhdvar(str,true,vgetconstwl(str));
     dplist[cur_dp]->addvar(v,var);
  }
  ckrservewd(str,dplist[cur_dp]->getname());
}
void vgen::cgassign     (symid v, symid sfg, symid lhs, symid rhs) { 
  if ( asignconst ) {
    asignconst = false;
    for (vector<symid>::iterator i = asignidlist.begin(); 
	 i != asignidlist.end(); 
	 ++i) 
      dplist[cur_dp]->getvarport((*i))->setwl( dplist[cur_dp]->getvarport(lhs)->getwl() );
    asignidlist.clear();
  }
  
  bool isreg = false;
  if ( dplist[cur_dp]->getvar(lhs) != NULL ) 
    if (dplist[cur_dp]->getvar(lhs)->isreg() ) 
      isreg = true;
  
  vhdvar *vhd_rhs, *vhd_lhs;

  vhd_rhs = multterop ? dplist[cur_dp]->getvarport(cur_ter) :
                        dplist[cur_dp]->getvarport(rhs);  

  vhd_lhs = dplist[cur_dp]->getvarport(lhs);
  
  // if (false) {
  if ( dplist[cur_dp]->getvarport(lhs)->isout() ) {   
    // this is assignment of an output signal
    //
    //     z = expr1;
    //
    // will be code-generated as:
    //
    //     t1 = expr1;
    //     z  = t1;
    //
    // and all RIGHT-HAND USES of z will use the name of t1
    //
    // -- implementation:
    //
    // we create an extra intermediate signal called 'portname'
    // and create an additional assignment to this signal
    // So we have:
    //
    //     z_int = expr1;
    //     z     = z_int;
    //
    // (the _int suffix is added in codegen methods vhdvar::getname() and
    //  vhdvar::getportname())
    //
    // The extra intermediate signal needs to be created only ONCE
    // Therefore we store it as part of a vhdvdp, indexed by the output
    // port.
    //
    // During codegen the name of the _port_ signal is
    // created as
    //   'z_int'  (when the signal is used RHS)
    // and as
    //   'z'      (when the signal is used LHS)
    //

    vhdvar * tmpout;
    vhdvar * port = dplist[cur_dp]->getvarport(lhs);

    if (dplist[cur_dp]->getoport_tr(port) != NULL) {
      tmpout =  dplist[cur_dp]->getoport_tr(port);
    } else {
      tmpout = new vhdvar(dplist[cur_dp]->getvarport(lhs)->getname(),
			  1,     // is a signal
			  dplist[cur_dp]->getvarport(lhs)->gettc(),
			  dplist[cur_dp]->getvarport(lhs)->getwl(),
			  false, // is a port
			  0);    // IO-direction - don't care
      dplist[cur_dp]->addvar(makeuniqid(),tmpout);
      dplist[cur_dp]->setoport_tr(port, tmpout);
    }

    vhdassign *var1 = new vhdassign(vhd_rhs, tmpout);
    //    var1->setvalid(); // mark this as non-dead code
    //    var1->addseteq(tmpout);
    dplist[cur_dp]->addassignInstr(makeuniqid(), sfg, var1, false);

    // new vhd_lhs is the tmpout var
    vhd_rhs = tmpout;
  }
  
  if (dplist[cur_dp]->getoptype(rhs) == "assign") {
	vhd_rhs = dplist[cur_dp]->getop(rhs)->geteq();
  }
  vhdassign* var = new vhdassign(vhd_rhs, vhd_lhs);

  dplist[cur_dp]->addassignInstr(v, sfg, var, isreg);
  multterop = false;
}

void vgen::cgindex      (symid v, symid basevar, int idx, int len) {
  ostrstr_t name;
  name << dplist[cur_dp]->getvarport(basevar)->getname();
  if (len > 1)
    name << "(" << (idx+len-1) << " downto " << idx << ")";
  else
    name << "(" << idx << ")"; 
  vhdvar* var = new vhdvar(dplist[cur_dp]->getvarport(basevar),
			   name.str(),
			   idx,
			   len,
			   dplist[cur_dp]->getvarport(basevar)->getsig(),
			   dplist[cur_dp]->getvarport(basevar)->gettc() );
  dplist[cur_dp]->addvar(v,var);
}

void vgen::cglutable    (symid v, int wlen, int sign, vector<char *> &content, symid _dp, char* nm) {
  vhdlut* t = new vhdlut(wlen,sign,content,nm);
  dplist[_dp]->addlut(v,t);
 ckrservewd(nm,dplist[_dp]->getname());
}

void vgen::cgluop       (symid v, symid L, symid table) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     dplist[cur_dp]->getlut(table)->getsign(),
			     dplist[cur_dp]->getlut(table)->getwlen(),
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);
  vhdlutop *s = new vhdlutop(dplist[cur_dp]->getlut(table),dplist[cur_dp]->getvarport(L));
  s->addseteq(newL);
  dplist[cur_dp]->addlutInstr(v,cur_sfg,s);
}

void vgen::cgadd        (symid v, symid L, symid R) {

  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),dplist[cur_dp]->getvarport(R),vhdbinop::add_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}
void vgen::cgconcat     (symid v, symid L, symid R) {
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     dplist[cur_dp]->getvarport(L)->getwl() + 
			     dplist[cur_dp]->getvarport(R)->getwl() ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::concat_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}
void vgen::cgsub        (symid v, symid L, symid R) {
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::sub_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}
void vgen::cgior        (symid v, symid L, symid R) {
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::ior_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}
void vgen::cgxor        (symid v, symid L, symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
    dplist[cur_dp]->addsfgvar(v,newL);
  else
    dplist[cur_dp]->addvar(v,newL);
  
  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::xor_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}

void vgen::cgand        (symid v, symid L, symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
    dplist[cur_dp]->addsfgvar(v,newL);
  else
    dplist[cur_dp]->addvar(v,newL);
  
  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::and_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}

void vgen::cgshr        (symid v, symid L, symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(), 
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(), 
				  dplist[cur_dp]->getvarport(L)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,newL);
  else
     dplist[cur_dp]->addvar(v,newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::shr_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,cur_sfg,s);
}

void vgen::cgshl        (symid v, symid L, symid R) { 
  vhdvar* newL;
  if(dplist[cur_dp]->getvarport(R)->isconst())
     newL = new vhdvar (dplist[cur_dp]->vautoname(),
			1,
			dplist[cur_dp]->getvarport(L)->gettc(),
			dplist[cur_dp]->getvarport(L)->getwl() + 
			vstringtoInt(dplist[cur_dp]->getvarport(R)->getname().c_str()),
			false,
			0);
  else
     newL = new vhdvar (dplist[cur_dp]->vautoname(),
			1,
			dplist[cur_dp]->getvarport(L)->gettc(),
			//			vmax(dplist[cur_dp]->getvarport(L)->getwl(),
			//			     dplist[cur_dp]->getvarport(R)->getwl()),
			dplist[cur_dp]->getvarport(L)->getwl() + 
			((1 << dplist[cur_dp]->getvarport(R)->getwl()) - 1),
			false,
			0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);
  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::shl_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgmul        (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(),
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     (dplist[cur_dp]->getvarport(L)->getwl() + dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::mul_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgmod        (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     vmax(dplist[cur_dp]->getvarport(L)->gettc(),
				  dplist[cur_dp]->getvarport(R)->gettc()),
			     vmax(dplist[cur_dp]->getvarport(L)->getwl(),
				  dplist[cur_dp]->getvarport(R)->getwl()) ,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::mod_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgeq         (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::eq_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgne         (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::ne_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cggrt        (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::grt_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgsmt        (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::smt_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cggrteq      (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::grteq_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgsmteq      (symid v,
			 symid L,
			 symid R) { 
  vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			     1,
			     0,
			     1,
			     false,
			     0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

  vhdbinop *s = new vhdbinop(dplist[cur_dp]->getvarport(L),
			     dplist[cur_dp]->getvarport(R),
			     vhdbinop::smteq_op);
  s->addseteq(newL);
  dplist[cur_dp]->addbinInstr(v,
			      cur_sfg,
			      s);
}
void vgen::cgcast       (symid v,
			 symid L,
			 int wlen,
			 int sign) { 
   vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			      1,
			      sign,
			      wlen,
			      false,
			      0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

   vhdunop *s = new vhdunop(dplist[cur_dp]->getvarport(L),
			    vhdunop::cast_op);
   s->addseteq(newL);
   dplist[cur_dp]->addunInstr(v,
			      cur_sfg,
			      s);  
}
void vgen::cgnot        (symid v,
			 symid L) { 
   vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
			      1,
			      dplist[cur_dp]->getvarport(L)->gettc(),
			      dplist[cur_dp]->getvarport(L)->getwl(),
			      false,
			      0);
  if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
     dplist[cur_dp]->addsfgvar(v,
			       newL);
  else
     dplist[cur_dp]->addvar(v,
			    newL);

   vhdunop *s = new vhdunop(dplist[cur_dp]->getvarport(L),
			    vhdunop::not_op);
   s->addseteq(newL);
   dplist[cur_dp]->addunInstr(v,
			      cur_sfg,
			      s);  
}
void vgen::cgneg        (symid v,
			 symid L) { 
  if(dplist[cur_dp]->getvarport(L)->isconst()) {
        string str = "-";
	str += dplist[cur_dp]->getvarport(L)->getname();
	vhdvar* var = new vhdvar(str,
				 true,
				 dplist[cur_dp]->getvarport(L)->getwl()+1);
	if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
	  dplist[cur_dp]->addsfgvar(v,
				    var);
	else
	  dplist[cur_dp]->addvar(v,
				 var);
  }
  else {
	vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
				   1,
				   dplist[cur_dp]->getvarport(L)->gettc(),
				   dplist[cur_dp]->getvarport(L)->getwl(),
				   false,
				   0);
	if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
	  dplist[cur_dp]->addsfgvar(v,
				    newL);
	else
	  dplist[cur_dp]->addvar(v,
				 newL);

	vhdunop *s = new vhdunop(dplist[cur_dp]->getvarport(L),
				 vhdunop::neg_op);
	s->addseteq(newL);
	dplist[cur_dp]->addunInstr(v,
				   cur_sfg,
				   s);
  }
}
void vgen::cgsel        (symid v,
			 symid L,
			 symid M,
			 symid R) { 
  if(!multterop) {
     vhdvar* newL = new vhdvar (dplist[cur_dp]->vautoname(),
				1,
				dplist[cur_dp]->getvarport(M)->gettc(),
				vmax(dplist[cur_dp]->getvarport(M)->getwl(),
				     dplist[cur_dp]->getvarport(R)->getwl()),
				false,
				0);
     if (dplist[cur_dp]->getsfgname(cur_sfg) == "fsm" )
        dplist[cur_dp]->addsfgvar(v,
				  newL);
     else
        dplist[cur_dp]->addvar(v,
			       newL);
     vhdterop *s = new vhdterop(dplist[cur_dp]->getvarport(L),
				dplist[cur_dp]->getvarport(M),
				dplist[cur_dp]->getvarport(R),
				vhdterop::sel_op);
     s->addseteq(newL);

     dplist[cur_dp]->addterInstr(v,
				 cur_sfg,
				 s);
     if (dplist[cur_dp]->getvarport(M)->isconst() || dplist[cur_dp]->getvarport(R)->isconst()) {
        asignconst = true;
        asignidlist.push_back(v);
     }

     if ((dplist[cur_dp]->getoptype(M) == "select") || (dplist[cur_dp]->getoptype(R) == "select"))
          multterop = true;
     else
	  multterop = false;
     cur_ter = v;
  }
  else {
     vhdterop *s = new vhdterop(dplist[cur_dp]->getvarport(L),
				dplist[cur_dp]->getvarport(M),
				dplist[cur_dp]->getvarport(R),
				vhdterop::sel_op);
     dplist[cur_dp]->addmultterInstr(cur_ter,
				     cur_sfg,
				     s);
  }

}


void vgen::preprocess() {
  cout <<"Pre-processing System ...    "<<endl;
  for (map<symid,
	 vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); i++) {
    if ((i->second))
	i->second->preprocess();
  }
  sys->preprocess();
}	

void vgen::dump(string clkname,
		string rstname,
		int synclk,
		int resetval) {
  cout <<"Output VHDL source ...       "<<endl;
  cout <<"---------------------------- "<<endl;
  for (map<symid,
	 vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); i++) {
    if ((i->second))
	i->second->dump(clkname,
			rstname,
			synclk,
			resetval);
  }
  sys->dump(clkname,
	    rstname,
	    synclk);
  vgen_lib("std_logic_arithext.vhd");
}

void vgen::debugreport() {
  cout <<" "<<endl;
  cout <<"debug option enabled ...    "<<endl;
  cout <<"---------------------------- "<<endl;
    string vhd = ".txt";
    string filename = "report" + vhd;

    vhdreport* report = new vhdreport(filename);

  cout<<"Generate report file: "<<filename<<endl;

  report->outputline("--------------------------------------");
  report->outputline("The list of datapath files... ");
  report->outputline("--------------------------------------");
  report->increaseindent();
  for (map<symid,vhdvalldp *>::iterator i = dplist.begin(); i != dplist.end(); i++) {
    if ((i->second))
	i->second->debugreport(report);
  }
  report->decreaseindent();
  sys->debugreport(report);  
}

