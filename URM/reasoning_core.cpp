//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"


MetaBank MetaBank::meta_prime("test.ax");




void applyshort(Binding& bin,int id,Statement& templ) {
    for (int a=0;a<bin.ara;a++) bin.localtypes[a].inplace_sub(id,1,templ);
    for (int b=0;b<bin.binds.size();b++) {
        bin.binds[b].head.inplace_sub(id,1,templ,3);
        bin.binds[b].body.inplace_sub(id,1,templ,3);
        for (int c=0;c<bin.binds[b].ara;c++) {
            bin.binds[b].itinerary[c].inplace_sub(id,1,templ);
        }
    }
}

struct StitchMPlexr {
    Binding center;
    std::vector<std::pair<int,Statement>> headargs;
    std::vector<std::pair<int,Statement>> bodyargs;
    StitchMPlexr(Binding& b) : center(b) {}
    StitchMPlexr(Binding& b,std::vector<std::pair<int,Statement>> head,std::vector<std::pair<int,Statement>> body) : center(b),headargs(head),bodyargs(body) {}
};

void Binding::gapinplace(int com,int amt) {
    for (int d=0;d<ara;d++) localtypes[d].idpush(tracks.loc(),amt,com);
    for (int d=0;d<binds.size();d++) {
        binds[d].head.idpush(tracks.loc(),amt,com);
        binds[d].body.idpush(tracks.loc(),amt,com);
        for (int k=0;k<binds[d].ara;k++) binds[d].itinerary[k].idpush(tracks.loc(),amt,com);
    }
}
void Binding::boostinplace(int cara,Strategy* charms) {
    for (int d=0;d<ara;d++) localtypes[d].expandLocs(tracks.loc(),localtypes[d].ara,cara,charms);
    for (int d=0;d<binds.size();d++) {
        binds[d].head.expandLocs(tracks.loc(),binds[d].ara,tracks.loc()+2,cara,charms);
        binds[d].body.expandLocs(tracks.loc(),binds[d].ara,tracks.loc()+2,cara,charms);
        expandRange(tracks.loc(),binds[d].ara,binds[d].itinerary,cara,charms);
    }
}

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
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
    for (int j=0;j<l.binds.size();j++) binds.push_back(l.binds[j].deepcopy());
    for (int j=0;j<r.binds.size();j++) {
        binds.push_back(r.binds[j].deepcopy());
        binds[binds.size()-1].head.idpush(l.tracks.loc(),l.ara-common,common);
        binds[binds.size()-1].body.idpush(l.tracks.loc(),l.ara-common,common);
        for (int k=0;k<binds[binds.size()-1].ara;k++) binds[binds.size()-1].itinerary[k].idpush(tracks.loc(),l.ara-common,common);
        binds[binds.size()-1].head.expandLocs(tracks.loc(),binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        binds[binds.size()-1].body.expandLocs(tracks.loc(),binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        expandRange(tracks.loc(),binds[binds.size()-1].ara,binds[binds.size()-1].itinerary,rcara,rcharms);
    }
}
Strategy ParameterContext::generateType(const Statement& a) const {
    if (a.id<0 or a.local<0) throw;
    if (a.local>=dat.size() or a.local<0) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
    if (dat[a.local].first[a.id].ara!=a.ara) throw;
    
    return dat[a.local].first[a.id].ndisp_sub(lloc+1,loc()-lloc,a.args,a.ara,0);
}
//Strategy ParameterContext::generateTypeSection(const Statement& a,int arg) const {
//    if (a.local>=dat.size() or a.local<0) throw;
//    if (a.id>=dat[a.local].second) throw;
//    int lloc = dat[a.local].first[a.id].local;
//    return dat[a.local].first[a.id].args[arg].ndisp_sub(lloc+1,loc()-lloc,a.args,arg,1);
//}
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
    Strategy htype = cont.generateType(plain);
    Strategy btype = cont.generateType(templ);
    bool wrk = decompose(htype.type,btype.type,cont);
    htype.cleanup();
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
        modl.cleanup();
        modr.cleanup();
        return true;
    }
    if (left.local==right.local and left.id==right.id) {
        if (left.ara!=right.ara) throw;
        if (left.local==-1) return true;
        Strategy calctype = params.generateType(left);
        for (int u=0;u<left.ara;u++) {
            ParameterContext continued = params.append(calctype.args[u]);
            if (!decompose(left.args[u], right.args[u],continued)) {
                calctype.cleanup();
                return false;
            }
        }
        calctype.cleanup();
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
int primeTrans(const Statement& a,int loc) {
    if (a.local==loc) return 1;
    else {
        int sum=0;
        for (int u=0;u<a.ara;u++) sum+=primeTrans(a.args[u],loc);
        return sum;
    }
}
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

Statement partialgodtrans(const Statement& a,int loc,int& place,ParameterContext& params) {
    if (a.local==loc) {
        return params.partialgenerateLocal(place,loc);
    } else {
        Statement res = Statement(a.id,a.local,a.ara);
        for (int u=0;u<a.ara;u++) {
            Strategy subcalctype = params.generateType(a);
            ParameterContext nn = params.append(subcalctype);
            res.args[u] = partialgodtrans(a.args[u],loc,place,nn);
        }
        return res;
    }
}
void formatgodtrans(const Statement& a,const Statement& expected,int loc,ParameterContext& params) {
    if (a.local==loc) {
        params.generateLocal(expected,loc);
    } else {
        for (int u=0;u<a.ara;u++) {
            Strategy subcalctype = params.generateType(a);
            ParameterContext nn = params.append(subcalctype);
            formatgodtrans(a.args[u],subcalctype.type,loc,nn);
            params.dat[loc].second=nn.dat[loc].second;
        }
    }
}

void separatePartials(Statement** a,std::vector<int>& b,int loc,int depth,std::vector<std::vector<int>>& out) {
    bool ide=true;
    for (int g=0;g<b.size();g++) if (a[g]->local<=loc) ide=false;
    if (depth!=0 and !ide) {out.push_back(b); return;}
    
//    std::vector<int> bb = b;
//    int ggg=0;
    for (int ggg=0;ggg<b.size();ggg++) {
        std::vector<int> w;
        for (int j=0;j<b.size();j++) {
            if (a[ggg]->local==a[j]->local and a[ggg]->id==a[j]->id) {
                if (j<ggg) break;
                w.push_back(b[j]);
            }
        }
        if (!w.size()) continue;
        std::vector<std::vector<int>> mplex1;
        std::vector<std::vector<int>> mplex2;
        mplex1.push_back(w);
        bool c=true;
        #define MP1 (c?mplex1:mplex2)
        #define MP2 (c?mplex2:mplex1)
        for (int j=0;j<a[ggg]->ara;j++) {
            for (int r=0;r<MP1.size();r++) {
                Statement** mul = new Statement*[MP1[r].size()];
                for (int g=0,s=0;s<MP1[r].size();) {
                
                    if (b[g]==MP1[r][s]) mul[s++]=&(a[g]->args[j]);
                    else g++;
                }
                separatePartials(mul,MP1[r],loc,depth+1,MP2);
                delete[] mul;
            }
            MP1.clear();
            c=!c;
        }
        for (int b=0;b<MP1.size();b++) out.push_back(MP1[b]);
        #undef MP1
        #undef MP2
    }
}
int primeFormat(Statement** a,int fibers,int loc,bool init) {
    bool ide=true;
    for (int g=0;g<fibers;g++) if (a[g]->local<=loc+1) ide=false;
    if (ide or init) {
        int sum=0;
        Statement res = Statement(a[0]->id,a[0]->local,a[0]->ara);
        Statement** nn;
        for (int j=0;j<res.ara;j++) {
            nn = new Statement*[fibers];
            for (int h=0;h<fibers;h++) nn[h] = &a[h]->args[j];
            sum+=primeFormat(nn,fibers,loc,false);
            delete[] nn;
        }
        return sum;
    }
    return 1;
}



Statement generateFormat(Statement** a,int fibers,ParameterContext& params,Statement exptype,int loc,Statement* circuits) {
    bool ide=true;
    for (int i=0;i<fibers;i++) a[i]->tostring();
    for (int g=0;g<fibers;g++) if (a[g]->local<=loc+1) ide=false;
    if (ide or params.loc()==loc+1) {
        Statement res = Statement(a[0]->id,a[0]->local,a[0]->ara);
        Statement** nn;
        for (int j=0;j<res.ara;j++) {
            nn = new Statement*[fibers];
            for (int h=0;h<fibers;h++) nn[h] = &(a[h]->args[j]);
            Strategy jf = params.generateTypeSection(res,j);
            ParameterContext nncon = params.append(jf);
            res.args[j] = generateFormat(nn,fibers,nncon,jf.type,loc,circuits);
            jf.cleanup();
            params.dat[loc].second=nncon.dat[loc].second;
            delete[] nn;
        }
        return res;
    } else {
        for (int h=0;h<fibers;h++) circuits[params.dat[loc+1].second*fibers+h]=a[h]->deepcopy();
        return params.generateLocal(exptype,loc+1);
    }
}


bool lazy_judgemental_eq(Statement& a,Statement& b,int loc) {
    if (a.local==loc or a.local>loc+1 or b.local==loc or b.local>loc+1) return true;
    if (a.id!=b.id or a.local!=b.local) return false;
    for (int u=0;u<a.ara;u++) {
        if (!lazy_judgemental_eq(a.args[u],b.args[u],loc)) return false;
    }
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
    bool n_loc=false,d_eli=true;throw;//check this for memory safety
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
        Statement* nxcircuits = new Statement[ara*fibers];
        for (int i=0;i<ara*fibers;i++) nxcircuits[i]=circuits[i].deepcopy();
        statout.push_back(Statement(-1,loc,0));
        circout->push_back(nxcircuits);
        return;
    }
    for (int i=0;i<ara;i++) {
        bool valid=true;
        for (int g=0;g<fibers;g++) if (!lazy_judgemental_eq(circuits[g+fibers*i],*(a[g]),loc)) valid=false;
        if (valid) {
            Statement nxt = Statement(i,loc+1,itinerary[i].ara);
            for (int q=0;q<itinerary[i].ara;q++) {
                nxt.args[q] = Statement(-1,loc,0);
            }
            if (circout) {
                Statement* nxcircuits = new Statement[ara*fibers];
                for (int t=0;t<ara*fibers;t++) nxcircuits[t]=circuits[t].deepcopy();
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
                for (int i=0;i<ara*fibers;i++) if (!lazy_judgemental_eq(nxcircuits[i],bindesian[v][ca%cartesian[v].size()][i],loc)) valid=false;
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
                delete bindesian[v][y];
            }
        }
    }
}
bool Binding::simplify() {
//    int check=0;
    for (int u=0;u<binds.size();u++) {
        ParameterContext tplusi = tracks.append(binds[u].itinerary,binds[u].ara);
        if (binds[u].body.local!=tracks.loc() and binds[u].body.contains(binds[u].head.id,tracks.loc())) return false;
        
        //universal->don't worry about nhead(unless there are two universal binds)
        //another flag->don't wory about nbod
        if (binds[u].universal and binds[u].concrete) continue;
        if (binds[u].universal) {
//            std::cout<<binds[u].head.tostring()<<" <-- "<<tracks.loc()<<"\n";
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            if (!judgemental_eq(nbod,binds[u].body)) {
                if (!decompose(binds[u].head,nbod,tplusi)) {nbod.cleanup();return false;}
                binds.erase(binds.begin()+u);
                u=0;
            }
            nbod.cleanup();
        } else if (binds[u].concrete) {
//            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            Statement nhead = binds[u].head.simp_substitute(*this,tplusi,u);
//            if (!judgemental_eq(nbod,binds[u].body)) throw;
            if (!judgemental_eq(nhead,binds[u].head)) {
                if (!decompose(nhead,binds[u].body,tplusi)) {nhead.cleanup();return false;}
                binds.erase(binds.begin()+u);
                u=0;
            }
            nhead.cleanup();
        } else {
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            Statement nhead = binds[u].body.local==tracks.loc()?
            binds[u].head.substitute_single(*this,tplusi):
            binds[u].head.simp_substitute(*this,tplusi,u);
            if (!judgemental_eq(nhead,binds[u].head) or !judgemental_eq(nbod,binds[u].body)) {
                binds.erase(binds.begin()+u);
                if (!decompose(nhead,nbod,tplusi)) {nbod.cleanup();nhead.cleanup();return false;}
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
Statement Binding::integrate(const Statement& bod,Strategy* itinerary,int iara) {
    int sm=ara;
    ara+=primeTrans(bod,1);
    Strategy* nstrats = new Strategy[ara];
    for (int w=0;w<sm;w++) nstrats[w] = localtypes[w];
    delete [] localtypes;
    localtypes = nstrats;
    tracks.dat[tracks.loc()].first = localtypes;
    ParameterContext ncon = tracks.append(itinerary,iara);
    tracks.dat[tracks.loc()].second = ara;
    return godtrans(bod,Statement(),tracks.loc(),ncon);
}


void Binding::divide(std::vector<Binding>& list,int tabs) {
//    loosecheck();
    if (!simplify()) return;
    std::vector<std::vector<SingleBind>> cartesian;
    std::vector<int> in_b;
    std::vector<std::vector<int>> out_b;
    for (int o=0;o<binds.size();o++) {
        if (binds[o].body.local!=tracks.loc() and binds[o].head.ara!=0 and !binds[o].universal) {
            in_b.push_back(o);
        }
    }
    if (in_b.size()) {
        Statement** in_a = new Statement*[in_b.size()];
        for (int r=0;r<in_b.size();r++) in_a[r] = &(binds[in_b[r]].head);
        separatePartials(in_a,in_b,tracks.loc(),0,out_b);
        delete[] in_a;
    }
    for (int h=0;h<out_b.size();h++) {
        Statement** mul1 = new Statement*[out_b[h].size()];
        Statement** mul2 = new Statement*[out_b[h].size()];
        for (int s=0;s<out_b[h].size();s++) {
            mul1[s]=&binds[out_b[h][s]].head;
            mul2[s]=&binds[out_b[h][s]].body;
        }
        std::vector<Statement> gather;
        int itinerarysize = primeFormat(mul1,out_b[h].size(),tracks.loc(),true);
        
        Statement* circ     = new Statement[itinerarysize*out_b[h].size()];
//        Strategy* itinerary = new Strategy[itinerarysize];
        
        ParameterContext tplusempty = tracks.append(new Strategy[itinerarysize],0);
        tplusempty.dat.push_back(std::pair<Strategy*,int>(new Strategy[itinerarysize],0));
//        int pl=0;
        Statement format = generateFormat(mul1,out_b[h].size(),tplusempty,Statement(),tracks.loc(),circ);
        
        bool valid=true;
        for (int ha=0;ha<binds.size();ha++) if (judgemental_eq(binds[ha].head,format)) valid=false;throw;//head already exists then mark as universal and check back.
        if (!valid) continue;
        
        cartesian.push_back(std::vector<SingleBind>());
        
        generatePartialA(mul2,out_b[h].size(),itinerarysize,tplusempty.dat[tracks.loc()+1].first,circ,gather,0,tracks.loc());

        for (int v=0;v<gather.size();v++) {
            bool nuniv = true;
            for (int i=0;i<format.ara;i++) if (format.args[i].local!=tracks.loc()+1) nuniv=false;
            cartesian[cartesian.size()-1].push_back(SingleBind(format.deepcopy(),gather[v],deepcopy(tplusempty.dat[tracks.loc()+1].first,itinerarysize),itinerarysize,nuniv,!gather[v].contains(tracks.loc())));
        }
        format.cleanup();
        delete[] tplusempty.dat[tracks.loc()+1].first;
        for (int c=0;c<itinerarysize*out_b[h].size();c++) circ[c].cleanup();
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
            nx.body = soap.integrate(nx.body,nx.itinerary,nx.ara);
            oldobs.cleanup();
            soap.binds.push_back(nx);
            
            ParameterContext conan = soap.tracks.append(nx.itinerary,nx.ara);
            Strategy calctype = conan.generateType(nx.head);
            if (!soap.typebind(nx.body,calctype.type,conan)) {valid=false;break;}
            
            ca/=cartesian[v].size();
        }
        if (valid) soap.divide(list,tabs>=0?tabs+1:-1);
    }
}

void capture(Statement&,Strategy*,std::vector<int>&);
void capture(Strategy&,Strategy*,std::vector<int>&);

void capture(Statement& ob,Strategy* lt,std::vector<int>& rmap) {
    if (ob.local==1) {
        bool aladded=false;
        for (int b=0;b<rmap.size();b++) if (rmap[b]==ob.id) aladded=true;
        if (!aladded) capture(lt[ob.id],lt,rmap);
    }
    for (int a=0;a<ob.ara;a++) capture(ob.args[a],lt,rmap);
}
void capture(Strategy& ob,Strategy* lt,std::vector<int>& rmap) {
    capture(ob.type,lt,rmap);
    if (ob.local==1) rmap.push_back(ob.id);
    for (int a=0;a<ob.ara;a++) capture(ob.args[a],lt,rmap);
}
bool uprefs(Statement& ob,int lim) {
    if (ob.local==1 and ob.id>=lim) return true;
    for (int a=0;a<ob.ara;a++) {
        if (uprefs(ob.args[a],lim)) return true;
    }
    return false;
}


void boilBinding(MetaBank* mb,std::vector<Stitching>& out,Binding& bin,std::vector<int>& caut,const Statement& khook) {
    Statement hook = khook.deepcopy();
    while (bin.binds.size()) {
        if (bin.binds[0].body.local!=1) {
            std::vector<Binding> buffer;
            std::vector<StitchMPlexr> car;
            car.push_back(StitchMPlexr(bin));
            for (int a=0;a<bin.binds[0].head.ara;a++) {
                Statement circref = bin.binds[0].body.deepcopy();
                std::vector<Statement> gather;
                Statement* hack = bin.binds[0].head.args+a;
                generatePartialA(&hack,1,circref.ara,bin.localtypes[bin.binds[0].body.id].args,circref.args,gather,0,1);
                circref.cleanup();
                for (int i=0;i<gather.size();i++) {
                    for (int f=0,limit=car.size();f<limit;f++) {
                        car.push_back(car[f]);
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].body.id].args,circref.ara);
                        car[car.size()-1].headargs.push_back(std::pair<int,Statement>(a,bod));
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].head.id].args[a]);
                        Statement incurer = bin.localtypes[bin.binds[0].head.id].snapshot(2);
                        Strategy calctype = conan.generateType(incurer);
                        if (!car[car.size()-1].center.typebind(bod,calctype.type,conan)) {car.erase(car.begin()+car.size()-1);}
                        calctype.cleanup();
                        incurer.cleanup();
                    }
                }
                circref.cleanup();
            }
            for (int b=0;b<bin.binds[0].body.ara;b++) {
                Statement circref = bin.binds[0].head.deepcopy();
                std::vector<Statement> gather;
                Statement* hack = bin.binds[0].body.args+b;
                generatePartialA(&hack,1,circref.ara,bin.localtypes[bin.binds[0].head.id].args,circref.args,gather,0,1);
                for (int i=0;i<gather.size();i++) if (gather[i].local!=2) {
                    for (int f=0,limit=car.size();f<limit;f++) {
                        car.push_back(car[f]);
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].head.id].args,circref.ara);
                        car[car.size()-1].bodyargs.push_back(std::pair<int,Statement>(b,bod));
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].body.id].args[b]);
                        Statement incurer = bin.localtypes[bin.binds[0].body.id].snapshot(2);
                        Strategy calctype = conan.generateType(incurer);
                        if (!car[car.size()-1].center.typebind(bod,calctype.type,conan)) {car.erase(car.begin()+car.size()-1);}
                        calctype.cleanup();
                        incurer.cleanup();
                    }
                }
                circref.cleanup();
            }
//            Strategy hdup = bin.localtypes[bin.binds[0].head.id].bring_depth(2,1);
//            Strategy bdup = bin.localtypes[bin.binds[0].body.id].bring_depth(2,1);
            for (int g=0;g<car.size();g++) {
                Strategy res = Strategy(Statement(0,0),bin.ara,1,car[g].headargs.size()+car[g].bodyargs.size());
                for (int a=0;a<car[g].headargs.size();a++) {
                    res.args[a] = bin.localtypes[bin.binds[0].head.id].args[car[g].headargs[a].first].deepcopy();
//                    expandRange(1,res.args[a].ara,res.args[a].args,bdup.ara,bdup.args);
                }
                for (int b=0;b<car[g].bodyargs.size();b++) {
                    res.args[car[g].headargs.size()+b] = bin.localtypes[bin.binds[0].body.id].args[car[g].bodyargs[b].first].deepcopy();
                    res.args[car[g].headargs.size()+b].idpush(2,car[g].headargs.size(),0);
//                    expandRange(1,res.args[car[g].headargs.size()+b].ara,res.args[car[g].headargs.size()+b].args,hdup.ara,hdup.args);
                }
                car[g].center.pad(2);
                car[g].center.localtypes[res.id] = res;
                car[g].center.localtypes[res.id+1] = res.deepcopy();
                car[g].center.localtypes[res.id+1].id++;
                car[g].center.localtypes[res.id+1].type = res.snapshot(2);
                Statement vhead = Statement(res.id+1,1,car[g].headargs.size()+car[g].bodyargs.size());
                Statement vbody = Statement(res.id+1,1,car[g].headargs.size()+car[g].bodyargs.size());
                for (int a=0;a<car[g].headargs.size();a++) {
                    vhead.args[a] = bin.localtypes[bin.binds[0].head.id].args[car[g].headargs[a].first].snapshot(3);
                    vbody.args[a] = car[g].headargs[a].second;
                }
                for (int b=0;b<car[g].bodyargs.size();b++) {
                    vhead.args[car[g].headargs.size()+b] = car[g].bodyargs[b].second;
                    vbody.args[car[g].headargs.size()+b] = bin.localtypes[bin.binds[0].body.id].args[car[g].bodyargs[b].first].snapshot(3);
                }
                applyshort(car[g].center,bin.binds[0].head.id,vhead);
                applyshort(car[g].center,bin.binds[0].body.id,vbody);
                hook.inplace_sub(bin.binds[0].head.id,1,vhead,2);
                hook.inplace_sub(bin.binds[0].body.id,1,vbody,2);
                vhead.id--;
                vbody.id--;
                ParameterContext tplush = car[g].center.tracks.append(car[g].center.localtypes[bin.binds[0].head.id]);
                ParameterContext tplusb = car[g].center.tracks.append(car[g].center.localtypes[bin.binds[0].body.id]);
                Statement htarget = car[g].center.localtypes[bin.binds[0].head.id].snapshot(2);
                Statement btarget = car[g].center.localtypes[bin.binds[0].body.id].snapshot(2);
                Strategy htype = tplush.generateType(htarget);
                Strategy btype = tplusb.generateType(btarget);
                std::vector<Binding> dis;
                if (car[g].center.decompose(htype.type,vhead,tplush) and car[g].center.decompose(btype.type,vbody,tplusb)) car[g].center.divide(dis,-1);
                std::vector<int> ncaut;//symbound to caut means it is also caut.
                for (int n=0;n<caut.size();n++) {
                    if (caut[n]==bin.binds[0].head.id or caut[n]==bin.binds[0].body.id) ncaut.push_back(res.id+1);
                    else ncaut.push_back(caut[n]);
                }
                for (int i=0;i<dis.size();i++) boilBinding(mb,out,dis[i],caut,khook);
                res.cleanup();
                vhead.cleanup();
                vbody.cleanup();
                htarget.cleanup();
                btarget.cleanup();
                htype.cleanup();
                btype.cleanup();
            }
            hook.cleanup();
            return;
        } else {
            if (!bin.binds[0].universal) throw;
            applyshort(bin,bin.binds[0].head.id,bin.binds[0].body);
            hook.inplace_sub(bin.binds[0].head.id,1,bin.binds[0].body,2);
        }
        bin.binds.erase(bin.binds.begin());
    }
    std::vector<int> rmap;
    capture(hook,bin.localtypes,rmap);
    Strategy* dgr = new Strategy[rmap.size()];
    for (int g=0;g<rmap.size();g++) {
        dgr[g] = bin.localtypes[rmap[g]].deepcopy();
        dgr[g].apply(1,rmap);
    }
    std::vector<int> ncaut;
    for (int n=0;n<caut.size();n++) {
        for (int m=0;m<rmap.size();m++) {
            if (rmap[m]==caut[n]) ncaut.push_back(m);
        }
    }
    out.push_back(Stitching(mb,dgr,rmap.size(),caut,hook));
    hook.cleanup();
}



void decpull(Statement& du,std::vector<int>& links,std::map<int,std::vector<Statement>>& refs) {
    if (du.local==1) {
        links.push_back(du.id);
        refs[du.id].push_back(du);
    }
    for (int a=0;a<du.ara;a++) decpull(du.args[a],links,refs);
}
void decpull(Strategy& du,std::vector<int>& links,std::map<int,std::vector<Statement>>& refs) {
    decpull(du.type,links,refs);
    for (int a=0;a<du.ara;a++) decpull(du.args[a],links,refs);
}


void killhack(Statement& ob,int id,int arg) {
    if (ob.local==1 and ob.id==id) {
        Statement* nstat = new Statement[ob.ara-1];
        ob.args[arg].cleanup();
        for (int h=0;h<arg;h++) nstat[h]=ob.args[h];
        for (int h=arg+1;h<ob.ara;h++) nstat[h-1]=ob.args[h];
        delete [] ob.args;
        ob.args=nstat;
        ob.ara--;
    }
    for (int a=0;a<ob.ara;a++) killhack(ob.args[a],id,arg);
}
void killhack(Strategy& ob,int id,int arg) {
    if (ob.local==1 and ob.id==id) {
        Strategy* nstat = new Strategy[ob.ara-1];
        ob.args[arg].cleanup();
        for (int h=0;h<arg;h++) nstat[h]=ob.args[h];
        for (int h=arg+1;h<ob.ara;h++) nstat[h-1]=ob.args[h];
        delete [] ob.args;
        ob.args=nstat;
        ob.ara--;
    }
    killhack(ob.type,id,arg);
    for (int a=0;a<ob.ara;a++) killhack(ob.args[a],id,arg);
}

Stitching::Stitching(MetaBank* mb,const Strategy& muse) : localtypes(muse.args),ara(muse.id),hook(muse.snapshot(1)) {calcopen(mb);}

Stitching::Stitching(MetaBank* mb,Strategy* gs,int a,std::vector<int>& ca,const Statement& h) : localtypes(gs),ara(a),caut(ca),hook(h.deepcopy()) {
    ParameterContext cl;
    cl.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
    cl.dat.push_back(std::pair<Strategy*,int>(gs,a));
    Strategy ga = cl.generateType(hook);
    quicktype = ga.type.deepcopy();
    ga.cleanup();
    calcopen(mb);
}

void Stitching::calcopen(MetaBank* mb) {
    std::map<int,std::vector<Statement>> refs;
    std::vector<int>* links = new std::vector<int>[ara];
    for (int a=0;a<ara;a++) {
        bool found=false;
        for (int g=0;g<caut.size();g++) if (caut[g]==a) found=true;
        if (!found) open.push_back(a);
    }
    for (int a=0;a<ara;a++) decpull(localtypes[a],links[a],refs);
    bool hit=true;
    while (hit) {
        for (int z=0;z<ara;z++) {
            for (int y=0;y<links[z].size();) {
                bool found=false;
                for (int x=0;x<open.size();x++) if (links[z][y]==open[x]) {found=true;break;}
                if (!found) links[z].erase(links[z].begin()+y);
                else y++;
            }
        }
        hit=false;
        for (int a=0;a<open.size();a++) if (refs.count(open[a]) and links[open[a]].size()==0) {open.erase(open.begin()+a);hit=true;}
    }
    for (int a=0;a<ara;a++) {
        if (!refs.count(a)) {
            for (int x=0;x<localtypes[a].ara-1;x++) {
                for (int y=x+1;x<localtypes[a].ara;) {
                    if (judgemental_eq(localtypes[a].args[x],localtypes[a].args[y])) for (int h=0;h<ara;h++) killhack(localtypes[h],a,y);
                    else y++;
                }
            }
        }
        for (int y=0;y<localtypes[a].ara;y++) if (localtypes[a].args[y].type.local==0) {
            bool obs=false;
            for (int e=0;e<mb->easytypes.size();e++) if (localtypes[a].args[y].type.id==mb->easytypes[e]) obs=true;
            if (obs) if (!refs.count(a)) for (int h=0;h<ara;h++) killhack(localtypes[h],a,y);
        }
    }
}


