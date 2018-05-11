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
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].depth_push(cutoff,amt);
    }
    return res;
}
//void Statement::clip_upperbound(int stat,int loc,std::map<std::pair<int,int>,int>& remap,int& mappoint) {
//    if (local>loc and local<stat) {
//        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
//            remap[std::pair<int,int>(id,local)] = mappoint++;
//        }
//    }
//    for (int u=0;u<ara;u++) {
//        args[u].clip_upperbound(stat,loc,remap,mappoint);
//    }
//}

Statement snapshothelper(int td,Strategy& ob) {
    Statement res = Statement(ob.id,td,ob.ara);
    for (int q=0;q<ob.ara;q++) res.args[q] = snapshothelper(td+1,ob.args[q]);
    return res;
}
Statement Strategy::snapshot(int depth) {
    Statement res = Statement(id,local,ara);
    for (int q=0;q<ara;q++) res.args[q] = snapshothelper(depth,args[q]);
    return res;
}

//Statement Statement::paste_upperbound(int stat,std::map<std::pair<int,int>,int>& remap,int loc) {//anything <=stat will be replaced.
//    Statement res = Statement(id,local,ara);
//    if (local>loc and local<stat) {
//        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
//            return Statement();
//        }
//        res.local = loc+1;
//        res.id = remap[std::pair<int,int>(id,local)];
//    }
//    for (int q=0;q<ara;q++) {
//        Statement possib = args[q].paste_upperbound(stat,remap,loc);
//        if (!possib.is_valid()) {
//            for (int o=0;o<q;o++) {
//                res.args[0].cleanup();
//            }
//            delete[] res.args;
//            return Statement();
//        }
//        res.args[q] = possib;
//    }
//    return res;
//}
Statement Statement::substitute_single(Binding& bind,ParameterContext& params) {
    Statement res = Statement(id,local,ara);
    Strategy calctype = params.generateType(*this);
    for (int h=0;h<ara;h++) {
        ParameterContext nn = params.append(calctype.args[h]);
        res.args[h] = args[h].substitute_single(bind,nn);
    }
    if (local==bind.tracks.loc()) {
        for (int s=0;s<bind.binds.size();s++) {
            if (bind.binds[s].head.id==id and bind.binds[s].body.local!=bind.tracks.loc()) {
                if (res.ara==0) {
                    return bind.binds[s].body.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                }
                if (bind.binds[s].universal) {
                    return bind.binds[s].body.ndisp_sub(bind.tracks.loc()+1,params.loc()-bind.tracks.loc(),res.args,res.ara,0);
                }
                Binding comparison = Binding(params,bind.binds[s].itinerary,bind.binds[s].ara);
                Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                Statement rightdec = res.depth_push(params.loc()+1,1);
                if (comparison.decompose(leftdec,rightdec,comparison.tracks)) {
                    std::vector<Binding> switcher;
                    comparison.divide(switcher,-1);
                    if (switcher.size()) {
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
Statement Statement::simp_substitute(Binding& bind,ParameterContext& params,int avoidex) {
    Statement res = Statement(id,local,ara);
    Strategy calctype = params.generateType(*this);
    for (int q=0;q<ara;q++) {
        ParameterContext continued = params.append(calctype.args[q]);
        res.args[q] = args[q].substitute_single(bind,continued);
    }
    if (res.local==bind.tracks.loc()) {
        for (int s=0;s<bind.binds.size();s++) {
            if (bind.binds[s].head.id==id and bind.binds[s].body.local!=bind.tracks.loc()) {
                if (s==avoidex) continue;
                if (res.ara==0) {
                    if (s<avoidex) continue;
                    return bind.binds[s].body.depth_push(bind.tracks.loc()+1,params.loc()-bind.tracks.loc());
                }
                if (bind.binds[s].universal) {
                    return bind.binds[s].body.ndisp_sub(bind.tracks.loc()+1,params.loc()-bind.tracks.loc(),res.args,res.ara,0);
                }
                if (s<avoidex) {
//                    if (bind.binds[s].universal) continue;
                    Binding reverse(params,bind.binds[avoidex].itinerary,bind.binds[avoidex].ara);
                    reverse.tracks.dat[reverse.tracks.loc()+1] = std::pair<Strategy*,int>(bind.binds[s].itinerary,bind.binds[s].ara);
                    Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+2,1);
                    Statement rightdec = res.depth_push(bind.tracks.loc()+1,1);
                    std::vector<Binding> testing;
                    if (reverse.decompose(leftdec,rightdec,reverse.tracks)) {
                        reverse.divide(testing,-1);
                        if (testing.size()) continue;
                    }
                }
                Binding comparison = Binding(params,bind.binds[s].itinerary,bind.binds[s].ara);
                Statement leftdec = bind.binds[s].head.depth_push(bind.tracks.loc()+1,1);
                Statement rightdec = res.depth_push(bind.tracks.loc()+2,1);
                if (comparison.decompose(leftdec,rightdec,comparison.tracks)) {
                    std::vector<Binding> switcher;
                    comparison.divide(switcher,-1);
                    if (switcher.size()) {
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


void Statement::inplace_sub(int tid,int tlocal,Statement& templ,int tdepth) {
    if (local==tlocal and id==tid) {
        Statement dupr = templ.ndisp_sub(tlocal+1,tdepth-tlocal-1,args,ara,0);
        cleanup();
        *this = dupr;
    } else for (int a=0;a<ara;a++) args[a].inplace_sub(tid,tlocal,templ,tdepth+1);
}

void Strategy::inplace_sub(int tid,int tlocal,Statement& templ) {
    type.inplace_sub(tid,tlocal,templ,local+2);
    for (int a=0;a<ara;a++) args[a].inplace_sub(tid,tlocal,templ);
}
//Statement Statement::substitute_level(Statement* repl,int safety,int level,ParameterContext& initialcontext,ParameterContext& finalcontext) {
//    //some part of initialcontext is level. it starts out as the end, but this changes. this is waht recur is.
//
//}

//Statement Statement::substitute_level_verbal(Statement* repl,int safety,int level,int recur,int tabs) {
//    for (int j=0;j<tabs;j++) {
//        std::cout<<"\t";
//    }
//    std::cout<<"Subbing(lvl "<<level<<"): "<<tostring()<<" with [";
//    for (int p=0;p<safety;p++) {
//        std::cout<<repl[p].tostring()<<",";
//    }
//    std::cout<<"]\n";
//    return substitute_level(repl,safety,level,recur);
//}
//Statement substitute_level_1disp(Statement jaj,Statement* repl,int safety,int level,int recur) {
//    return jaj.depth_push(level,1).substitute_level(repl,safety,level+1,recur).depth_push(level+1,-1);
//}
//Statement Statement::substitute_level(Statement* repl,int safety,int level,int recur) {
//    if (local==level) {
//        if (safety<=id) throw;
//        Statement* fargs = new Statement[ara];
//        for (int s=0;s<ara;s++) fargs[s] = args[s].substitute_level(repl,safety,level,recur+1);
////        Statement a = repl[id].depth_push(level,recur+1).substitute_level(fargs,ara,level+recur+1,0).depth_push(level+recur+1,-1);
//        Statement a = substitute_level_1disp(repl[id].depth_push(level,recur),fargs,ara,level+recur,0);
//        for (int s=0;s<ara;s++) fargs[s].cleanup();
//        return a;
//    } else {
//        Statement ret = Statement(id,local,ara);
//        for (int w=0;w<ara;w++) ret.args[w] = args[w].substitute_level(repl,safety,level,recur+1);
//        return ret;
//    }
//}
Statement Statement::ndisp_sub(int slevel,int inc,Statement* repl,int safety,int recur) {
    if (local==slevel) {
        if (safety<=id) throw;
        Statement* fargs = new Statement[ara];
        for (int s=0;s<ara;s++) fargs[s] = args[s].ndisp_sub(slevel,inc,repl,safety,recur+1);
//        Statement a = repl[id].depth_push(level,recur+1).substitute_level(fargs,ara,level+recur+1,0).depth_push(level+recur+1,-1);
//        Statement a = substitute_level_1disp(repl[id].depth_push(level,recur),fargs,ara,level+recur,0);
//        Statement a = repl[id].depth_push(slevel+inc,recur+1).substitute_level(fargs,ara,,0);
        Statement a = repl[id].ndisp_sub(slevel+inc,recur,fargs,ara,0);//reduce
        for (int s=0;s<ara;s++) fargs[s].cleanup();
        return a;
    } else {
        Statement ret = Statement(id,local>slevel?local+inc-1:local,ara);
        for (int w=0;w<ara;w++) ret.args[w] = args[w].ndisp_sub(slevel,inc,repl,safety,recur+1);
        return ret;
    }
}
Strategy Strategy::ndisp_sub(int slevel,int inc,Statement* repl,int safety,int recur) {
    Strategy res = Strategy(type.ndisp_sub(slevel,inc,repl,safety,recur),id,local+inc-1,ara);
    for (int w=0;w<ara;w++) res.args[w] = args[w].ndisp_sub(slevel,inc,repl,safety,recur+1);
    return res;
}
//Strategy Strategy::typechecksub_1disp(Statement* repl,int safety,int level,int recur) {
//    Strategy res = Strategy(type.substitute_level(repl,safety,level,recur).depth_push(level,-1),id,local-1,ara);
//    for (int w=0;w<ara;w++) {
//        res.args[w] = args[w].typechecksub_1disp(repl,safety,level,recur+1);
//    }
//    return res;
//}
Strategy Strategy::bring_depth(int cutoff,int depth) {
    Strategy res = Strategy(type.depth_push(cutoff,depth),id,local+depth,ara);//>=cutoff; cutoff=stmodif+1,depth=stmodif+1-loc
    for (int w=0;w<ara;w++) {
        res.args[w] = args[w].bring_depth(cutoff,depth);
    }
    return res;
}





//Statement Statement::applyreverse(std::vector<int>& transfer) const {
//    Statement res = Statement(id,local,ara);
//    if (local==1) {
//        bool found = false;
//        for (int k=0;k<transfer.size();k++) {
//            if (transfer[k]==id) {
//                res.id=k;
//                found=true;
//            }
//        }
//        if (!found) {
//            throw;
//        }
//    }
//    for (int u=0;u<ara;u++) {
//        res.args[u] = args[u].applyreverse(transfer);
//    }
//    return res;
//}
//Statement Statement::apply(std::vector<int>& transfer,int omm) const {
//    Statement res = Statement(id,local,ara);
//    if (local==1) {
//        if (id>=transfer.size()) res.id=id+omm-transfer.size();
//        else res.id=transfer[id];
//    }
//    for (int u=0;u<ara;u++) {
//        res.args[u] = args[u].apply(transfer,omm);
//    }
//    return res;
//}
//Strategy Strategy::applyreverse(std::vector<int>& transfer) {
//    Strategy res = Strategy(type.applyreverse(transfer),id,local,ara);
//    if (local==1) {
//        bool found = false;
//        for (int k=0;k<transfer.size();k++) {
//            if (transfer[k]==id) {
//                res.id=k;
//                found=true;
//            }
//        }
//        if (!found) {
//            throw;
//        }
//    }
//    for (int u=0;u<ara;u++) {
//        res.args[u] = args[u].applyreverse(transfer);
//    }
//    return res;
//}
//Strategy Strategy::apply(std::vector<int>& transfer,int omm) {
//    Strategy res = Strategy(type.apply(transfer,omm),id,local,ara);
//    if (local==1) {
//        if (id>=transfer.size()) res.id=id+omm-transfer.size();
//        else res.id=transfer[id];
//    }
//    for (int u=0;u<ara;u++) {
//        res.args[u] = args[u].apply(transfer,omm);
//    }
//    return res;
//}
Strategy Strategy::substitute_single(Binding& bind,ParameterContext& params) {
    ParameterContext nn = params.append(args,ara);
    Strategy res = Strategy(type.substitute_single(bind,nn),id,local,ara);
    for (int u=0;u<ara;u++) res.args[u] = args[u].substitute_single(bind,nn);
    return res;
}
//void Statement::extract(std::vector<int>& transfer,Binding& reference) {
//    if (local==1) reference.localtypes[id].substitute_single(reference,reference.tracks).extract(transfer,reference);
//    for (int u=0;u<ara;u++) args[u].extract(transfer,reference);
//}
//void Strategy::extract(std::vector<int>& transfer,Binding& reference) {
//    for (int u=0;u<ara;u++) args[u].extract(transfer,reference);
//    type.extract(transfer,reference);
//    if (local==1) {
//        for (int d=0;d<reference.binds.size();d++) {
//            if (reference.binds[d].head.id==id and reference.binds[d].body.local!=1) {
//                reference.binds[d].body.extract(transfer,reference);
//                for (int x=0;x<reference.binds[d].ara;x++) {
//                    reference.binds[d].itinerary[x].extract(transfer,reference);
//                }
//            }
//        }
//        bool found = false;
//        for (int k=0;k<transfer.size();k++) {
//            if (transfer[k]==id) found = true;
//        }
//        if (!found) {
//            transfer.push_back(id);
//        }
//    }
//}


