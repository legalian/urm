//
//  reasoning_core.cpp
//  URM
//
//  Created by Parker on 7/13/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"
#include "parser_core.hpp"


MetaBank::MetaBank() {
    if (Statement::universe==0) {
        Statement::universe = new Statement(0,0);
        Statement::universe->type = Statement::universe;
    }
    
    stratnames.push_back("U");
    Statement* context = parse_TTML(
    
    "[U"//0
    "|AFF:U"

    "|[A:U|A|A]EQ:U"
    "|[AFF|AFF]GT:U"
    "|[U]NOT:U"
    
    "|[U|U]AND:U"//5
    
    "|[AFF|AFF]ADD:AFF"
    "|[AFF|AFF]SUBTRACT:AFF"
    "|[AFF|AFF]MULTIPLY:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<2/0:AFF>))|AFF]DIVIDE:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<2/0:AFF>))|AFF]MODULO:AFF"//10

//    "|[A:U|a:A]IDENTITY:EQ(A,a,a)"
//    "|[A:U|B:U|A|EQ(U,A,B)]EQUIVALENCE:B"
//    "|[A:U|a:A|b:A]REFLEXIVITY:EQ(U,EQ(A,b,a),EQ(A,a,b))"
//    "|[A:U|B:U|[A]f:B|a:A|b:A|EQ(A,a,b)]PRESERVATION:EQ(B,f(a),f(b))"
//    
//    "|[T:U|K:U|[K]T|[NOT(K)]T]DEC:T"//15
//    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:K     ]DEC_POSITIVE:EQ(T,DEC(T,K,a({0/4:K}),b({0/4:NOT(K)})),a(k))"
//    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:NOT(K)]DEC_NEGATIVE:EQ(T,DEC(T,K,a({0/4:K}),b({0/4:NOT(K)})),b(k))"
//    
//    
//    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A]TERMINATION:U"
//    "|[A:U|[A]B:U|[a1:A]f:B(a1)|a3:A]UNRECURSIVE:TERMINATION(A,B({0/3:A}),f({0/3:A}),a3)"
//    
//    "|[A:U|[A]h:A|[A]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/4:A}),f({0/4:A},{1/4:B(h({0/4:A}))}(h({0/4:A}))),h(a3))]RECURSION:TERMINATION(A,B({0/3:A}),f({0/3:A},{1/3:B(h({0/3:A}))}(h({0/3:A}))),a3)"//20
//    
//    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A|TERMINATION(A,B({0/4:A}),f({0/4:A},{1/4:B({0/5:A})}({0/5:A})),a3)]INDUCTION:B(a3)"
//    
//    
//    
//    
//    //
//////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(delta,<2/0:AFF>)|GT(e,a3)]CLASSICAL_INDUCTION_CATALYST_FORWARD:TERMINATION(AFF,B({0/3:AFF}),ADD({0/3:AFF},),a3)"
//////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(<2/0:AFF>,delta)|GT(a3,e)]CLASSICAL_INDUCTION_CATALYST_BACKWARD:"
////    
////
//    "|HEAP:U"
//    "|[HEAP|AFF]AVAILABLE:AFF"
//    "|[HEAP|AFF]ALLOCATED:U"
//    "|[HEAP|AFF]ACCESSIBLE:AFF"//25
//    
//    "|[HEAP|AFF]RESERVE:HEAP"
//    "|[h:HEAP|a:AFF|ALLOCATED(h,a)]DELETE:HEAP"
//    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))|AFF]SET:HEAP"
//    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))]GET:AFF"
//    
//    
//    
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]U]zipper:U"//30
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zippertest:zipper(t({0/3:AFF}({0/4:HEAP}({0/5:U}({0/6:AFF}({0/7:HEAP}({0/8:U})))))))"
//    
//    "|[[AFF]AFF]testingfunc:AFF"
//    "|[a:AFF|b:AFF|EQ(AFF,testingfunc(ADD(a,{0/5:AFF})),testingfunc(ADD(b,{0/5:AFF})))]testingshortcircuit:U"
//    "|[b:AFF]testingblock:NOT(NOT(NOT(testingshortcircuit(b,b,IDENTITY(AFF,testingfunc(ADD(b,{0/8:AFF})))))))"
//    
//    "|[[[HEAP]HEAP]s:U]imminenttest:s("//35
//    "INDUCTION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/4:HEAP}({0/4:HEAP}),{0/4:HEAP}),{0/3:HEAP},<1/0:"
//    "TERMINATION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/5:HEAP}({0/5:HEAP}),{0/5:HEAP}),{0/3:HEAP})"
//    ">)"
//    ")"
    
    

//    "|[h:HEAP|T:U|t:T|T2:U|ALLOCATED(h,T2)]TRANS_RESERVE:ALLOCATED(RESERVE(h,T,t),T2)"
//    "|[h:HEAP|T:U|t:T|al:ALLOCATED(h,T)|T2:U|ALLOCATED(h,T2)]TRANS_SET:ALLOCATED(SET(h,T,t,al),T2)"
//    "|[h:HEAP|T:U|al:ALLOCATED(h,T)|T2:U|al2:ALLOCATED(h,T2)|NOT(EQ(U,T,T2))]TRANS_DELETE_1:ALLOCATED(DELETE(h,T,al),T2)"
//    "|[h:HEAP|T:U|al:ALLOCATED(h,T)|al2:ALLOCATED(h,T)|NOT(EQ(ALLOCATED(h,T),al,al2))]TRANS_DELETE_2:ALLOCATED(DELETE(h,T,al),T)"
//    
//    "|[h:HEAP|T:U|t:T]ALLOC_RESERVE:ALLOCATED(RESERVE(h,T,t),T)"
//    "|[h:HEAP|T:U|t:T]GET_RESERVE:EQ(T,GET(RESERVE(h,T,t),T,ALLOC_RESERVE(h,T,t)),t)"
//    "|[h:HEAP|T:U|t:T|al:ALLOCATED(h,T)]GET_SET:EQ(T,GET(SET(h,T,t,al),T,TRANS_SET(h,T,t,al,T,al)),t)"
//    
//    "|[h:HEAP|T:U|t:T|T2:U|al2:ALLOCATED(h,T2)]TGET_RESERVE:EQ(T2,GET(h,T2,al2),GET(RESERVE(h,T,t),T2,TRANS_RESERVE(h,T,t,T2,al2)))"
//    "|[h:HEAP|T:U|t:T|al:ALLOCATED(h,T)|T2:U|al2:ALLOCATED(h,T2)|NOT(EQ(U,T,T2))]TGET_SET_1:EQ(T2,GET(h,T2,al2),GET(SET(h,T,t,al),T2,TRANS_SET(h,T,t,al,T2,al2)))"
//    "|[h:HEAP|T:U|t:T|al:ALLOCATED(h,T)|al2:ALLOCATED(h,T)|NOT(EQ(ALLOCATED(h,T),al,al2))]TGET_SET_2:EQ(T2,GET(h,T,al2),GET(SET(h,T,t,al),T2,TRANS_SET(h,T,t,al,T,al2)))"
//    "|[h:HEAP|T:U|al:ALLOCATED(h,T)|T2:U|al2:ALLOCATED(h,T2)|pf:NOT(EQ(U,T,T2))]TGET_DELETE_1:EQ(T2,GET(h,T2,al2),GET(DELETE(h,T,al),T2,TRANS_DELETE_1(h,T,al,T2,al2,pf)))"
//    "|[h:HEAP|T:U|al:ALLOCATED(h,T)|al2:ALLOCATED(h,T)|pf:NOT(EQ(ALLOCATED(h,T),al,al2))]TGET_DELETE_2:EQ(T,GET(h,T,al2),GET(DELETE(h,T,al),T,TRANS_DELETE_2(h,T,al,al2,pf)))"
    
    

//    "|SCOPE:U"
//    "|[x:SCOPE|[SCOPE]r:U|[c:SCOPE|r(c)]d:U|[c:SCOPE|R:r(c)|d(c,R)]f:SCOPE|r(x)]CONDITIONAL:SCOPE"
//    "|[x:SCOPE|[SCOPE]r:U|[c:SCOPE|r(c)]d:U|[c:SCOPE|R:r(c)|d(c,R)]f:SCOPE|r(x)|[c:SCOPE|R:r(c)|D:d(c,R)]r(f(c,R,D))]LOOP:SCOPE"
//    "|[T:U|[[SCOPE]T]SCOPE]ADDPROPERTY:SCOPE"
//    "|[T:U|[SCOPE]T|SCOPE|T]MODIFY:SCOPE"
    
    "]",-1);
    context->globtypecheck();
    strategies=context->args;
    delete context;
}
void Statement::getsolvepoints(std::map<int,Statement*>& endpoints,bool stage,bool initial) {
    if (local==1 and (stage or type->local!=1)) {
        if (endpoints.find(id)==endpoints.end()) {
            endpoints[id]=deepcopy();
        } else {
            endpoints[id]=0;
        }
    }
    for (int r=0;r<args.size();r++) {
        if (args[r]->local!=1 and initial) {
            std::map<int,Statement*> nested;
            args[r]->getsolvepoints(nested,stage,true);
            
            for (auto it=nested.begin();it!=nested.end();it++) {
            
                if (it->second!=0) endpoints[it->first] = it->second;
            }
        } else if (stage or args[r]->type->local!=1 or args[r]->local!=1) {
            args[r]->getsolvepoints(endpoints,stage,false);
        }
    }
    if (type!=universe and type!=0) type->getsolvepoints(endpoints, stage,false);
}
void Soln::expand(MetaBank* mb,SolveInstance* inst) {
    if (!expanded) {
        expanded=true;
        ids=head->maxloc(0);
        for (int j=0;j<head->args.size();j++) {
            Binding newbinds;
            if (newbinds.decompose(head,head->args[j],2,0)) {
                std::vector<Binding> alternates;
                newbinds.divide(alternates,0,mb);
                for (int x=0;x<alternates.size();x++) {
//                    append(mb,inst,new Entry(alternates[x],ids));
                    binqueue.push_back(new Entry(alternates[x],ids));
                }
            }
        }
        for (int w=0;w<mb->strategies.size();w++) {
            int nids=ids;
            std::map<std::pair<int,int>,int> remap;
            mb->strategies[w]->clip_upperbound(mb->strategies[w]->local,true,remap,nids);
            Statement* adjust = mb->strategies[w]->paste_upperbound(mb->strategies[w]->local,true,remap,head==0?0:&head->args,0,true);
            Binding newbinds;
            if (newbinds.decompose(head,adjust,2,0)) {
                std::vector<Binding> alternates;
                newbinds.divide(alternates,0,mb);
                for (int x=0;x<alternates.size();x++) {
//                    append(mb,inst,new Entry(alternates[x],nids));
                    binqueue.push_back(new Entry(alternates[x],nids));
                }
            }
        }
    }
    for (int j=0;j<binqueue.size();j++) {
    
        std::vector<Statement*> solvepoints = binqueue[j]->endpoints(mb,head);
        std::vector<Statement*> antisolvepoints;
    //    head->local_list(&antisolvepoints);
        if (head->type) head->type->local_list(&antisolvepoints);
        bool obsolete=false;
        
        for (int q=(int)bin.size()-1;q>=0;q--) {
            if (bin[q]->asymmetricObsoletes(binqueue[j],&antisolvepoints)) {
                obsolete=true;
            }
        }
        if (!obsolete) {
            bin.push_back(binqueue[j]);
            if (solvepoints.size()==0) {
                for (int y=0;y<upstream.size();y++) {
                    upstream[y].linked->binqueue.push_back(upstream[y].transform(binqueue[j]));
                }
            }
            
            for (int g=0;g<solvepoints.size();g++) {
                Soln* next = inst->makeorcreate(solvepoints[g]);
                binqueue[j]->downstream.push_back(next);
//            }
//            for (int u=0;u<binqueue[j]->downstream.size();u++) {
        //////        bin[bin.size()-1].links.push_back(SolnLink(this,next,&bin[bin.size()-1]));
                SolnLink gra = SolnLink(solvepoints[g],next->head,this,binqueue[j]);
                next->upstream.push_back(gra);
                for (int w=0;w<next->bin.size();w++) {
                    if (next->bin[w]->downstream.size()==0) {
                        binqueue.push_back(gra.transform(next->bin[w]));
                    }
                }
            }
            for (int q=(int)bin.size()-2;q>=0;q--) {
                if (binqueue[j]->asymmetricObsoletes(bin[q],&antisolvepoints)) {
                    remove(mb,inst,q);
                }
            }
        }
    }
    binqueue.clear();
}
void Soln::remove(MetaBank* mb,SolveInstance* inst,int n) {
    Entry* nbind = bin[n];
    bin.erase(bin.begin()+n);
    for (int o=0;o<nbind->downstream.size();o++) {
        for (int p=(int)nbind->downstream[o]->upstream.size()-1;p>=0;p--) {
            if (nbind->downstream[o]->upstream[p].container==nbind) {
                nbind->downstream[o]->upstream.erase(nbind->downstream[o]->upstream.begin()+p);
            }
        }
        if (nbind->downstream[o]->upstream.size()==0) {
            for (int h=(int)nbind->downstream[o]->bin.size()-1;h>=0;h--) {
                nbind->downstream[o]->remove(mb,inst,h);
            }
        }
    }
//    std::cout<<"deleted "<<nbind<<"\n";
//    delete nbind;
}



std::vector<Statement*> Entry::endpoints(MetaBank* mb,Statement* head) {
    std::vector<Statement*> solvepoints;
    bool satisfied=false;
    while (!satisfied) {
        solvepoints = bind.solvepoints(head);
        
        satisfied=true;
        for (int y=0;y<solvepoints.size();y++) {
            if (solvepoints[y]->type->local==0 and solvepoints[y]->type->id==1 and solvepoints[y]->type->specifier==0) {
                Statement* donworryaboudit = new Statement(solvepoints[y]->type->deepcopy(),0,0);
                donworryaboudit->specifier=2;
                if (!bind.ensureValidity(solvepoints[y], donworryaboudit,2,0)) throw;
                donworryaboudit->cleanup();
                satisfied=false;
            }
            else if (solvepoints[y]->type->observable(mb) and solvepoints[y]->type->observe_boolean(mb)) {
                Statement* donworryaboudit = new Statement(solvepoints[y]->type->deepcopy(),0,0);
                donworryaboudit->specifier=1;
                if (!bind.ensureValidity(solvepoints[y], donworryaboudit,2,0)) throw;
                donworryaboudit->cleanup();
                satisfied=false;
            }
        }
        if (!satisfied) {
            for (int i=0;i<solvepoints.size();i++) {
                solvepoints[i]->cleanup();
            }
        }
    }
    return solvepoints;
}
bool Entry::asymmetricObsoletes(Entry* other, std::vector<Statement*>* terms) {
    if (downstream.size()!=0) return false;
    
    
//    std::cout<<"testing group: ";
//    for (int i=0;i<terms->size();i++) {
//        std::cout<<(*terms)[i]->tostringheavy()<<",";
//    }
//    std::cout<<"\n";
//    std::cout<<bind.tostringheavy()<<"other:\n"<<other->bind.tostringheavy();
    
    for (int s=0;s<terms->size();s++) {
        for (int a=0;a<bind.decoms.size();a++) {
            
            
            Binding comparison;
            
//            if ((*terms)[s]->type and bind.decoms[a].first->type)std::cout<<"matching: "<<(*terms)[s]->tostringheavy()<<":"<<(*terms)[s]->type->tostringheavy()<<" and "<<bind.decoms[a].first->tostringheavy()<<":"<<bind.decoms[a].first->type->tostringheavy();
//            else std::cout<<"matching: "<<(*terms)[s]->tostringheavy()<<" and "<<bind.decoms[a].first->tostringheavy();
            if (comparison.decompose((*terms)[s],bind.decoms[a].first,2,1)) {//needs to be an extra validation.
                bool founden=false;
                for (int b=0;b<other->bind.decoms.size();b++) {
                    Binding comparison2;
                    if (comparison2.decompose(other->bind.decoms[b].first,bind.decoms[a].first,2,1)) {
                        if (comparison2.decompose(other->bind.decoms[b].second,bind.decoms[a].second,2,1)) {
                        
                            founden=true;
                        }
                    }
                }
//                if (!founden) std::cout<<"KILL IS "<<(*terms)[s]->tostringheavy()<<"\n";
                if (!founden) {
                
                    return false;
                }
            }
            
//            Statement* head2=head2->symmetricbindavoid(1,head1->maxloc(1));
//            Statement* body2=body2->symmetricbindavoid(1,head1->maxloc(1));
//            if (!comparison.decompose(head1,head2,2,stmodif+1)) {//needs to be an extra validation.
//                return true;
//            }
//            Statement* subbody1 = body1->substitute(&comparison, 2,stmodif+1,0);
//            Statement* subbody2 = body2->substitute(&comparison, 2,stmodif+1,0);
//            bool res = decompose(subbody1,subbody2,2,stmodif);
//            subbody1->cleanup();
//            subbody2->cleanup();
//            head2->cleanup();
//            body2->cleanup();
//            return res;
            
            
        }
    }
    
    return true;
}


//void SolveInstance::remove(Soln* rem) {
//
//    for (int p=0;p<solns.size();p++) {
//        if (solns[p]==rem) {
//            delete rem;
//            solns.erase(solns.begin()+p);
//            return;
//        }
//    }
//    throw;
//}
Soln* SolveInstance::makeorcreate(Statement* match) {
    for (int g=0;g<solns.size();g++) {
        std::map<int,int> forward;
        std::map<int,int> backward;
        if (amorphousmatch(match,solns[g]->head,forward,backward)) return solns[g];
    }
    
    solns.push_back(new Soln(match));
    return solns[solns.size()-1];
}

void SolveInstance::increment(MetaBank* mb) {
//    std::cout<<"ROUND ITENERARY:\n";
//    for (int w=0;w<solns.size();w++) {
//        std::cout<<solns[w]->tostringheavy();
//    }
    for (int w=0;w<solns.size();w++) {
        solns[w]->expand(mb,this);
    }
    for (int w=(int)solns.size()-1;w>0;w--) {
        if (solns[w]->upstream.size()==0) {
            delete solns[w];
            solns.erase(solns.begin()+w);
        }
    }
        
    
}
std::vector<Statement*> Binding::solvepoints(Statement* start) {
    Statement* subbed = start->substitute(this,2,0);
    std::map<int,Statement*> points;
    std::vector<Statement*> res;
    subbed->getsolvepoints(points,false,true);
    for (auto it=points.begin();it!=points.end();it++) {
        if (it->second!=0) res.push_back(it->second);
    }
    
    if (res.size()==0) {
        points.clear();
        subbed->getsolvepoints(points,true,true);
        for (auto it=points.begin();it!=points.end();it++) {
            if (it->second!=0) res.push_back(it->second);
        }
    }
    return res;
}
Entry* SolnLink::transform(Entry* input) {

    Entry* res = new Entry(container->bind,container->ids);
    for (int y=0;y<input->bind.decoms.size();y++) {
        res->bind.insert(input->bind.decoms[y].first->scramble(mapr,res->ids),input->bind.decoms[y].second->scramble(mapr,res->ids),0);
    }
    return res;
}



//Statement* MetaBank::solve(std::string parse) {
//    return solve(parse_TTML(parse,0));
//}
Statement* MetaBank::solve(Statement* target) {
    SolveInstance inst;
    inst.solns.push_back(new Soln());
    Binding kgra;
    kgra.decoms.push_back(std::pair<Statement*,Statement*>(inst.solns[0]->head->deepcopy(),target));
    inst.solns[0]->binqueue.push_back(new Entry(kgra,target->maxloc(0)));
    while (true) {
        inst.increment(this);
        Statement* res = inst.solns[0]->getsolution();
        if (res!=0) return res;
    }
}

//
//bool Soln::iscomplete() {
//    Statement* subhead = head->substitute(&binds,2,0,0);
//    Statement* endpoint = subhead->getsolvepoint2();
//    subhead->cleanup();
//    if (endpoint!=0) endpoint->cleanup();
//    return endpoint==0;
//}












