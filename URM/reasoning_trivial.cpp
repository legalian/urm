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

Entry::Entry(Binding q) : bind(q) {}

SingleBind::SingleBind(Statement a,Statement b,Strategy* c,int buf) : head(a),body(b),itinerary(c),ara(buf) {}

Binding::Binding(MetaBank* mb,Strategy* lt,int buf) : localtypes(lt), ara(buf) {
    tracks.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
//    partials = new Statement[ara];
//    for (int u=0;u<ara;u++) {
//        partials[u] = Statement(-1,tracks.loc());
//    }
}
Binding::Binding(ParameterContext& t,Strategy* lt,int buf) : tracks(t),localtypes(lt),ara(buf) {
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
//    partials = new Statement[ara];
//    for (int u=0;u<ara;u++) {
//        partials[u] = Statement(-1,tracks.loc());
//    }
}
Binding::Binding(ParameterContext& t,std::vector<Strategy>& lt,std::vector<SingleBind>& cc,bool& valid) : tracks(t) {
//    if (lt.size()!=gt.size()) throw;
//    partials   = new Statement[ara];
    ara = lt.size();
    localtypes = new Strategy[ara];
    for (int a=0;a<ara;a++) {
        localtypes[a] = lt[a];
    }
    tracks.dat[tracks.loc()] = std::pair<Strategy*,int>(localtypes,ara);
    
    for (int u=0;u<cc.size();u++) {
        ParameterContext conan = tracks.append(cc[u].itinerary,cc[u].ara);
        Strategy calctype = conan.generateType(cc[u].head);
        if (!typebind(cc[u].body,calctype.type,conan)) valid=false;
        binds.push_back(cc[u]);
    }
}
//Binding::Binding(int buf) : localtypes(new Strategy[buf]),partials(new Statement[buf]),ara(buf) {
//    tracks.push_back(std::pair<Strategy*,int>(localtypes,ara));
//    partials = new Statement[ara];
//    for (int u=0;u<ara;u++) {
//        partials[u] = Statement(-1,tracks.size()-1);
//    }
//}

Binding::~Binding() {

}

//Statement::Statement(int idr,int loc) {
//    local=loc;id=idr;
//}
void Statement::cleanup() {
    for (int v=0;v<ara;v++) {
        args[v].cleanup();
    }
    local=9001;
    id=900;
    if (ara) delete[] args;
}
void Strategy::cleanup() {
    for (int v=0;v<ara;v++) {
        args[v].cleanup();
    }
    type.cleanup();
    local=9001;
    id=900;
    delete[] args;
}
Statement Statement::deepcopy() {
    Statement res = Statement(id,local,ara);
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].deepcopy();
    }
    return res;
}
Strategy Strategy::deepcopy() {
    Strategy res = Strategy(type.deepcopy(),id,local,ara);
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].deepcopy();
    }
    return res;
}
Statement::Statement() {}
Statement::Statement(int idr,int loc) {
    local=loc;id=idr;
}
Statement::Statement(int idr,int loc,int buf) {
    local=loc;id=idr;ara=buf;
    args = new Statement[buf];
}
Statement::Statement(int idr,int loc,Statement a) {
    local=loc;id=idr;ara=1;
    args = new Statement[1];
    args[0] = a;
}
Statement::Statement(int idr,int loc,Statement a,Statement b) {
    local=loc;id=idr;ara=2;
    args = new Statement[2];
    args[0] = a;
    args[1] = b;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c) {
    local=loc;id=idr;ara=3;
    args = new Statement[3];
    args[0] = a;
    args[1] = b;
    args[2] = c;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c,Statement d) {
    local=loc;id=idr;ara=4;
    args = new Statement[4];
    args[0] = a;
    args[1] = b;
    args[2] = c;
    args[3] = d;
}
Strategy::Strategy() {}
Strategy::Strategy(Statement ty,int idr,int loc) {
    local=loc;id=idr;type=ty;
}
Strategy::Strategy(Statement ty,int idr,int loc,int buf) {
    local=loc;id=idr;type=ty;ara=buf;
    if (buf) args = new Strategy[buf];
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a) {
    local=loc;id=idr;type=ty;ara=1;
    args = new Strategy[1];
    args[0] = a;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b) {
    local=loc;id=idr;type=ty;ara=2;
    args = new Strategy[2];
    args[0] = a;
    args[1] = b;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c) {
    local=loc;id=idr;type=ty;ara=3;
    args = new Strategy[3];
    args[0] = a;
    args[1] = b;
    args[2] = c;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c,Strategy d) {
    local=loc;id=idr;type=ty;ara=4;
    args = new Strategy[4];
    args[0] = a;
    args[1] = b;
    args[2] = c;
    args[3] = d;
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
//bool Statement::containsloop(int a,int sdepth) {
//    if (local==sdepth and id==a) return true;
//    for (int q=0;q<ara;q++) {
//        if (args[q].containsloop(a,sdepth)) return true;
//    }
//    return false;
//}

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

//Statement Statement::scramble(std::map<int,int>& mapr,int& push,int sdepth) {
//    Statement res = Statement(id,local,ara);
//    if (local==sdepth) {
//        if (mapr.find(id)!=mapr.end()) {
//            res.id=mapr[id];
//        } else {
//            res.id=push++;
//            mapr[id]=res.id;
//        }
//    }
//    for (int q=0;q<ara;q++) {
//        res.args[q] = args[q].scramble(mapr,push,sdepth);
//    }
//    return res;
//}
//void Statement::unscramble(std::map<int,int>& mapr,int& push,int sdepth) {
//    if (local==sdepth) {
//        if (mapr.find(id)==mapr.end()) {
//            mapr[id]=push++;
//        }
//        id=mapr[id];
//    }
//    for (int q=0;q<ara;q++) {
//        args[q].unscramble(mapr,push,sdepth);
//    }
//}


//Soln::Soln() : expanded(true),head(Statement(-1,0)) {}
SolveInstance::SolveInstance(Statement point,Strategy* types,int arat) : mainsoln(point,types,arat) {}
Soln::Soln(Statement point,Strategy* types,int arat) : head(point),loctypes(types),ara(arat) {}

//Statement Soln::getsolution() {
//    for (int y=0;y<bin.size();y++) {
//        if (bin[y]->downstream.size()==0) {
////            std::cout<<bin[y]->bind.tostringheavy()<<"asdf\n";
////            return head->substitute(&bin[y]->bind);
//        }
//    }
//    return Statement();
//}
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
//void Statement::local_list(std::vector<Statement*>* list) {
//    if (local==1) {
//        list->push_back(this);
//    }
//    for (int q=0;q<args.size();q++) {
//        args[q]->local_list(list);
//    }
//}
//bool amorphousmatch(Statement a,Statement b,std::map<int,int>& forward,std::map<int,int>& backward,std::vector<std::pair<Strategy*,int>> params) {
//    if (a.local!=b.local or a.ara!=b.ara) return false;
//    if (a.local==1) {
//        if (forward.find(a.id) !=forward.end()  and forward[a.id]!=b.id) return false;
//        if (backward.find(b.id)!=backward.end() and backward[b.id]!=a.id) return false;
//        forward[a.id]=b.id;
//        backward[b.id]=a.id;
//    } else if (a.id!=b.id) return false;
//    for (int i=0;i<a.ara;i++) {
//        if (!amorphousmatch(a.args[i],b.args[i],forward,backward)) return false;
//    }
//    Statement ltype = a.generate_type(params);
//    Statement rtype = b.generate_type(params);
//    if (!amorphousmatch(ltype,rtype,forward,backward,params)) return false;
//    ltype.cleanup();
//    rtype.cleanup();
//    
////    if ((a->type!=Statement::universe or b->type!=Statement::universe) and !amorphousmatch(a->type,b->type,forward,backward)) return false;
//    return true;
//}


ParameterContext ParameterContext::append(Strategy a) {
    return append(a.args,a.ara);
}
ParameterContext ParameterContext::append(Strategy* l,int buf) {
    ParameterContext nn = *this;
    nn.dat.push_back(std::pair<Strategy*,int>(l,buf));
    return nn;
}
int ParameterContext::loc() {
    return dat.size()-1;
}
Strategy ParameterContext::generateType(Statement a) {

    if (a.local==-1) return Strategy(Statement(1,0),a.id,a.local);
    if (a.local>=dat.size() or a.local<0) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
//    std::string es = dat[a.local].first[a.id].tostring();
//    std::string ex = dat[a.local].first[a.id].bring_depth(lloc+1,loc()-lloc).tostring();
//    std::string ea = a.tostring();

//    Strategy tent = dat[a.local].first[a.id].bring_depth(lloc+1,loc()-lloc).typechecksub(a.args,a.ara,loc()+1,0);
//    std::cout<<a.tostring()<<"\n";
    Strategy tent = dat[a.local].first[a.id].bring_depth(lloc+1,loc()-lloc).typechecksub_1disp(a.args,a.ara,loc()+1,0);//depth push is negative here. This causes crash.
//    Statement testjusttype = tent.type.depth_push(loc()+1,);
//    std::cout<<dat.size()<<"|"<<tent.type.tostring()<<"\n";
//    if (tent.type.tostring().size()>10) throw;
//    std::string eee = tent.tostring();

//    tent.type.constcheck(*this);
    
//    for (int o=0;o<tent.ara;o++) {
//        tent.args[o].typecheck(*this);
//    }
    
    return tent;//it's incrementing things it thinks it contains
//    return dat[a.local].first[a.id].typechecksub(a.args,a.ara,lloc+1,(int)dat.size(),1);
}
Strategy ParameterContext::generateTypeSection(Statement a,int arg) {
    if (a.local>=dat.size() or a.local<0 or a.ara<=arg) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
    return dat[a.local].first[a.id].args[arg].bring_depth(lloc+1,loc()-lloc).typechecksub_1disp(a.args,arg,loc()+1,1);
}

bool judgemental_eq(Strategy a,Strategy b) {
    if (a.id!=b.id or a.local!=b.local) return false;
    if (a.ara!=b.ara) throw;
    for (int k=0;k<a.ara;k++) {
        if (!judgemental_eq(a.args[k],b.args[k])) return false;
    }
    return judgemental_eq(a.type,b.type);
}




SolnLink::SolnLink(std::vector<int> dfa,Soln* link,Entry* a) : mapr(dfa),container(a),linked(link) {}

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








