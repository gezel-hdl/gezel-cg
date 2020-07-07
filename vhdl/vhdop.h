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

#ifndef VHDOP_H
#define VHDOP_H

#include <vector>
using std::vector;

#include <string>
using std::string;

class vhdterop;
class vhdvar;
class vhdout;

class vhdop {
 public:
  vhdop();
  virtual ~vhdop();
  
  virtual void addseteq(vhdvar*); 
  virtual void addter(vhdterop* t);
  virtual void vhdoutop (vhdout*,bool seteqisreg, bool isfsmcmb);
  virtual vhdvar* getleft();
  virtual vhdvar* getmiddle();
  virtual vhdvar* getright();	
  virtual string gettype();
  virtual string getoperator();
  virtual vector<vhdterop *> getteroplist();
  virtual vhdvar* geteq();
  virtual bool isvalid();
  virtual void setvalid();
};


#endif
