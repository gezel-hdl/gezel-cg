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



#ifndef VHDVAR_H
#define VHDVAR_H

#include <iostream>
#include <string>
using std::string;
using std::ostream;

class vhdvar {
  string name;
  string bitstream;
  int   sig;/* 0 reg, 1 sig */
  int   tc; /* 0 nc,  1 tc  */
  int	wl;
//*******************************
  bool  port;
  int   io; /* 0 in, 1 out  */
//*******************************
  bool  constint;
  bool  bitstring;
//*******************************
  bool  index;
  int   idx;
  vhdvar* basevar;
 
  bool  setdefault;
  
  public:
  vhdvar(string name, int sig, int tc, int wl,bool port,int io);
  vhdvar(string name, string bitstream,bool constint);
  vhdvar(string name, bool constint, int _wl);
  vhdvar(vhdvar* _basevar, string _name, int _idx, int _wl, int _sig, int _tc);
  ~vhdvar();
  
  bool isreg();
  bool isconst();
  bool isbitstring();
  bool isout();
  bool isindex();
  bool isport();
  bool issetdefault();
  
  int gettc();
  int getwl();
  int getsig();
  int getio();
  void setio(int);
  void setwl(int);
  void setnotdefault();
  string getname();
  string getregname();
  string getportname();
  string getbitstream();
  vhdvar* getbasevar();
  void updateparameter(string _name, int _tc, int _wl, int _sig, bool _port, bool _constant);
  
  void dumpport(ostream &);
  void dumpdeclare(ostream &);
  void dump(ostream & );
};

#endif
