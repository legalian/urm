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
    
    Statement* context = parse_TTML(
    
    "[U"//0
    "|AFF:U"
    
    "|[A:U|A|A]EQ:U"
    "|[AFF|AFF]GT:U"
    "|[U]NOT:U"
    
//    "|[U|U]AND:U"//5
//    "|[A:U|B:U|AND(A,B)]ANDACC:A"
//    "|[A:U|B:U]AND_COMMUTATIVE:EQ(U,AND(A,B),AND(B,A))"//could add member specification axioms for AND
//    "|[A:U|B:U|A|B]UNION:AND(A,B)"
////
//    "|[A:U]DOUBLENEG:NOT(NOT(A))"
//    "|[A:U]EXCLUSION:NOT(AND(A,NOT(A)))"//10
//    "|[A:U|B:U|A|NOT(AND(A,B))]INCLUSION:NOT(B)"
////
//    "|[a:AFF|b:AFF]TRICHOTOMY_E:EQ(U,EQ(AFF,a,b),AND(NOT(GT(a,b)),NOT(GT(b,a))))"
//    "|[a:AFF|b:AFF]TRICHOTOMY_G:EQ(U,GT(a,b),AND(NOT(EQ(AFF,a,b)),NOT(GT(b,a))))"
//
    "|[AFF|AFF]ADD:AFF"
    "|[AFF|AFF]SUBTRACT:AFF"//15
    "|[AFF|AFF]MULTIPLY:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<2/0:AFF>))|AFF]DIVIDE:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<2/0:AFF>))|AFF]MODULO:AFF"
    
//    "|[a:AFF|b:AFF|c:AFF|d:AFF|GT(a,b)|NOT(GT(d,c))]ADDITIVE:GT(ADD(a,c),ADD(b,d))"
//    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT(c,<2/0:AFF>)]MULTIPLICATIVE:GT(MULTIPLY(a,c),MULTIPLY(b,c))"//20
//    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT(<2/0:AFF>,c)]FLIPPEDMULTIPLICATIVE:GT(MULTIPLY(b,c),MULTIPLY(a,c))"
////
//    "|[a:AFF|b:AFF]ZEROPRODUCT:EQ(U,EQ(AFF,MULTIPLY(a,b),<2/0:AFF>),NOT(AND(NOT(EQ(AFF,b,<2/0:AFF>)),NOT(EQ(AFF,a,<2/0:AFF>)))))"
////
//    "|[a:AFF|b:AFF|c:AFF]ADD_ASSOCIATIVE:EQ(AFF,ADD(ADD(a,b),c),ADD(a,ADD(b,c)))"
//    "|[a:AFF|b:AFF|c:AFF]MULTIPLY_ASSOCIATIVE:EQ(AFF,MULTIPLY(MULTIPLY(a,b),c),MULTIPLY(a,MULTIPLY(b,c)))"
//
//    "|[a:AFF|b:AFF]ADD_COMMUTATIVE:EQ(AFF,ADD(a,b),ADD(b,a))"//25
//    "|[a:AFF|b:AFF]MULTIPLY_COMMUTATIVE:EQ(AFF,MULTIPLY(a,b),MULTIPLY(b,a))"

    "|[a:AFF]ADD_IDENTITY:EQ(AFF,ADD(a,<2/0:AFF>),a)"
//    "|[a:AFF]MULTIPLY_IDENTITY:EQ(AFF,MULTIPLY(a,<2/1:AFF>),a)"
//    "|[a:AFF]ADD_IDENTITY_ANNIHALATE:EQ(AFF,SUBTRACT(a,a),<2/0:AFF>)"
//    "|[a:AFF|g:NOT(EQ(AFF,a,<2/0:AFF>))]MULTIPLY_IDENTITY_ANNIHALATE:EQ(AFF,DIVIDE(a,g,a),<2/1:AFF>)"//30
//
//    "|[a:AFF|b:AFF|c:AFF]DISTRIBUTIVE:EQ(AFF,MULTIPLY(a,ADD(b,c)),ADD(MULTIPLY(a,b),MULTIPLY(a,c)))"
//
//    "|[a:AFF|b:AFF]SUBTRACT_REWRITE:EQ(AFF,SUBTRACT(a,b),ADD(a,MULTIPLY(<2/-1:AFF>,b)))"
//    "|[a:AFF|b:AFF|g:NOT(EQ(AFF,a,<2/0:AFF>))]DIVIDE_REWRITE:EQ(AFF,DIVIDE(a,g,b),MULTIPLY(b,DIVIDE(a,g,<2/1:AFF>)))"

    "|[A:U|a:A]IDENTITY:EQ(A,a,a)"
    "|[A:U|B:U|A|EQ(U,A,B)]EQUIVALENCE:B"//35
    "|[A:U|a:A|b:A]REFLEXIVITY:EQ(U,EQ(A,b,a),EQ(A,a,b))"
    "|[A:U|B:U|[A]f:B|a:A|b:A|EQ(A,a,b)]PRESERVATION:EQ(B,f(a),f(b))"
    
//    "|[T:U|K:U|[K]T|[NOT(K)]T]DEC:T"
//    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:K     ]DEC_POSITIVE:EQ(T,DEC(T,K,a({0/4:K}),b({0/4:NOT(K)})),a(k))"
//    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:NOT(K)]DEC_NEGATIVE:EQ(T,DEC(T,K,a({0/4:K}),b({0/4:NOT(K)})),b(k))"//40
//    
//    
//    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A]TERMINATION:U"
//    "|[A:U|[A]B:U|[a1:A]f:B(a1)|a3:A]UNRECURSIVE:TERMINATION(A,B({0/3:A}),f({0/3:A}),a3)"
//    
//    "|[A:U|[A]h:A|[A]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/4:A}),f({0/4:A},{1/4:B(h({0/4:A}))}(h({0/4:A}))),h(a3))]RECURSION:TERMINATION(A,B({0/3:A}),f({0/3:A},{1/3:B(h({0/3:A}))}(h({0/3:A}))),a3)"
//    
//    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A|TERMINATION(A,B({0/4:A}),f({0/4:A},{1/4:B({0/5:A})}({0/5:A})),a3)]INDUCTION:B(a3)"
    
    
    //not this:
///////////    "|[[AFF]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/4:A}),f({0/4:A},{1/4:B(h({0/4:A}))}(h({0/4:A}))),h(a3))]RECURSION:TERMINATION(A,B({0/3:A}),f({0/3:A},{1/3:B(h({0/3:A}))}(h({0/3:A}))),a3)"//set h to ++
    
    //
////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(delta,<2/0:AFF>)|GT(e,a3)]CLASSICAL_INDUCTION_CATALYST:TERMINATION(AFF,B({0/3:AFF}),ADD({0/3:AFF},),a3)"
////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(<2/0:AFF>,delta)|GT(a3,e)]TERMINATION_MAPPING:"
//    
//
//    "|HEAP:U"//45
//    "|[HEAP|AFF]AVAILABLE:AFF"
//    "|[HEAP|AFF]ALLOCATED:U"
//    "|[HEAP|AFF]ACCESSIBLE:AFF"
//    
//    "|[HEAP|AFF]RESERVE:HEAP"
//    "|[h:HEAP|a:AFF|ALLOCATED(h,a)]DELETE:HEAP"//50
//    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))|AFF]SET:HEAP"
//    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))]GET:AFF"
    
    
    
    
    
    
    

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
    
    

//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]U]zipper:U"
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zippertest:zipper(t({0/3:AFF}({0/4:HEAP}({0/5:U}({0/6:AFF}({0/7:HEAP}({0/8:U})))))))"
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U|zipper(t({0/4:AFF}({0/5:HEAP}({0/6:U}({0/7:AFF}({0/8:HEAP}({0/9:U})))))))]zipperconfirm:U"
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zipperfinal:zipperconfirm(t({0/3:AFF}({0/4:HEAP}({0/5:U}({0/6:AFF}({0/7:HEAP}({0/8:U})))))),zippertest(t({0/4:AFF}({0/5:HEAP}({0/6:U}({0/7:AFF}({0/8:HEAP}({0/9:U}))))))))"
//    "|[[AFF]AFF]testingfunc:AFF"
//    "|[a:AFF|b:AFF|EQ(AFF,testingfunc(ADD(a,{0/5:AFF})),testingfunc(ADD(b,{0/5:AFF})))]testingshortcircuit:U"
//    "|[b:AFF]testingblock:NOT(NOT(NOT(testingshortcircuit(b,b,IDENTITY(AFF,testingfunc(ADD(b,{0/8:AFF})))))))"
//    "|[[[HEAP]HEAP]s:U]imminenttest:s("
//    "INDUCTION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/4:HEAP}({0/4:HEAP}),{0/4:HEAP}),{0/3:HEAP},<1/0:"
//    "TERMINATION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/5:HEAP}({0/5:HEAP}),{0/5:HEAP}),{0/3:HEAP})"
//    ">)"
//    ")"

    
    "]",-1,&stratnames);
    stratnames[0]="U";
    context->globtypecheck();
    strategies=context->args;
    delete context;
}
void Statement::obsolete(std::map<int,bool>& obsoleted) {
    if (local==1 and specifier==0) {
        obsoleted[id]=true;
    }
    for (int u=0;u<args.size();u++) {
        args[u]->obsolete(obsoleted);
    }
}
void Statement::getsolvepoints(std::map<int,Statement*>& endpoints,std::map<int,bool>& obsoleted,bool stage,bool inlocal) {
    if (local==1) {
        if (stage or type->local!=1 or type->specifier!=0) type->obsolete(obsoleted);
        if (stage or type->local!=1 or type->specifier!=0) endpoints[id]=deepcopy();
    }
    if (inlocal) {
//        std::cout<<tostringdoubleheavy()<<"\n";
        type->obsolete(obsoleted);
    }
    for (int r=0;r<args.size();r++) args[r]->getsolvepoints(endpoints, obsoleted, stage,local==1 or inlocal);
}
void Soln::expand(MetaBank* mb,SolveInstance* inst) {
    if (!expanded) {
        expanded=true;
        ids=head->maxloc(0);
        for (int j=0;j<head->args.size();j++) {
            Binding newbinds(0);
            if (newbinds.decompose(head,head->args[j],2,0)) {
                binqueue.push_back(new Entry(newbinds,ids));
                for (int n=0;n<newbinds.decoms.size();n++) {
                    if (newbinds.decoms[n].first->maxloc(1) or newbinds.decoms[n].first->maxloc(2) or
                        newbinds.decoms[n].second->maxloc(1) or newbinds.decoms[n].second->maxloc(2)) {
                        std::cout<<"INTERNAL STATE CORRUPTED:\n"<<tostringheavy()<<"\n";
                        throw;
                    }
                }
            }
        }
        std::cout<<"TESTING AGAINST EACH STRATEGY\n\n\n";
        for (int w=0;w<mb->strategies.size();w++) {
            int nids=ids;
            std::map<std::pair<int,int>,int> remap;
            mb->strategies[w]->clip_upperbound(mb->strategies[w]->local,true,remap,nids);
            Statement* adjust = mb->strategies[w]->paste_upperbound_prim(mb->strategies[w]->local+1,remap,head==0?0:&head->args,0,false);// this is a test. normally ture
            Binding newbinds(0);
            std::cout<<"MATCHING:\n";
            std::cout<<"\t"<<head->tostringdoubleheavy()<<"\n";
            std::cout<<"\t"<<adjust->tostringdoubleheavy()<<"\n";
            
            if (newbinds.decompose(head,adjust,2,0)) {
                std::cout<<"SUCCESS<-==-=-==--=-=-=-=------"<<mb->stratnames[w]<<"\n";
                std::cout<<newbinds.tostringheavy()<<"\n";
                binqueue.push_back(new Entry(newbinds,nids));
                for (int n=0;n<newbinds.decoms.size();n++) {
                    if (newbinds.decoms[n].first->maxloc(2) or newbinds.decoms[n].second->maxloc(2)) {
                        std::cout<<"INTERNAL STATE CORRUPTED:\n"<<tostringheavy()<<"\n";
                        throw;
                    }
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
//                std::cout<<donworryaboudit<<"\n";
                if (!bind.ensureValidity(solvepoints[y], donworryaboudit,2)) throw;
                
                donworryaboudit->cleanup();
                satisfied=false;
            }
            else if (solvepoints[y]->type->observable(mb) and solvepoints[y]->type->observe_boolean(mb)) {
                Statement* donworryaboudit = new Statement(solvepoints[y]->type->deepcopy(),0,0);
                donworryaboudit->specifier=1;
                if (!bind.ensureValidity(solvepoints[y], donworryaboudit,2)) throw;
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
    for (int s=0;s<terms->size();s++) {
        for (int a=0;a<bind.decoms.size();a++) {
            Binding comparison(1);
            if (comparison.decompose((*terms)[s],bind.decoms[a].first,2,0)) {//needs to be an extra validation.
                bool founden=false;
                for (int b=0;b<other->bind.decoms.size();b++) {
                    Binding comparison2(1);
                    if (comparison2.decompose(other->bind.decoms[b].first,bind.decoms[a].first,2,0)) {
                        if (comparison2.decompose(other->bind.decoms[b].second,bind.decoms[a].second,2,0)) {
                        
                            founden=true;
                        }
                    }
                }
//                if (!founden) std::cout<<"KILL IS "<<(*terms)[s]->tostringheavy()<<"\n";
                if (!founden) {
                
                    return false;
                }
            }
        }
    }
    return true;
}
Soln* SolveInstance::makeorcreate(Statement* match) {
    for (int g=0;g<solns.size();g++) {
        std::map<int,int> forward;
        std::map<int,int> backward;
        if (amorphousmatch(match,solns[g]->head,forward,backward)) return solns[g];
    }
    solns.push_back(new Soln(match));
    return solns[solns.size()-1];
}
bool Soln::recieveEnergy(MetaBank* mb,SolveInstance* si,int energy) {
    if (binqueue.size() or !expanded) {
        expand(mb,si);
        return true;
    }
    for (int p=0;energy==-1 or p<energy;p++) {
        for (int e=0;e<bin.size();e++) {
            for (int a=0;a<bin[e]->downstream.size();a++) {
                if (bin[e]->downstream[a]->recieveEnergy(mb,si,p)) return true;
            }
        }
    }
    return false;
}
void SolveInstance::increment(MetaBank* mb) {
    solns[0]->recieveEnergy(mb,this,-1);
    for (int w=(int)solns.size()-1;w>0;w--) {
        if (solns[w]->upstream.size()==0) {
            delete solns[w];
            solns.erase(solns.begin()+w);
        }
    }
    heavyvisualize();
    flushvisualizer();
}
std::vector<Statement*> Binding::solvepoints(Statement* start) {
    Statement* subbed = start->substitute(this,2,0);
    if (subbed->maxloc(1) or subbed->maxloc(2) or subbed->maxloc(3) or subbed->maxloc(4)) {
        std::cout<<subbed->tostringdoubleheavy()<<"\n";
        throw;
    }
    std::map<int,Statement*> points;
    std::map<int,bool> obsoleted;
    std::vector<Statement*> res;
    subbed->getsolvepoints(points,obsoleted,false,false);
    for (auto it=points.begin();it!=points.end();it++) {
        if (!obsoleted[it->first]) res.push_back(it->second);
    }
    if (res.size()==0) {
        points.clear();
        obsoleted.clear();
        subbed->getsolvepoints(points,obsoleted,true,false);
        for (auto it=points.begin();it!=points.end();it++) {
            if (!obsoleted[it->first]) res.push_back(it->second);
        }
    }
    return res;
}
Entry* SolnLink::transform(Entry* input) {
    Entry* res = new Entry(container->bind,container->ids);
    for (int y=0;y<input->bind.decoms.size();y++) {
        res->bind.insert(input->bind.decoms[y].first->scramble(mapr,res->ids),input->bind.decoms[y].second->scramble(mapr,res->ids));
    }
    return res;
}
Statement* MetaBank::solve(Statement* target) {
    SolveInstance inst;
    inst.solns.push_back(new Soln());
    Binding kgra(0);
    kgra.decoms.push_back(std::pair<Statement*,Statement*>(inst.solns[0]->head->deepcopy(),target));
    inst.solns[0]->binqueue.push_back(new Entry(kgra,target->maxloc(0)));
    while (true) {
        inst.increment(this);
        Statement* res = inst.solns[0]->getsolution();
        if (res!=0) {
            return res;
        }
    }
}












