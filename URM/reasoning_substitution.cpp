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
    if (type) res->type = type->depth_push(cutoff,amt);
    else res->type=0;
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

Statement* Statement::paste_upperbound_prim(int stat,std::map<std::pair<int,int>,int>& remap,std::vector<Statement*>* prepend,bool inc) {//anything <=stat will be replaced.
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(Statement::universe,id,local);
    if (local!=0 and local<=stat and local%2==1) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
            return 0;
        }
        res->local = 1;
        res->id = remap[std::pair<int,int>(id,local)];
        if (prepend) {
            for (int j=0;j<prepend->size();j++) {
                res->args.push_back((*prepend)[j]->deepcopy());
            }
        }
    }
    for (int q=0;q<args.size();q++) {
        Statement* possib = args[q]->paste_upperbound_prim(inc?stat+1:stat,remap,prepend,inc);
        if (possib==0) {
        
            res->cleanup();
            return 0;
        }
        res->args.push_back(possib);
    }
    res->type = type->paste_upperbound_prim(inc?stat+1:stat,remap,prepend,false);
    return res;
}
Statement* Statement::paste_upperbound_sec(int stat,std::map<std::pair<int,int>,int>& remap,int general) {//anything <=stat will be replaced.
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(Statement::universe,id,local);
    if (local<=stat and local>general) {
        if (remap.find(std::pair<int,int>(id,local))==remap.end()) {
//            std::cout<<"failure: "<<id<<" , "<<local<<"\n";
            return 0;
        }
        res->local = general;
        res->id = remap[std::pair<int,int>(id,local)];
    }
    for (int q=0;q<args.size();q++) {
        Statement* possib = args[q]->paste_upperbound_sec(stat,remap,general);
        if (possib==0) {
            res->cleanup();
            return 0;
        }
        res->args.push_back(possib);
    }
    res->type = type->paste_upperbound_sec(stat,remap,general);
    return res;
}

Statement* Statement::substitute(Binding* bind,int stat,int sub_level) {
    if (stat>100 or sub_level>100) {
        throw;
    }
    if (local<=0 and args.size()==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->substitute(bind,stat+1,sub_level));
    }
    if (type) res->type = type->substitute(bind,stat+1,sub_level);
    else res->type = 0;
    
    if (local==sub_level) {
        if (bind->partials[id].second->is_complete()) {
            Binding comparison(sub_level+1);
            if (comparison.decompose(bind->partials[id].first,res->depth_push(sub_level+1,1),0)) {
                res->cleanup();
                return bind->partials[id].second->substitute(&comparison,0,sub_level+1);
            }
        }
        for (int s=0;s<bind->binds.size();s++) {
            if (bind->binds[s].first->id==id and bind->binds[s].second->is_complete() and bind->binds[s].second->local!=sub_level) {
                Binding comparison(sub_level+1);
                if (comparison.decompose(bind->binds[s].first,res->depth_push(sub_level+1,1),0)) {
                    res->cleanup();
                    return bind->binds[s].second->substitute(&comparison,0,sub_level+1);
                }
            }
        }
    }
    return res;
}



Statement* Statement::lam_substitute(Binding* bind,int stat,int sub_level,int avoidex,bool& changed) {
    if (stat>100 or sub_level>100) throw;
    if (local<=0 and args.size()==0) return this;
    Statement* res = new Statement(id,local);
    for (int q=0;q<args.size();q++) {
        res->args.push_back(args[q]->lam_substitute(bind,stat+1,sub_level,-1,changed));
    }
    if (type) res->type = type->lam_substitute(bind,stat+1,sub_level,-1,changed);
    else res->type = 0;
    
    if (local==sub_level) {
        if (bind->partials[id].second->is_complete()) {
            Binding comparison(sub_level+1);
            if (comparison.decompose(bind->partials[id].first,res->depth_push(sub_level+1,1),0)) {
                res->cleanup();
                return bind->partials[id].second->substitute(&comparison,0,sub_level+1);
            }
        }
        for (int s=0;s<bind->binds.size();s++) {
            if (bind->binds[s].first->id==id and bind->binds[s].second->is_complete() and bind->binds[s].second->local!=sub_level) {
                if (s<=avoidex) {
                    Binding reverse(sub_level+1);
                    if (reverse.decompose(bind->binds[s].first->depth_push(sub_level+1,1),res,0)) continue;
                }
                Binding comparison(sub_level+1);
                if (comparison.decompose(bind->binds[s].first,res->depth_push(sub_level+1,1),0)) {//check depth matchups here... from theory
                    changed=true;
                    res->cleanup();
                    return bind->binds[s].second->substitute(&comparison,0,sub_level+1);
                }
            }
        }
    }
    return res;
}




