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
    
    "[U"
    "|AFF:U"
    
    "|[A:U|A|A]EQ:U"
    "|[AFF|AFF]GT:U"
    "|[U]NOT:U"
    
    "|[U|U]AND:U"
    "|[A:U|B:U|AND(A,B)]ANDACC:A"
    "|[A:U|B:U|AND(A,B)]AND_COMMUTATIVE:AND(B,A)"
    "|[A:U|B:U|C:U|AND(A,AND(B,C))]AND_ASSOCIATIVE:AND(AND(A,B),C)"
    "|[A:U|B:U|a:AND(A,B)]AND_REVERSED:EQ(AND(A,B),AND_COMMUTATIVE(B,A,AND_COMMUTATIVE(A,B,a)),a)"
    "|[A:U|B:U|A|B]UNION:AND(A,B)"
    "|[A:U|B:U|a:A|b:B]MEMBER:EQ(A,ANDACC(A,B,UNION(A,B,a,b)),a)"
    
//
    "|[A:U]DOUBLENEG:EQ(U,NOT(NOT(A)),A)"
    "|[A:U]EXCLUSION:NOT(AND(A,NOT(A)))"
    "|[A:U|B:U|A|NOT(AND(A,B))]INCLUSION:NOT(B)"
//




    "|[a:AFF|b:AFF]TRICHOTOMY_E:EQ(U,EQ(AFF,a,b),AND(NOT(GT(a,b)),NOT(GT(b,a))))"
    "|[a:AFF|b:AFF]TRICHOTOMY_G:EQ(U,GT(a,b),AND(NOT(EQ(AFF,a,b)),NOT(GT(b,a))))"
//
    "|[AFF|AFF]ADD:AFF"
    "|[AFF|AFF]SUBTRACT:AFF"
    "|[AFF|AFF]MULTIPLY:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<0>))|AFF]DIVIDE:AFF"
    "|[a:AFF|NOT(EQ(AFF,a,<0>))|AFF]MODULO:AFF"
    
    
//    "|[a:AFF|b:AFF|EQ(AFF,MODULO())]"
    
    "|[a:AFF|b:AFF|c:AFF|d:AFF|GT(a,b)|NOT(GT(d,c))]ADDITIVE:GT(ADD(a,c),ADD(b,d))"
    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT(c,<0>)]MULTIPLICATIVE:GT(MULTIPLY(a,c),MULTIPLY(b,c))"
    "|[a:AFF|b:AFF|c:AFF|GT(a,b)|GT(<0>,c)]FLIPPEDMULTIPLICATIVE:GT(MULTIPLY(b,c),MULTIPLY(a,c))"
//
//
    "|[a:AFF|b:AFF|c:AFF]ADD_ASSOCIATIVE:EQ(AFF,ADD(ADD(a,b),c),ADD(a,ADD(b,c)))"
    "|[a:AFF|b:AFF|c:AFF]MULTIPLY_ASSOCIATIVE:EQ(AFF,MULTIPLY(MULTIPLY(a,b),c),MULTIPLY(a,MULTIPLY(b,c)))"

    "|[a:AFF|b:AFF]ADD_COMMUTATIVE:EQ(AFF,ADD(a,b),ADD(b,a))"
    "|[a:AFF|b:AFF]MULTIPLY_COMMUTATIVE:EQ(AFF,MULTIPLY(a,b),MULTIPLY(b,a))"

    "|[a:AFF]ADD_IDENTITY:EQ(AFF,ADD(a,<0>),a)"
    "|[a:AFF]MULTIPLY_IDENTITY:EQ(AFF,MULTIPLY(a,<1>),a)"
    "|[a:AFF]ADD_IDENTITY_ANNIHALATE:EQ(AFF,SUBTRACT(a,a),<0>)"
    "|[a:AFF|g:NOT(EQ(AFF,a,<0>))]MULTIPLY_IDENTITY_ANNIHALATE:EQ(AFF,DIVIDE(a,g,a),<1>)"

    "|[a:AFF|b:AFF|c:AFF]DISTRIBUTIVE:EQ(AFF,MULTIPLY(a,ADD(b,c)),ADD(MULTIPLY(a,b),MULTIPLY(a,c)))"

    "|[a:AFF|b:AFF]SUBTRACT_REWRITE:EQ(AFF,SUBTRACT(a,b),ADD(a,MULTIPLY(<-1>,b)))"
    "|[a:AFF|b:AFF|g:NOT(EQ(AFF,a,<0>))]DIVIDE_REWRITE:EQ(AFF,DIVIDE(a,g,b),MULTIPLY(b,DIVIDE(a,g,<1>)))"

    "|[A:U|a:A]IDENTITY:EQ(A,a,a)"
    "|[A:U|B:U|A|EQ(U,A,B)]EQUIVALENCE:B"
    "|[A:U|B:U|[A]B|[B]A]HOMOMORPH:EQ(U,A,B)"
    "|[A:U|a:A|b:A|EQ(A,b,a)]REFLEXIVITY:EQ(A,a,b)"
    "|[A:U|B:U|[A]f:B|a:A|b:A|EQ(A,a,b)]PRESERVATION:EQ(B,f(a),f(b))"
    
    
    
    //here onwards is optional for tentative testing
    
    
    
    
//    "|[[AFF]AFF]test:U"
//    "|[[AFF]a:AFF]test2:test(a({0/2}))"
    
    
    
    "|[T:U|K:U|[K]T|[NOT(K)]T]DEC:T"
    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T|k:K]DEC_POSITIVE:EQ(T,DEC(T,K,a({0/3}),b({0/3})),a(k))"
    "|[T:U|K:U|[K]a:T|[NOT(K)]b:T]DEC_COMMUTE:EQ(T,DEC(T,K,a({0/3}),b({0/3})),DEC(T,NOT(K),b({0/3}),a(EQUIVALENCE(NOT(NOT(K)),K,{0/3},DOUBLENEG(K)))))"

    
    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A]TERMINATION:U"
    
    
    "|[A:U|[A]B:U|[a1:A]f:B(a1)|a3:A]UNRECURSIVE:TERMINATION(A,B({0/2}),f({0/2}),a3)"
    



    
//    "|[A:U|[A]h:A|[A]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|j:A|J:B(j)|[b:B(h(j))]EQ(B(j),f(j,b),J)|TERMINATION(A,A,DEC(A,EQ(A,{0/4},j),{1/4}(h({0/4}))))]CLOSURE:TERMINATION(A,B({0/3}),f({0/3},{1/3}(h({0/3}))),a3)"
        
        
        
    
    "|[A:U|[A]h:A|[A]B:U|[a1:A|B(h(a1))]f:B(a1)|a3:A|TERMINATION(A,B({0/3}),f({0/3},{1/3}(h({0/3}))),h(a3))]RECURSION:TERMINATION(A,B({0/2}),f({0/2},{1/2}(h({0/2}))),a3)"
    
//
    

    "|[A:U|[A]B:U|[a1:A|[a2:A]B(a2)]f:B(a1)|a3:A|TERMINATION(A,B({0/3}),f({0/3},{1/3}({0/4})),a3)]INDUCTION:B(a3)"
    
    
    
    
    
    
//    "|[A:U|B:U|[A]P:U|[A]f:A|[AFF]h:A|EQ(A,h(<0>),t)|[x:AFF]EQ(A,f(h(a)),h(ADD(x,1)))|y:AFF|P(h(y))|[A]c:B]SERIES:TERMINATION(A,B,DEC(B,P({0/3}),c({0/3}),{1/3}(f({0/3}))),t)"



    "|[trash:NOT(EQ(AFF,<1>,<0>))|A:U|[A]B:U|[A]f:A|[a1:A|B(f(a1))]F:B(a1)|t:A|[AFF]h:A|EQ(A,h(<0>),t)|[x:AFF]EQ(A,f(h(x)),h(ADD(x,<1>)))|y:AFF|NOT(GT(<0>,y))|EQ(AFF,MODULO(<1>,trash,y),<0>)|j:B(h(y))|[vv:B(f(h(y)))]EQ(B(h(y)),F(h(y),vv),j)]SERIES:TERMINATION(A,B({0/2}),F({0/2},{1/2}(f({0/2}))),t)"




//    "|[A:U|B:U|[A]P:U|[A]f:A|[AFF]h:A|EQ(A,h(<0>),t)|[x:AFF]EQ(A,f(h(a)),h(ADD(x,1)))|y:AFF|P(h(y))|[A]c:B]SERIES:TERMINATION(A,B,DEC(B,P({0/3}),c({0/3}),{1/3}(f({0/3}))),t)"
    
    
    
//
//
    "|HEAP:U"
    "|[HEAP|AFF]ALLOCATED:U"
    
    "|[HEAP|AFF]RESERVE:HEAP"
    "|[h:HEAP|a:AFF|b:AFF|[c:AFF|GT(ADD(a,b),c)|NOT(GT(a,c))]ALLOCATED(h,c)]DELETE:HEAP"
    "|[h:HEAP|a:AFF|ALLOCATED(h,a)|U]SET:HEAP"
    "|[h:HEAP|a:AFF|ALLOCATED(h,a)]GET:U"
    
    
    

//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]U]zipper:U"
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zippertest:zipper(t({0/2}({0/3}({0/4}({0/5}({0/6}({0/7})))))))"
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U|zipper(t({0/3}({0/4}({0/5}({0/6}({0/7}({0/8})))))))]zipperconfirm:U"
//    
//    "|[[[[[[[U]HEAP]AFF]U]HEAP]AFF]t:U]zipperfinal:"
//    "zipperconfirm(t({0/2}({0/3}({0/4}({0/5}({0/6}({0/7})))))),zippertest(t({0/3}({0/4}({0/5}({0/6}({0/7}({0/8}))))))))"
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
    
//    std::cout<<"Accepted axioms:\n";
    strategies = context.args;
    ara = context.ara;
//    for (int u=0;u<ara;u++) {
//        std::cout<<"\t"<<strategies[u].tostring()<<"\n";
//    }
    context.typecheck(ParameterContext());
}
void Statement::obsoletesolvepoints(std::map<int,bool>& points) {
    if (local==1 and points.count(id)) points[id]=false;
    for (int u=0;u<ara;u++) args[u].obsoletesolvepoints(points);
}
void Strategy::obsoletesolvepoints(std::map<int,bool>& points) {
    type.obsoletesolvepoints(points);
    for (int u=0;u<ara;u++) args[u].obsoletesolvepoints(points);
}
void Statement::getsolvepoints(std::map<int,bool>& points) {
    if (local==1) points[id]=true;
    for (int r=0;r<ara;r++) args[r].getsolvepoints(points);
}
Statement idpush(Statement a,Statement* app,int amt,int amt2,bool decr) {
    Statement res = Statement(a.local==(decr?3:1)?a.id+amt:a.local==(decr?4:2)?a.id+amt2:a.id,a.local>2 and decr?a.local-2:a.local,a.local==(decr?3:1)?a.ara+amt2:a.ara);
    int p=0;
    if (a.local==(decr?3:1)) {
        for (;p<amt2;p++) res.args[p]=app[p];
        for (;p<res.ara;p++) res.args[p] = idpush(a.args[p-amt2],app,amt,amt2,decr);
    } else {
        for (;p<a.ara;p++) res.args[p] = idpush(a.args[p],app,amt,amt2,decr);
    }
    return res;
}
Strategy idpush(Strategy a,Statement* app,int amt,int amt2,bool decr) {
    Strategy res = Strategy(idpush(a.type,app,amt,amt2,decr),a.local==(decr?4:2)?a.id+amt2:a.id,a.local>2 and decr?a.local-2:a.local,a.ara);
    for (int p=0;p<res.ara;p++) res.args[p] = idpush(a.args[p],app,amt,amt2,decr);
    return res;
}

void Soln::expand(MetaBank* mb,SolveInstance* inst) {
    if (!expanded) {
        expanded=true;
        for (int j1=0,j2=0;j2<mb->ara;j1==initial.localtypes[head].ara?j2++:j1++) {
            bool cphase=j1==initial.localtypes[head].ara;
            Strategy beftransloc = cphase?mb->strategies[j2]:initial.localtypes[head].args[j1];
            Binding soap = initial;
            Statement headsite = initial.localtypes[head].snapshot();
            std::vector<Strategy> nlocs;
            for (int i=0;i<soap.ara;i++) nlocs.push_back(soap.localtypes[i]);
            Statement bodysite = Statement(cphase?j2:j1,cphase?0:2,beftransloc.ara);
            for (int u=0;u<beftransloc.ara;u++) {
                Strategy& ptransloc = beftransloc.args[u];
                bodysite.args[u] = Statement(nlocs.size(),1,headsite.ara+ptransloc.ara);
                Strategy nstrat = Strategy(idpush(ptransloc.type,headsite.args,initial.ara,headsite.ara,!cphase),nlocs.size(),1,headsite.ara+ptransloc.ara);
                for (int g=0;g<headsite.ara;g++) {
                    nstrat.args[g] = initial.localtypes[head].args[g];
                    bodysite.args[u].args[g] = nstrat.args[g].snapshot().depth_push(3,1);
                }
                for (int g=0;g<ptransloc.ara;g++) {
                    nstrat.args[g+headsite.ara] = idpush(ptransloc.args[g],headsite.args,initial.ara,headsite.ara,!cphase);
                    bodysite.args[u].args[g+headsite.ara] = ptransloc.args[g].snapshot().depth_push(2,1);
                }
//                std::cout<<"\t\t"<<nstrat.tostringheavy()<<"\n";
                nlocs.push_back(nstrat);
            }
            soap.ara = nlocs.size();
            soap.localtypes = new Strategy[soap.ara];
            for (int i=0;i<soap.ara;i++) soap.localtypes[i] = nlocs[i];
            soap.tracks.dat[soap.tracks.loc()] = std::pair<Strategy*,int>(soap.localtypes,soap.ara);
            ParameterContext tocompare = soap.tracks.append(initial.localtypes[head].args,initial.localtypes[head].ara);
            if (soap.crossdecompose(headsite,bodysite,tocompare)) {
//                int bef = binqueue.size();
                soap.divide(binqueue,0);
//                if (binqueue.size()!=bef) std::cout<<"\t\t"<<binqueue.size()-bef<<" results.\n";
            }
        }
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
    }
    for (int j=0,bkmax=binqueue.size();j<bkmax;j++) {
        std::map<int,bool> solvepoints;
        ParameterContext insub = binqueue[j].tracks.append(binqueue[j].localtypes[head].args,binqueue[j].localtypes[head].ara);
        binqueue[j].localtypes[head].snapshot().substitute_single(binqueue[j],insub).getsolvepoints(solvepoints);
        binqueue[j].localtypes[head].obsoletesolvepoints(solvepoints);
        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) binqueue[j].localtypes[g->first].obsoletesolvepoints(solvepoints);
        Entry* nxtbin = new Entry(binqueue[j]);
        bool submit=true;
        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) if (g->second) {
            submit=false;
            std::vector<int> afa;
            if (g->first>=binqueue[j].ara) throw;
            binqueue[j].localtypes[g->first].substitute_single(binqueue[j],binqueue[j].tracks).extract(afa,binqueue[j]);
            Strategy* augtypes = new Strategy[afa.size()];
            std::vector<SingleBind> soapbinds;
            for (int p=0;p<afa.size();p++) {
                if (afa[p]>=binqueue[j].ara) throw;
                augtypes[p] = binqueue[j].localtypes[afa[p]].substitute_single(binqueue[j],binqueue[j].tracks).applyreverse(afa);
                for (int o=0;o<binqueue[j].binds.size();o++) {
                    if (binqueue[j].binds[o].head.id==g->first and binqueue[j].binds[o].body.local!=1) {
                        Strategy* transform_itinerary = new Strategy[binqueue[j].binds[o].ara];
                        for (int q=0;q<binqueue[j].binds[o].ara;q++) {
                            transform_itinerary[q] = binqueue[j].binds[o].itinerary[q].applyreverse(afa);
                        }
                        soapbinds.push_back(SingleBind(binqueue[j].binds[o].head.applyreverse(afa),binqueue[j].binds[o].body.applyreverse(afa),
                        transform_itinerary,binqueue[j].binds[o].ara
                        ));
                    }
                }
            }
            Binding soap = Binding(mb,augtypes,afa.size());
            soap.binds = soapbinds;
            int ffin=-1;
            for (int y=0;y<afa.size();y++) if (g->first==afa[y]) ffin=y;
            if (ffin==-1) throw;
            Soln* next = inst->makeorcreate(soap,ffin);
            nxtbin->downstream.push_back(next);
            SolnLink gra = SolnLink(afa,this,nxtbin);
            next->upstream.push_back(gra);
            for (int w=0;w<next->bin.size();w++) {
                if (next->bin[w]->downstream.size()==0) {
                    gra.transform(next->bin[w]->bind,binqueue);
                }
            }
        }
        if (submit) {
            for (int y=0;y<upstream.size();y++) {
                upstream[y].transform(binqueue[j],upstream[y].linked->binqueue);
            }
        }
        bin.push_back(nxtbin);
    }
    binqueue.clear();
}
Soln* SolveInstance::makeorcreate(Binding& nnew,int nhead) {
    for (int g=0;g<solns.size();g++) {
        bool val=true;
        if (nnew.ara!=solns[g]->initial.ara or nhead!=solns[g]->head or nnew.binds.size()!=solns[g]->initial.binds.size()) continue;
        for (int j=0;j<nnew.ara;j++) {
            if (!judgemental_eq(nnew.localtypes[j],solns[g]->initial.localtypes[j])) {
                val=false;
                break;
            }
        }
        if (!val) continue;
        for (int a=0;a<solns[g]->initial.binds.size();a++) {
            bool found=false;
            for (int b=0;b<nnew.binds.size();b++) {
                if (judgemental_eq(solns[g]->initial.binds[a].head,nnew.binds[b].head)) {
                    if (!judgemental_eq(solns[g]->initial.binds[a].body,nnew.binds[b].body)) val=false;
                    found=true;
                    break;
                }
            }
            if (!found) val=false;
            if (!val) break;
        }
        if (val) {
            return solns[g];
        }
    }
    solns.push_back(new Soln(nnew,nhead));
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
    clock_t t = clock();
    printf ("Calculating...\n");
    solns[0]->recieveEnergy(mb,this,-1);
    t = clock() - t;
    std::cout<<"round elapsed: "<<(((float)t)/CLOCKS_PER_SEC)<<"\n";
    
//    gephi_visualize();
//    gephi_flushvisualizer();
}

void SolnLink::transform(Binding input,std::vector<Binding>& out) {
    Binding splitter = container->bind;
    Strategy* arranged = new Strategy[container->bind.ara+input.ara-mapr.size()];
    for (int f=0;f<container->bind.ara;f++) {
        arranged[f] = container->bind.localtypes[f];
    }
    int xmm = container->bind.ara;
    for (int z=mapr.size();z<input.ara;z++) {
        arranged[xmm++] = input.localtypes[z].apply(mapr,container->bind.ara);
    }
    splitter.localtypes = arranged;
    splitter.ara = xmm;
    splitter.tracks.dat[1] = std::pair<Strategy*,int>(arranged,xmm);
    for (int f=0;f<input.binds.size();f++) {
        splitter.binds.push_back(SingleBind(
            input.binds[f].head.apply(mapr,container->bind.ara),
            input.binds[f].body.apply(mapr,container->bind.ara),input.binds[f].itinerary,input.binds[f].ara));
    }
    splitter.divide(out,0);
}
Statement MetaBank::solve(Strategy* loc,int buf,int nhead) {
    Binding incoming = Binding(&meta_prime,loc,buf);
    SolveInstance inst(incoming,nhead);
    while (true) {
        inst.increment(this);
        for (int y=0;y<inst.solns[0]->bin.size();y++) {
            if (inst.solns[0]->bin[y]->downstream.size()==0) {
                inst.gephi_visualize();
//                inst.gephi_flushvisualizer();
                ParameterContext augpra = inst.solns[0]->bin[y]->bind.tracks.append(loc[nhead]);
                return loc[nhead].snapshot().substitute_single(inst.solns[0]->bin[y]->bind,augpra);
            }
        }
    }
}












