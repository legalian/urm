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
    
    Strategy context = parse_TTML(
    
    "[U"//0
    "|AFF:U"
    
    "|[A:U|A|A]EQ:U"
    "|[AFF|AFF]GT:U"
    "|[U]NOT:U"
    
    "|[U|U]AND:U"//5
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
    "|[a:AFF|NOT(EQ(AFF,a,{0/-1}))|AFF]DIVIDE:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,{0/-1}))|AFF]MODULO:AFF"
    
//    "|[a:AFF|b:AFF|c:AFF|d:AFF|GT(a,b)|NOT(GT(d,c))]ADDITIVE:GT(ADD(a,c),ADD(b,d))"
//    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT(c,{0/-1})]MULTIPLICATIVE:GT(MULTIPLY(a,c),MULTIPLY(b,c))"//20
//    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT({0/-1},c)]FLIPPEDMULTIPLICATIVE:GT(MULTIPLY(b,c),MULTIPLY(a,c))"
////
////
//    "|[a:AFF|b:AFF|c:AFF]ADD_ASSOCIATIVE:EQ(AFF,ADD(ADD(a,b),c),ADD(a,ADD(b,c)))"
//    "|[a:AFF|b:AFF|c:AFF]MULTIPLY_ASSOCIATIVE:EQ(AFF,MULTIPLY(MULTIPLY(a,b),c),MULTIPLY(a,MULTIPLY(b,c)))"
//
//    "|[a:AFF|b:AFF]ADD_COMMUTATIVE:EQ(AFF,ADD(a,b),ADD(b,a))"//25
//    "|[a:AFF|b:AFF]MULTIPLY_COMMUTATIVE:EQ(AFF,MULTIPLY(a,b),MULTIPLY(b,a))"

//    "|[a:AFF]ADD_IDENTITY:EQ(AFF,ADD(a,{0/-1}),a)"
//    "|[a:AFF]MULTIPLY_IDENTITY:EQ(AFF,MULTIPLY(a,{1/-1}),a)"
//    "|[a:AFF]ADD_IDENTITY_ANNIHALATE:EQ(AFF,SUBTRACT(a,a),{0/-1})"
//    "|[a:AFF|g:NOT(EQ(AFF,a,{0/-1}))]MULTIPLY_IDENTITY_ANNIHALATE:EQ(AFF,DIVIDE(a,g,a),{1/-1})"//30
//
//    "|[a:AFF|b:AFF|c:AFF]DISTRIBUTIVE:EQ(AFF,MULTIPLY(a,ADD(b,c)),ADD(MULTIPLY(a,b),MULTIPLY(a,c)))"
//
//    "|[a:AFF|b:AFF]SUBTRACT_REWRITE:EQ(AFF,SUBTRACT(a,b),ADD(a,MULTIPLY({-1/-1},b)))"
//    "|[a:AFF|b:AFF|g:NOT(EQ(AFF,a,{0/-1}))]DIVIDE_REWRITE:EQ(AFF,DIVIDE(a,g,b),MULTIPLY(b,DIVIDE(a,g,{1/-1})))"

    "|[A:U|a:A]IDENTITY:EQ(A,a,a)"
    "|[A:U|B:U|A|EQ(U,A,B)]EQUIVALENCE:B"//35
    "|[A:U|a:A|b:A]REFLEXIVITY:EQ(U,EQ(A,b,a),EQ(A,a,b))"
    "|[A:U|B:U|[A]f:B|a:A|b:A|EQ(A,a,b)]PRESERVATION:EQ(B,f(a),f(b))"
    
    
    
    //here onwards is optional for tentative testing
    
    
    
    
    "|[[AFF]AFF]test:U"
    "|[[AFF]a:AFF]test2:test(a({0/2}))"
    
    
    
    "|[T:U|K:U|[K]T|[NOT(K)]T]DEC:T"
    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:K     ]DEC_POSITIVE:EQ(T,DEC(T,K,a({0/3}),b({0/3})),a(k))"
    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:NOT(K)]DEC_NEGATIVE:EQ(T,DEC(T,K,a({0/3}),b({0/3})),b(k))"//40

    
    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A]TERMINATION:U"
    
    
//    "|[A:U|[A]B:U|[a1:A]f:B(a1)|a3:A]UNRECURSIVE:TERMINATION(A,B({0/3}),f({0/3}),a3)"
//    
//    "|[A:U|[A]h:A|[A]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/4}),f({0/4},{1/4}(h({0/4}))),h(a3))]RECURSION:TERMINATION(A,B({0/3}),f({0/3},{1/3}(h({0/3}))),a3)"
//


    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A|TERMINATION(A,B({0/3}),f({0/3},{1/3}({0/4})),a3)]INDUCTION:B(a3)"
    
    
    //not this:
///////////    "|[[AFF]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/4}),f({0/4},{1/4}(h({0/4}))),h(a3))]RECURSION:TERMINATION(A,B({0/3}),f({0/3},{1/3}(h({0/3}))),a3)"//set h to ++
    
    //
////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(delta,<2/0:AFF>)|GT(e,a3)]CLASSICAL_INDUCTION_CATALYST:TERMINATION(AFF,B({0/3:AFF}),ADD({0/3:AFF},),a3)"
////    "|[[AFF]B:U|a3:AFF|e:AFF|delta:AFF|GT(<2/0:AFF>,delta)|GT(a3,e)]TERMINATION_MAPPING:"
//    
//
    "|HEAP:U"//45
    "|[HEAP|AFF]AVAILABLE:AFF"
    "|[HEAP|AFF]ALLOCATED:U"
    "|[HEAP|AFF]ACCESSIBLE:AFF"
    
    "|[HEAP|AFF]RESERVE:HEAP"
    "|[h:HEAP|a:AFF|ALLOCATED(h,a)]DELETE:HEAP"//50
    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))|AFF]SET:HEAP"
    "|[h:HEAP|a:AFF|b:AFF|NOT(GT(b,ACCESSIBLE(h,a)))]GET:AFF"
    
    
    
    
    
    
    

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
    
    

    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]U]zipper:U"
    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zippertest:zipper(t({0/2}({0/3}({0/4}({0/5}({0/6}({0/7})))))))"
    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U|zipper(t({0/3}({0/4}({0/5}({0/6}({0/7}({0/8})))))))]zipperconfirm:U"
    
    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zipperfinal:"
    "zipperconfirm(t({0/2}({0/3}({0/4}({0/5}({0/6}({0/7})))))),zippertest(t({0/3}({0/4}({0/5}({0/6}({0/7}({0/8}))))))))"
    "|[[AFF]AFF]testingfunc:AFF"
//    "|[a:AFF|b:AFF|EQ(AFF,testingfunc(ADD(a,{0/5:AFF})),testingfunc(ADD(b,{0/5:AFF})))]testingshortcircuit:U"
//    "|[b:AFF]testingblock:NOT(NOT(NOT(testingshortcircuit(b,b,IDENTITY(AFF,testingfunc(ADD(b,{0/8:AFF})))))))"
    
//    "|[[[HEAP]HEAP]s:U]imminenttest:s("
//    "INDUCTION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/4:HEAP}({0/4:HEAP}),{0/4:HEAP}),{0/3:HEAP},<1/0:"
//    "TERMINATION(HEAP,HEAP,DEC(HEAP,GT(<2/2:AFF>,<2/5:AFF>),{1/5:HEAP}({0/5:HEAP}),{0/5:HEAP}),{0/3:HEAP})"
//    ">)"
//    ")"

    
    "]",-1,&stratnames);
    stratnames[0]="U";
    
    std::cout<<"Accepted axioms:\n";
    strategies = context.args;
    ara = context.ara;
    for (int u=0;u<ara;u++) {
        std::cout<<"\t"<<strategies[u].tostring()<<"\n";
    }
    context.typecheck(ParameterContext());
}
void Statement::obsolete(std::map<int,bool>& obsoleted) {
    if (local==1) {
        obsoleted[id]=true;
    }
    for (int u=0;u<ara;u++) {
        args[u].obsolete(obsoleted);
    }
}
void Statement::getsolvepoints(std::map<int,Statement>& endpoints,std::map<int,bool>& obsoleted,ParameterContext& params,bool stage,bool inlocal) {
//    if (id>=params[local].second) throw;
//    int lloc = params[local].first[id].local;
//    Strategy calctype = params[local].first[id].typechecksub(args,ara,lloc+1,(int)params.size(),1);
//    Strategy* calctype = (*params[local])[id]->typechecksub(&args, (*params[local])[id]->local+1,(int)params.size(),1);
    Strategy calctype = params.generateType(*this);

    if (local==1) {
        if (stage or calctype.type.local!=1) calctype.type.obsolete(obsoleted);
        if (stage or calctype.type.local!=1) endpoints[id]=deepcopy();
    }
    if (inlocal) {
        calctype.type.obsolete(obsoleted);
    }
    for (int r=0;r<ara;r++) {
        ParameterContext continued = params.append(calctype.args[r]);
//        continued.push_back(std::pair<Strategy*,int>(calctype.args[r].args,calctype.args[r].ara));
        args[r].getsolvepoints(endpoints, obsoleted, continued,stage,local==1 or inlocal);
    }
    calctype.cleanup();
}


Statement idpush(Statement a,int amt) {
    Statement res = Statement(a.local==1?a.id+amt:a.id,a.local,a.ara);
    for (int p=0;p<a.ara;p++) {
        res.args[p] = idpush(a.args[p],amt);
    }
    return res;
}
Strategy idpush(Strategy a,int amt) {
    Strategy res = Strategy(idpush(a.type,amt),a.local==1?a.id+amt:a.id,a.local,a.ara);
    for (int p=0;p<a.ara;p++) {
        res.args[p] = idpush(a.args[p],amt);
    }
    return res;
}
void Soln::expand(MetaBank* mb,SolveInstance* inst) {
    if (!expanded) {
        expanded=true;
        for (int j=0;j<head.ara;j++) {
            Binding newbinds(mb,loctypes,ara);
            if (newbinds.decompose(head,head.args[j])) {
                std::vector<Binding> splitter;
                newbinds.divide(splitter,0);
                for (int k=0;k<splitter.size();k++) {
                    binqueue.push_back(new Entry(splitter[k]));
                }
            }
        }
        for (int w=0;w<mb->ara;w++) {
            Statement adjust = mb->strategies[w].snapshot();
            Strategy* newloctypes = new Strategy[adjust.ara+ara];
            for (int k=0;k<ara;k++) {
                newloctypes[k] = loctypes[k];
            }
            for (int u=0;u<adjust.ara;u++) {
                adjust.args[u].id = ara+u;
                newloctypes[ara+u] = idpush(mb->strategies[w].args[u],ara);
//                newloctypes[ara+u].id = ara+u;
            }
            Binding newbinds(mb,newloctypes,adjust.ara+ara);
            if (newbinds.decomposeverbal(head,adjust,newbinds.tracks,1)) {
                std::vector<Binding> splitter;
                newbinds.divide(splitter,0);
                for (int k=0;k<splitter.size();k++) {
                    std::cout<<"\t\tSUCCESS\n";
                    std::cout<<splitter[k].tostring(0);
                    binqueue.push_back(new Entry(splitter[k]));
                }
            }
        }
    }
    for (int j=0;j<binqueue.size();j++) {
    
        std::vector<Statement> solvepoints = binqueue[j]->endpoints(mb,head);
        std::cout<<binqueue[j]->bind.tostring(0)<<"\n";
        
//        std::vector<Statement*> antisolvepoints;
    //    head->local_list(&antisolvepoints);
//        if (head->type) head->type->local_list(&antisolvepoints);
//        bool obsolete=false;
        
//        for (int q=(int)bin.size()-1;q>=0;q--) {
////            if (bin[q]->asymmetricObsoletes(binqueue[j],&antisolvepoints)) {
////                obsolete=true;
////            }
//        }
//        if (!obsolete) {
            bin.push_back(binqueue[j]);
            if (solvepoints.size()==0) {
                for (int y=0;y<upstream.size();y++) {
                    upstream[y].linked->binqueue.push_back(upstream[y].transform(binqueue[j]));
                }
            }
            
            for (int g=0;g<solvepoints.size();g++) {
                std::vector<int> afa;
                std::vector<int> dfa;
                solvepoints[g].extract(afa,binqueue[j]->bind.tracks);
                Strategy* augtypes = new Strategy[afa.size()];
//                for (int p=afa.size()-1;p>=0;p--) {
                for (int p=0;p<afa.size();p++) {
                    dfa.push_back(afa[afa.size()-1-p]);
//                    throw;//this must be fixed with scrambling function.
                    augtypes[p] = binqueue[j]->bind.localtypes[dfa[p]].applyreverse(dfa);
                    std::cout<<augtypes[p].tostring()<<"\n";
                }
                
                
                
                
                Soln* next = inst->makeorcreate(solvepoints[g].applyreverse(dfa),augtypes,afa.size());
                
                binqueue[j]->downstream.push_back(next);
                SolnLink gra = SolnLink(dfa,this,binqueue[j]);
                next->upstream.push_back(gra);
                for (int w=0;w<next->bin.size();w++) {
                    if (next->bin[w]->downstream.size()==0) {
                        binqueue.push_back(gra.transform(next->bin[w]));
                    }
                }
//            }
//            for (int q=(int)bin.size()-2;q>=0;q--) {
////                if (binqueue[j]->asymmetricObsoletes(bin[q],&antisolvepoints)) {
////                    remove(mb,inst,q);
////                }
            }
//        }
    }
    binqueue.clear();
}
void Soln::remove(SolveInstance* inst,int n) {
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
                nbind->downstream[o]->remove(inst,h);
            }
        }
    }
//    std::cout<<"deleted "<<nbind<<"\n";
//    delete nbind;
}



std::vector<Statement> Entry::endpoints(MetaBank* mb,Statement head) {
    std::vector<Statement> solvepoints;
//    bool satisfied=false;
//    while (!satisfied) {
//    throw;
    std::vector<Statement> interp;
    head.substitute(&bind,bind.tracks,interp);
    
    std::map<int,Statement> points;
    std::map<int,bool> obsoleted;
//    std::vector<std::pair<Strategy*,int>> p;
//    p.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
//    p.push_back(std::pair<Strategy*,int>(bind.localtypes,bind.ara));
    std::cout<<head.tostring()<<"=--==--==--==--==-\n";
    for (bool stage=false;;stage=true) {
        for (int p=0;p<interp.size();p++) {
            std::cout<<interp[p].tostring()<<"<-----\n";
            interp[p].getsolvepoints(points,obsoleted,bind.tracks,false,false);
            for (auto it=points.begin();it!=points.end();it++) {
                if (!obsoleted[it->first]) solvepoints.push_back(it->second);
            }
        }
        points.clear();
        obsoleted.clear();
        if (solvepoints.size() or stage) break;
    }
        
//        satisfied=true;
//        for (int y=0;y<solvepoints.size();y++) {
//            Statement* type = solvepoints[y]->generate_type();
//            if (type->local==0 and type->id==1) {
//                Statement* donworryaboudit = new Statement(0,-1);
////                donworryaboudit->specifier=2;
////                std::cout<<donworryaboudit<<"\n";
//                if (!bind.easydecompose(solvepoints[y], donworryaboudit,mb)) throw;//split needs to be performed here
//                
//                donworryaboudit->cleanup();
//                satisfied=false;
//            } else if (type->observable(mb) and type->observe_boolean(mb)) {
//                Statement* donworryaboudit = new Statement(0,-2);
////                donworryaboudit->specifier=1;
//                if (!bind.easydecompose(solvepoints[y],donworryaboudit,mb)) throw;
//                donworryaboudit->cleanup();
//                satisfied=false;
//            }
//        }
//        if (!satisfied) {
//            for (int i=0;i<solvepoints.size();i++) {
//                solvepoints[i]->cleanup();
//            }
//        }
//    }
    return solvepoints;
}
//bool Entry::asymmetricObsoletes(Entry* other, std::vector<Statement*>* terms) {
//    if (downstream.size()!=0) return false;
//    for (int s=0;s<terms->size();s++) {
//        for (int a=0;a<bind.decoms.size();a++) {
//            Binding comparison(1);
//            if (comparison.decompose((*terms)[s],bind.decoms[a].first,2,0)) {//needs to be an extra validation.
//                bool founden=false;
//                for (int b=0;b<other->bind.decoms.size();b++) {
//                    Binding comparison2(1);
//                    if (comparison2.decompose(other->bind.decoms[b].first,bind.decoms[a].first,2,0)) {
//                        if (comparison2.decompose(other->bind.decoms[b].second,bind.decoms[a].second,2,0)) {
//                        
//                            founden=true;
//                        }
//                    }
//                }
////                if (!founden) std::cout<<"KILL IS "<<(*terms)[s]->tostringheavy()<<"\n";
//                if (!founden) {
//                
//                    return false;
//                }
//            }
//        }
//    }
//    return true;
//}
Soln* SolveInstance::makeorcreate(Statement match,Strategy* potypes,int a) {
    for (int g=0;g<solns.size();g++) {
        bool val=true;
        if (a!=solns[g]->ara) continue;
        for (int j=0;j<a;j++) {
            if (!judgemental_eq(potypes[j],solns[g]->loctypes[j])) {
                val=false;
                break;
            }
        }
        if (val and judgemental_eq(match,solns[g]->head)) return solns[g];
    }
    solns.push_back(new Soln(match,potypes,a));
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

    mainsoln.recieveEnergy(mb,this,-1);
    for (int w=(int)solns.size()-1;w>0;w--) {
        if (solns[w]->upstream.size()==0) {
            delete solns[w];
            solns.erase(solns.begin()+w);
        }
    }
    heavyvisualize();
    flushvisualizer();
}
Entry* SolnLink::transform(Entry* input) {
    throw;
    Entry* res = new Entry(container->bind);
//    for (int y=0;y<input->bind.decoms.size();y++) {
//        res->bind.insert(input->bind.decoms[y].first->scramble(mapr,res->ids),input->bind.decoms[y].second->scramble(mapr,res->ids));
//    }
    return res;
}
Statement MetaBank::solve(Statement type,Strategy* loc,int buf) {
    SolveInstance inst(type,loc,buf);
    while (true) {
        inst.increment(this);
        for (int y=0;y<inst.mainsoln.bin.size();y++) {
            if (inst.mainsoln.bin[y]->downstream.size()==0) {
    //            std::cout<<bin[y]->bind.tostringheavy()<<"asdf\n";
    //            return head->substitute(&bin[y]->bind);
                std::cout<<"FOUND\n";
                throw;
            }
        }
    }
}












