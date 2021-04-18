//
//  reasoning_aux.cpp
//  URM
//
//  Created by Parker on 5/11/18.
//  Copyright © 2018 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

Binding::~Binding() {
    for (int j=0;j<ara;j++) localtypes[j].cleanup();
    if (ara) delete[] localtypes;
    for (int u=0;u<binds.size();u++) binds[u].cleanup();
}
void SingleBind::cleanup() {
    head.cleanup();
    body.cleanup();
    for (int j=0;j<ara;j++) itinerary[j].cleanup();
    if (ara) delete[] itinerary;
}
void Statement::cleanup() {
    for (int v=0;v<ara;v++) args[v].cleanup();
    local=9001;
    id=900;
    if (ara) delete[] args;
    ara = 0;
    args = 0;
}
void Strategy::cleanup() {
    for (int v=0;v<ara;v++) args[v].cleanup();
    type.cleanup();
    local=9001;
    id=900;
    ara = 90;
    delete[] args;
    args = 0;
}
Statement Statement::deepcopy() const {return Statement(id,local,ara,::deepcopy(args,ara));}
Strategy Strategy::deepcopy() const {return Strategy(type.deepcopy(),id,local,ara,::deepcopy(args,ara));}
Statement* deepcopy(Statement* args,int ara) {
    if (ara==0) return 0;
    Statement* nx = new Statement[ara];
    for (int a=0;a<ara;a++) nx[a]=args[a].deepcopy();
    return nx;
}
Strategy* deepcopy(Strategy* args,int ara) {
    if (ara==0) return 0;
    Strategy* nx = new Strategy[ara];
    for (int a=0;a<ara;a++) nx[a]=args[a].deepcopy();
    return nx;
}
Statement::Statement() {}
Statement::Statement(int idr,int loc):local(loc),id(idr) {}
Statement::Statement(int idr,int loc,int buf):local(loc),id(idr),ara(buf) {
    if (buf) args = new Statement[buf];
}
Statement::Statement(int idr,int loc,int buf,Statement* ar):local(loc),id(idr),ara(buf),args(ar) {}
Statement::Statement(int idr,int loc,Statement a):local(loc),id(idr),ara(1) {
    args = new Statement[1];
    args[0] = a;
}
Statement::Statement(int idr,int loc,Statement a,Statement b):local(loc),id(idr),ara(2) {
    args = new Statement[2];
    args[0] = a;args[1] = b;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c):local(loc),id(idr),ara(3) {
    args = new Statement[3];
    args[0] = a;args[1] = b;args[2] = c;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c,Statement d):local(loc),id(idr),ara(4) {
    args = new Statement[4];
    args[0] = a;args[1] = b;args[2] = c;args[3] = d;
}
Strategy::Strategy() {}
Strategy::Strategy(Statement ty,int idr,int loc):local(loc),id(idr),type(ty) {}
Strategy::Strategy(Statement ty,int idr,int loc,int buf):local(loc),id(idr),type(ty),ara(buf) {
    if (buf) args = new Strategy[buf];
}
Strategy::Strategy(Statement ty,int idr,int loc,int buf,Strategy* ar):local(loc),id(idr),type(ty),ara(buf),args(ar) {}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a):local(loc),id(idr),type(ty),ara(1) {
    args = new Strategy[1];
    args[0] = a;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b):local(loc),id(idr),type(ty),ara(2) {
    args = new Strategy[2];
    args[0] = a;args[1] = b;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c):local(loc),id(idr),type(ty),ara(3) {
    args = new Strategy[3];
    args[0] = a;args[1] = b;args[2] = c;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c,Strategy d):local(loc),id(idr),type(ty),ara(4) {
    args = new Strategy[4];
    args[0] = a;args[1] = b;args[2] = c;args[3] = d;
}
int MetaBank::getAxiom(std::string ax) const {
    for (int fs=0;fs<stratnames.size();fs++) {
        if (stratnames[fs]==ax) {
            return fs;
        }
    }
    std::cout<<"Cannot find axiom: "<<ax<<"\n";
    throw;
}

ParameterContext ParameterContext::append(const Strategy& a) const {return append(a.args,a.ara);}
ParameterContext ParameterContext::append(Strategy* l,int buf) const {
    ParameterContext nn;
    nn.dat = dat;
    nn.dat.push_back({l,buf});
    return nn;
}

int ParameterContext::loc() const {return dat.size()-1;}
SingleBind::SingleBind(Statement a,Statement b,Strategy* c,int buf,bool uni,bool conc) : head(a),body(b),itinerary(c),ara(buf),universal(a.ara==0?true:uni),concrete(conc) {}
SingleBind SingleBind::deepcopy() const {
    SingleBind dup = SingleBind(head.deepcopy(),body.deepcopy(),::deepcopy(itinerary,ara),ara,universal,concrete);
    return dup;
}
Binding::Binding(MetaBank* mb,Strategy* lt,int buf) : localtypes(lt), ara(buf) {
    tracks.dat.push_back({mb->strategies,mb->ara});
    tracks.dat.push_back({localtypes,ara});
}
Binding::Binding(const ParameterContext& t,Strategy* lt,int buf) : tracks(t),localtypes(lt),ara(buf) {
    tracks.dat.push_back({localtypes,ara});
}
Binding& Binding::operator = (const Binding& other) {
    ara = other.ara;
    if (ara) localtypes = new Strategy[ara];
    for (int u=0;u<ara;u++) localtypes[u] = other.localtypes[u].deepcopy();
    for (int j=0;j<other.binds.size();j++) binds.push_back(other.binds[j].deepcopy());
    if (other.tracks.dat.size()) {
        for (int f=0;f<other.tracks.loc();f++) tracks.dat.push_back(other.tracks.dat[f]);
        tracks.dat.push_back({localtypes,ara});
    }
    return *this;
}
Binding::Binding(const Binding& other) {
    *this = other;
}
bool Statement::contains(int i,int loc) const {
    if (id==i and local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(i,loc)) return true;
    return false;
}
bool Statement::contains(int loc) const {
    if (local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(loc)) return true;
    return false;
}



