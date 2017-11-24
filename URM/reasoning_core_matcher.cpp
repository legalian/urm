//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

bool Statement::is_complete() {
    if (this==Statement::gap) return false;
    for (int u=0;u<args.size();u++) {
        if (!args[u]->is_complete()) return false;
    }
    return true;
}

bool Binding::substitute() {

    std::vector<std::pair<Statement*,Statement*>> purify;
    bool changeflag=false;
    for (int u=0;u<binds.size();u++) {
        purify.push_back(std::pair<Statement*,Statement*>(
            binds[u].first->lam_substitute(this,2,1,u,changeflag),
            binds[u].second->lam_substitute(this,2,1,u,changeflag)
        ));
    }
    
//    std::cout<<"purify:\n"<<tostringheavy()<<"\n";
    if (changeflag) {
        binds.clear();
        for (int u=0;u<purify.size();u++) {
            if (!decompose(purify[u].first,purify[u].second,0)) return false;
        }
    }
//    std::cout<<"postpurify:\n"<<tostringheavy()<<"\n";
    return true;
}

std::pair<Statement*,Statement*> emplace(Statement* head,Statement* body,int stat,int stmodif) {
//    if (head->maxloc(stmodif+1) or body->maxloc(stmodif+1)) throw;
    
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
    head->clip_upperbound(stat,false,remap,mappoint);
    Statement* modhead = head->paste_upperbound_sec(stat,remap,stmodif+1);
    Statement* modbody = body->paste_upperbound_sec(stat,remap,stmodif+1);
    
    if (modhead!=0 and modbody!=0) {
        return std::pair<Statement*,Statement*>(modhead,modbody);
    }
    if (modhead!=0) modhead->cleanup();
    if (modbody!=0) modbody->cleanup();
    return std::pair<Statement*,Statement*>(0,0);
}
bool Binding::decompose(Statement* left,Statement* right,int stat) {
//    if (left==right) return true;
    if (left==0 or right==0) throw;
    if (left==Statement::gap or right==Statement::gap) return true;

//    std::cout<<"matching "<<left->tostringheavy()<<":"<<left->type->tostringheavy()<<" and "<<right->tostringheavy()<<":"<<right->type->tostringheavy()<<"\n";
    if (left->local==sdepth or right->local==sdepth) {
        if (!decompose(left->type,right->type,stat+1)) return false;
        if (left->local==sdepth and right->local==sdepth) {
            std::pair<Statement*,Statement*> a = emplace(left,right,stat+sdepth+1,sdepth);
//            if (partials[left->id]==0) partials[left->id]=Statement::gap;
            if (a.first!=0) binds.push_back(a);
            else {
                std::pair<Statement*,Statement*> b = emplace(right,left,stat+sdepth+1,sdepth);
//                if (partials[right->id]==0) partials[right->id]=Statement::gap;
                if (b.first!=0) binds.push_back(b);
                else return false;
            }
        } else if (right->local!=sdepth) {
            std::pair<Statement*,Statement*> c = emplace(left,right,stat+sdepth+1,sdepth);
            if (c.first==0) return false;
//            if (partials[left->id]==0) partials[left->id]=Statement::gap;
            binds.push_back(c);
        } else if (left->local!=sdepth) {
            std::pair<Statement*,Statement*> c = emplace(right,left,stat+sdepth+1,sdepth);
            if (c.first==0) return false;
//            if (partials[left->id]==0) partials[left->id]=Statement::gap;
            binds.push_back(c);
        }
        return substitute();
    }
    if (left->local==right->local and left->id==right->id) {
        if (left->args.size()!=right->args.size()) throw;
        for (int u=0;u<left->args.size();u++) {
            if (!decompose(left->args[u], right->args[u],stat+1)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
//bool Binding::compare(Statement* head1,Statement* head2,Statement* body1,Statement* body2) {
//    head2=head2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1))->localswitch(stmodif+1);
//    body2=body2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1))->localswitch(stmodif+1);
//    head1=head1->localswitch(stmodif+1);
//    body1=body1->localswitch(stmodif+1);
//    std::vector<std::pair<Statement*,Statement*>> pairs;
//    for (int i=0;i<head1->args.size();i++) {
//        pairs.push_back(std::pair<Statement*,Statement*>(head1->args[i],head2->args[i]));
//    }
////    std::cout<<"COMPARING:\n";
////    std::cout<<head1->tostringheavy()<<" | "<<body1->tostringheavy()<<"\n";
////    std::cout<<head2->tostringheavy()<<" | "<<body2->tostringheavy()<<"\n";
////    std::cout<<"\n";
//    bool res = decompose(body1,body2,2,&pairs);
//    head2->cleanup();
//    body2->cleanup();
//    head1->cleanup();
//    body1->cleanup();
//    return res;
//}
void checkPartialDescent(Statement* a,Statement* partial,
                        int stmodif,std::map<int,std::pair<Statement*,Statement*>>& opartials,std::vector<Statement*>& out,
                        std::vector<Statement*>& parameters,std::vector<Statement*>& nparams,
                        bool& changed,MetaBank* mb) {
    if (a==Statement::gap) {
        out.push_back(partial->deepcopy());
    }
    if (a->local==stmodif) {
        std::string traceback="";
        Statement* subbed = opartials[a->id].second->safe_substitute_level(&a->args,stmodif+1,stmodif+1,1,traceback);
        checkPartialDescent(subbed,partial,stmodif,opartials,out,parameters,nparams,changed,mb);
        subbed->cleanup();
        return;
    }
    if (partial!=Statement::gap) {
        if (a->id!=partial->id or a->local!=partial->local) return;
        if (a->args.size()==0) {
            out.push_back(a->deepcopy());
            return;
        }
        std::vector<Statement*> buffer;
        if (a->args.size()==1) {
            checkPartialDescent(a->args[0],partial->args[0],stmodif,opartials, buffer, parameters,nparams,changed,mb);
            for (int g=0;g<buffer.size();g++) {
                
                
        
            
                out.push_back(new Statement(Statement::gap/*THIS TYPE NEEDS TO BE REPLACED*/,a->id,a->local,buffer[g]));
            }
            return;
        }
        std::vector<Statement*> cartesian1;
        std::vector<Statement*> cartesian2;
        bool c1=true;
        #define CAR1 (c1?cartesian1:cartesian2)
        #define CAR2 (c1?cartesian2:cartesian1)
        checkPartialDescent(a->args[0],partial->args[0],stmodif,opartials, buffer, parameters,nparams,changed,mb);
        for (int g=0;g<buffer.size();g++) {
            cartesian1.push_back(new Statement(/*this too*/0,partial->id,partial->local,buffer[g]));
        }
        buffer.clear();
        for (int q=1;q<a->args.size();q++) {
            checkPartialDescent(a->args[a->args.size()-1],partial->args[a->args.size()-1],stmodif,opartials,buffer,parameters,nparams,changed,mb);
            for (int u=0;u<CAR1.size();u++) {
                for (int p=0;p<buffer.size();p++) {
                    Statement* dup = CAR1[u]->deepcopy();
                    dup->args.push_back(buffer[p]);
                    (q<a->args.size()-1?CAR2:out).push_back(dup);
                }
            }
            buffer.clear();
            CAR1.clear();
            c1 = not c1;
        }
        #undef CAR1
        #undef CAR2
    } else {
        for (int p=0;p<parameters.size();p++) {
            out.push_back(nparams[p]->deepcopy());
        }
        Statement* neg=new Statement(Statement::gap/*THIS TYPE NEEDS TO BE REPLACED*/,a->id,a->local);
        for (int y=0;y<a->args.size();y++) {
            neg->args.push_back(Statement::gap);
        }
        changed=true;
        checkPartialDescent(a,neg,stmodif,opartials,out,parameters,nparams,changed,mb);
        delete neg;
    }
}

void Binding::divide(std::vector<Binding>& list,MetaBank* mb) {
    std::vector<std::map<int,std::pair<Statement*,Statement*>>> cartesian1;
    std::vector<std::map<int,std::pair<Statement*,Statement*>>> cartesian2;
    bool changed=false;
    bool c1=true;
    std::vector<Statement*> buffer;
    #define CAR1 (c1?cartesian1:cartesian2)
    #define CAR2 (c1?cartesian2:cartesian1)
    cartesian1.push_back(partials);
    for (int q=1;q<binds.size();q++) {
        for (int u=0;u<CAR1.size();u++) {
            checkPartialDescent(binds[q].second,CAR1[u][binds[q].first->id].second,sdepth,partials,buffer,binds[q].first->args,changed,mb);
            for (int p=0;p<buffer.size();p++) {
                std::map<int,std::pair<Statement*,Statement*>> dup = CAR1[u];
                dup[binds[q].first->id]=std::pair<Statement*,Statement*>(partials[binds[q].first->id].first,buffer[p]);
                
                CAR2.push_back(dup);
            }
            buffer.clear();
        }
        CAR1.clear();
        c1 = not c1;
    }
    
    if (!changed) {
        list.push_back(*this);
        return;
    } else {
        for (int y=0;y<CAR1.size();y++) {
            Binding taat = *this;
            taat.partials = CAR1[y];
            //validify
            bool valid=true;
            for (auto it = taat.partials.begin();it!=taat.partials.end();it++) {
                for (int u=0;u<taat.binds.size();u++) {
                    if (taat.binds[u].first->id==it->first) {
                        std::string traceback;
                        Statement* comparer = it->second.second->safe_substitute_level(&taat.binds[u].first->args,2,2,1,traceback);
                        if (!taat.decompose(taat.binds[u].second,comparer,0)) {
                            valid=false;
                            break;
                        }
                    }
                }
                if (!valid) break;
            }
            if (valid) taat.divide(list,mb);
        }
    }
    #undef CAR1
    #undef CAR2
}



//BindingObject::BindingObject(Binding start) {
//    
//}








