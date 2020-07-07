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

#include "vhdvar.h"


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

//--------------------------------------------------------------

vhdvar::vhdvar(string _name, 
	       int _sig, 
	       int _tc, 
	       int _wl,
	       bool _port, 
	       int _io)  {
  name = _name; sig = _sig; tc = _tc; wl = _wl; port = _port; io = _io;
  constint = false;
  bitstring = false;
  index = false; idx = 0; 
  bitstream = "\0";
  setdefault = true;
}

vhdvar::vhdvar(string _name, string _bitstream, bool _constint) {
  name = _name;
  bitstream = _bitstream;
  constint = false;
  bitstring = true;
  wl = _bitstream.length() - 2;
  sig = 1;
  tc = 0;
  port = false;
  index = false; idx = 0; 
  setdefault = true;
}

vhdvar::vhdvar(string _name, bool _constint, int _wl) {
  name = _name;
  bitstream = "\0";
  constint = _constint;
  bitstring = false;
  wl = _wl;
  sig = 1;
  tc = 0;
  port = false;
  index = false; idx = 0; 
  setdefault = true;
}

vhdvar::vhdvar(vhdvar* _basevar, string _name, int _idx, 
	       int _wl, int _sig, int _tc) {
  name = _name;
  constint = false;
  wl = _wl;
  sig = _sig;
  tc = _tc;
  port = false;
  index = true;
  idx = _idx;
  bitstream = "\0";
  bitstring = false;
  basevar = _basevar;
  setdefault = true;
}  

vhdvar::~vhdvar() {}

bool vhdvar::isreg() {
  if(sig) return false;
  else    return true;
}

bool vhdvar::isconst() {
  return constint;
}

bool vhdvar::isbitstring() {
  return bitstring;
}

bool vhdvar::isout() {
  if (io == 1) return true;
  else	 return false;
}

bool vhdvar::isindex() {
  return index;
}

bool vhdvar::isport() {
  if(port)
    return true;
  else
    return false;
}

bool vhdvar::issetdefault() {
  return setdefault;
}

int vhdvar::gettc() {
  return tc;
}

int vhdvar::getwl() {
  return wl;
}

int vhdvar::getsig() {
  return sig;
}

int vhdvar::getio() {
  return io;
}

void vhdvar::setio(int v) {
  io = v;
}

void vhdvar::setwl(int len) {
  wl = len;
}

void vhdvar::setnotdefault() {
  setdefault = false;
}

string vhdvar::getname() {
  // for output ports, return "portname_int"
  // for all other signals, return the name itself
  // this exception supports _reading_ of output signals in dp
  string k;
  if (port && io) 
    k = name + "_int";
  else
    k = name;
  return k;
}

string vhdvar::getportname() {
  // simply return the base name
  // this method is called for LHS assignments of output ports
  return name;
}

string vhdvar::getregname() {
  ostrstr_t ostrm;
  if(isindex()) {
    if (wl > 1) {
      ostrm<<basevar->getname() + "_wire";
      ostrm<<"("<<idx+wl-1<<" downto "<<idx<<")";
      return ostrm.str();
    }		
    else {
      ostrm<<basevar->getname() + "_wire";	
      ostrm<<"("<<idx<<")";
      return ostrm.str();
    }
  }
  else
    return name + "_wire";
}

string vhdvar::getbitstream() {
    return bitstream;
}

vhdvar* vhdvar::getbasevar() {
    return basevar;
}

void vhdvar::updateparameter(string _name, int _tc, int _wl, 
			     int _sig, bool _port, bool _constant) {
  tc = _tc;
  wl = _wl;
  sig = _sig;
  name = _name;
  port = _port;
  constint = _constant;	 	
}
 
void vhdvar::dump(ostream& os) {}

void vhdvar::dumpport(ostream& os) {}

void vhdvar::dumpdeclare(ostream& os) {}

