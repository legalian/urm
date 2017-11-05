//
//  type_completion.cpp
//  URM
//
//  Created by Parker on 9/2/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

#ifdef safe_play
void Statement::erase_deltasub() {
    deltasub=0;
    if (type!=universe) type->erase_deltasub();
    for (int w=0;w<args.size();w++) {
        args[w]->erase_deltasub();
    }
}
Statement* Statement::safe_substitute_level(std::vector<Statement*>* repl,int level,int reflex,int recur,int follow,bool parity,std::string& traceback) {
    if (args.size()==0 and local==0) {
        return this;
    }
    follow+=deltasub;
    if (follow==0 and local==level) {
        if (repl->size()<=id) {
            traceback=tostringdoubleheavy()+"\n";
            return 0;
        }
        std::vector<Statement*> fargs;
        for (int s=0;s<args.size();s++) {
            Statement* subbed = args[s]->safe_substitute_level(repl,level,reflex, recur+1,follow,parity,traceback);
            if (subbed==0) return 0;
            fargs.push_back(subbed);
        }
//        std::cout<<"SUBBING: "<<tostringdoubleheavy()<<" for "<<(*repl)[id]->tostringdoubleheavy()<<"\n";
        Statement* a = (*repl)[id]->safe_substitute_level(&fargs,reflex,level+recur+1,0,follow,!parity,traceback);
        if (a==0) {
            traceback=tostringdoubleheavy()+" --> "+(*repl)[id]->tostringdoubleheavy()+"\n"+traceback;
            return 0;
        }
        a=a->depth_push(reflex,recur-1,0);
        for (int s=0;s<args.size();s++) {
            fargs[s]->cleanup();
        }
        if (parity) a->deltasub++;
        else a->deltasub--;
        return a;
    } else {
        Statement* ret = new Statement(id,local);
        ret->type = type->safe_substitute_level(repl,level,reflex,recur+1,follow,parity,traceback);
        if (ret->type==0) {
            delete ret;
            return 0;
        }
        for (int w=0;w<args.size();w++) {
            Statement* subbed = args[w]->safe_substitute_level(repl,level,reflex,recur+1,follow,parity,traceback);
            if (subbed==0) {
                ret->cleanup();
                return 0;
            }
            ret->args.push_back(subbed);
        }
        ret->specifier = specifier;
        ret->deltasub=deltasub;
        return ret;
    }
}
Statement* Statement::typechecksub(std::vector<Statement*>* repl,int level,int reflex,int recur) {
//    std::cout<<"TYPESUBBING: "<<tostringdoubleheavy()<<"\n";
    
    if (args.size()==0 and local==0) {
        return this;
    }
    Statement* res = new Statement(id,local);
    std::string traceback="";
    res->type = type->safe_substitute_level(repl,level,reflex,recur,0,true,traceback);
    if (res->type==0) {
        traceback=type->tostringdoubleheavy()+"\n"+traceback;
        std::cout<<"\nTraceback:\n"<<traceback<<"\n";
        throw;
    }
    
    for (int w=0;w<args.size();w++) {
        res->args.push_back(args[w]->typechecksub(repl,level,reflex,recur+1));
    }
    return res;
}
bool assert_eq(Statement* a,Statement* b) {//a is actual type, b is expected.
    if (not (a->local==b->local and a->id==b->id and a->args.size()==b->args.size())) return false;
    for (int u=0;u<a->args.size();u++) {
        if (!assert_eq(a->args[u],b->args[u])) return false;
    }
    if (a->type!=Statement::universe or b->type!=Statement::universe) {
        if (!assert_eq(a->type,b->type)) return false;
    }
    return true;
}
void Statement::typecheck(Statement* type) {
    std::map<int,std::vector<Statement*>*> varbank;
    varbank[0]=&MetaBank::meta_prime.strategies;
    typecheck(type,varbank,2,true);
}
void Statement::globtypecheck() {
    std::map<int,std::vector<Statement*>*> varbank;
    varbank[0]=&args;
    std::cout<<"Accepted axioms:\n";
    for (int u=0;u<args.size();u++) {
        std::cout<<"\t"<<args[u]->tostringrecursivedoubleheavy()<<"\n";
        args[u]->headlesstypecheck(varbank, 1);
    }
}
void Statement::typecheck(Statement* typ,std::map<int,std::vector<Statement*>*> params,int stat,bool haslocals) {
    static int degreeOfSuccess=0;
    degreeOfSuccess++;
    params[stat]=&typ->args;
    
    Statement* subtype;
    if (haslocals and local==1) {
        if (args.size()) throw;
        subtype = typ;
    } else {
        if (params.find(local)==params.end()) throw;
        if (params[local]->size()<=id) throw;
        subtype = typ->typechecksub(&args, (*params[local])[id]->local+1,stat+1,1);
    }

    if (specifier!=0) return;
    if (local==1 and haslocals) {
        if (!assert_eq(type,subtype->type)) {
            std::cout<<tostringheavy()<<" failed; mismatch: "<<type->tostringheavy()<<" and "<<subtype->type->tostringheavy()<<"\n";throw;
        }
        return;
    }
    if (params.find(local)==params.end()) throw;
    if (params[local]->size()<=id) throw;
    int lp1 = (*params[local])[id]->local+1;
    Statement* root = (*params[local])[id]->depth_push(lp1,stat-lp1,0);
    Statement* thistyp = root->typechecksub(&args, stat ,stat+1,1);
    root->cleanup();
    if (!assert_eq(type,subtype->type)) {
        std::cout<<"\nthistyp is: "<<thistyp->tostringheavy()<<"\n";
        std::cout<<"\n"<<tostringheavy()<<" failed; mismatch:\n\t"<<type->tostringheavy()<<"\nand:\n\t"<<subtype->type->tostringheavy()<<"\n";throw;
    }
    if (!assert_eq(type,thistyp->type)) {
        std::cout<<"\n"<<tostringheavy()<<" failed; mismatch:\n\t"<<type->tostringheavy()<<"\nand:\n\t"<<thistyp->type->tostringheavy()<<"\n";throw;
    }
    if (thistyp->args.size()!=args.size()) throw;
    for (int s=0;s<thistyp->args.size();s++) {
        args[s]->typecheck(thistyp->args[s],params,stat+1,haslocals);
    }
    if (this!=Statement::universe) {
        type->typecheck(Statement::universe,params,stat+1,haslocals);
    }
    subtype->cleanup();
}
void Statement::headlesstypecheck(std::map<int,std::vector<Statement*>*> params,int stat) {
    params[stat]=&args;
    type->typecheck(Statement::universe,params,stat+1,false);
    for (int s=0;s<args.size();s++) {
        args[s]->headlesstypecheck(params,stat+1);
    }
}
#endif


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


