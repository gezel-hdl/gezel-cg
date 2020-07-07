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

#include "vhdportnet.h"
#include "vhdvar.h"
#include "vhdout.h"

//--------------------------------------------------------------
vhdnet::vhdnet(symid _in, symid _out) : input(_in), output(_out) {}

vhdnet::~vhdnet() {}

vhdvar* vhdnet::getvar() {
  return var;
}

symid vhdnet::getinput() {
  return input;
}

symid vhdnet::getoutput() {
  return output;
}

void vhdnet::addvar(vhdvar* v) {
  var = v;
}

//--------------------------------------------------------------
vhdmapentry::vhdmapentry(vhdvar* _portv, vhdvar* _targetv) : portv(_portv), targetv(_targetv) {}

vhdmapentry::~vhdmapentry() {}

vhdvar* vhdmapentry::getportv() {
  return portv;
}

vhdvar* vhdmapentry::gettargetv() {
  return targetv;
}

void vhdmapentry::settargetv(vhdvar* v) {
  targetv = v;
}

//--------------------------------------------------------------
vhdportmap::vhdportmap(vhdvalldp* curdp) {
  dp = curdp;
  if(curdp->isclonedp()) {
    clone = true;
    parentnm = curdp->getparentname();
  }
  else
    clone = false;
}

vhdportmap::~vhdportmap() {
  for (vector<vhdmapentry *>::iterator i = entry.begin(); i != entry.end(); ++i)
    delete (*i);
}

void vhdportmap::addentry(vector<vhdmapentry *> t) {
  entry = t;
}

void vhdportmap::addoneentry(vhdmapentry* t) {
  entry.push_back(t);
}

string vhdportmap::getparentnm() {
  return parentnm;
}

vector<vhdvar *> vhdportmap::gettargetlist() {
  vector<vhdvar *> list;
  
  for (vector<vhdmapentry *>::iterator i = entry.begin(); i != entry.end(); ++i)
    list.push_back((*i)->gettargetv());
  return list;
}

bool vhdportmap:: gethasclk() {
  return hasclk;
}

void vhdportmap::updatehasclk() {
  hasclk = dp->gethasclk();
}

bool vhdportmap::isclone() {
  return clone;
}

void vhdportmap::dumpportvar(vhdout* os) {
  for (vector<vhdmapentry *>::iterator i = entry.begin(); i != entry.end(); ++i)
    os->dumpportvar((*i)->getportv());
}

void vhdportmap::dumpmapvar(vhdout* os) {
  for (vector<vhdmapentry *>::iterator i = entry.begin(); i != entry.end(); ++i)
    os->dumpmapvar((*i)->getportv(),(*i)->gettargetv());
}

