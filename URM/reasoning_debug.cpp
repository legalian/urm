//
//  type_completion.cpp
//  URM
//
//  Created by Parker on 9/2/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"



bool judgemental_eq(Statement a,Statement b) {
    if (a.local!=b.local or a.id!=b.id) return false;
    if (a.ara!=b.ara) throw;
    for (int u=0;u<a.ara;u++) {
        if (!judgemental_eq(a.args[u],b.args[u])) return false;
    }
    return true;
}
void Statement::constcheck(ParameterContext params) {
    if (local<0 or id==-1) {
        if (ara!=0) throw;
        return;
    }
    if (local==0 and ara==0) return;
    Strategy calctype = params.generateType(*this);
    if (calctype.ara!=ara) throw;
    for (int u=0;u<ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        args[u].typecheck(calctype.args[u].type,nn);
    }
}
void Statement::typecheck(Statement type,ParameterContext& params) {
    if (local<0 or id==-1) {
        if (ara!=0) throw;
        return;
    }
    Strategy calctype = params.generateType(*this);
    if (!judgemental_eq(calctype.type,type)) {
        std::cout<<tostring()<<": ";
        std::cout<<calctype.type.tostring()<<" =/= "<<type.tostring()<<"\n";
        throw;
    }
    if (calctype.ara!=ara) throw;
    for (int u=0;u<ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        args[u].typecheck(calctype.args[u].type,nn);
    }
}
void Strategy::typecheck(ParameterContext params) {
    params.dat.push_back(std::pair<Strategy*,int>(args,ara));
    for (int u=0;u<ara;u++) {
        args[u].typecheck(params);
    }
    if (type.is_valid()) type.typecheck(Statement(0,0),params);
}


