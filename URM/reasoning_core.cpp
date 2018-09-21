//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"


MetaBank MetaBank::meta_prime("core.ax");





Binding::Binding(const Binding& l,const Binding& r,int common,int rcara,Strategy* rcharms) {
    ara = l.ara+r.ara-common;
    localtypes = new Strategy[ara];
    for (int a=0;a<l.ara;a++) localtypes[a] = l.localtypes[a].deepcopy();
    for (int a=common;a<r.ara;a++) {
        localtypes[a+l.ara-common] = r.localtypes[a].deepcopy();
        localtypes[a+l.ara-common].idpush(l.tracks.loc(),l.ara-common,common);
        if (rcara) localtypes[a+l.ara-common].expandLocs(l.tracks.loc(),localtypes[a+l.ara-common].ara,rcara,rcharms);
    }
    for (int a=0;a<l.tracks.dat.size()-1;a++) tracks.dat.push_back(l.tracks.dat[a]);
    tracks.dat.push_back({localtypes,ara});
    for (int j=0;j<l.binds.size();j++) binds.push_back(l.binds[j].deepcopy());
    for (int j=0;j<r.binds.size();j++) {
        binds.push_back(r.binds[j].deepcopy());
        binds[binds.size()-1].head.idpush(l.tracks.loc(),l.ara-common,common);
        binds[binds.size()-1].body.idpush(l.tracks.loc(),l.ara-common,common);
        for (int k=0;k<binds[binds.size()-1].ara;k++) binds[binds.size()-1].itinerary[k].idpush(tracks.loc(),l.ara-common,common);
        if (rcara) binds[binds.size()-1].head.expandLocs(tracks.loc(),binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        if (rcara) binds[binds.size()-1].body.expandLocs(tracks.loc(),binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        if (rcara) expandRange(tracks.loc(),binds[binds.size()-1].ara,binds[binds.size()-1].itinerary,rcara,rcharms);
    }
//    loosecheck();
}
Strategy ParameterContext::generateType(const Statement& a) const {
    if (a.id<0 or a.local<0) throw;
    if (a.local>=dat.size() or a.local<0) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
    if (dat[a.local].first[a.id].ara!=a.ara) {throw;}
    
    return dat[a.local].first[a.id].ndisp_sub(lloc+1,loc()-lloc,a.args,a.ara,0);
}
bool judgemental_eq(const Strategy& a,const Strategy& b) {
    if (a.id!=b.id or a.local!=b.local) return false;
    if (a.ara!=b.ara) return false;
    for (int k=0;k<a.ara;k++) {
        if (!judgemental_eq(a.args[k],b.args[k])) return false;
    }
    return judgemental_eq(a.type,b.type);
}
bool judgemental_eq(const Statement& a,const Statement& b) {
    if (a.local!=b.local or a.id!=b.id) return false;
    if (a.ara!=b.ara) throw;
    for (int u=0;u<a.ara;u++) {
        if (!judgemental_eq(a.args[u],b.args[u])) return false;
    }
    return true;
}

Statement increase(Statement& ob,int low,int delta) {
    Statement res = Statement(ob.id>low?ob.id+delta:ob.id,ob.local,ob.ara);
    for (int g=0;g<ob.ara;g++) res.args[g] = increase(ob.args[g],low,delta);
    return res;
}
Statement restrictsantize(Statement& ob) {
    Statement res = Statement(ob.id,ob.local,ob.id==1?0:ob.ara);
    for (int g=0;g<res.ara;g++) res.args[g] = restrictsantize(ob.args[g]);
    return res;
}

void Binding::insert(SingleBind c) {
    for (int j=0;j<binds.size();j++) if (judgemental_eq(c.head,binds[j].head) and judgemental_eq(c.body,binds[j].body)) return;
    binds.push_back(c);
}

//bool Binding::crossdecomposeverbal(Statement& left,Statement& right,ParameterContext& params,int tabs) {
//    for (int j=0;j<tabs;j++) std::cout<<"\t";
//    std::cout<<"Cross-Comparing: "<<left.tostring()<<" and "<<right.tostring()<<"\n";
//    return crossdecompose(left,right,params);
//
////    Strategy ltype = params.generateType(left);
////    Strategy rtype = params.generateType(right);
////    if (!decomposeverbal(ltype.type,rtype.type,params,tabs)) return false;
////    ltype.cleanup();
////    rtype.cleanup();
////    return decomposeverbal(left,right,params,tabs);
//}
//bool Binding::decomposeverbal(Statement& left,Statement& right,ParameterContext& params,int tabs) {
//    for (int j=0;j<tabs;j++) std::cout<<"\t";
//    std::cout<<"Comparing: "<<left.tostring()<<" and "<<right.tostring()<<"\n";
//    return decompose(left,right,params);
//}
bool Binding::crossdecompose(const Statement& left,const Statement& right,const ParameterContext& params) {
    Strategy ltype = params.generateType(left);
    Strategy rtype = params.generateType(right);
    bool wrk = decompose(ltype.type,rtype.type,params);
    ltype.cleanup();
    rtype.cleanup();
    if (!wrk) return false;
    return decompose(left,right,params);
}
bool Binding::bindonuniversal(int tar,const Statement& templ) {
    ParameterContext cont = tracks.append(localtypes[tar]);
    Statement plain = localtypes[tar].snapshot(tracks.loc()+1);
    Strategy btype = cont.generateType(templ);
//    std::cout<<"COMPARING: "<<localtypes[tar].type.tostring()<<" | "<<btype.type.tostring()<<"\n";
//    for (int m=0;m<ara;m++) std::cout<<localtypes[m].ara<<",";std::cout<<"\n";
    bool wrk = decompose(localtypes[tar].type,btype.type,cont);
    btype.cleanup();
    if (!wrk) {plain.cleanup();return false;}
    binds.push_back(SingleBind(plain,templ.deepcopy(),deepcopy(localtypes[tar].args,localtypes[tar].ara),localtypes[tar].ara,true,!templ.contains(tracks.loc())));
    return true;
}
bool Binding::decompose(const Statement& left,const Statement& right,const ParameterContext& params) {
    if ((left.local==tracks.loc() and left.id==-1) or (right.local==tracks.loc() and right.id==-1)) return true;
    if (left.local==tracks.loc() or right.local==tracks.loc()) {
        if (judgemental_eq(left,right)) return true;
        Statement modl = params.morph(tracks.loc(),left,params.loc());
        Statement modr = params.morph(tracks.loc(),right,params.loc());
        if (left.local!=tracks.loc())
        insert(SingleBind(modr,modl,params.compress(tracks.loc()),params.index(tracks.loc()),false,!modl.contains(tracks.loc())));
        else
        insert(SingleBind(modl,modr,params.compress(tracks.loc()),params.index(tracks.loc()),false,!modr.contains(tracks.loc())));
        return true;
    }
    if (left.local==right.local and left.id==right.id) {
        if (left.ara!=right.ara) throw;
        if (left.local==-1) return true;
        if (left.ara) {
            Strategy calctype = params.generateType(left);
            for (int u=0;u<left.ara;u++) {
                ParameterContext continued = params.append(calctype.args[u]);
                if (!decompose(left.args[u], right.args[u],continued)) {
                    calctype.cleanup();
                    return false;
                }
            }
            calctype.cleanup();
        }
        return true;
    }
    return false;
}
bool Binding::typebind(const Statement& a,const Statement& type,const ParameterContext& params) {
    Strategy calctype = params.generateType(a);
    if (!decompose(calctype.type,type,params)) {calctype.cleanup();return false;}
    if (calctype.ara!=a.ara) throw;
    for (int u=0;u<a.ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        typebind(a.args[u],calctype.args[u].type,nn);
    }
    calctype.cleanup();
    return true;
}
Strategy ParameterContext::morph(int local,const Strategy& a,int recur) const {
    Strategy res = Strategy(morph(local,a.type,a.local-recur),a.id,local+1+recur,a.ara);
    for (int j=0;j<a.ara;j++) res.args[j] = morph(local,a.args[j],recur+1);
    return res;
}
Statement ParameterContext::morph(int local,const Statement& a,int limit) const {
    int nl=a.local;
    int ni=a.id;
    if (a.local>limit) nl=a.local+local+1-limit;
    else if (a.local>local) {
        nl=local+1;
        for (int j=local+1;j<a.local;j++) ni+=dat[j].second;
    }
    Statement res = Statement(ni,nl,a.ara);
    for (int j=0;j<a.ara;j++) res.args[j] = morph(local,a.args[j],limit);
    return res;
}
Strategy* ParameterContext::compress(int loc) const {
    Strategy* end = new Strategy[index(loc)];
    int k=0;
    for (int j=loc+1;j<dat.size();j++) {
        for (int h=0;h<dat[j].second;h++) {
            end[k] = morph(loc,dat[j].first[h],0);
            end[k].id=k;
            k++;
        }
    }
    return end;
}
Statement* ParameterContext::artifact(int local) const {
    Statement* end = new Statement[index(local)];
    int k=0;
    for (int j=local+1;j<dat.size();j++) {
        for (int h=0;h<dat[j].second;h++) {
            end[k] = dat[j].first[h].snapshot(loc()+1);
            end[k].local=j;
            k++;
        }
    }
    return end;
}
int ParameterContext::index(int loc) const {
    int k=0;
    for (int j=loc+1;j<dat.size();j++) k+=dat[j].second;
    return k;
}
Statement ParameterContext::partialgenerateLocal(int id,int locl) const {
    Statement nstat = Statement(id,locl);
    nstat.ara=index(locl);
    nstat.args=artifact(locl);
    return nstat;
}
Statement ParameterContext::generateLocal(const Statement& expected,int locl) {
    Strategy nloc = Strategy(morph(locl,expected,loc()),dat[locl].second,locl);
    Statement nstat = Statement(dat[locl].second,locl);
    nstat.ara=nloc.ara=index(locl);
    nloc.args=compress(locl);
    nstat.args=artifact(locl);
    dat[locl].first[dat[locl].second]=nloc;
    dat[locl].second++;
    return nstat;
}
//int primeTrans(const Statement& a,int loc) {
//    if (a.local==loc) return 1;
//    else {
//        int sum=0;
//        for (int u=0;u<a.ara;u++) sum+=primeTrans(a.args[u],loc);
//        return sum;
//    }
//}
//Statement godtrans(const Statement& a,const Statement& expected,int loc,ParameterContext& params) {
//    if (a.local==loc) {
//        return params.generateLocal(expected,loc);
//    } else {
//        Statement res = Statement(a.id,a.local,a.ara);
//        for (int u=0;u<a.ara;u++) {
//            Strategy subcalctype = params.generateTypeSection(res,u);
//            ParameterContext nn = params.append(subcalctype);
//            res.args[u] = godtrans(a.args[u],subcalctype.type,loc,nn);
//            params.dat[loc].second=nn.dat[loc].second;
//        }
//        return res;
//    }
//}

Statement partialgodtrans(const Statement& a,int loc,int& place,const ParameterContext& params) {

    if (a.local==loc) {
        return params.partialgenerateLocal(place++,loc);
    } else {
        Statement res = Statement(a.id,a.local,a.ara);
        Strategy subcalctype = params.generateType(a);
        for (int u=0;u<a.ara;u++) {
            ParameterContext nn = params.append(subcalctype.args[u]);
            res.args[u] = partialgodtrans(a.args[u],loc,place,nn);
        }
        return res;
    }
}
void formatgodtrans(const Statement& a,const Statement& expected,int loc,ParameterContext& params) {
    if (a.local==loc) {
        params.generateLocal(expected,loc);
    } else {
        Strategy subcalctype = params.generateType(a);
        for (int u=0;u<a.ara;u++) {
            ParameterContext nn = params.append(subcalctype.args[u]);
            formatgodtrans(a.args[u],subcalctype.args[u].type,loc,nn);
            params.dat[loc].second=nn.dat[loc].second;
        }
    }
}
//int primeFormat(Statement** a,int fibers,int loc,bool init) {
//    bool ide=true;
//    for (int g=0;g<fibers;g++) if (a[g]->local<=loc+1) ide=false;
//    if (ide or init) {
//        int sum=0;
//        Statement res = Statement(a[0]->id,a[0]->local,a[0]->ara);
//        Statement** nn;
//        for (int j=0;j<res.ara;j++) {
//            nn = new Statement*[fibers];
//            for (int h=0;h<fibers;h++) nn[h] = &a[h]->args[j];
//            sum+=primeFormat(nn,fibers,loc,false);
//            delete[] nn;
//        }
//        return sum;
//    }
//    return 1;
//}
bool lazy_judgemental_eq(const Statement& a,const Statement& b,int loc) {
    if (a.local==loc or b.local==loc) return true;
    if (a.id!=b.id or a.local!=b.local) return false;
    for (int u=0;u<a.ara;u++) if (!lazy_judgemental_eq(a.args[u],b.args[u],loc)) return false;
    return true;
}

bool lazy_judgemental_eq_GPA(Statement& a,Statement& b,int loc) {
    if (a.local==loc or a.local>loc+1 or b.local==loc or b.local>loc+1) return true;
    if (a.id!=b.id or a.local!=b.local) return false;
    for (int u=0;u<a.ara;u++) if (!lazy_judgemental_eq_GPA(a.args[u],b.args[u],loc)) return false;
    return true;
}
void lazy_mutate_circuit(Statement& a,Statement& b,int loc) {
    if (b.local==loc or b.local>loc+1) return;
    if (a.local==loc or a.local>loc+1) {
        a.cleanup();
        a=b.deepcopy();
        return;
    }
    for (int u=0;u<a.ara;u++) lazy_mutate_circuit(a.args[u],b.args[u],loc);
}


void generatePartialA(Statement** a,int fibers,int ara,Strategy* itinerary,Statement* circuits,std::vector<Statement>& statout,std::vector<Statement*>* circout,int loc) {
    bool n_loc=false,d_eli=true;
    int gi=-1,gl=-1,ga=-1;
    for (int g=0;g<fibers;g++) {
        if (a[g]->local==loc+1) {d_eli=false;n_loc=true;}
        else if (a[g]->local!=loc) {
            if (!n_loc) {gi=a[g]->id;gl=a[g]->local;ga=a[g]->ara;}
            else if (gi!=a[g]->id or gl!=a[g]->local) {d_eli=false;}
            n_loc=true;
        }
    }
    if (!n_loc) {
        if (circout) circout->push_back(deepcopy(circuits,ara*fibers));
        statout.push_back(Statement(-1,loc,0));
        return;
    }
    for (int i=0;i<ara;i++) {
        bool valid=true;
        for (int g=0;g<fibers;g++) if (!lazy_judgemental_eq_GPA(circuits[g+fibers*i],*(a[g]),loc)) valid=false;
        if (valid) {
            Statement nxt = Statement(i,loc+1,itinerary[i].ara);
            for (int q=0;q<itinerary[i].ara;q++) {
                nxt.args[q] = Statement(-1,loc,0);
            }
            if (circout) {
                Statement* nxcircuits = deepcopy(circuits,ara*fibers);
                for (int g=0;g<fibers;g++) lazy_mutate_circuit(nxcircuits[g+fibers*i],*(a[g]),loc);
                circout->push_back(nxcircuits);
            }
            statout.push_back(nxt);
        }
    }
    if (d_eli) {
        std::vector<std::vector<Statement>> cartesian;
        std::vector<std::vector<Statement*>> bindesian;
        Statement** nnr;
        for (int q=0;q<ga;q++) {
            nnr = new Statement*[fibers];
            for (int o=0;o<fibers;o++) {
                if (a[o]->local==loc) nnr[o]=a[o];
                else nnr[o]=&(a[o]->args[q]);
            }
            cartesian.push_back(std::vector<Statement>());
            bindesian.push_back(std::vector<Statement*>());
            generatePartialA(nnr,fibers,ara,itinerary,circuits,cartesian[q],&bindesian[q],loc);
            delete nnr;
        }
        int cmax = 1;
        for (int v=0;v<ga;v++) cmax*=cartesian[v].size();
        for (int c=0;c<cmax;c++) {
            Statement nn = Statement(gi,gl,ga);
            Statement* nxcircuits = new Statement[ara*fibers];
            for (int i=0;i<ara*fibers;i++) nxcircuits[i]=circuits[i].deepcopy();
            bool valid = true;
            for (int v=0,ca=c;v<ga;v++) {
                for (int i=0;i<ara*fibers;i++) if (!lazy_judgemental_eq_GPA(nxcircuits[i],bindesian[v][ca%cartesian[v].size()][i],loc)) valid=false;
                if (!valid) break;
                for (int i=0;i<ara*fibers;i++) lazy_mutate_circuit(nxcircuits[i],bindesian[v][ca%cartesian[v].size()][i],loc);
                nn.args[v] = cartesian[v][ca%cartesian[v].size()].deepcopy();
                ca/=cartesian[v].size();
            }
            if (valid) {
                statout.push_back(nn);
                if (circout) circout->push_back(nxcircuits);
                else {
                    for (int i=0;i<ara*fibers;i++) nxcircuits[i].cleanup();
                    delete[] nxcircuits;
                }
            }
        }
        for (int v=0;v<ga;v++) {
            for (int y=0;y<cartesian[v].size();y++) {
                cartesian[v][y].cleanup();
                for (int i=0;i<ara*fibers;i++) bindesian[v][y][i].cleanup();
                delete [] bindesian[v][y];
            }
        }
    }
}
bool Binding::simplify() {
    for (int u=0;u<binds.size();u++) {
        if (binds[u].body.local!=tracks.loc() and binds[u].body.contains(binds[u].head.id,tracks.loc())) return false;
        ParameterContext tplusi = tracks.append(binds[u].itinerary,binds[u].ara);
        if (binds[u].universal and binds[u].concrete) continue;
        if (binds[u].universal) {
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            binds[u].body.cleanup();
            binds[u].body=nbod;
        } else if (binds[u].concrete) {
            Statement nhead = binds[u].head.simp_substitute(*this,tplusi,u);
            if (nhead.local!=tracks.loc()) {
                if (!decompose(nhead,binds[u].body,tplusi)) {nhead.cleanup();return false;}
                binds[u].cleanup();
                binds.erase(binds.begin()+u);
                u=0;
            } else if (!judgemental_eq(nhead,binds[u].head)) {
                binds[u].head.cleanup();
                binds[u].head=nhead.deepcopy();
                u=0;
            }
            nhead.cleanup();
        } else {
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            Statement nhead = binds[u].body.local==tracks.loc()?
            binds[u].head.substitute_single(*this,tplusi):
            binds[u].head.simp_substitute(*this,tplusi,u);
            if (!judgemental_eq(nhead,binds[u].head) or !judgemental_eq(nbod,binds[u].body)) {
                binds[u].head.cleanup();
                binds[u].body.cleanup();
                if (nbod.local!=tracks.loc() and nhead.local!=tracks.loc()) {
                    binds[u].cleanup();
                    binds.erase(binds.begin()+u);
                    if (!decompose(nhead,nbod,tplusi)) {nbod.cleanup();nhead.cleanup();return false;}
                } else if (nhead.local!=tracks.loc()) {
                    binds[u].head=nbod.deepcopy();
                    binds[u].body=nhead.deepcopy();
                } else {
                    binds[u].head=nhead.deepcopy();
                    binds[u].body=nbod.deepcopy();
                }
                u=0;
            }
            nbod.cleanup();nhead.cleanup();
        }
    }
    return true;
}

void Binding::pad(int amt) {
    int sm=ara;
    ara+=amt;
    Strategy* nstrats = new Strategy[ara];
    for (int w=0;w<sm;w++) nstrats[w] = localtypes[w];
    delete [] localtypes;
    localtypes = nstrats;
    tracks.dat[tracks.loc()].first = localtypes;
    tracks.dat[tracks.loc()].second = ara;
}
Statement Binding::integrate(const Statement& bod,const Statement& type,Strategy* itinerary,int iara) {
    int sm=ara;
    ParameterContext ncon = tracks.append(itinerary,iara);
    Statement gtreturn = partialgodtrans(bod,tracks.loc(),ara,ncon);
    
    
    Strategy* nstrats = new Strategy[ara];
    for (int w=0;w<sm;w++) nstrats[w] = localtypes[w];
    delete [] localtypes;
    localtypes = nstrats;
    
    tracks.dat[tracks.loc()].first = localtypes;
    ncon.dat[tracks.loc()].first = localtypes;
    
    formatgodtrans(gtreturn,type,tracks.loc(),ncon);
    
    tracks.dat[tracks.loc()].second = ara;
    return gtreturn;
}


void Binding::divide(std::vector<Binding>& list,int tabs) {
//    loosecheck();
    if (!simplify()) return;
    std::vector<std::vector<SingleBind>> cartesian;
    std::map<int,std::vector<int>> out_b;
    for (int o=0;o<binds.size();o++) {
        if (binds[o].body.local!=tracks.loc() and !binds[o].universal) {
            out_b[binds[o].head.id].push_back(o);
        }
    }
    
    for (auto h=out_b.begin();h!=out_b.end();h++) {
        Statement** mul1 = new Statement*[h->second.size()];
        Statement** mul2 = new Statement*[h->second.size()];
        for (int s=0;s<h->second.size();s++) {
            mul1[s]=&binds[h->second[s]].head;
            mul2[s]=&binds[h->second[s]].body;
        }
        std::vector<Statement> gather;
        Statement* circ     = new Statement[localtypes[h->first].ara*h->second.size()];
        Statement format = localtypes[h->first].snapshot(tracks.loc()+1);
        
        cartesian.push_back(std::vector<SingleBind>());
        
        generatePartialA(mul2,h->second.size(),localtypes[h->first].ara,localtypes[h->first].args,circ,gather,0,tracks.loc());

        for (int v=0;v<gather.size();v++) {
            cartesian[cartesian.size()-1].push_back(SingleBind(format.deepcopy(),gather[v],
                deepcopy(localtypes[h->first].args,localtypes[h->first].ara),
                localtypes[h->first].ara,true,!gather[v].contains(tracks.loc())));
        }
        format.cleanup();
        for (int c=0;c<localtypes[h->first].ara*h->second.size();c++) circ[c].cleanup();
        delete[] circ;
        delete[] mul1;
        delete[] mul2;
    }

    
    if (!cartesian.size()) {
        list.push_back(*this);
        return;
    }
    int cmax = 1;
    for (int v=0;v<cartesian.size();v++) cmax*=cartesian[v].size();
    for (int c=0;c<cmax;c++) {
        Binding soap = *this;
        bool valid=true;
        for (int v=0,ca=c;v<cartesian.size();v++) {
            SingleBind nx = cartesian[v][ca%cartesian[v].size()].deepcopy();
            Statement oldobs = nx.body;
            nx.body = soap.integrate(oldobs,Statement(),nx.itinerary,nx.ara);
            
            oldobs.cleanup();
            soap.binds.push_back(nx);
            
            ParameterContext conan = soap.tracks.append(nx.itinerary,nx.ara);
            Strategy calctype = conan.generateType(nx.head);
            if (!soap.typebind(nx.body,calctype.type,conan)) {valid=false;break;}
            ca/=cartesian[v].size();
        }
        if (valid) soap.divide(list,tabs>=0?tabs+1:-1);
    }
    for (int v=0;v<cartesian.size();v++){
        for (int w=0;w<cartesian.size();w++) cartesian[v][w].cleanup();
    }
}












