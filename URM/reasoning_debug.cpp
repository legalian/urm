//
//  type_completion.cpp
//  URM
//
//  Created by Parker on 9/2/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"




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
//void Binding::check() {
//    if (tracks.dat[tracks.loc()].first!=localtypes) throw;
//    if (tracks.dat[tracks.loc()].second!=ara) throw;
//
//    for (int c=0;c<binds.size();c++) {
//        ParameterContext tplusi = tracks.append(binds[c].itinerary,binds[c].ara);
//        Statement dena = tplusi.generateType(binds[c].head).type.substitute_single(*this,tplusi);
//        Statement denb = tplusi.generateType(binds[c].body).type.substitute_single(*this,tplusi);
//        if (!judgemental_eq(dena,denb)) throw;
//    }
//}
void Statement::loosecheck(ParameterContext& params) {
    Strategy prop = params.generateType(*this);
    for (int h=0;h<ara;h++) {
        ParameterContext nn = params.append(prop.args[h]);
        args[h].loosecheck(nn);
    }
}
void Strategy::loosecheck(ParameterContext& params) {
    ParameterContext nn = params.append(*this);
    type.loosecheck(nn);
    for (int g=0;g<ara;g++) args[g].loosecheck(nn);
}
void Binding::loosecheck() {
    for (int h=0;h<ara;h++) localtypes[h].loosecheck(tracks);
    for (int s=0;s<binds.size();s++) {
        ParameterContext tplusi = tracks.append(binds[s].itinerary,binds[s].ara);
        binds[s].head.loosecheck(tplusi);
        binds[s].body.loosecheck(tplusi);
    }
}
//void ParameterContext::typecheck() {
//    for (int g=0;g<dat[loc()].second;g++) {
//        dat[loc()].first[g].typecheck(*this);
//    }
//    if (loc()>0) {
//        ParameterContext sub = *this;
//        sub.dat.erase(sub.dat.begin()+loc());
//        sub.typecheck();
//    }
//}
void Statement::typecheck(Statement& type,ParameterContext& params) {
    if (local<0) {
        if (type.ara or type.local or type.id!=1) throw;
        if (ara) throw;
        return;
    }
    if (local==0 and id==0) {
        if (type.id or type.local or type.ara) throw;
        if (ara) throw;
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
void Strategy::typecheck(ParameterContext& params) {
    ParameterContext nn = params.append(*this);
    for (int u=0;u<ara;u++) args[u].typecheck(nn);
    Statement passin = Statement(0,0);
    if (type.id!=0 or type.local!=0) type.typecheck(passin,nn);
}


