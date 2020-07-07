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


#ifndef VHDUNOP_H
#define VHDUNOP_H

#include <string>
using std::string;

#include "vhdop.h"

class vhdunop : public vhdop {
  public:
	enum _optype {neg_op,not_op,cast_op};

  private:
	_optype type;
	vhdvar* left;

	vhdvar* seteq;
        bool valid;

  public:
	vhdunop(vhdvar* ,_optype);
	~vhdunop();

	string gettype();
	vhdvar* getleft();
	string getoperator();
	vhdvar* geteq();
	void addseteq(vhdvar*); 
	bool isvalid();
	void setvalid();
	void vhdoutop (vhdout*,bool seteqisreg, bool isfsmcmb);
};

#endif
