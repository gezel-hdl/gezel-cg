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



#include "vhdout.h"
#include "rterror.h"

//--------------------------------------------------------------
vhdout::vhdout(string filename) : os(filename.c_str()) {
  //  os.open(filename.c_str());

  indent = 0;

  if (os.fail()) {
      fdlerror(ERROR_CANNOT_OPEN_VHD_FILE, filename.c_str());
  }
}

vhdout::~vhdout() {}

void vhdout::comment(string str) {
  doindent();
  os <<"--"<<str<<"\n";  
}

void vhdout::library(string str) {
  doindent();
  os <<"library "<<str<<";"<<"\n";
}

void vhdout::lib_use(string str) {
  doindent();
  os <<"use "<<str<<";"<<"\n";
}

void vhdout::increaseindent() {
  indent++;
}

void vhdout::decreaseindent() {
  indent--;
}

void vhdout::doindent() {
  for (int i = 0; i < indent; i ++)
	os << "   ";
}

void vhdout::outputline(string str) {
  doindent();
  os << str<<"\n";
}

void vhdout::outputchar(string str) {
  os << str;
}

void vhdout::nextline(int line) {
  for (int i = 0; i < line; i ++)
	os << "\n";
}

void vhdout::beginentity(string name) {
  doindent();
  os <<"entity "<<name<<" is"<<"\n";
}

void vhdout::beginentitygeneric() {
  increaseindent();
  doindent();
  os <<"generic("<<"\n";
  increaseindent();
  first = true;
}

void vhdout::dumpgeneric(string p, string t, string d) {
  if (!first) {
    os <<";"<<"\n";
  } else first = false;

  doindent();
  os << p << " : " << t << " := " << d;
}

void vhdout::endentitygeneric() {
  os << "\n";
  decreaseindent();
  doindent();
  os <<");"<<"\n";
}

void vhdout::beginentityport() {
  doindent();
  os <<"port("<<"\n";
  increaseindent();
  first = true;
}

void vhdout::dumpattrdef(string s1, string s2) {
  doindent();
  os << "attribute " << s1 << " : " << s2 << ";\n";
}

void vhdout::dumpattruse(string s1, string s2, string s3) {
  doindent();
  os << "attribute " << s1 << " of " << s2 << " : signal is " << "\"" << s3 << "\";\n";
}

void vhdout::dumpportvar(vhdvar* v) {
  //  cerr << "dumpportvar: var name is " <<  v->getportname() << "\n";

  if (!first) {
    os <<";"<<"\n";
  } else first = false;

  doindent();
  os << v->getportname()<<":";
  if (v->isout()) os <<"out ";
  else os <<"in ";
  if (v->getwl() > 1)
    os <<"std_logic_vector("<<v->getwl()-1<<" downto 0)";
  else
    os <<"std_logic";
}

void vhdout::endports (string name, bool hasclk, string clkname, string rstname) {
  if(hasclk) {
    if(first) {
      doindent();
      os <<rstname<<" : in std_logic;"<<"\n";
      doindent();
      os <<clkname<<" : in std_logic"<<"\n";
    }
    else {
      os <<";"<<"\n";
      doindent();
      os <<rstname<<" : in std_logic;"<<"\n";
      doindent();
      os <<clkname<<" : in std_logic"<<"\n";
    }
  }
  
  os <<"\n";
  decreaseindent();
  doindent();
  os <<");"<<"\n";

}  

void vhdout::endentity (string name) {

  decreaseindent();
  doindent();
  os <<"end "<<name<<";"<<"\n";
}  

void vhdout::begincomponent(string name) {
  doindent();
  os <<"component "<<name<<"\n";
  increaseindent();
  doindent();
  os <<"port("<<"\n";
  increaseindent();
  first = true;
}

void vhdout::endcomponent (string name, bool hasclk, string clkname, string rstname) {
  if(hasclk) {
    if (first) {
       doindent();
       os <<rstname<<" : in std_logic;"<<"\n";
       doindent();
       os <<clkname<<" : in std_logic"<<"\n";
    }
    else {
       os <<";"<<"\n";    
       doindent();
       os <<rstname<<" : in std_logic;"<<"\n";
       doindent();
       os <<clkname<<" : in std_logic"<<"\n";
    }
  }
  decreaseindent();
  doindent();
  os <<");"<<"\n";

  decreaseindent();
  doindent();
  os <<"end component;"<<"\n";
}  

void vhdout::architecture(string type, string name) {
  doindent();
  os <<"architecture "<<type<<" of "<<name<<" is"<<"\n";
}

void vhdout::dumpvardeclare(vhdvar* v) {
  doindent();
  os <<"signal "<< v->getname()<<":";
  if (v->getwl() > 1)
	os <<"std_logic_vector("<<v->getwl()-1<<" downto 0);"<<"\n";
  else
        os <<"std_logic;"<<"\n";
  if (v->isreg()) {
    os <<"signal "<< v->getname()<<"_wire"<<":";
    if (v->getwl() > 1)
	os <<"std_logic_vector("<<v->getwl()-1<<" downto 0);"<<"\n";
    else
        os <<"std_logic;"<<"\n";
  }  
}  

void vhdout::beginstatedeclare() {
  doindent();
  first = true;
  os << "type STATE_TYPE is (";
}

void vhdout::eachstate(string st) {
  if (first) {
	os << st;
	first = false;
  }
  else
	os <<","<<st;
}

void vhdout::endstatedeclare() {
  os <<");"<<"\n";
  doindent();
  os << "signal STATE:STATE_TYPE;"<<"\n";
}

void vhdout::lutdeclare(string name, int wlen,int size) {
  doindent();
  if (size > 1)
    os <<"Type "<<name<<" is Array (Natural range <>) of std_logic_vector("<<(wlen - 1)<<" downto 0);"<<"\n";
}

void vhdout::luttable(string name,
                     string luttypename,
                     int lutwl,
                     vector<gval *> value,
                     int size) {
 doindent();
 if(size == 1) {
    os <<"constant "<<name<<" : integer := ";
    for (vector<gval *>::iterator i = value.begin(); i != value.end();
++i)
           os <<(*i)<<";"<<"\n";
 }
 else {
    int cnt = 0;
    os << "constant " << name << " : " << luttypename<< " := (";
    for (vector<gval*>::iterator i = value.begin(); i != value.end();
++i){

      if (cnt != 0)
        os << ",";

      if ( ((cnt % 4) == 0) && cnt)
        os << "\n\t";

      (*i)->setbase(2);
      os << "B\"";
      (*i)->writestream(os);
      os << "\"";

      cnt++;
    }
    os <<");"<<"\n";
 }
}

void vhdout::dumpbitstream(vhdvar* v) {
  doindent();
  os <<v->getportname()<<" <= "<<v->getbitstream()<<";"<<"\n";
}

void vhdout::outputram(int ramsize, int ramwl, int addrwl,string clkname) {
  if (ramsize > 2) {
    doindent();
    os <<"type ram_type is array ("<<ramsize-1<<" downto 0) of std_logic_vector ("<<ramwl-1<<" downto 0);"<<"\n";
    doindent();
    os << "signal RAM : ram_type;"<<"\n";
    doindent();
    os << "signal sig_1 : std_logic_vector("<<addrwl-1<<" downto 0);"<<"\n";
    doindent();
    os << "begin"<<"\n";
    doindent();
    os << "process("<<clkname<<")"<<"\n";
    doindent();
    os << "begin"<<"\n";
    increaseindent();
    doindent();
    os << "if ("<<clkname<<"'event and "<<clkname<<" = '1') then "<<"\n";
    increaseindent();
    doindent();
    os << "if ( wr = '1' ) then "<<"\n";
    increaseindent();
    doindent();
    os << "RAM(conv_integer(unsigned(address))) <= idata;"<<"\n";
    decreaseindent();
    doindent();
    os << "end if; "<<"\n";
    doindent();
    os << "sig_1 <= address;"<<"\n";
    decreaseindent();
    doindent();
    os << "end if; "<<"\n";
    decreaseindent();
    doindent();
    os << "end process; "<<"\n";
    doindent();
    os << "odata <= RAM(conv_integer(unsigned(sig_1)));"<<"\n";
  } else {
    // address wordlength = 1 bit
    doindent();
    os <<"type ram_type is array ("<<ramsize-1<<" downto 0) of std_logic_vector ("<<ramwl-1<<" downto 0);"<<"\n";
    doindent();
    os << "signal RAM : ram_type;"<<"\n";
    doindent();
    os << "signal sig_1 : std_logic;"<<"\n";
    doindent();
    os << "begin"<<"\n";
    doindent();
    os << "process("<<clkname<<")"<<"\n";
    doindent();
    os << "begin"<<"\n";
    increaseindent();
    doindent();
    os << "if ("<<clkname<<"'event and "<<clkname<<" = '1') then "<<"\n";
    increaseindent();
    doindent();
    os << "if ( wr = '1' ) then "<<"\n";
    increaseindent();
    doindent();
    os << "RAM(conv_integer(address)) <= idata;"<<"\n";
    decreaseindent();
    doindent();
    os << "end if; "<<"\n";
    doindent();
    os << "sig_1 <= address;"<<"\n";
    doindent();
    os << "odata <= RAM(conv_integer(sig_1));"<<"\n";
    decreaseindent();
    doindent();
    os << "end if; "<<"\n";
    decreaseindent();
    doindent();
    os << "end process; "<<"\n";
    doindent();
  }
}

void vhdout::beginportmap(string name,string parent) {
  doindent();
  os <<"label_"<<name<<" : "<<parent<<" port map ("<<"\n";
  increaseindent();
  increaseindent();
  first = true;
}

void vhdout::dumpmapvar(vhdvar* v, vhdvar* t) {
  if (!first) {
    os <<","<<"\n";    
  }
  else
    first = false;

  doindent();
  if(t->getwl() > v->getwl()) {
    os << v->getportname()<<" => "<<t->getportname()<<"("<<v->getwl()-1<<" downto 0)";
    cout<<"VHD WARNING: MISMATCH IN WORDLENGHT IN PORTMAP.."<<v->getname()<<" -> "<<t->getname()<<"\n";
  }
  else
    os << v->getportname()<<" => "<<t->getportname();
}

void vhdout::endportmap (bool hasclk, string clknameport, string rstnameport, string clkname, string rstname) {
  if(hasclk) {
    if (first) {
       doindent();
       os <<rstnameport<<" => "<<rstname<<","<<"\n";
       doindent();
       os <<clknameport<<" => "<<clkname<<"\n";
    }
    else {
       os <<","<<"\n";    
       doindent();
       os <<rstnameport<<" => "<<rstname<<","<<"\n";
       doindent();
       os <<clknameport<<" => "<<clkname<<"\n";
    }
  }
  decreaseindent();
  doindent();
  os <<");"<<"\n";
  decreaseindent();
}  

void vhdout::begin() {
  doindent();
  os <<"begin"<<"\n";
}

void vhdout::process(string label, string sense) {
  doindent();
  os <<label<<": process ("<<sense<<")"<<"\n";
}

void vhdout::ifresethigh(string rstname, int resethighval) {
  doindent();
  os << "if ("<<rstname<<" = '" << resethighval << "') then"<<"\n";
}

void vhdout::elseclk(string clkname) {
  doindent();
  os << "elsif "<<clkname<<"' event and "<<clkname<<" = '1' then"<<"\n";
}

void vhdout::ifclk(string clkname) {
  doindent();
  os << "if "<<clkname<<"' event and "<<clkname<<" = '1' then"<<endl;
}

void vhdout::defaultzero(vhdvar* v, bool zero) {
  doindent();
  if (v->issetdefault()) {
    if (zero) {
      if (v->getwl() > 1)
	os << v->getportname()<<" <= (others=>'0');"<<"\n";
      else
	os << v->getportname()<<" <= '0';"<<"\n";
    }
    else
      os << v->getportname()<<" <= "<<v->getname()<<";"<<"\n";
  }
}

void vhdout::endif() {
  doindent();
  os <<"end if;"<<"\n";
}

void vhdout::endprocess(string label) {
  doindent();
  os <<"end process "<<label<<";"<<"\n";
}

void vhdout::casestr(string cmd) {
  doindent();
  os <<"case "<<cmd<<" is"<<"\n";
}

void vhdout::endcase() {
  doindent();
  os <<"end case;"<<"\n";
}

void vhdout::when(string c) {
  doindent();
  os << "when "<<c<<" =>"<<"\n";
}

string vhdout::setstringsign(string nm,int sign){
  string target;
  if (sign)
	target = "signed(" + nm + ")";
  else
	target = "unsigned(" + nm + ")";
  return target;
}
string vhdout::to_std_vector(string nm, int twl,int wl){
  string target;
  if(wl == twl)
	target = "to_stdlogicvector(" + nm + ");" + "\n";
  else if(twl > wl)
	target = "ext(to_stdlogicvector(" + nm + ")," + vintostring(twl) + ");" + "\n";
  else
	target = "to_stdlogicvector(" + nm + ")(" + vintostring(twl-1) + " downto 0);" + "\n";
  return target;
}
string vhdout::conv_std_vector(string nm, int twl){
  string target;
  if(twl > 1)
	target = "conv_std_logic_vector(" + nm + "," + vintostring(twl) + ")";
  else target = "'" + nm + "'";
  return target;
}
string vhdout::setstringlen(int twl,string nm,int wl,int sign){
  string target;
  if (twl == wl)
	target = nm;
  else
    if (wl > twl)
      target = nm + "(" + vintostring(twl-1) + " downto 0)";
    else {
      if (wl == 1)
	if (sign)
	  target = "logic_sign_ext(" + nm + "," + vintostring(twl) + ")";
	else
	  target = "logic_zero_ext(" + nm + "," + vintostring(twl) + ")";
      else
	if (sign)
	  target = "conv_std_logic_vector(signed(" + nm + ")," + vintostring(twl) + ")";
	else
	  target = "conv_std_logic_vector(unsigned(" + nm + ")," + vintostring(twl) + ")";
    }
  
  /*
  else if (wl > 1) {
	if (wl > twl)
		target = nm + "(" + vintostring(twl-1) + " downto 0)";
	else {
		if (wl == 1)
			if (sign)
				target = "logic_sign_ext(" + nm + "," + vintostring(twl) + ")";
			else
				target = "logic_zero_ext(" + nm + "," + vintostring(twl) + ")";
		else
			if (sign)
				target = "conv_std_logic_vector(signed(" + nm + ")," + vintostring(twl) + ")";
			else
				target = "conv_std_logic_vector(unsigned(" + nm + ")," + vintostring(twl) + ")";
	}
  }
  else {
    target = nm + "(0)";
  }
  */

  return target;
}

void vhdout::binoparith(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm,int lsign,bool rconst, int rwl, string rnm,int rsign) {
  doindent();
  string target,tmp_target;

  if (op == " & ") {
    target = target + lnm + op + rnm;
    os << nm << " <= " << target << ";" << endl;
    return;
  }
  
  if (op == " * ") {
    if (lconst)
      tmp_target = conv_std_vector(lnm,wl);
    else
      tmp_target = setstringsign(lnm,lsign);

    target = target + tmp_target + op;

    if (rconst)
      tmp_target = conv_std_vector(rnm,wl);
    else
      tmp_target = setstringsign(rnm,rsign);

    target = target + tmp_target;
    os << nm << " <= " << target << ";"<<endl;

    return;
  } 
  
  if (lconst)
    tmp_target = conv_std_vector(lnm,wl);
  else
    tmp_target = setstringlen(wl,lnm,lwl,lsign);

  tmp_target = setstringsign(tmp_target,sign);

  target = target + tmp_target + op;

  if (rconst)
    tmp_target = conv_std_vector(rnm,wl);
  else
    tmp_target = setstringlen(wl,rnm,rwl,rsign);

  tmp_target = setstringsign(tmp_target,sign);
  target = target + tmp_target;
  
  os << nm << " <= " << target << ";"<<endl;
}

void vhdout::binoparithlogic(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm,int lsign,bool rconst, int rwl, string rnm,int rsign) {
  doindent();
  string target,tmp_target;

  target = nm + " <= ";

  if (lconst)
	tmp_target = conv_std_vector(lnm,wl);
  else
	tmp_target = setstringlen(wl,lnm,lwl,lsign);
  target = target + tmp_target + op;
  if (rconst)
	tmp_target = conv_std_vector(rnm,wl);
  else
	tmp_target = setstringlen(wl,rnm,rwl,rsign);
  target = target + tmp_target;

  os << target << ";"<<endl;
}

void vhdout::binopshift(string op, int sign, int wl, string nm, bool lconst, int lwl, string lnm,int lsign,bool rconst, int rwl, string rnm) {
  doindent();
  os <<nm<<" <= ";
  
  
  if (op == " << ") {
    if (lwl > 1) {		  
      if ( sign ) 
	os <<"conv_std_logic_vector(shl(conv_signed(";
      else	
	os <<"conv_std_logic_vector(shl(conv_unsigned(";
      if (lsign)
	os <<"signed(";
      else
	os <<"unsigned(";
      if (rconst) 		
	os <<lnm<<"),"<<wl<<"),conv_unsigned("<<rnm<<","<<wl<<")),"<<wl<<");"<<"\n";
      else
	os <<lnm<<"),"<<wl<<"),unsigned("<<rnm<<")),"<<wl<<");"<<"\n";
    }
    else {
      if ( sign ) 
	os <<"conv_std_logic_vector(shl(signed(";
      else	
	os <<"conv_std_logic_vector(shl(unsigned(";
      if (lsign)
	os << "logic_sign_ext("<<lnm<<","<<wl<<")),";
      else
	os << "logic_zero_ext("<<lnm<<","<<wl<<")),";
      if (rconst) 		
	os <<"conv_unsigned("<<rnm<<","<<wl<<")),"<<wl<<");"<<"\n";
      else
	os <<"unsigned("<<rnm<<")),"<<wl<<");"<<"\n";
    }
  }
  else {
    if (lwl > 1) {		  
      if ( sign ) 
	os <<"conv_std_logic_vector(shr(signed(";
      else	
	os <<"conv_std_logic_vector(shr(unsigned(";
      if (rconst) 		
	os <<lnm<<"),"<<"conv_unsigned("<<rnm<<","<<wl<<")),"<<wl<<");"<<"\n";
      else
	os <<lnm<<"),"<<"unsigned("<<rnm<<")),"<<wl<<");"<<"\n";
    }  
    else {
      if ( sign ) 
	os <<"conv_std_logic_vector(shr(signed(";
      else	
	os <<"conv_std_logic_vector(shr(unsigned(";
      if (rconst) 		
	os <<"conv_std_logic_vector("<<lnm<<","<<wl<<")),"<<"conv_unsigned("<<rnm<<","<<wl<<")),"<<wl<<");"<<"\n";
      else
	os <<"conv_std_logic_vector("<<lnm<<","<<wl<<")),"<<"unsigned("<<rnm<<")),"<<wl<<");"<<"\n";
    }
  }
  
}

void vhdout::binoplogic(string op, string nm, bool lconst, string lnm, int lsign,int lwl, bool rconst, string rnm, int rsign) {
  doindent();

  string target,tmp_target;


  target = "if (";

  if (lconst)
	target = target + lnm;
  else {
     if(lwl > 1){
	  tmp_target = setstringsign(lnm,lsign);
        target = target + tmp_target + op;
        if (rconst)
           target = target + rnm + ") then" + "\n";
        else {
  	     tmp_target = setstringsign(rnm,rsign);
    	     target = target + tmp_target + ") then" + "\n";	
        }
     }
     else {
	  target = target + lnm + op;
        if (rconst)
            target = target + "'" + rnm + "') then" + "\n";
        else
 	      target = rnm + ") then" + "\n";	
     }
  }

  os <<target;
  increaseindent();
  doindent();
  os <<nm<<" <= "<<"'1';"<<"\n";
  decreaseindent();
  doindent();
  os<<"else"<<"\n";
  increaseindent();
  doindent();
  os <<nm<<" <= "<<"'0';"<<"\n";
  decreaseindent();
  doindent();
  os <<"end if;"<<"\n";
}

void vhdout::dunopnot(string nm,int wl,bool lconst,int lwl, string lnm,int lsign) {
  doindent();
  string target,tmp_target;

  target = nm + " <= " + " not "; 
  if (lconst)
	tmp_target = conv_std_vector(lnm,wl);
  else
	tmp_target = setstringlen(wl,lnm,lwl,lsign);
  target = target + tmp_target;
  os << target << ";"<<endl;
}

void vhdout::dunopneg(string nm,int wl,int sign,int lwl, string lnm, bool lconst,int lsign) {
  doindent();
  string target,tmp_target;

  target = nm + " <= " + " -"; 

  if (lconst)
	tmp_target = conv_std_vector(lnm,wl);
  else
	tmp_target = setstringlen(wl,lnm,lwl,lsign);
  tmp_target = setstringsign(tmp_target,sign);
  target = target + tmp_target;
  os << target << ";"<<endl;

}

void vhdout::dunopcast(string nm,int wl,int sign,int lwl, string lnm, bool lconst,int lsign) {
  doindent();
  os <<nm<<" <= "; 

  if (lconst)
    if(wl > 1)
      if (sign)
	os <<"conv_std_logic_vector(conv_signed("<<lnm<<","<<wl<<"),"<<wl<<");"<<"\n";
      else
	os <<"conv_std_logic_vector(conv_unsigned("<<lnm<<","<<wl<<"),"<<wl<<");"<<"\n";
    else
      cout<<"VHD ERROR: CANNOT - ONE BIT OBJ"<<"\n";
  else if (wl == lwl)
    if (lwl == 1)
      os << lnm<<";"<<"\n";
    else if (sign)
      os <<"conv_std_logic_vector(signed("<<lnm<<"),"<<wl<<");"<<"\n";
    else
      os <<"conv_std_logic_vector(signed("<<lnm<<"),"<<wl<<");"<<"\n";
  else
    if(wl > 1)
      if (lwl > wl)
	os <<lnm<<"("<<(wl-1)<<" downto 0);"<<"\n";
      else {
	if (lwl == 1)
	  os <<"conv_std_logic_vector("<<lnm<<","<<wl<<");"<<"\n";
	else if (sign)
	  os <<"conv_std_logic_vector(signed("<<lnm<<"),"<<wl<<");"<<"\n";
	else
	  os <<"conv_std_logic_vector(unsigned("<<lnm<<"),"<<wl<<");"<<"\n";
      }
    else
      os <<lnm<<"(0));"<<"\n";
}

void vhdout::teropsel(string nm,int wl,string lnm,int lsign, int lwl, string mnm,bool mconst,int mwl,int msign,bool misbit,string rnm,bool rconst,int rwl,int rsign,bool risbit) {
  doindent();
  string target,tmp_target;

  if (lwl > 1) {
     if (lsign)
       os <<"if (signed("<<lnm<<") > 0) then"<<"\n";
     else
       os <<"if (unsigned("<<lnm<<") > 0) then"<<"\n";
  }
  else
     os <<"if ("<<lnm<<" = '1') then"<<"\n";


  increaseindent();
  doindent();
  target = nm + " <= ";
  if (mconst)
    if(misbit)
	tmp_target = to_std_vector(mnm,wl,mwl);
    else
	tmp_target = conv_std_vector(mnm,wl);
  else
	tmp_target = setstringlen(wl,mnm,mwl,msign);

  target = target + tmp_target;
  os << target<<";"<<endl;

  decreaseindent();
  doindent();
  os <<"else"<<"\n";

  increaseindent();
  doindent();
  target = nm + " <= ";

  if (rconst)
    if(risbit)
	tmp_target = to_std_vector(rnm,wl,rwl);
    else
	tmp_target = conv_std_vector(rnm,wl);
  else
	tmp_target = setstringlen(wl,rnm,rwl,rsign);

  target = target + tmp_target;
  os << target<<";"<<endl;
  decreaseindent();
  endif();
}

void vhdout::teropmultsel(bool firstsel,string nm,int wl,string lnm,int lsign, int lwl, string mnm,bool mconst,int mwl,int msign,bool misbit) {
  doindent();
  string target, tmp_target;

  if (firstsel) os <<"if";
  else		os <<"elsif";

  if (lwl > 1) {
     if (lsign)
       os <<" (signed("<<lnm<<") > 0) then"<<"\n";
     else
       os <<" (unsigned("<<lnm<<") > 0) then"<<"\n";
  }
  else
     os <<" ("<<lnm<<" = '1') then"<<"\n";


  increaseindent();
  doindent();
  target = nm + " <= ";
  if (mconst)
    if(misbit)
	tmp_target = to_std_vector(mnm,wl,mwl);
    else
	tmp_target = conv_std_vector(mnm,wl);
  else
	tmp_target = setstringlen(wl,mnm,mwl,msign);

  target = target + tmp_target;
  os << target<<";"<<endl;
  decreaseindent();
}


void vhdout::teropendsel(string nm,int wl,string lnm,int lsign, int lwl, string mnm,bool mconst,int mwl,int msign,bool misbit,string rnm,bool rconst,int rwl,int rsign,bool risbit) {
  doindent();
  string target, tmp_target;

  if (lwl > 1) {
     if (lsign)
       os <<"elsif (signed("<<lnm<<") > 0) then"<<"\n";
     else
       os <<"elsif (unsigned("<<lnm<<") > 0) then"<<"\n";
  }
  else
     os <<"elsif ("<<lnm<<" = '1') then"<<"\n";


  increaseindent();
  doindent();
  target = nm + " <= ";
  if (mconst)
    if(misbit)
	tmp_target = to_std_vector(mnm,wl,mwl);
    else
	tmp_target = conv_std_vector(mnm,wl);
  else
	tmp_target = setstringlen(wl,mnm,mwl,msign);

  target = target + tmp_target;
  os << target<<";"<<endl;

  decreaseindent();
  doindent();
  os <<"else"<<"\n";

  increaseindent();
  doindent();
  target = nm + " <= ";

  if (rconst)
    if(risbit)
	tmp_target = to_std_vector(rnm,wl,rwl);
    else
	tmp_target = conv_std_vector(rnm,wl);
  else
	tmp_target = setstringlen(wl,rnm,rwl,rsign);

  target = target + tmp_target;
  os << target<<";"<<endl;
  decreaseindent();
  endif();
}

void vhdout::assign(string nm,
		    int    wl,
		    bool   lconst,
		    int    lwl,
		    string lnm,
		    int    lsign,
		    bool   lisbit) {
  doindent();
  string target, tmp_target;

  target = nm + " <= ";
  if (lconst)
    if(lisbit)
	tmp_target = to_std_vector(lnm,wl,lwl);
    else
	tmp_target = conv_std_vector(lnm,wl);
  else
	tmp_target = setstringlen(wl,lnm,lwl,lsign);

  target = target + tmp_target;
  os << target<<";"<<endl;
}

void vhdout::lutop(string nm, int wl, string lutnm, int lutsize, string lnm,int lwl,bool lconst) {
  doindent();
  if(lutsize == 1) 
    os << nm<<" <= conv_std_logic_vector("<<lutnm<<","<<wl<<");"<<"\n";  
  else if (lconst)
    os << nm<<" <= "<<lutnm<<"(conv_integer(unsigned(conv_std_logic_vector("<<lnm<<","<<wl<<"))));"<<"\n";
  else	
    os << nm<<" <= "<<lutnm<<"(conv_integer(unsigned("<<lnm<<")));"<<"\n";
}

void vhdout::assignreg(vhdvar* v) {
  doindent();
  os <<v->getname()<<" <= "<<v->getregname()<<";"<<endl;
}

void vhdout::assigndefaultreg(vhdvar* v) {
  doindent();
  os << v->getname()<<"_wire <= "<<v->getname()<<";"<<"\n";
}

void vhdout::close() {
  os.close();
}
