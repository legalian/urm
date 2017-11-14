//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

bool Binding::insert(Statement* left,Statement* right,int stmodif) {
    bool place=true;
    for (int y=(int)decoms.size()-1;y>=0;y--) {
        if (decoms[y].first->id==left->id) {
            if (!compare(decoms[y].first,left,decoms[y].second,right,stmodif)) return false;
            Binding comparison1;
            Binding comparison2;
            if        (comparison1.decompose(decoms[y].first->mapl(stmodif+1,stmodif+2),left,2,stmodif+2)) {
                place=false;
            } else if (comparison2.decompose(left->mapl(stmodif+1,stmodif+2),decoms[y].first,2,stmodif+2)) {
                decoms.erase(decoms.begin()+y);
            }
        }
    }
    Binding resolver;
    if (place) {
        resolver.decoms.push_back(gentleSubstitute(this,left,right,stmodif));
        //substitute incorrectly handles depth.
    }
    for (int r=0;r<decoms.size();r++) {//head substitution problem
        Statement* old1 = decoms[r].first;
        Statement* old2 = decoms[r].second;
        decoms[r]=gentleSubstitute(&resolver,decoms[r].first,decoms[r].second,stmodif);//may still contain substitutable locals if binding has loops.
        if (decoms[r].second->containsloop(decoms[r].first->id)) return false;
        old1->cleanup();
        old2->cleanup();
    }
    if (place) {
        decoms.push_back(std::pair<Statement*,Statement*>(resolver.decoms[0].first->deepcopy(),resolver.decoms[0].second->deepcopy()));
    }
    return true;
}
bool Binding::ensureValidity(Statement* head,Statement* body,int stat,int stmodif) {//anything <=stat will be replaced.
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
    head->clip_upperbound(stat,false,remap,mappoint);
    Statement* modhead = head->paste_upperbound_sec(stat,remap,stmodif+1);
    Statement* modbody = body->paste_upperbound_sec(stat,remap,stmodif+1);
    if (modhead!=0 and modbody!=0) {
        return insert(modhead,modbody,stmodif);
    }
    
    if (modhead!=0) modhead->cleanup();
    if (modbody!=0) modbody->cleanup();
    return false;
}
bool Binding::decompose(Statement* left,Statement* right,int stat,int stmodif) {
    if (left==right) return true;
    if (left==0 or right==0) return false;
//    std::cout<<"matching "<<left->tostringheavy()<<":"<<left->type->tostringheavy()<<" and "<<right->tostringheavy()<<":"<<right->type->tostringheavy()<<"\n";
    if ((left->local==1 and left->specifier==stmodif) or (right->local==1 and right->specifier==stmodif)) {
        if (!decompose(left->type,right->type,stat+1,stmodif)) {
            return false;
        }
        bool leftpriority  = left->args.size()>right->args.size() or (left->args.size()==right->args.size() and left->id>right->id);
        bool rightpriority = left->args.size()<right->args.size() or (left->args.size()==right->args.size() and left->id<right->id);
        if (right->local!=1 or right->specifier!=stmodif or (left->local==1 and left->specifier==stmodif and leftpriority)) {
            return ensureValidity(left,right,stat,stmodif);
        }
        if (left->local!=1 or left->specifier!=stmodif or (right->local==1 and right->specifier==stmodif and rightpriority)) {
            return ensureValidity(right,left,stat,stmodif);
        }
    }
    if (left->local==right->local and left->id==right->id and left->specifier==right->specifier) {
        if (left->args.size()!=right->args.size()) throw;
        for (int u=0;u<left->args.size();u++) {
            if (!decompose(left->args[u], right->args[u],stat!=-1?stat+1:-1,stmodif)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
bool Binding::compare(Statement* head1,Statement* head2,Statement* body1,Statement* body2,int stmodif) {
    Binding comparison;
    head2=head2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1));
    body2=body2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1));
    if (!comparison.decompose(head1,head2,2,stmodif+1)) {//needs to be an extra validation.
        return true;
    }
    Statement* subbody1 = body1->substitute(&comparison, 2,stmodif+1);
    Statement* subbody2 = body2->substitute(&comparison, 2,stmodif+1);
    bool res = decompose(subbody1,subbody2,2,stmodif);
    subbody1->cleanup();
    subbody2->cleanup();
    head2->cleanup();
    body2->cleanup();
    return res;
}

Statement* Binding::typecomplete(Statement* body,int stmodif,int& curid,MetaBank* typechain) {
//    throw;
    if (body->args.size()>0 and body->local==0) {
        std::map<std::pair<int,int>,int> remap;
        typechain->strategies[body->id]->clip_upperbound(0,true,remap,curid);
        Statement* adjust = typechain->strategies[body->id]->paste_upperbound_prim(0,remap,0,stmodif,false);//this was true. this is a test
        Statement* copied = new Statement(Statement::universe,body->id,0);
        for (int y=0;y<body->args.size();y++) {
            Statement* nxt = typecomplete(body->args[y],stmodif,curid,typechain);
            if (nxt==0 or !decompose(nxt,adjust->args[y],-1,stmodif)) {
                copied->cleanup();
                return 0;
            }
            copied->args.push_back(nxt);
        }
        copied->type = adjust->type->substitute(this,-1,stmodif);
        return copied;
    }
    return body->deepcopy();
}

bool Binding::functionalAnalysis(std::vector<Binding>& end,
                                    Statement* match,Statement* entire,
                                    int stat,int param,int stmodif,
                                    Statement* function,Statement* site,Statement* replacer,MetaBank* carry) {
    bool res=false;
    Binding newdup = *this;
    if (newdup.decompose(site,match,stat,stmodif)) {
        int nids=0;
        Binding typecheck;
        Statement* typed = typecheck.typecomplete(function,stmodif+2,nids,carry);
        if (typed!=0 and newdup.ensureValidity(entire,typed,2,stmodif)) {
            end.push_back(newdup);
            res=true;
        }
        if (typed!=0) typed->cleanup();
    }
    for (int y=0;y<site->args.size();y++) {
        *replacer = *site;
        Statement scratch = Statement(site->args[y]->type->deepcopy(),param,1);
        scratch.specifier=stmodif+1;
        replacer->args[y] = &scratch;
        bool nxt = functionalAnalysis(end,match,entire,stat+1,param,stmodif,function,site->args[y],&scratch,carry);
        res = res or nxt;
    }
    return res;
}
void Binding::divide(std::vector<Binding>& given,int stmodif,MetaBank* carry) {
    for (int u=0;u<decoms.size();u++) {
        if (decoms[u].second->maxloc(stmodif)==0) {
            std::map<int,int> used;
            for (int w=0;w<decoms[u].first->args.size();w++) {
                if (decoms[u].first->args[w]->local==1 and decoms[u].first->args[w]->specifier==stmodif+1 and decoms[u].first->args[w]->args.size()==0 and used.find(decoms[u].first->args[w]->id)==used.end()) {
                    used[decoms[u].first->args[w]->id] = w;
                    continue;
                }
                Statement* decomhead = decoms[u].first;
                Statement* decombody = decoms[u].second;
                decoms.erase(decoms.begin()+u);
                
                Statement* cancer = decomhead->deepcopy();
                int nstatid = cancer->maxloc(stmodif+1);
                Statement* cancertype = cancer->args[w]->deepcopy();
                cancer->args[w]->cleanup();
                cancer->args[w] = new Statement(cancertype,nstatid,1);
                cancer->args[w]->specifier=stmodif+1;
                std::vector<Binding> interm;
//                interm.push_back(*this);
//                bool con;
//                do {
                    Statement scratch = Statement(decombody->type->deepcopy(),nstatid,1);
                    scratch.specifier=stmodif+1;
                    functionalAnalysis(interm,decomhead->args[w],cancer,0,nstatid,stmodif,&scratch,decombody,&scratch,carry);
                    for (int q=0;q<interm.size();q++) {
                        interm[q].divide(given,stmodif,carry);
                    }
//                } while (con);
                cancer->cleanup();
                decomhead->cleanup();
                decombody->cleanup();
                return;
            }
        }
    }
    given.push_back(*this);
}
