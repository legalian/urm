//
//  reasoning_substitution.cpp
//  URM
//
//  Created by Parker on 9/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"



//Statement* Statement::safe_depth_tap(int level,int target,int follow) {
//    if (args.size()==0 and local==0) {
//        return this;
//    }
//    follow+=deltasub;
//    Statement* ret;
//    if (follow==0 and local==level) {
//        ret = new Statement(id,target);
//    } else {
//        ret = new Statement(id,local);
//    }
//    ret->type = type->safe_depth_tap(level,target,follow);
//    for (int w=0;w<args.size();w++) {
//        ret->args.push_back(args[w]->safe_depth_tap(level,target,follow));
//    }
//    ret->deltasub=deltasub;
//    ret->specifier=specifier;
//    return ret;
//}



Statement* Statement::depth_push(int dold,int dnew) {//anything >stat will be incremented.
//    std::cout<<"DEPTH PUSH:"<<cutoff<<","<<amt<<"\n";
    if (dnew<dold) throw;
    if (local==0 and args.size()==0) {
        return this;
    }
//    follow+=deltasub;
//    follow = 0;
    Statement* res = new Statement(id,specifier==0 and local>=dold?local+dnew-dold:local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->depth_push(dold,dnew));
    }
    if (type) res->type = type->depth_push(dold,dnew);
    else res->type=0;
    res->deltasub=deltasub;
    res->specifier=specifier;
    res->debugdepth=debugdepth+dnew-dold;
    return res;
}
void Statement::clip_upperbound(int stat,bool par,std::map<std::pair<int,int>,int>& remap,int& mappoint) {//anything <=stat will be replaced.
    if ((local!=0 and local!=1 and local<=stat) or (par and local%2==1)) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            remap[std::pair<int,int>(id,local)] = mappoint++;
        }
    }
    for (int u=0;u<args.size();u++) {
        args[u]->clip_upperbound(stat,par,remap,mappoint);
    }
}
Statement* Statement::paste_upperbound(int stat,bool par,std::map<std::pair<int,int>,int>& remap,std::vector<Statement*>* prepend,int general,bool inc) {//anything <=stat will be replaced.
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(Statement::universe,id,local);
    res->specifier=specifier;
    if (local!=0 and local<=stat and ((!par and local!=1) or (par and local%2==1))) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            return 0;
        }
        res->local = 1;
        res->specifier = general;
        res->id = remap[std::pair<int,int>(id,local)];
        if (prepend) {
            for (int j=0;j<prepend->size();j++) {
                res->args.push_back((*prepend)[j]->deepcopy());
            }
        }
    }
    for (int q=0;q<args.size();q++) {
        Statement* possib = args[q]->paste_upperbound(inc?stat+1:stat,par,remap,prepend,general,inc);
        if (possib==0) {
        
            res->cleanup();
            return 0;
        }
        res->args.push_back(possib);
    }
    res->type = type->paste_upperbound(inc?stat+1:stat,par,remap,prepend,general,false);
    return res;
}
Statement* Statement::substitute(Binding* bind,int stat,int sub_level) {
    if (stat>100 or sub_level>100) {
        throw;
    }
    if (local==0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->substitute(bind,stat+1,sub_level));
    }
    res->specifier=specifier;
    if (type) res->type = type->substitute(bind,stat+1,sub_level);
    else res->type = 0;
    
    if (local==1 and specifier==sub_level) {
        for (int s=0;s<bind->decoms.size();s++) {
            if (bind->decoms[s].first->id==id) {
                Binding comparison;
                if (comparison.decompose(bind->decoms[s].first->mapl(sub_level+1,sub_level+2),res,-1,sub_level+2)) {
                    res->cleanup();
                    return bind->decoms[s].second->mapl(sub_level+1,sub_level+2)->substitute(&comparison,0,sub_level+2);
                }
            }
        }
    }
    return res;
}




