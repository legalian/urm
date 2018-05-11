//
//  reasoning_core.cpp
//  URM
//
//  Created by Parker on 7/13/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"
#include "solver_core.hpp"
#include "parser_core.hpp"
#include <streambuf>
#include <fstream>

//struct LemLink {
//    int src;
//    std::vector<int> mapr;
//    LemLink(int s,std::vector<int> m) :src(s),mapr(m) {}
//};
//struct LemWrap {
//    Binding bindr;
//    std::vector<LemLink> links;
//    LemWrap(Binding b) : bindr(b) {}
//};





//void expandAll(MetaBank* mb,int head,Binding& initial,std::vector<Binding>& out) {
//    for (int j2=0;j2<mb->ara;j2++) {
//        Strategy beftransloc = mb->strategies[j2];
//        Binding soap = initial;
//        Statement headsite = initial.localtypes[head].snapshot();
//        std::vector<Strategy> nlocs;
//        for (int i=0;i<soap.ara;i++) nlocs.push_back(soap.localtypes[i]);
//        Statement bodysite = Statement(j2,0,beftransloc.ara);
//        for (int u=0;u<beftransloc.ara;u++) {
//            Strategy& ptransloc = beftransloc.args[u];
//            bodysite.args[u] = Statement(nlocs.size(),1,headsite.ara+ptransloc.ara);
//            Strategy nstrat = Strategy(idpush(ptransloc.type,headsite.args,initial.ara,headsite.ara),nlocs.size(),1,headsite.ara+ptransloc.ara);
//            for (int g=0;g<headsite.ara;g++) {
//                nstrat.args[g] = initial.localtypes[head].args[g];
//                bodysite.args[u].args[g] = nstrat.args[g].snapshot().depth_push(3,1);
//            }
//            for (int g=0;g<ptransloc.ara;g++) {
//                nstrat.args[g+headsite.ara] = idpush(ptransloc.args[g],headsite.args,initial.ara,headsite.ara);
//                bodysite.args[u].args[g+headsite.ara] = ptransloc.args[g].snapshot().depth_push(2,1);//this
//            }
//            nlocs.push_back(nstrat);
//        }
//        soap.ara = nlocs.size();
//        soap.localtypes = new Strategy[soap.ara];
//        for (int i=0;i<soap.ara;i++) soap.localtypes[i] = nlocs[i];
//        soap.tracks.dat[soap.tracks.loc()] = std::pair<Strategy*,int>(soap.localtypes,soap.ara);
//        
//        ParameterContext tocompare = soap.tracks.append(initial.localtypes[head].args,initial.localtypes[head].ara);
//        if (soap.crossdecompose(headsite,bodysite,tocompare)) {
//            soap.divide(out,0);
//        }
//    }
//}
//bool judgemental_eq(LemWrap& a,LemWrap& b) {
////    if (a.head!=b.head) return false;
//
//
//    if (a.bindr.ara!=b.bindr.ara or a.bindr.binds.size()!=b.bindr.binds.size() or a.links.size()!=b.links.size()) return false;
//    for (int j=0;j<a.bindr.ara;j++) {
//        if (!judgemental_eq(a.bindr.localtypes[j],b.bindr.localtypes[j])) return false;
//    }
//    for (int c=0;c<a.bindr.binds.size();c++) {
//        bool found=false;
//        for (int d=0;d<b.bindr.binds.size();d++) {
//            if (judgemental_eq(a.bindr.binds[c].head,b.bindr.binds[d].head)) {
//                if (!judgemental_eq(a.bindr.binds[c].body,b.bindr.binds[d].body)) return false;
//                found=true;
//                break;
//            }
//        }
//        if (!found) return false;
//    }
//    for (int c=0;c<a.links.size();c++) {
//        bool found=false;
//        for (int d=0;d<b.links.size();d++) {
//            if (judgemental_eq()) {
//                
//                found=true;
//                break;
//            }
//        }
//        if (!found) return false;
//    }
//    throw;//compare links too. also, ensure that holes that compare equivalent will share the same head index
//    return true;
//}

//Statement idpush
//void steparch(Binding& rep,Statement& head,Statement& bod,std::vector<Binding>& out) {
//    }


//void reparch(MetaBank* mb,std::vector<looparch>& out) {
//    std::vector<looparch> subby;
//    reparch(<#Statement &head#>, <#Statement &bod#>, <#std::vector<Binding> &out#>)
//    
//    for (int h=0;h<out.size();h++) {
//        Binding& rep = out[h];
//        Binding B(rep);
//        B.ara = rep.ara*2;
//        B.localtypes = new Strategy[rep.ara*2];
//        for (int j=0;j<rep.ara;j++) {
//            B.localtypes[j] = rep.localtypes[j];
//            B.localtypes[j] = idpush(rep.localtypes[j],0,rep.ara,0);
//        }
//        for (int j=0;j<rep.binds.size();j++) {
//            Strategy* transform_itinerary = new Strategy[rep.binds[j].ara];
//            for (int q=0;q<rep.binds[j].ara;q++) {
//                transform_itinerary[q] = idpush(rep.binds[j].itinerary[q],0,rep.ara,0);
//            }
//            B.binds.push_back(SingleBind(
//                idpush(rep.binds[j].head,0,rep.ara,0),
//                idpush(rep.binds[j].body,0,rep.ara,0),
//                transform_itinerary,rep.binds[j].ara,rep.binds[j].universal,rep.binds[j].concrete
//            ));
//        }
//        Statement sem = idpush(head,0,rep.ara,0);
//        if (B.crossdecompose(bod,sem,B.tracks)) {
//            B.divide(out,0);
//        }
//    }
//}


//void shortarch(MetaBank*,int,Binding&,std::vector<LemWrap>&,std::vector<LemShort>&);
//void shortarch(MetaBank* mb,int head,LemWrap& initial,std::vector<LemWrap>& outlem,std::vector<LemWrap> shorts) {
//    shorts.push_back(initial);
//    std::vector<LemWrap> cartesian;
//    std::vector<Binding> iniph;
//    expandAll(mb,head,initial.bindr,iniph);
//    for (int k=0;k<iniph.size();k++) {
//        cartesian.push_back(LemWrap(iniph[k]));
//    }
//    while (cartesian.size()) {
//        std::map<int,bool> solvepoints;
//        ParameterContext insub = cartesian[0].bindr.tracks.append(cartesian[0].bindr.localtypes[head].args,cartesian[0].bindr.localtypes[head].ara);
//        cartesian[0].bindr.localtypes[head].snapshot().substitute_single(cartesian[0].bindr,insub).getsolvepoints(solvepoints);
//        cartesian[0].bindr.localtypes[head].obsoletesolvepoints(solvepoints);
//        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) cartesian[0].bindr.localtypes[g->first].obsoletesolvepoints(solvepoints);
//        throw;//links should obsolete solvepoints. this can be done just via head.
//        bool submit=true;
//        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) if (g->second) {
//            submit=false;
//            std::vector<int> afa;
//
//            cartesian[0].bindr.localtypes[g->first].substitute_single(cartesian[0].bindr,cartesian[0].bindr.tracks).extract(afa,cartesian[0].bindr);
//            Strategy* augtypes = new Strategy[afa.size()];
//            std::vector<SingleBind> soapbinds;
//            for (int p=0;p<afa.size();p++) {
//                augtypes[p] = cartesian[0].bindr.localtypes[afa[p]].substitute_single(cartesian[0].bindr,cartesian[0].bindr.tracks).applyreverse(afa);
//                for (int o=0;o<cartesian[0].bindr.binds.size();o++) {
//                    if (cartesian[0].bindr.binds[o].head.id==g->first and cartesian[0].bindr.binds[o].body.local!=1) {
//                        Strategy* transform_itinerary = new Strategy[cartesian[0].bindr.binds[o].ara];
//                        for (int q=0;q<cartesian[0].bindr.binds[o].ara;q++) {
//                            transform_itinerary[q] = cartesian[0].bindr.binds[o].itinerary[q].applyreverse(afa);
//                        }
//                        soapbinds.push_back(SingleBind(
//                            cartesian[0].bindr.binds[o].head.applyreverse(afa),
//                            cartesian[0].bindr.binds[o].body.applyreverse(afa),
//                            transform_itinerary,cartesian[0].bindr.binds[o].ara,cartesian[0].bindr.binds[o].universal,cartesian[0].bindr.binds[o].concrete
//                        ));
//                    }
//                }
//            }
//            LemWrap soap = LemWrap(Binding(mb,augtypes,afa.size()));
//            soap.bindr.binds = soapbinds;
//            int ffin=-1;
//            for (int y=0;y<afa.size();y++) if (g->first==afa[y]) ffin=y;
//            if (ffin==-1) throw;
//            bool val=false;
//            int zd;
//            for (zd=0;zd<shorts.size();zd++) {
//                val=true;
//                if (soap.ara!=shorts[zd].attach.ara or ffin!=shorts[zd].head or soap.binds.size()!=shorts[zd].attach.binds.size()) continue;
//                for (int j=0;j<soap.ara;j++) {
//                    if (!judgemental_eq(soap.localtypes[j],shorts[zd].attach.localtypes[j])) {
//                        val=false;
//                        break;
//                    }
//                }
//                if (!val) continue;
//                for (int a=0;a<shorts[zd].attach.binds.size();a++) {
//                    bool found=false;
//                    for (int b=0;b<soap.binds.size();b++) {
//                        if (judgemental_eq(shorts[zd].attach.binds[a].head,soap.binds[b].head)) {
//                            if (!judgemental_eq(shorts[zd].attach.binds[a].body,soap.binds[b].body)) val=false;
//                            found=true;
//                            break;
//                        }
//                    }
//                    if (!found) val=false;
//                    if (!val) break;
//                }
//                throw;//compare links too. also, ensure that holes that compare equivalent will share the same head index
//                if (val) break;
//            }
//            if (val) {
////                LemWrap nc = cartesian[0];
//                soap.links.push_back(LemLink(zd,ffin,afa));
//                cartesian.push_back(soap);
//            } else {
//                std::vector<LemWrap> grag;
//                shortarch(mb,ffin,soap,grag,shorts);
//
//
//                for (int zr=0;zr<grag.size();zr++) {
//                    Binding splitter = cartesian[0].bindr;
//                    Strategy* arranged = new Strategy[splitter.ara+grag[zr].bindr.ara-afa.size()];
//                    for (int f=0;f<splitter.ara;f++) {
//                        arranged[f] = splitter.localtypes[f];
//                    }
//                    int xmm = splitter.ara;
//                    for (int z=afa.size();z<grag[zr].bindr.ara;z++) {
//                        arranged[xmm++] = grag[zr].bindr.localtypes[z].apply(afa,splitter.ara);
//                    }
//                    splitter.localtypes = arranged;
//                    splitter.ara = xmm;
//                    splitter.tracks.dat[1] = std::pair<Strategy*,int>(arranged,xmm);
//                    for (int f=0;f<grag[zr].bindr.binds.size();f++) {
//                        Strategy* transform_itinerary = new Strategy[grag[zr].bindr.binds[f].ara];
//                        for (int q=0;q<grag[zr].bindr.binds[f].ara;q++) {
//                            transform_itinerary[q] = grag[zr].bindr.binds[f].itinerary[q].apply(afa,cartesian[0].bindr.ara);
//                        }
//                        splitter.binds.push_back(SingleBind(
//                            grag[zr].bindr.binds[f].head.apply(afa,cartesian[0].bindr.ara),
//                            grag[zr].bindr.binds[f].body.apply(afa,cartesian[0].bindr.ara),
//                            transform_itinerary,grag[zr].bindr.binds[f].ara,grag[zr].bindr.binds[f].universal,grag[zr].bindr.binds[f].concrete
//                        ));
//                    }
//                    throw;//transform lemwrap
//                    std::vector<Binding> out;
//                    splitter.divide(out,0);
//                }
//            }
//        }
//        if (submit) outlem.push_back(cartesian[0]);
//        cartesian.erase(cartesian.begin());
//    }
//}


MetaBank::MetaBank(const std::string& filename) {
    std::ifstream file(filename);
    std::string strn((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    
    Strategy context = parse_TTML(strn,-1,&stratnames);
    stratnames[0]="U";
    strategies = context.args;
    ara = context.ara;
    ParameterContext empty;
    context.typecheck(empty);
    
    
    Strategy* simstrats = new Strategy[2];
    simstrats[0] = Strategy(Statement(0,0),0,1);
    simstrats[1] = Strategy(Statement(0,1),1,1);
    Binding aleph(this,simstrats,2);
    
    for (int g=0;g<ara;g++) {
        stitchbank.push_back(Stitching(this,strategies[g]));
    }
    
    offlineLearningStep();
}





//void Statement::obsoletesolvepoints(std::map<int,bool>& points) {
//    if (local==1 and points.count(id)) points[id]=false;
//    for (int u=0;u<ara;u++) args[u].obsoletesolvepoints(points);
//}
//void Strategy::obsoletesolvepoints(std::map<int,bool>& points) {
//    type.obsoletesolvepoints(points);
//    for (int u=0;u<ara;u++) args[u].obsoletesolvepoints(points);
//}
//void Statement::getsolvepoints(std::map<int,bool>& points) {
//    if (local==1) points[id]=true;
//    for (int r=0;r<ara;r++) args[r].getsolvepoints(points);
//}







//Lemma shortarch(MetaBank* mb,int ide,Binding& bind,std::vector<Lemma>&) {
////    Binding mlem = Binding(<#ParameterContext &#>, <#Strategy *#>, <#int#>)
//    Lemma res = Lemma(bind);
//    res.identifier = ide;
//    res.ownership = mb->strategies[ide].ara;
//    
//    
//    for (int g=0;g<)
//    res.children;
//    
//    
//
//
//}


//void MetaBank::generateLemma(std::vector<Lemma>& out,Strategy& strat,Binding& bind,std::vector<>&) {
//    int solvelimit = bind.ara;
//    Lemma initial = Lemma();
//    //populate(strat):
//    //  while cartesian initial(strat) populated:
//    //      each (pop member)'s active solvepoints:
//    //          each strat that applies:
//    //          strat already in tree: backlink
//    //          strat is new: cartesian<-active solvepoint = populate(active solvepoint)
//    //      no solvepoints: out<-member
//    //
//    //each strat: out<-populate(strat)
//}



//Statement idpush(Statement a,Statement* app,int amt,int amt2,bool decr) {
//    Statement res = Statement(a.local==(decr?3:1)?a.id+amt:a.local==(decr?4:2)?a.id+amt2:a.id,a.local>2 and decr?a.local-2:a.local,a.local==(decr?3:1)?a.ara+amt2:a.ara);
//    int p=0;
//    if (a.local==(decr?3:1)) {
//        for (;p<amt2;p++) res.args[p]=app[p];
//        for (;p<res.ara;p++) res.args[p] = idpush(a.args[p-amt2],app,amt,amt2,decr);
//    } else {
//        for (;p<a.ara;p++) res.args[p] = idpush(a.args[p],app,amt,amt2,decr);
//    }
//    return res;
//}
//Strategy idpush(Strategy a,Statement* app,int amt,int amt2,bool decr) {
//    Strategy res = Strategy(idpush(a.type,app,amt,amt2,decr),a.local==(decr?4:2)?a.id+amt2:a.id,a.local>2 and decr?a.local-2:a.local,a.ara);
//    for (int p=0;p<res.ara;p++) res.args[p] = idpush(a.args[p],app,amt,amt2,decr);
//    return res;
//}
////void Soln::typecheck() {
////    for (int f=0;f<initial.ara;f++) {
////        initial.localtypes[f].typecheck(initial.tracks);
////    }
//////    for (int f=0;f<binqueue[j].ara;f++) {
//////        binqueue[j].localtypes[f].typecheck(binqueue[j].tracks);
//////    }
////
////}
//void Soln::expand(MetaBank* mb,SolveInstance* inst) {
//    for (int j=0;j<binqueue.size();j++) {
//        binqueue[j].loosecheck();
//    }
////    std::cout<<"-=-=--==-=\n";
////    for (int g=0;g<initial.ara;g++) {
////        std::cout<<initial.localtypes[g].tostring()<<"\n";
////    }
//    initial.loosecheck();
//    if (!expanded) {
//        expanded=true;
//        int jchecked=binqueue.size();
//        for (int j1=0,j2=0;j2<mb->ara;j1==initial.localtypes[head].ara?j2++:j1++) {
//            bool cphase=j1==initial.localtypes[head].ara;
//            Strategy beftransloc = cphase?mb->strategies[j2]:initial.localtypes[head].args[j1];
//            Binding soap = initial;
//            Statement headsite = initial.localtypes[head].snapshot();
//            std::vector<Strategy> nlocs;
//            for (int i=0;i<soap.ara;i++) nlocs.push_back(soap.localtypes[i]);
//            Statement bodysite = Statement(cphase?j2:j1,cphase?0:2,beftransloc.ara);
//            for (int u=0;u<beftransloc.ara;u++) {
//                Strategy& ptransloc = beftransloc.args[u];//local of ptransloc could be either 1 or 3- this causes fucking obvious problem
//                bodysite.args[u] = Statement(nlocs.size(),1,headsite.ara+ptransloc.ara);
//                Strategy nstrat = Strategy(idpush(ptransloc.type,headsite.args,initial.ara,headsite.ara,!cphase),nlocs.size(),1,headsite.ara+ptransloc.ara);
//                for (int g=0;g<headsite.ara;g++) {
//                    nstrat.args[g] = initial.localtypes[head].args[g];
//                    bodysite.args[u].args[g] = nstrat.args[g].snapshot().depth_push(3,1);
//                }
//                for (int g=0;g<ptransloc.ara;g++) {
//                    nstrat.args[g+headsite.ara] = idpush(ptransloc.args[g],headsite.args,initial.ara,headsite.ara,!cphase);
//                    bodysite.args[u].args[g+headsite.ara] = ptransloc.args[g].snapshot().depth_push(2,1);
//                }
////                std::cout<<"\t\t"<<nstrat.tostringheavy()<<"\n";
//                nlocs.push_back(nstrat);
////                nstrat.loosecheck(initial.tracks);
//            }
//            soap.ara = nlocs.size();
//            soap.localtypes = new Strategy[soap.ara];
//            for (int i=0;i<soap.ara;i++) soap.localtypes[i] = nlocs[i];
//            soap.tracks.dat[soap.tracks.loc()] = std::pair<Strategy*,int>(soap.localtypes,soap.ara);
//            
//            soap.loosecheck();
//            
//            ParameterContext tocompare = soap.tracks.append(initial.localtypes[head].args,initial.localtypes[head].ara);
//            if (soap.crossdecompose(headsite,bodysite,tocompare)) {
////                int bef = binqueue.size();
//                soap.divide(binqueue,0);
////                if (binqueue.size()!=bef) std::cout<<"\t\t"<<binqueue.size()-bef<<" results.\n";
//            }
//            for (;jchecked<binqueue.size();jchecked++) {
//                binqueue[jchecked].loosecheck();
//            }
//        }
////        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
//    }
//    for (int j=0,bkmax=binqueue.size();j<bkmax;j++) {
//        std::map<int,bool> solvepoints;
//        ParameterContext insub = binqueue[j].tracks.append(binqueue[j].localtypes[head].args,binqueue[j].localtypes[head].ara);
//        binqueue[j].localtypes[head].snapshot().substitute_single(binqueue[j],insub).getsolvepoints(solvepoints);
//        binqueue[j].localtypes[head].obsoletesolvepoints(solvepoints);
//        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) binqueue[j].localtypes[g->first].obsoletesolvepoints(solvepoints);
//        Entry* nxtbin = new Entry(binqueue[j]);
//        bool submit=true;
//        for (auto g = solvepoints.begin();g!=solvepoints.end();g++) if (g->second) {
//            submit=false;
//            std::vector<int> afa;
//            if (g->first>=binqueue[j].ara) throw;
////            binqueue[j].loosecheck();
//            //binqueue[j].localtypes gets corrupted
////            std::cout<<"-==--=-==-=--=-==-=-\n";
//            binqueue[j].localtypes[g->first].substitute_single(binqueue[j],binqueue[j].tracks).extract(afa,binqueue[j]);
//            Strategy* augtypes = new Strategy[afa.size()];
//            std::vector<SingleBind> soapbinds;
//            for (int p=0;p<afa.size();p++) {
//                if (afa[p]>=binqueue[j].ara) throw;
//                augtypes[p] = binqueue[j].localtypes[afa[p]].substitute_single(binqueue[j],binqueue[j].tracks).applyreverse(afa);
//                for (int o=0;o<binqueue[j].binds.size();o++) {
//                    if (binqueue[j].binds[o].head.id==g->first and binqueue[j].binds[o].body.local!=1) {
//                        Strategy* transform_itinerary = new Strategy[binqueue[j].binds[o].ara];
//                        for (int q=0;q<binqueue[j].binds[o].ara;q++) {
//                            transform_itinerary[q] = binqueue[j].binds[o].itinerary[q].applyreverse(afa);
//                        }
//                        soapbinds.push_back(SingleBind(
//                            binqueue[j].binds[o].head.applyreverse(afa),
//                            binqueue[j].binds[o].body.applyreverse(afa),
//                            transform_itinerary,binqueue[j].binds[o].ara,binqueue[j].binds[o].universal,binqueue[j].binds[o].concrete
//                        ));
//                    }
//                }
//            }
//            Binding soap = Binding(mb,augtypes,afa.size());
//            soap.binds = soapbinds;
//            int ffin=-1;
//            for (int y=0;y<afa.size();y++) if (g->first==afa[y]) ffin=y;
//            if (ffin==-1) throw;
//            Soln* next = inst->makeorcreate(soap,ffin);
//            nxtbin->downstream.push_back(next);
//            SolnLink gra = SolnLink(afa,this,nxtbin);
//            next->upstream.push_back(gra);
//            for (int w=0;w<next->bin.size();w++) {
//                if (next->bin[w]->downstream.size()==0) {
//                    gra.transform(next->bin[w]->bind,binqueue);
//                }
//            }
//        }
//        if (submit) {
//            for (int y=0;y<upstream.size();y++) {
//                upstream[y].transform(binqueue[j],upstream[y].linked->binqueue);
//            }
//        }
//        bin.push_back(nxtbin);
//    }
//    binqueue.clear();
//}
//Soln* SolveInstance::makeorcreate(Binding& nnew,int nhead) {
//    for (int g=0;g<solns.size();g++) {
//        bool val=true;
//        if (nnew.ara!=solns[g]->initial.ara or nhead!=solns[g]->head or nnew.binds.size()!=solns[g]->initial.binds.size()) continue;
//        for (int j=0;j<nnew.ara;j++) {
//            if (!judgemental_eq(nnew.localtypes[j],solns[g]->initial.localtypes[j])) {
//                val=false;
//                break;
//            }
//        }
//        if (!val) continue;
//        for (int a=0;a<solns[g]->initial.binds.size();a++) {
//            bool found=false;
//            for (int b=0;b<nnew.binds.size();b++) {
//                if (judgemental_eq(solns[g]->initial.binds[a].head,nnew.binds[b].head)) {
//                    if (!judgemental_eq(solns[g]->initial.binds[a].body,nnew.binds[b].body)) val=false;
//                    found=true;
//                    break;
//                }
//            }
//            if (!found) val=false;
//            if (!val) break;
//        }
//        if (val) {
//            return solns[g];
//        }
//    }
//    solns.push_back(new Soln(nnew,nhead));
//    return solns[solns.size()-1];
//}
//bool Soln::recieveEnergy(MetaBank* mb,SolveInstance* si,int energy) {
//    if (binqueue.size() or !expanded) {
//        expand(mb,si);
//        return true;
//    }
//    for (int p=0;energy==-1 or p<energy;p++) {
//        for (int e=0;e<bin.size();e++) {
//            for (int a=0;a<bin[e]->downstream.size();a++) {
//                if (bin[e]->downstream[a]->recieveEnergy(mb,si,p)) return true;
//            }
//        }
//    }
//    return false;
//}
//void SolveInstance::increment(MetaBank* mb) {
//    clock_t t = clock();
//    printf ("Calculating...\n");
//    solns[0]->recieveEnergy(mb,this,-1);
//    t = clock() - t;
//    std::cout<<"round elapsed: "<<(((float)t)/CLOCKS_PER_SEC)<<"\n";
//    
////    gephi_visualize();
////    gephi_flushvisualizer();
//}
//
//void SolnLink::transform(Binding input,std::vector<Binding>& out) {
//    Binding splitter = container->bind;
//    Strategy* arranged = new Strategy[container->bind.ara+input.ara-mapr.size()];
//    for (int f=0;f<container->bind.ara;f++) {
//        arranged[f] = container->bind.localtypes[f];
//    }
//    int xmm = container->bind.ara;
//    for (int z=mapr.size();z<input.ara;z++) {
//        arranged[xmm++] = input.localtypes[z].apply(mapr,container->bind.ara);
//    }
//    splitter.localtypes = arranged;
//    splitter.ara = xmm;
//    splitter.tracks.dat[1] = std::pair<Strategy*,int>(arranged,xmm);
//    for (int f=0;f<input.binds.size();f++) {
//        Strategy* transform_itinerary = new Strategy[input.binds[f].ara];
//        for (int q=0;q<input.binds[f].ara;q++) {
//            transform_itinerary[q] = input.binds[f].itinerary[q].apply(mapr,container->bind.ara);
//        }
//        splitter.binds.push_back(SingleBind(
//            input.binds[f].head.apply(mapr,container->bind.ara),
//            input.binds[f].body.apply(mapr,container->bind.ara),
//            transform_itinerary,input.binds[f].ara,input.binds[f].universal,input.binds[f].concrete));
//    }
//    splitter.divide(out,0);
//}
//Statement MetaBank::solve(Strategy* loc,int buf,int nhead) {
//    Binding incoming = Binding(&meta_prime,loc,buf);
//    SolveInstance inst(incoming,nhead);
//    while (true) {
//        inst.increment(this);
//        for (int y=0;y<inst.solns[0]->bin.size();y++) {
//            if (inst.solns[0]->bin[y]->downstream.size()==0) {
//                inst.gephi_visualize();
////                inst.gephi_flushvisualizer();
//                ParameterContext augpra = inst.solns[0]->bin[y]->bind.tracks.append(loc[nhead]);
//                return loc[nhead].snapshot().substitute_single(inst.solns[0]->bin[y]->bind,augpra);
//            }
//        }
//    }
//}












