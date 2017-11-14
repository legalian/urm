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

Entry::Entry(Binding q,int id) : bind(q) ,ids(id) {}

Binding::Binding() {}
Binding::Binding(const Binding& other) {
//    for (auto it=other.specs.begin();it!=other.specs.end();it++) {
//        specs[it->first]=it->second->deepcopy();
//    }
    for (int it=0;it<other.decoms.size();it++) {
        decoms.push_back(std::pair<Statement*,Statement*>(other.decoms[it].first->deepcopy(),other.decoms[it].second->deepcopy()));
    }
}
Binding& Binding::operator=(const Binding & other) {
    for (int it=0;it<decoms.size();it++) {
        decoms[it].first->cleanup();
        decoms[it].second->cleanup();
    }
    decoms.clear();
    for (int it=0;it<other.decoms.size();it++) {
        decoms.push_back(std::pair<Statement*,Statement*>(other.decoms[it].first->deepcopy(),other.decoms[it].second->deepcopy()));
    }
    return *this;
}
Binding::~Binding() {
    for (int it=0;it<decoms.size();it++) {
        decoms[it].first->cleanup();
        decoms[it].second->cleanup();
    }
}

Statement::Statement(int idr,int loc) {
    local=loc;id=idr;
}
void Statement::cleanup() {
    if (local!=0 or args.size()!=0) {
        for (int v=0;v<args.size();v++) {
            args[v]->cleanup();
        }
        if (type) type->cleanup();
        local=9001;
        id=900;
        type = (Statement*)99;
        delete this;
    }
}
Statement* Statement::deepcopy() {
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->deepcopy());
    }
    if (type) res->type = type->deepcopy();
    else res->type=0;
//#ifdef safe_play
//    res->deltasub=deltasub;
//#endif
    res->specifier=specifier;
    return res;
}

Statement::Statement(Statement* ty,int idr,int loc) {
    local=loc;id=idr;type=ty;
}
Statement::Statement(Statement* ty,int idr,int loc,Statement* a) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
}
Statement::Statement(Statement* ty,int idr,int loc,Statement* a,Statement* b) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
    args.push_back(b);
}
Statement::Statement(Statement* ty,int idr,int loc,Statement* a,Statement* b,Statement* c) {
    local=loc;id=idr;type=ty;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
}
Statement::Statement(Statement* ty,int idr,int loc,Statement* a,Statement* b,Statement* c,Statement* d) {
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
bool Statement::containsloop(int a) {
    if (local==1 and specifier==0 and id==a) return true;
    for (int q=0;q<args.size();q++) {
        if (args[q]->containsloop(a)) return true;
    }
    return false;
}

Statement* Statement::symmetricbindavoid(int stmodif,int amt) {
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(local==1&&specifier==stmodif?amt+id:id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->symmetricbindavoid(stmodif,amt));
    }
    if (type) res->type = type->symmetricbindavoid(stmodif,amt);
    else res->type=0;
//#ifdef safe_play
//    res->deltasub=deltasub;
//#endif
    res->specifier=specifier;
    return res;
}

Statement* Statement::scramble(std::map<int,int>& mapr,int& push) {
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    if (local==1) {
        if (mapr.find(id)!=mapr.end()) {
            res->id=mapr[id];
        } else {
            res->id=push++;
            mapr[id]=res->id;
        }
    }
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->scramble(mapr,push));
    }
    if (type) res->type = type->scramble(mapr,push);
    else res->type = 0;
    
    res->specifier=specifier;
    return res;
}
void Statement::unscramble(std::map<int,int>& mapr,int& push) {
    if (local==1 and specifier==0) {
        if (mapr.find(id)==mapr.end()) {
            mapr[id]=push++;
        }
        id=mapr[id];
    }
    for (int q=0;q<args.size();q++) {
        args[q]->unscramble(mapr,push);
    }
}


std::pair<Statement*,Statement*> gentleSubstitute(Binding* bind,Statement* a,Statement* b,int stmodif) {
    if (bind->decoms.size()==0) {
        return std::pair<Statement*,Statement*>(a->deepcopy(),b->deepcopy());
    }
//    if (a->args.size()==0) {
//        if (b->maxloc(stmodif)==0) {
//            return std::pair<Statement*,Statement*>(a->deepcopy(),b->deepcopy());
//        } else {
//            return std::pair<Statement*,Statement*>(a->deepcopy(),b->substitute(bind,2,stmodif));
//        }
//    } else {
        if (b->maxloc(stmodif)==0) {
            return std::pair<Statement*,Statement*>(a->substitute(bind,2,stmodif),b->deepcopy());
        } else {
            return std::pair<Statement*,Statement*>(a->substitute(bind,2,stmodif),b->substitute(bind,2,stmodif));
        }
//    }
}
Soln::Soln() : expanded(true), head(new Statement(0,-1,1)) {}
Soln::Soln(Statement* point) : head(point) {}

Statement* Soln::getsolution() {
    for (int y=0;y<bin.size();y++) {
        if (bin[y]->downstream.size()==0) {
//            std::cout<<bin[y]->bind.tostringheavy()<<"asdf\n";
            return head->substitute(&bin[y]->bind,2,0);
        }
    }
    return 0;
}
int Statement::maxloc(int loc) {
    int amt=local==1&&specifier==loc?id+1:0;
    for (int r=0;r<args.size();r++) {
        int an=args[r]->maxloc(loc);
        if (an>amt) amt=an;
    }
    return amt;
}
Statement* Statement::mapl(int stmodif,int nstmodif) {
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->mapl(stmodif,nstmodif));
    }
    if (type) res->type = type->mapl(stmodif,nstmodif);
    else res->type = 0;
//#ifdef safe_play
//    res->deltasub=deltasub;
//#endif
    res->specifier=(local==1 and specifier==stmodif)?nstmodif:specifier;
    return res;
}
void Statement::local_list(std::vector<Statement*>* list) {
    if (local==0 and args.size()==0) return;
    if (local==1) {
        list->push_back(this);
    }
    for (int q=0;q<args.size();q++) {
        args[q]->local_list(list);
    }
    if (type) type->local_list(list);
}
bool amorphousmatch(Statement* a,Statement* b,std::map<int,int>& forward,std::map<int,int>& backward) {
    if (a==0 and b==0) return true;
    if (a==0 or b==0) return false;
    if (a->local!=b->local or a->args.size()!=b->args.size() or a->specifier!=b->specifier) return false;
    if (a->local==1 and a->specifier==0) {
        if (forward.find(a->id) !=forward.end()  and forward[a->id]!=b->id) return false;
        if (backward.find(b->id)!=backward.end() and backward[b->id]!=a->id) return false;
        forward[a->id]=b->id;
        backward[b->id]=a->id;
    } else if (a->id!=b->id) return false;
    for (int i=0;i<a->args.size();i++) {
        if (!amorphousmatch(a->args[i],b->args[i],forward,backward)) return false;
    }
    if ((a->type!=Statement::universe or b->type!=Statement::universe) and !amorphousmatch(a->type,b->type,forward,backward)) return false;
    return true;
}

SolnLink::SolnLink(Statement* from,Statement* to,Soln* link,Entry* a) : container(a),linked(link) {
    std::map<int,int> backward;
    amorphousmatch(from,to,backward,mapr);
}












