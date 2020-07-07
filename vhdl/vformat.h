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

#ifndef VFORMAT_H
#define VFORMAT_H

#include <iostream>
#include <fstream>
using namespace std;

// indenting
void vindent(ostream &os);
void vindentinc();
void vindentdec();

// comments
void vcomment (ostream &_os, const char *a, const char *b);
void vcomment (ostream &_os, const char *a);
void vcomment (ostream &os);
void vcommenttime(ostream &_os, const char *a);

// types
void vdumptype(ostream &os, int wlen);

void vline(ostream &os, const char *s);
void vline(ostream &os, const char *s, const char *s2);
void vline(ostream &os, const char *s, const char *s2, const char *s3);
void vline(ostream &os);
void vraw (ostream &os, const char *s);
void vseparator(ostream &os);

#endif
