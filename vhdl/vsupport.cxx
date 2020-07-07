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

#include "vsupport.h"
#include "vhdvar.h"
#include "rterror.h"
#include "asymbol.h"
#include "vhdout.h"

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

//--------------------------------------------------------------
int vgetconstwl (char* str){
  int i = 0;
  int value;
  value = atoi (str);

  if (value == 0) return 0;

  for (i = 0; value > 0; i ++) {
	value = value / 2;
  }
  return i;
}

string vhextostring(char* value){
  gval *v = make_gval(value);
  string n = v->getbitstring();
  if (n.size() > 1)
    return "\"" + n + "\"";
  else
    return "\'" + n + "\'";
}

  //    ostrstr_t out;
  //    out<<'"';
  //    //convert from hex to binary stream   
  //    for(unsigned int i=2;i<strlen(value);i++) {
  //       switch(value[i]){
  //	    case '0': out<<"0000";break;
  //	    case '1': out<<"0001";break;
  //	    case '2': out<<"0010";break;
  //	    case '3': out<<"0011";break;
  //	    case '4': out<<"0100";break;
  //	    case '5': out<<"0101";break;
  //	    case '6': out<<"0110";break;
  //	    case '7': out<<"0111";break;
  //	    case '8': out<<"1000";break;
  //	    case '9': out<<"1001";break;
  //	    case 'a':
  //            case 'A': out<<"1010";break;
  //	    case 'b':
  //	    case 'B': out<<"1011";break;
  //	    case 'c':
  //	    case 'C': out<<"1100";break;
  //	    case 'd':
  //	    case 'D': out<<"1101";break;
  //	    case 'e': 
  //	    case 'E': out<<"1110";break;
  //	    case 'f':
  //	    case 'F': out<<"1111";break;}
  //    }
  //    out<<'"';
  //    return out.str();

string vbintostring(char* value){
  gval *v = make_gval(value);
  string n = v->getbitstring();
  if (n.size() > 1)
    return "\"" + n + "\"";
  else
    return "\'" + n + "\'";
}

//    ostrstr_t out;
//    if (strlen(value) > 3) 
//      out<<'"';   
//    else
//      out<<'\'';   
//    for(unsigned int i=2;i<strlen(value);i++) 
//       out <<value[i];
//    if (strlen(value) > 3) 
//      out<<'"';   
//    else
//      out<<'\'';   
//    return out.str();
//}

string vintostring(int num){
    ostrstr_t out;
    out<<num;
    return out.str();
}

string vautoname(string base, string name) {
    return (base + "_" + name);
}

int vstringtoInt(const char* num) {
   int result = 0;
   while (*num >= '0' && *num <='9') {
      result = (result * 10) + (*num - '0');
      num++;
   }   
   return result;
}

void ckrservewd(string nm, string dpname) {
  bool found = false; 
  
  if ((nm == "abs")
      or (nm == "access")
      or (nm == "after")
      or (nm == "alias")
      or (nm == "all")
      or (nm == "and")
      or (nm == "architecture")
      or (nm == "array")
      or (nm == "assert")
      or (nm == "attribute")
      or (nm == "begin")
      or (nm == "block")
      or (nm == "body")
      or (nm == "buffer")
      or (nm == "bus")
      or (nm == "case")
      or (nm == "component")
      or (nm == "configuration")
      or (nm == "constant")
      or (nm == "disconnect")
      or (nm == "downto")
      or (nm == "else")
      or (nm == "elsif")
      or (nm == "end")
      or (nm == "entity")
      or (nm == "exit")
      or (nm == "file")
      or (nm == "for")
      or (nm == "function")
      or (nm == "generate")
      or (nm == "generic")
      or (nm == "group")
      or (nm == "guarded")
      or (nm == "if")
      or (nm == "impure")
      or (nm == "in")
      or (nm == "inertial")
      or (nm == "inout")
      or (nm == "is")
      or (nm == "label")
      or (nm == "library")
      or (nm == "linkage")
      or (nm == "literal")
      or (nm == "loop")
      or (nm == "map")
      or (nm == "mod")
      or (nm == "nand")
      or (nm == "new")
      or (nm == "next")
      or (nm == "nor")
      or (nm == "not")
      or (nm == "null")
      or (nm == "of")
      or (nm == "on")
      or (nm == "open")
      or (nm == "or")
      or (nm == "others")
      or (nm == "out")
      or (nm == "package")
      or (nm == "port")
      or (nm == "postponed")
      or (nm == "procedure")
      or (nm == "process")
      or (nm == "pure")
      or (nm == "range")
      or (nm == "record")
      or (nm == "register")
      or (nm == "eject")
      or (nm == "rem")
      or (nm == "report")
      or (nm == "return")
      or (nm == "rol")
      or (nm == "ror")
      or (nm == "select")
      or (nm == "severity")
      or (nm == "signal")
      or (nm == "shared")
      or (nm == "sla")
      or (nm == "sll")
      or (nm == "sra")
      or (nm == "srl")
      or (nm == "subtype")
      or (nm == "then")
      or (nm == "to")
      or (nm == "transport")
      or (nm == "type")
      or (nm == "unaffected")
      or (nm == "units")
      or (nm == "until")
      or (nm == "use")
      or (nm == "variable")
      or (nm == "wait")
      or (nm == "when")
      or (nm == "while")
      or (nm == "with")
      or (nm == "xnor")
      or (nm == "xor") ) 
    found = true; 
  else
    found = false; 
  if(found) {
    cerr <<"Error: "<<nm<<" in datapath ["<<dpname<<"] is a VHDL reserved word exit."<<endl;
    exit(0);
  }
  
  string nm_end = nm.substr(nm.size() - 1,1);
  if (nm_end == "_") {
    cerr <<"Error: "<<nm<<" in datapath ["<<dpname<<"] name ending with _ is not allowed."<<endl;
    exit(0);
  }		
  
  if (nm.size() > 3) {
    nm_end = nm.substr(nm.size() - 4,4);
    if (nm_end == "_int") {
      cerr <<"Error: "<<nm<<" in datapath ["<<dpname<<"] name ending with _int is not allowed."<<endl;
      exit(0);
    }		
  }
  
  return;
  
}

void vgen_lib(string name){
	
  vhdout* vhdlout = new vhdout(name);
  
  cout<<"Generate file: "<<name<<endl;
  
  /**********************************************************************************************/

  vhdlout->outputline("--------------------------------------------------------------------------");
  vhdlout->outputline("--                                                                      --");
  vhdlout->outputline("--                                                                      --");
  vhdlout->outputline("-- This source file may be used and distributed without restriction     --");
  vhdlout->outputline("-- provided that this copyright statement is not removed from the file  --");
  vhdlout->outputline("-- and that any derivative work contains this copyright notice.         --");
  vhdlout->outputline("--                                                                      --");
  vhdlout->outputline("--	Package name: STD_LOGIC_ARITHEXT                                 --");
  vhdlout->outputline("--	Purpose: 			                                         --");
  vhdlout->outputline("--	 A set of arithemtic, conversion, and comparison functions       --");
  vhdlout->outputline("--	 STD_LOGIC		               	                             --");
  vhdlout->outputline("--					                                         --");
  vhdlout->outputline("--------------------------------------------------------------------------");

  vhdlout->outputline("library IEEE;");
  vhdlout->outputline("use IEEE.std_logic_1164.all;");

  vhdlout->outputline("package std_logic_arithext is");


  vhdlout->outputline("    function LOGIC_SIGN_EXT(ARG: STD_LOGIC; SIZE: INTEGER) return STD_LOGIC_VECTOR;");

  vhdlout->outputline("    function LOGIC_ZERO_EXT(ARG: STD_LOGIC; SIZE: INTEGER) return STD_LOGIC_VECTOR;");

  vhdlout->outputline("end std_logic_arithext;");



  vhdlout->outputline("library IEEE;");
  vhdlout->outputline("use IEEE.std_logic_1164.all;");

  vhdlout->outputline("package body std_logic_arithext is");
 	
  vhdlout->outputline("    function LOGIC_SIGN_EXT(ARG: STD_LOGIC; SIZE: INTEGER) return STD_LOGIC_VECTOR is");
  vhdlout->outputline("	subtype rtype is STD_LOGIC_VECTOR (SIZE-1 downto 0);");
  vhdlout->outputline("	variable result: rtype;");
  vhdlout->outputline("	-- synopsys built_in SYN_ZERO_EXTEND");

  vhdlout->outputline("    begin");
  vhdlout->outputline("	if (ARG = '0') then");
  vhdlout->outputline("	    result := rtype'(others => '0');");
  vhdlout->outputline("	elsif (ARG = '1') then");
  vhdlout->outputline("	    result := rtype'(others => '1');	");
  vhdlout->outputline("	elsif (ARG = 'X') then");
  vhdlout->outputline("	    result := rtype'(others => 'X');");
  vhdlout->outputline("	end if;");
  vhdlout->outputline("	return result;");
  vhdlout->outputline("    end;");
    

  vhdlout->outputline("    function LOGIC_ZERO_EXT(ARG: STD_LOGIC; SIZE: INTEGER) return STD_LOGIC_VECTOR is");
  vhdlout->outputline("	subtype rtype is STD_LOGIC_VECTOR (SIZE-1 downto 0);");
  vhdlout->outputline("	variable result: rtype;");

  vhdlout->outputline("    begin");
  vhdlout->outputline("	result := rtype'(others => '0');");
  vhdlout->outputline("	result(0) := ARG;");
  vhdlout->outputline("	if (result(0) = 'X') then");
  vhdlout->outputline("	    result := rtype'(others => 'X');");
  vhdlout->outputline("	end if;");
  vhdlout->outputline("	return result;");
  vhdlout->outputline("    end;");


  vhdlout->outputline("end std_logic_arithext;");

  /**********************************************************************************************/
  vhdlout->close();
  delete vhdlout;
  vhdlout = NULL;

}



