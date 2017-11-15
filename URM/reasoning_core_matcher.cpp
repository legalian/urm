//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

bool Binding::insert(Statement* left,Statement* right) {
    if (left->local!=1 or left->specifier!=stmodif) throw;
    if (right->maxloc(stmodif+2)) throw;
    if (left->maxloc(stmodif+2)) throw;
    if (left->maxloc(stmodif+1)<right->maxloc(stmodif+1)) {
        std::cout<<left->tostringheavy()<<" | "<<right->tostringheavy()<<"\n";
        throw;
    }
    for (int n=0;n<decoms.size();n++) {
        if (decoms[n].first->maxloc(stmodif+2) or decoms[n].second->maxloc(stmodif+2)) {
            std::cout<<"INTERNAL STATE CORRUPTED:\n"<<tostringheavy()<<"\n";
            throw;
        }
    }
//    if (stmodif==0) std::cout<<"PRIOR STATE:\n"<<tostringheavy()<<"\n";
    //end debug
    
    bool place=true;
    for (int y=(int)decoms.size()-1;y>=0;y--) {
        if (decoms[y].first->id==left->id) {
            if (!compare(decoms[y].first,left,decoms[y].second,right)) return false;
            Binding comparison1(stmodif+2);
            Binding comparison2(stmodif+2);
            if        (comparison1.decompose(decoms[y].first->mapl(stmodif+1,stmodif+2),left,2,0)) {
                place=false;
            } else if (comparison2.decompose(left->mapl(stmodif+1,stmodif+2),decoms[y].first,2,0)) {
                decoms.erase(decoms.begin()+y);
            }
        }
    }
    Binding resolver(stmodif);
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
    for (int n=0;n<decoms.size();n++) {
        if (decoms[n].first->maxloc(stmodif+2) or decoms[n].second->maxloc(stmodif+2)) {
            std::cout<<"INTERNAL STATE CORRUPTED:\n"<<tostringheavy()<<"\n";
            throw;
        }
    }
    return true;
}
bool Binding::ensureValidity(Statement* head,Statement* body,int stat) {//anything <=stat will be replaced.
    if (head->maxloc(stmodif+1) or body->maxloc(stmodif+1)) return false;
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
    head->clip_upperbound(stat,false,remap,mappoint);
//    std::cout<<"inserting "<<head->tostringheavy()<<" | "<<body->tostringheavy()<<"\n";
    Statement* modhead = head->paste_upperbound_sec(stat,remap,stmodif+1);
    Statement* modbody = body->paste_upperbound_sec(stat,remap,stmodif+1);
//    std::cout<<"theninserting "<<modhead->tostringheavy()<<" | "<<modbody->tostringheavy()<<"\n";
//    std::cout<<head->tostringheavy()<<"\n";
//    std::cout<<body->tostringheavy()<<"\n";
//    std::cout<<modhead->tostringheavy()<<"\n";
//    std::cout<<modbody->tostringheavy()<<"\n";
    
    if (modhead!=0 and modbody!=0) {
        return insert(modhead,modbody);
    }
    if (modhead!=0) modhead->cleanup();
    if (modbody!=0) modbody->cleanup();
    return false;
}
bool Binding::decompose(Statement* left,Statement* right,int stat,std::vector<std::pair<Statement*,Statement*>>* knowchecks) {
    for (int n=0;n<decoms.size();n++) {
        if (decoms[n].first->maxloc(stmodif+2) or decoms[n].second->maxloc(stmodif+2)) {
            std::cout<<"INTERNAL STATE CORRUPTED:\n"<<tostringheavy()<<"\n";
            throw;
        }
    }
    
    
    if (left==right) return true;
    if (left==0 or right==0) return false;
    if (knowchecks) {
        for (int u=0;u<knowchecks->size();u++) {
        
            std::cout<<"COMPARING:\n";
            std::cout<<left->tostringheavy()<<" -0- "<<right->tostringheavy()<<"\n";
            std::cout<<(*knowchecks)[u].first->tostringheavy()<<" -0- "<<(*knowchecks)[u].second->tostringheavy()<<"\n";
            if (left->is(stat,(*knowchecks)[u].first) and right->is(stat,(*knowchecks)[u].second)) {
                return true;
            }
        }
    }
//    std::cout<<"matching "<<left->tostringheavy()<<":"<<left->type->tostringheavy()<<" and "<<right->tostringheavy()<<":"<<right->type->tostringheavy()<<"\n";
    if ((left->local==1 and left->specifier==stmodif) or (right->local==1 and right->specifier==stmodif)) {
        if (!decompose(left->type,right->type,stat+1,knowchecks)) {
            return false;
        }
        bool leftpriority  = left->args.size()>right->args.size() or (left->args.size()==right->args.size() and left->id>right->id);
        bool rightpriority = left->args.size()<right->args.size() or (left->args.size()==right->args.size() and left->id<right->id);
        if (right->local!=1 or right->specifier!=stmodif or (left->local==1 and left->specifier==stmodif and leftpriority)) {
            return ensureValidity(left,right,stat);
        }
        if (left->local!=1 or left->specifier!=stmodif or (right->local==1 and right->specifier==stmodif and rightpriority)) {
            return ensureValidity(right,left,stat);
        }
    }
    if (left->local==right->local and left->id==right->id and left->specifier==right->specifier) {
        if (left->args.size()!=right->args.size()) throw;
        for (int u=0;u<left->args.size();u++) {
            if (!decompose(left->args[u], right->args[u],stat!=-1?stat+1:-1,knowchecks)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
bool Binding::compare(Statement* head1,Statement* head2,Statement* body1,Statement* body2) {
    head2=head2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1));
    body2=body2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1));
    std::vector<std::pair<Statement*,Statement*>> pairs;
    for (int i=0;i<head1->args.size();i++) {
        pairs.push_back(std::pair<Statement*,Statement*>(head1->args[i],head2->args[i]));
    }
//    std::cout<<"COMPARING:\n";
//    std::cout<<head1->tostringheavy()<<" | "<<body1->tostringheavy()<<"\n";
//    std::cout<<head2->tostringheavy()<<" | "<<body2->tostringheavy()<<"\n";
//    std::cout<<"\n";
    bool res = decompose(body1,body2,2,&pairs);
    head2->cleanup();
    body2->cleanup();
    return res;
}
