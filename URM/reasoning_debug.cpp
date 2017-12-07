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
//void Statement::typecheck(Statement* type) {
//    std::map<int,std::vector<Statement*>*> varbank;
//    varbank[0]=&MetaBank::meta_prime.strategies;
//    typecheck(type,varbank,2,true);
//}
Statement* Statement::generate_type(std::vector<std::vector<Strategy*>*>& params) {
    if (local==-1) return new Statement(1,0);
    return (*params[local])[id]->typechecksub(&args,(*params[local])[id]->local+1,(int)params.size()+1,1)->type;
}
void Statement::constcheck(std::vector<std::vector<Strategy*>*> params) {
    if (local<0 or this==gap) {
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
    if (local<0 or this==gap) {
        if (args.size()!=0) throw;
        return;
    }
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
//void Strategy::globtypecheck() {
//    std::cout<<"Accepted axioms:\n";
//    for (int u=0;u<args.size();u++) {
//        std::cout<<"\t"<<args[u]->tostring()<<"\n";
////        args[u]->headlesstypecheck(varbank, 1);
//    }
//}
//void Statement::typecheck(Statement* typ,std::map<int,std::vector<Statement*>*> params,int stat,bool haslocals) {
//    static int degreeOfSuccess=0;
//    degreeOfSuccess++;
//    params[stat]=&typ->args;
//    
//    Statement* subtype;
//    if (haslocals and local==1) {
//        if (args.size()) throw;
//        subtype = typ;
//    } else {
//        if (params.find(local)==params.end()) throw;
//        if (params[local]->size()<=id) throw;
////        std::cout<<"COMPARE: "<<typ->local+1<<" AND "<<(*params[local])[id]->local+1<<"\n";
//        subtype = typ->typechecksub(&args, typ->local+1,stat+1,1);
////        if (typ->local+1!=(*params[local])[id]->local+1) {
////            std::cout<<"-=-=-=-=-="<<stat<<"\n";
////            std::cout<<typ->tostringdoubleheavy()<<"\n";
////            for (int u=0;u<args.size();u++) {
////                std::cout<<"\t"<<args[u]->tostringdoubleheavy()<<"\n";
////            }
////            std::cout<<subtype->tostringdoubleheavy()<<"\n";
////        }
//    }
//
//    if (local==-1 or local==-2) return;
//    if (local==1 and haslocals) {
//        if (!assert_eq(type,subtype->type)) {
//            std::cout<<tostringheavy()<<" failed; mismatch: "<<type->tostringheavy()<<" and "<<subtype->type->tostringheavy()<<"\n";throw;
//        }
//        return;
//    }
//    if (params.find(local)==params.end()) throw;
//    if (params[local]->size()<=id) throw;
//    int lp1 = (*params[local])[id]->local+1;
//    Statement* root = (*params[local])[id]->depth_push(lp1,stat-lp1);
//    Statement* thistyp = root->typechecksub(&args, stat ,stat+1,1);
//    root->cleanup();
//    if (!assert_eq(type,subtype->type)) {
//        std::cout<<"\nthistyp is: "<<thistyp->type->tostringdoubleheavy()<<"\n";
//        std::cout<<"\n"<<tostringheavy()<<" failed; mismatch:\n\t"<<type->tostringheavy()<<"\nand:\n\t"<<subtype->type->tostringheavy()<<"\n";throw;
//    }
//    if (!assert_eq(type,thistyp->type)) {
//        std::cout<<"\n"<<tostringheavy()<<" failed; mismatch:\n\t"<<type->tostringheavy()<<"\nand:\n\t"<<thistyp->type->tostringheavy()<<"\n";throw;
//    }
//    if (thistyp->args.size()!=args.size()) throw;
//    for (int s=0;s<thistyp->args.size();s++) {
//        args[s]->typecheck(thistyp->args[s],params,stat+1,haslocals);
//    }
//    if (this!=Statement::universe) {
//        type->typecheck(Statement::universe,params,stat+1,haslocals);
//    }
//    subtype->cleanup();
//}
//void Strategy::typecheck(std::map<int,std::vector<Statement*>*> params,int stat) {
//    params[stat]=&args;
//    type->typecheck(Statement::universe,params,stat+1,false);
//    for (int s=0;s<args.size();s++) {
//        args[s]->headlesstypecheck(params,stat+1);
//    }
//}
//void Statement::headlesstypecheck(std::map<int,std::vector<Statement*>*> params,int stat) {
//    params[stat]=&args;
//    type->typecheck(Statement::universe,params,stat+1,false);
//    for (int s=0;s<args.size();s++) {
//        args[s]->headlesstypecheck(params,stat+1);
//    }
//}
//#endif


//Statement* MetaBank::followpath(std::string parse,std::string target) {
//    std::vector<int> path;
//    int accum = 0;
//    for (int q=0;q<target.size();q++) {
//        if (target[q]==',') {
//            path.push_back(accum);
//            accum=0;
//        } else {
//            accum = accum*10+(target[q]-'0');
//        }
//    }
//    path.push_back(accum);
//    return followpath(Soln(parseTTML(this,parse,0),Binding()),path);
//}
//
//Statement* MetaBank::followpath(Soln target,std::vector<int> path) {
//    std::vector<Soln> activereg;
//    std::vector<Soln> passivereg;
//    activereg.push_back(target);
//    for (int v=0;v<path.size();v++) {
//        std::cout<<"New round:"<<stratnames[path[v]]<<"\n";
//        for (int q=0;q<activereg.size();q++) {
//            activereg[q].specificsolve(strategies[path[v]],&passivereg,this);
//        } 
//        activereg=passivereg;
//        passivereg.clear();
//    }
//    for (int q=0;q<activereg.size();q++) {
//        std::cout<<"Possibility:\n";
//        std::cout<<activereg[q].binds.tostringheavy();
//        return activereg[q].getsolution();
//        if (activereg[q].iscomplete()) {
//            std::cout<<"Solution found.\n";
//            return activereg[q].getsolution();
//        }
//    }
//    throw;
//}


