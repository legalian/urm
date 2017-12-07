//
//  reasoning_trivial.cpp
//  URM
//
//  Created by Parker on 9/15/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

MetaBank MetaBank::meta_prime;
Statement* Statement::universe=0;
Statement* Statement::gap=0;

Entry::Entry(Binding q) : bind(q) {}

SingleBind::SingleBind(Statement* a,Statement* b,std::vector<Strategy*>& c) : head(a),body(b),itinerary(c) {}

Branches::Branches(int j) {
    for (int k=0;k<j;k++) {
        branches.push_back(std::vector<int>());
    }
}
Binding::Binding(MetaBank* mb,std::vector<Strategy*>& lt,std::vector<Branches>& pi) : localtypes(lt),principles(pi) {
    tracks.push_back(&mb->strategies);
    tracks.push_back(&localtypes);
    for (int u=0;u<lt.size();u++) {
        partials.push_back(Statement::gap);
    }
}
Binding::Binding(std::vector<std::vector<Strategy*>*>& t,std::vector<Strategy*>& lt,std::vector<Branches>& pi) : tracks(t),localtypes(lt),principles(pi) {
    tracks.push_back(&localtypes);
    for (int u=0;u<lt.size();u++) {
        partials.push_back(Statement::gap);
    }
}

Binding::~Binding() {

}

//Statement::Statement(int idr,int loc) {
//    local=loc;id=idr;
//}
void Statement::cleanup() {
    if (local>0 or args.size()!=0) {
        for (int v=0;v<args.size();v++) {
            args[v]->cleanup();
        }
//        if (type) type->cleanup();
        local=9001;
        id=900;
//        type = (Statement*)99;
        delete this;
    }
}
void Strategy::cleanup() {
    if (local!=0 or args.size()!=0) {
        for (int v=0;v<args.size();v++) {
            args[v]->cleanup();
        }
        if (type) type->cleanup();
        local=9001;
        id=900;
//        type = (Statement*)99;
        delete this;
    }
}
Statement* Statement::deepcopy() {
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->deepcopy());
    }
    return res;
}
Strategy* Strategy::deepcopy() {
    Strategy* res = new Strategy(type->deepcopy(),id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->deepcopy());
    }
    return res;
}

Statement::Statement(int idr,int loc) {
    local=loc;id=idr;
}
Statement::Statement(int idr,int loc,Statement* a) {
    local=loc;id=idr;
    args.push_back(a);
}
Statement::Statement(int idr,int loc,Statement* a,Statement* b) {
    local=loc;id=idr;
    args.push_back(a);
    args.push_back(b);
}
Statement::Statement(int idr,int loc,Statement* a,Statement* b,Statement* c) {
    local=loc;id=idr;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
}
Statement::Statement(int idr,int loc,Statement* a,Statement* b,Statement* c,Statement* d) {
    local=loc;id=idr;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
    args.push_back(d);
}
Strategy::Strategy(Statement* ty,int idr,int loc) {
    local=loc;id=idr;type=ty;
}
Strategy::Strategy(Statement* ty,int idr,int loc,Strategy* a) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
}
Strategy::Strategy(Statement* ty,int idr,int loc,Strategy* a,Strategy* b) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
    args.push_back(b);
}
Strategy::Strategy(Statement* ty,int idr,int loc,Strategy* a,Strategy* b,Strategy* c) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
}
Strategy::Strategy(Statement* ty,int idr,int loc,Strategy* a,Strategy* b,Strategy* c,Strategy* d) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
    args.push_back(d);
}
int MetaBank::getAxiom(std::string ax) {
    for (int fs=0;fs<stratnames.size();fs++) {
        if (stratnames[fs]==ax) {
            return fs;
        }
    }
    std::cout<<"Cannot find axiom: "<<ax<<"\n";
    throw;
}
bool Statement::containsloop(int a,int sdepth) {
    if (local==sdepth and id==a) return true;
    for (int q=0;q<args.size();q++) {
        if (args[q]->containsloop(a,sdepth)) return true;
    }
    return false;
}

//Statement* Statement::symmetricbindavoid(int sdepth,int amt) {
//    if (local==0 and args.size()==0) {
//        return this;
//    }
//    Statement* res = new Statement(local==1&&specifier==stmodif?amt+id:id,local);
//    for (int q=0;q<args.size();q++) {
//        res->args.push_back(args[q]->symmetricbindavoid(stmodif,amt));
//    }
//    if (type) res->type = type->symmetricbindavoid(stmodif,amt);
//    else res->type=0;
////#ifdef safe_play
////    res->deltasub=deltasub;
////#endif
//    res->specifier=specifier;
//    return res;
//}

Statement* Statement::scramble(std::map<int,int>& mapr,int& push,int sdepth) {
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    if (local==sdepth) {
        if (mapr.find(id)!=mapr.end()) {
            res->id=mapr[id];
        } else {
            res->id=push++;
            mapr[id]=res->id;
        }
    }
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->scramble(mapr,push,sdepth));
    }
//    if (type) res->type = type->scramble(mapr,push,sdepth);
//    else res->type = 0;
    return res;
}
void Statement::unscramble(std::map<int,int>& mapr,int& push,int sdepth) {
    if (local==sdepth) {
        if (mapr.find(id)==mapr.end()) {
            mapr[id]=push++;
        }
        id=mapr[id];
    }
    for (int q=0;q<args.size();q++) {
        args[q]->unscramble(mapr,push,sdepth);
    }
}


Soln::Soln() : expanded(true), head(new Statement(-1,1)) {}
Soln::Soln(Statement* point) : head(point) {}

Statement* Soln::getsolution() {
    for (int y=0;y<bin.size();y++) {
        if (bin[y]->downstream.size()==0) {
//            std::cout<<bin[y]->bind.tostringheavy()<<"asdf\n";
//            return head->substitute(&bin[y]->bind);
        }
    }
    return 0;
}
//int Statement::maxloc() {
//    int amt=local==1?id+1:0;
//    for (int r=0;r<args.size();r++) {
//        int an=args[r]->maxloc();
//        if (an>amt) amt=an;
//    }
//    return amt;
//}
//Statement* Statement::mapl(int stmodif,int nstmodif) {
//    if (local==0 and args.size()==0) {
//        return this;
//    }
//    Statement* res = new Statement(id,local);
//    for (int q=0;q<args.size();q++) {
//        res->args.push_back(args[q]->mapl(stmodif,nstmodif));
//    }
//    if (type) res->type = type->mapl(stmodif,nstmodif);
//    else res->type = 0;
////#ifdef safe_play
////    res->deltasub=deltasub;
////#endif
//    res->specifier=(local==1 and specifier==stmodif)?nstmodif:specifier;
//    return res;
//}
void Statement::local_list(std::vector<Statement*>* list) {
    if (local<=0 and args.size()==0) return;
    if (local==1) {
        list->push_back(this);
    }
    for (int q=0;q<args.size();q++) {
        args[q]->local_list(list);
    }
//    if (type) type->local_list(list);
}
bool amorphousmatch(Statement* a,Statement* b,std::map<int,int>& forward,std::map<int,int>& backward) {
    if (a==0 and b==0) return true;
    if (a==0 or b==0) return false;
    if (a->local!=b->local or a->args.size()!=b->args.size()) return false;
    if (a->local==1) {
        if (forward.find(a->id) !=forward.end()  and forward[a->id]!=b->id) return false;
        if (backward.find(b->id)!=backward.end() and backward[b->id]!=a->id) return false;
        forward[a->id]=b->id;
        backward[b->id]=a->id;
    } else if (a->id!=b->id) return false;
    for (int i=0;i<a->args.size();i++) {
        if (!amorphousmatch(a->args[i],b->args[i],forward,backward)) return false;
    }
//    if ((a->type!=Statement::universe or b->type!=Statement::universe) and !amorphousmatch(a->type,b->type,forward,backward)) return false;
    return true;
}

SolnLink::SolnLink(Statement* from,Statement* to,Soln* link,Entry* a) : container(a),linked(link) {
    std::map<int,int> backward;
    amorphousmatch(from,to,backward,mapr);
}

Binding Binding::emptycopy() {
    Binding out = *this;
    out.binds.clear();
    return out;
}

//bool Statement::is(int disp,int cutoff,Statement* other) {
//    if (other->id!=id) return false;
//    if (local>cutoff) {
//        if (other->local+disp!=local) return false;
//    } else if (other->local!=local) return false;
//    for (int y=0;y<args.size();y++) {
//        if (!args[y]->is(disp,cutoff,other->args[y])) return false;
//    }
//    return true;
//}








