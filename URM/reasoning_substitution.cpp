//
//  reasoning_substitution.cpp
//  URM
//
//  Created by Parker on 9/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

Statement* Statement::depth_push(int cutoff,int amt) {//anything >=stat will be incremented.
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local>=cutoff?local+amt:local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->depth_push(cutoff,amt));
    }
//    if (type) res->type = type->depth_push(cutoff,amt);
//    else res->type=0;
    return res;
}
void Statement::clip_upperbound(int stat,int stmodif,std::map<std::pair<int,int>,int>& remap,int& mappoint) {
    if (local>stmodif and local<stat) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            remap[std::pair<int,int>(id,local)] = mappoint++;
        }
    }
    for (int u=0;u<args.size();u++) {
        args[u]->clip_upperbound(stat,stmodif,remap,mappoint);
    }
}

Statement* Strategy::snapshot() {
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->snapshot());
    }
    return res;
}

//std::vector<Strategy*>& poptypes, etc
//int& remapper,bool pair

Statement* Strategy::locsnapshot(std::vector<Statement*>* prepend,bool pair,int stat,std::vector<Strategy*>& poptypes,std::vector<Branches>& popprin) {
    Statement* res = new Statement(id,local);
    if (pair) {
        res->local = 1;
        res->id = poptypes.size();
        Strategy* ret = deepcopy();
        if (prepend) {
            for (int j=0;j<prepend->size();j++) {
                res->args.push_back((*prepend)[j]->deepcopy());
            }
        }
        poptypes.push_back(ret);
        int nextid = poptypes.size();
        Branches res(0);
        for (int x=0;x<args.size();x++) {
            res.branches.push_back(std::vector<int>());
            for (int y=0;y<args[x]->args.size();y++) {
                
            }
        }
        popprin.push_back(res);
//        popprin.push_back(locsnapshot(prepend,true,1,nextid));
    }
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->locsnapshot(prepend,!pair,stat+1,poptypes,popprin));
    }
    return res;
}
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
Statement* Statement::paste_upperbound(int stat,std::map<std::pair<int,int>,int>& remap,int general) {//anything <=stat will be replaced.
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    if (local==general) {
        res->local=general+1;
    }
    if (local>general and local<stat) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            return 0;
        }
        res->local = general;
        res->id = remap[std::pair<int,int>(id,local)];
    }
    for (int q=0;q<args.size();q++) {
        Statement* possib = args[q]->paste_upperbound(stat,remap,general);
        if (possib==0) {
            res->cleanup();
            return 0;
        }
        res->args.push_back(possib);
    }
    return res;
}

std::string Statement::substitute_tostring(Binding* bind) {
    std::vector<Statement*> din;
    substitute(bind,bind->tracks,din);
    return din[0]->tostring();
}
void Statement::substitute(Binding* bind,std::vector<std::vector<Strategy*>*>& params,std::vector<Statement*>& results) {
    bool trash;
    substitute(bind,params,results,-1,trash,false);
}
void Statement::substitute(Binding* bind,std::vector<std::vector<Strategy*>*>& params,std::vector<Statement*>& results,int avoidex,bool& changed,bool gapyield) {
    #define out (local==bind->tracks.size()-1?buffer:results)
    #define CAR1 (c1?cartesian1:cartesian2)
    #define CAR2 (c1?cartesian2:cartesian1)
    if (local<=0 and args.size()==0) {results.push_back(this);return;}
    Strategy* calctype = (*params[local])[id]->typechecksub(&args, (*params[local])[id]->local+1,(int)params.size(),1);
    std::vector<std::vector<Strategy*>*> continued;
    std::vector<Statement*> cartesian1;
    std::vector<Statement*> cartesian2;
    std::vector<Statement*> buffer;
    std::vector<Statement*> sb;
    bool c1=true;
    switch (args.size()) {
        case 0:
        if (local<=0) out.push_back(this);
        else out.push_back(deepcopy());
        break;
        case 1:
        continued = params;
        continued.push_back(&calctype->args[0]->args);
        args[0]->substitute(bind,continued,sb,-1,changed,gapyield);
        for (int i=0;i<sb.size();i++) out.push_back(new Statement(id,local,sb[i]));
        break;
        default:
        continued = params;
        continued.push_back(&calctype->args[0]->args);
        args[0]->substitute(bind,continued,sb,-1,changed,gapyield);
        for (int i=0;i<sb.size();i++) cartesian1.push_back(new Statement(id,local,sb[i]));
        sb.clear();
        for (int q=1;q<args.size();q++) {
            continued = params;
            continued.push_back(&calctype->args[q]->args);
            args[q]->substitute(bind,continued,sb,-1,changed,gapyield);
            for (int u=0;u<CAR1.size();u++) {
                for (int p=0;p<sb.size();p++) {
                    Statement* dup = CAR1[u]->deepcopy();
                    dup->args.push_back(sb[p]);
                    (q<args.size()-1?CAR2:out).push_back(dup);
                }
            }
            sb.clear();
            CAR1.clear();
            c1 = not c1;
        }
    }
    bool found=false;
    for (int g=0;g<buffer.size();g++) {
        if (buffer[g]->is_complete()) {
            if (bind->partials[id]->is_complete()) {
                changed=true;
                results.push_back(bind->partials[id]->substitute_level(&buffer[g]->args,2,2,1));
                return;
            } else if (gapyield) {
                results.push_back(bind->partials[id]->substitute_level(&buffer[g]->args,2,2,1));
                changed=true;
            }
            for (int s=0;s<bind->binds.size();s++) {
                if (bind->binds[s].head->id==id and bind->binds[s].body->is_complete() and bind->binds[s].body->local!=bind->tracks.size()-1) {
                    if (s==avoidex) continue; else if (s<=avoidex) {
                        Binding reverse = Binding(params,bind->binds[avoidex].itinerary,bind->binds[avoidex].principles);
                        reverse.tracks[reverse.tracks.size()-2] = &bind->binds[s].itinerary;
                        if (reverse.decompose(bind->binds[s].head->depth_push(bind->tracks.size(),1),buffer[g]->depth_push(bind->tracks.size()+1,1))) continue;
                    }
                    Binding comparison = Binding(params,bind->binds[s].itinerary,bind->binds[s].principles);
                    if (comparison.decompose(
                    bind->binds[s].head->depth_push(bind->tracks.size(),params.size()-bind->tracks.size()),
                    buffer[g]->depth_push(params.size(),1)
                    )) {//check depth matchups here... from theory
                        std::vector<Binding> switcher;
                        comparison.divide(switcher);
                        for (int o=0;o<switcher.size();o++) {
                            Statement* result = bind->binds[s].body
                                ->depth_push(bind->tracks.size(),params.size()-bind->tracks.size())
                                ->substitute_level(&comparison.partials,comparison.tracks.size()-1,comparison.tracks.size(),0);
                            if (result->is_complete()) {
                                results.push_back(result);
                            }
                        }
                        changed=true;
                        found=true;
                    }
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

Statement* Statement::substitute_level(std::vector<Statement*>* repl,int level,int reflex,int recur) {
    if (args.size()==0 and local<=0) {
        return this;
    }
    if (local==level) {
        if (repl->size()<=id) {
            throw;
        }
        std::vector<Statement*> fargs;
        for (int s=0;s<args.size();s++) {
            Statement* subbed = args[s]->substitute_level(repl,level,reflex, recur+1);
            fargs.push_back(subbed);
        }
        Statement* a = (*repl)[id]->depth_push(reflex,recur-1)->substitute_level(&fargs,reflex+recur-1,level+recur+1,0);
        for (int s=0;s<args.size();s++) {
            fargs[s]->cleanup();
        }
        return a;
    } else {
        Statement* ret = new Statement(id,local);
        for (int w=0;w<args.size();w++) {
            ret->args.push_back(args[w]->substitute_level(repl,level,reflex,recur+1));
        }
        return ret;
    }
}
Strategy* Strategy::typechecksub(std::vector<Statement*>* repl,int level,int reflex,int recur) {
    Strategy* res = new Strategy(type->substitute_level(repl,level,reflex,recur),id,local);
    for (int w=0;w<args.size();w++) {
        res->args.push_back(args[w]->typechecksub(repl,level,reflex,recur+1));
    }
    return res;
}
Strategy* Strategy::bring_depth(int cutoff,int depth) {
    Strategy* res = new Strategy(type->depth_push(cutoff,depth),id,local+depth);//>=cutoff; cutoff=stmodif+1,depth=stmodif+1-loc
    for (int w=0;w<args.size();w++) {
        res->args.push_back(args[w]->bring_depth(cutoff,depth));
    }
    return res;
}



