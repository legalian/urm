//
//  reasoning_substitution.cpp
//  URM
//
//  Created by Parker on 9/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

Statement Statement::depth_push(int cutoff,int amt) const {//anything >=stat will be incremented.
    Statement res = Statement(id,local>=cutoff?local+amt:local,ara);
    if (amt<0 and local>=cutoff and local+amt<cutoff) throw;
    for (int q=0;q<ara;q++) res.args[q] = args[q].depth_push(cutoff,amt);
    return res;
}
Statement snapshothelper(int td,const Strategy& ob) {
    Statement res = Statement(ob.id,td,ob.ara);
    for (int q=0;q<ob.ara;q++) res.args[q] = snapshothelper(td+1,ob.args[q]);
    return res;
}
Statement Strategy::snapshot(int depth) const {
    Statement res = Statement(id,local,ara);
    for (int q=0;q<ara;q++) res.args[q] = snapshothelper(depth,args[q]);
    return res;
}
Statement Statement::substitute_single(const Binding& bind,const ParameterContext& params) const {
    Statement res = Statement(id,local,ara);
    Strategy calctype = params.generateType(*this);
    for (int h=0;h<ara;h++) {
        ParameterContext nn = params.append(calctype.args[h]);
        res.args[h] = args[h].substitute_single(bind,nn);
    }
    calctype.cleanup();
    if (local==bind.tracks.loc()) {
        for (int s=0;s<bind.binds.size();s++) {
            if (bind.binds[s].head.id==id and bind.binds[s].body.local!=bind.tracks.loc()) {
                if (res.ara==0) {
                    res.cleanup();
                    return bind.binds[s].body.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                }
                if (bind.binds[s].universal) {
                    Statement dres = bind.binds[s].body.ndisp_sub(bind.tracks.loc()+1,params.loc()-bind.tracks.loc(),res.args,res.ara,0);
                    res.cleanup();
                    return dres;
                }
                Binding comparison = Binding(params,::deepcopy(bind.binds[s].itinerary,bind.binds[s].ara),bind.binds[s].ara);
                Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                Statement rightdec = res.depth_push(params.loc()+1,1);
                bool decomposable = comparison.decompose(leftdec,rightdec,comparison.tracks);
                leftdec.cleanup();rightdec.cleanup();
                if (decomposable) {
                    std::vector<Binding> switcher;
                    comparison.divide(switcher,-1);
                    if (switcher.size()) {
                        res.cleanup();
                        return bind.binds[s].body
                        .depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc())
                        .substitute_single(switcher[0],switcher[0].tracks).depth_push(params.loc()+1,-1);
                    }
                }
            }
        }
    }
    return res;
}
Statement Statement::simp_substitute(const Binding& bind,const ParameterContext& params,int avoidex) const {
    Statement res = Statement(id,local,ara);
    Strategy calctype = params.generateType(*this);
    for (int q=0;q<ara;q++) {
        ParameterContext continued = params.append(calctype.args[q]);
        res.args[q] = args[q].substitute_single(bind,continued);
    }
    calctype.cleanup();
    if (res.local==bind.tracks.loc()) {
        for (int s=0;s<bind.binds.size();s++) {
            if (bind.binds[s].head.id==id and bind.binds[s].body.local!=bind.tracks.loc()) {
                if (s==avoidex) continue;
                if (res.ara==0) {
                    if (s<avoidex) continue;
                    res.cleanup();
                    return bind.binds[s].body.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                }
                if (bind.binds[s].universal) {
                    Statement resd = bind.binds[s].body.ndisp_sub(bind.tracks.loc()+1,params.loc()-bind.tracks.loc(),res.args,res.ara,0);
                    res.cleanup();
                    return resd;
                }
                if (s<avoidex) {
//                    if (bind.binds[s].universal) continue;
                    Binding reverse(params,bind.binds[avoidex].itinerary,bind.binds[avoidex].ara);
                    reverse.tracks.dat[reverse.tracks.loc()+1] = {bind.binds[s].itinerary,bind.binds[s].ara};
                    Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+2,1);
                    Statement rightdec = res.depth_push(bind.tracks.loc()+1,1);
                    std::vector<Binding> testing;
                    bool dnu = reverse.decompose(leftdec,rightdec,reverse.tracks);
                    leftdec.cleanup();rightdec.cleanup();
                    if (dnu) {
                        reverse.divide(testing,-1);
                        if (testing.size()) continue;
                    }
                }
                Binding comparison = Binding(params,bind.binds[s].itinerary,bind.binds[s].ara);
                Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+1,1);
                Statement rightdec = res.depth_push(bind.tracks.loc()+2,1);
                bool dnu =comparison.decompose(leftdec,rightdec,comparison.tracks);
                leftdec.cleanup();rightdec.cleanup();
                if (dnu) {
                    std::vector<Binding> switcher;
                    comparison.divide(switcher,-1);
                    if (switcher.size()) {
                        res.cleanup();
                        return bind.binds[s].body
                        .depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc())
                        .substitute_single(switcher[0],switcher[0].tracks)
                        .depth_push(switcher[0].tracks.loc(),-1);
                    }
                }
            }
        }
    }
    return res;
}
void Statement::inplace_sub(int tid,int tlocal,const Statement& templ,int tdepth) {
    for (int a=0;a<ara;a++) args[a].inplace_sub(tid,tlocal,templ,tdepth+1);
    if (local==tlocal and id==tid) {
        Statement dupr = templ.ndisp_sub(tlocal+1,tdepth-tlocal-1,args,ara,0);
        cleanup();
        *this = dupr;
    }
}
void Strategy::inplace_sub(int tid,int tlocal,const Statement& templ) {
    type.inplace_sub(tid,tlocal,templ,local+2);
    for (int a=0;a<ara;a++) args[a].inplace_sub(tid,tlocal,templ);
}
Statement Statement::ndisp_sub(int slevel,int inc,Statement* repl,int safety,int recur) const {
    if (local==slevel) {
        if (safety<=id) throw;
        Statement* fargs = new Statement[ara];
        for (int s=0;s<ara;s++) fargs[s] = args[s].ndisp_sub(slevel,inc,repl,safety,recur+1);
        Statement a = repl[id].ndisp_sub(slevel+inc,recur,fargs,ara,0);//reduce
        for (int s=0;s<ara;s++) fargs[s].cleanup();
        delete[] fargs;
        return a;
    } else {
        Statement ret = Statement(id,local>slevel?local+inc-1:local,ara);
        for (int w=0;w<ara;w++) ret.args[w] = args[w].ndisp_sub(slevel,inc,repl,safety,recur+1);
        return ret;
    }
}
Strategy Strategy::ndisp_sub(int slevel,int inc,Statement* repl,int safety,int recur) const {
    Strategy res = Strategy(type.ndisp_sub(slevel,inc,repl,safety,recur),id,local+inc-1,ara);
    for (int w=0;w<ara;w++) res.args[w] = args[w].ndisp_sub(slevel,inc,repl,safety,recur+1);
    return res;
}
Strategy Strategy::bring_depth(int cutoff,int depth) const {
    Strategy res = Strategy(type.depth_push(cutoff,depth),id,local+depth,ara);//>=cutoff; cutoff=stmodif+1,depth=stmodif+1-loc
    for (int w=0;w<ara;w++) res.args[w] = args[w].bring_depth(cutoff,depth);
    return res;
}
Strategy Strategy::substitute_single(const Binding& bind,const ParameterContext& params) const {
    ParameterContext nn = params.append(args,ara);
    Strategy res = Strategy(type.substitute_single(bind,nn),id,local,ara);
    for (int u=0;u<ara;u++) res.args[u] = args[u].substitute_single(bind,nn);
    return res;
}
void Statement::apply(int loc,std::vector<int>& map) {
    if (local==loc) for (int m=0;m<map.size();m++) if (map[m]==id) {id=m;break;}
    for (int a=0;a<ara;a++) args[a].apply(loc,map);
}
void Strategy::apply(int loc,std::vector<int>& map) {
    if (local==loc) for (int m=0;m<map.size();m++) if (map[m]==id) {id=m;break;}
    type.apply(loc,map);
    for (int a=0;a<ara;a++) args[a].apply(loc,map);
}
void Statement::idpush(int loc,int amt,int thresh) {
    if (local==loc and id>=thresh) id+=amt;
    for (int a=0;a<ara;a++) args[a].idpush(loc,amt,thresh);
}
void Strategy::idpush(int loc,int amt,int thresh) {
    type.idpush(loc,amt,thresh);
    if (local==loc and id>=thresh) id+=amt;
    for (int a=0;a<ara;a++) args[a].idpush(loc,amt,thresh);
}
void expandRange(int loc,int& ara,int gu,int dep,Statement*& rang,int cara,Strategy* charms) {
    Statement* r = rang;
    rang = new Statement[ara+cara];
    for (int j=0;j<ara;j++) rang[j] = r[j];
    for (int j=0;j<cara;j++) {
        rang[ara+j] = charms[j].snapshot(dep);
        rang[ara+j].idpush(loc+1,gu,0);
    }
    delete [] r;
    ara+=cara;
}
void expandRange(int loc,int& ara,Strategy*& rang,int cara,Strategy* charms) {
    Strategy* r = rang;
    rang = new Strategy[ara+cara];
    for (int j=0;j<ara;j++) rang[j] = r[j];
    for (int j=0;j<cara;j++) {
        rang[ara+j] = charms[j].deepcopy();
        rang[ara+j].idpush(loc+1,ara,0);
    }
    delete [] r;
    ara+=cara;
}
void Statement::expandLocs(int loc,int gu,int dep,int cara,Strategy* charms) {
    for (int q=0;q<ara;q++) args[q].expandLocs(loc,gu,dep+1,cara,charms);
    if (local==loc) expandRange(loc,ara,gu,dep,args,cara,charms);
}
void Strategy::expandLocs(int loc,int gu,int cara,Strategy* charms) {
    type.expandLocs(loc,gu,local+2,cara,charms);
    for (int q=0;q<ara;q++) args[q].expandLocs(loc,gu,cara,charms);
    if (local==loc) expandRange(loc,ara,args,cara,charms);
}







