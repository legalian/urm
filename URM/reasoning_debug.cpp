//
//  type_completion.cpp
//  URM
//
//  Created by Parker on 9/2/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"



bool assert_eq(Statement* a,Statement* b) {
    if (a->local!=b->local or a->id!=b->id) return false;
    if (a->args.size()!=b->args.size()) throw;
    for (int u=0;u<a->args.size();u++) {
        if (!assert_eq(a->args[u],b->args[u])) return false;
    }
    return true;
}
Statement* Statement::generate_type(std::vector<std::vector<Strategy*>*>& params) {
    if (local==-1) return new Statement(1,0);
    if (id==-1) throw;
    return (*params[local])[id]->typechecksub(&args,(*params[local])[id]->local+1,(int)params.size()+1,1)->type;
}
void Statement::constcheck(std::vector<std::vector<Strategy*>*> params) {
    if (local<0 or id==-1) {
        if (args.size()!=0) throw;
        return;
    }
    Strategy* calctype = (*params[local])[id]->typechecksub(&args, (*params[local])[id]->local+1,(int)params.size(),1);
//    if (!assert_eq(calctype->type,type->type)) throw;
    if (calctype->args.size()!=args.size()) throw;
    for (int u=0;u<args.size();u++) {
        args[u]->typecheck(calctype->args[u],params);
    }
    

}
void Statement::typecheck(Strategy* type,std::vector<std::vector<Strategy*>*> params) {
    params.push_back(&type->args);
    if (local<0 or id==-1) {
        if (args.size()!=0) throw;
        return;
    }
    if (params.size()<=local or (*params[local]).size()<=id) throw;
    Strategy* calctype = (*params[local])[id]->typechecksub(&args, (*params[local])[id]->local+1,(int)params.size(),1);
    if (!assert_eq(calctype->type,type->type)) throw;
    if (calctype->args.size()!=args.size()) throw;
    for (int u=0;u<args.size();u++) {
        args[u]->typecheck(calctype->args[u],params);
    }
}
void Strategy::typecheck(std::vector<std::vector<Strategy*>*> params) {
    params.push_back(&args);
    for (int u=0;u<args.size();u++) {
        args[u]->typecheck(params);
    }
    Strategy* standard = new Strategy(Statement::universe,0,0);
    if (type) type->typecheck(standard,params);
    standard->cleanup();
}


