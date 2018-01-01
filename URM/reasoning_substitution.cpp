//
//  reasoning_substitution.cpp
//  URM
//
//  Created by Parker on 9/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

Statement Statement::depth_push(int cutoff,int amt) {//anything >=stat will be incremented.
    Statement res = Statement(id,local>=cutoff?local+amt:local,ara);
    if (local==cutoff and amt<0) throw;
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].depth_push(cutoff,amt);
    }
    return res;
}
void Statement::clip_upperbound(int stat,int stmodif,std::map<std::pair<int,int>,int>& remap,int& mappoint) {
    if (local>stmodif and local<stat) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            remap[std::pair<int,int>(id,local)] = mappoint++;
        }
    }
    for (int u=0;u<ara;u++) {
        args[u].clip_upperbound(stat,stmodif,remap,mappoint);
    }
}

Statement Strategy::snapshot() {//this really concerns me
//    throw;
    Statement res = Statement(id,local,ara);
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].snapshot();
    }
    return res;
}

//std::vector<Strategy*>& poptypes, etc
//int& remapper,bool pair

//Statement* Strategy::locsnapshot(std::vector<Statement*>* prepend,bool pair,int stat,std::vector<Strategy*>& poptypes) {
//    Statement* res = new Statement(id,local);
//    if (pair) {
//        res->local = 1;
//        res->id = poptypes.size();
//        Strategy* ret = deepcopy();
//        if (prepend) {
//            for (int j=0;j<prepend->size();j++) {
//                res->args.push_back((*prepend)[j]->deepcopy());
//            }
//        }
//        poptypes.push_back(ret);
////        popprin.push_back(locsnapshot(prepend,true,1,nextid));
//    }
//    for (int q=0;q<args.size();q++) {
//        res->args.push_back(args[q]->locsnapshot(prepend,!pair,stat+1,poptypes));
//    }
//    return res;
//}
//Statement* Strategy::locsnapshot(std::vector<Statement*>* prepend,bool pair,int stat,int& paint) {
//    Statement* res = new Statement(id,stat);
//    if (pair) {
//        res->local = 1;
//        res->id = paint++;
//        if (prepend) {
//            for (int j=0;j<prepend->size();j++) {
//                res->args.push_back((*prepend)[j]->deepcopy());
//            }
//        }
//    }
//    for (int q=0;q<args.size();q++) {
//        res->args.push_back(args[q]->locsnapshot(prepend,!pair,stat+1,paint));
//    }
//    return res;
//}
Statement Statement::paste_upperbound(int stat,std::map<std::pair<int,int>,int>& remap,int general) {//anything <=stat will be replaced.
    Statement res = Statement(id,local,ara);
    if (local>general+1 and local<stat) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            return Statement();
        }
        res.local = general;
        res.id = remap[std::pair<int,int>(id,local)];
    }
    for (int q=0;q<ara;q++) {
        Statement possib = args[q].paste_upperbound(stat,remap,general);
        if (!possib.is_valid()) {
            for (int o=0;o<q;o++) {
                res.args[0].cleanup();
            }
            delete[] res.args;
            return Statement();
        }
        res.args[q] = possib;
    }
    return res;
}

std::string Statement::substitute_tostring(Binding* bind) {
    std::vector<Statement> din;
    substitute(bind,bind->tracks,din);
    std::string res = din[0].tostring();
    for (int y=0;y<din.size();y++) {
        din[y].cleanup();
    }
    return res;
}
void Statement::substitute(Binding* bind,ParameterContext& params,std::vector<Statement>& results) {
    bool trash;
    substitute(bind,params,results,-1,trash);
}
//Statement gapify(Statement p,Binding* o,int rec) {
//    throw;//Any place you can use gapify, you can just repeatedly substitute with the same parameters. This saves cycles too.
//    
////    if ((p->local<=0 or p->id==-1) and p->args.size()==0) return p;
//    Statement res = Statement(p.id,p.local,p.ara);
//    for (int u=0;u<p.ara;u++) {
//        res.args[u] = gapify(p.args[u],o,rec+1);
//    }
//    if (res.local==o->tracks.loc()) {
////        if (o->partials[res->id]->is_complete(o->tracks.loc())) {
//        throw;
//        return gapify(o->partials[res.id],o,o->tracks.loc()+2)
//            .depth_push(o->tracks.loc()+1,rec-o->tracks.loc()-1)
//            .substitute_level(res.args,res.ara,rec,0);
////        }
//    }
//    return res;
//}
bool containsdepth(Statement a,int d) {
    if (a.local==d) return true;
    for (int j=0;j<a.ara;j++) {
        if (containsdepth(a.args[j],d)) return true;
    }
    return false;
}
void Statement::substitute(Binding* bind,ParameterContext& params,std::vector<Statement>& results,int avoidex,bool& changed) {
    #define out (local==bind->tracks.loc()?buffer:results)
    if ((local<=0 or id==-1) and ara==0) {results.push_back(*this);return;}
    std::vector<Statement> buffer;
    
    Strategy calctype = params.generateType(*this);
    std::vector<std::vector<Statement>> cartesian;
    
    for (int q=0;q<ara;q++) {
        ParameterContext continued = params.append(calctype.args[q]);
        cartesian.push_back(std::vector<Statement>());
        args[q].substitute(bind,continued,cartesian[q],-1,changed);
    }
    int cmax = 1;
    for (int v=0;v<ara;v++) {
        cmax*=cartesian[v].size();
    }
    for (int c=0;c<cmax;c++) {
        int ca = c;
        Statement nn = Statement(id,local,ara);
        for (int v=0;v<ara;v++) {
            nn.args[v] = cartesian[v][ca%cartesian[v].size()].deepcopy();
            ca/=cartesian[v].size();
        }
        out.push_back(nn);
    }
    for (int v=0;v<ara;v++) {
        for (int y=0;y<cartesian[v].size();y++) {
            cartesian[v][y].cleanup();
        }
    }
    bool found=false;
    for (int g=0;g<buffer.size();g++) {
        if (bind->partials[id].is_complete(bind->tracks.loc())) {
            changed=true;
            Statement pushd = bind->partials[id].depth_push(bind->tracks.loc()+1,params.loc()-bind->tracks.loc());
            
//            Statement incpart = soap.partials[soap.binds[w].head.id].depth_push(tracks.loc()+1,1);
////            std::cout<<incpart.tostring()<<"\n";
//            Statement compare = incpart.substitute_level(soap.binds[w].head.args,soap.binds[w].head.ara,tracks.loc()+2,0).depth_push(tracks.loc()+2,-1);
            
            
            
            
            
            results.push_back(pushd.substitute_level(buffer[g].args,buffer[g].ara,params.loc()+1,0).depth_push(params.loc()+1,-1));
            return;
        }// else if (gapyield) {
//            results.push_back(bind->partials[id]->substitute_level(&buffer[g]->args,bind->tracks.loc()+1,params.loc()+1,1));
//            changed=true;
//        }
//        std::cout<<bind->tostring()<<"ajklsdjlkfalsflkalksljafkl;\n";
        for (int s=0;s<bind->binds.size();s++) {
        
            if (bind->binds[s].head.id==id and bind->binds[s].body.local!=bind->tracks.loc() and bind->binds[s].body.is_complete(bind->tracks.loc())) {
                if (s==avoidex) continue; else if (s<=avoidex) {
                    Binding reverse(params,bind->binds[avoidex].itinerary,bind->binds[avoidex].ara);
                    reverse.tracks.dat[reverse.tracks.loc()+1-2] = std::pair<Strategy*,int>(bind->binds[s].itinerary,bind->binds[s].ara);
                    if (reverse.decompose(bind->binds[s].head.depth_push(bind->tracks.loc()+1,1),buffer[g].depth_push(bind->tracks.loc()+2,1))) continue;
                }
                Binding comparison = Binding(params,bind->binds[s].itinerary,bind->binds[s].ara);
                if (comparison.decompose(
                bind->binds[s].head.depth_push(bind->tracks.loc()+1,params.loc()-bind->tracks.loc()),
                buffer[g].depth_push(params.loc()+1,1)
                )) {
                    std::vector<Binding> switcher;
                    if (comparison.binds.size()) throw;
                    comparison.divide(switcher,5);
                    for (int o=0;o<switcher.size();o++) {//this all needs to go; based on invalid assumption that comparison doesn't have params behind it.
                        Statement result = bind->binds[s].body.depth_push(bind->tracks.loc()+1,params.loc()-bind->tracks.loc());
                        while (containsdepth(result,comparison.tracks.loc())) {
                            std::cout<<bind->binds[s].body.tostring()<<"\n";
                            std::cout<<result.tostring()<<"\n";
//                            throw;//this is completely fucked up.
                            Statement kkr = result;
                            result = result.substitute_level_verbal(comparison.partials,comparison.ara,comparison.tracks.loc(),0,3);
                            throw;
                            kkr.cleanup();
                        }
                        results.push_back(result.depth_push(comparison.tracks.loc(),-1));
                    }
                    
                    changed=true;
                    found=true;
                }
            }
        }
    }
    if (!found) {
        for (int k=0;k<buffer.size();k++) {
            results.push_back(buffer[k]);
        }
    }
    #undef out
    #undef CAR1
    #undef CAR2
}
//Statement Statement::substitute_level(Statement* repl,int safety,int level,ParameterContext& initialcontext,ParameterContext& finalcontext) {
//    //some part of initialcontext is level. it starts out as the end, but this changes. this is waht recur is.
//
//}

Statement Statement::substitute_level_verbal(Statement* repl,int safety,int level,int recur,int tabs) {
    for (int j=0;j<tabs;j++) {
        std::cout<<"\t";
    }
    std::cout<<"Subbing(lvl "<<level<<"): "<<tostring()<<" with [";
    for (int p=0;p<safety;p++) {
        std::cout<<repl[p].tostring()<<",";
    }
    std::cout<<"]\n";
    return substitute_level(repl,safety,level,recur);
}
Statement substitute_level_1disp(Statement jaj,Statement* repl,int safety,int level,int recur) {
    return jaj.depth_push(level,1).substitute_level(repl,safety,level+1,recur).depth_push(level+1,-1);
}
Statement Statement::substitute_level(Statement* repl,int safety,int level,int recur) {
    if (local==level) {
        if (safety<=id) {
            throw;
        }
        Statement* fargs = new Statement[ara];
        for (int s=0;s<ara;s++) {
            fargs[s] = args[s].substitute_level(repl,safety,level,recur+1);
        }
//        Statement a = repl[id].depth_push(level,recur+1).substitute_level(fargs,ara,level+recur+1,0).depth_push(level+recur+1,-1);
        Statement a = substitute_level_1disp(repl[id].depth_push(level,recur),fargs,ara,level+recur,0);
        for (int s=0;s<ara;s++) {
            fargs[s].cleanup();
        }
        return a;
    } else {
        Statement ret = Statement(id,local,ara);
        for (int w=0;w<ara;w++) {
            ret.args[w] = args[w].substitute_level(repl,safety,level,recur+1);
        }
        return ret;
    }
}
Strategy Strategy::typechecksub_1disp(Statement* repl,int safety,int level,int recur) {
    Strategy res = Strategy(type.substitute_level(repl,safety,level,recur).depth_push(level,-1),id,local,ara);
    for (int w=0;w<ara;w++) {
        res.args[w] = args[w].typechecksub_1disp(repl,safety,level,recur+1);
    }
    return res;
}
Strategy Strategy::bring_depth(int cutoff,int depth) {
    Strategy res = Strategy(type.depth_push(cutoff,depth),id,local+depth,ara);//>=cutoff; cutoff=stmodif+1,depth=stmodif+1-loc
    for (int w=0;w<ara;w++) {
        res.args[w] = args[w].bring_depth(cutoff,depth);
    }
    return res;
}





Statement Statement::applyreverse(std::vector<int>& transfer) {
    Statement res = Statement(id,local,ara);
    if (local==1) {
        bool found = false;
        for (int k=0;k<transfer.size();k++) {
            if (transfer[k]==id) {
                res.id=k;
                found=true;
            }
        }
        if (!found) {
            throw;
        }
    }
    for (int u=0;u<ara;u++) {
        res.args[u] = args[u].applyreverse(transfer);
    }
    return res;
}
Statement Statement::apply(std::vector<int>& transfer) {
    Statement res = Statement(id,local,ara);
    if (local==1) {
        if (id>=transfer.size()) {
            throw;
        }
        res.id=transfer[id];
    }
    for (int u=0;u<ara;u++) {
        res.args[u] = args[u].apply(transfer);
    }
    return res;
}
Strategy Strategy::applyreverse(std::vector<int>& transfer) {
    Strategy res = Strategy(type.applyreverse(transfer),id,local,ara);
    if (local==1) {
        bool found = false;
        for (int k=0;k<transfer.size();k++) {
            if (transfer[k]==id) {
                res.id=k;
                found=true;
            }
        }
        if (!found) {
            throw;
        }
    }
    for (int u=0;u<ara;u++) {
        res.args[u] = args[u].applyreverse(transfer);
    }
    return res;
}
Strategy Strategy::apply(std::vector<int>& transfer) {
    Strategy res = Strategy(type.apply(transfer),id,local,ara);
    if (local==1) {
        if (id>=transfer.size()) {
            throw;
        }
        res.id=transfer[id];
    }
    for (int u=0;u<ara;u++) {
        res.args[u] = args[u].apply(transfer);
    }
    return res;
}
void Statement::extract(std::vector<int>& transfer,ParameterContext& params) {
    if (local==1) {
        bool found = false;
        for (int k=0;k<transfer.size();k++) {
            if (transfer[k]==id) found = true;
        }
        if (!found) {
            transfer.push_back(id);
        }
    }
    Strategy calctype = params.generateType(*this);
    for (int u=0;u<ara;u++) {
        ParameterContext continued = params.append(calctype.args[u]);
        args[u].extract(transfer,continued);
    }
    if (local==1) {
        calctype.type.extract(transfer,params);
    }
}


