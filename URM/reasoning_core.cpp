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


void Statement::idpush(int loc,int amt,int thresh) {
    if (local==loc and id>thresh) id+=amt;
    for (int a=0;a<ara;a++) args[a].idpush(loc,amt,thresh);
}
void Strategy::idpush(int loc,int amt,int thresh) {
    type.idpush(loc,amt,thresh);
    if (local==loc and id>thresh) id+=amt;
    for (int a=0;a<ara;a++) args[a].idpush(loc,amt,thresh);
}
//Statement idpush(Statement& a,int amt,int thresh) {
//    Statement res = Statement(a.local==1 and a.id>thresh?a.id+amt:a.id,a.local,a.ara);
//    for (int p=0;p<a.ara;p++) res.args[p] = idpush(a.args[p],amt,thresh);
//    return res;
//}
//Strategy idpush(Strategy& a,int amt,int thresh) {
//    Strategy res = Strategy(idpush(a.type,amt,thresh),a.local==1 and a.id>thresh?a.id+amt:a.id,a.local,a.ara);
//    for (int p=0;p<res.ara;p++) res.args[p] = idpush(a.args[p],amt,thresh);
//    return res;
//}

SingleBind::SingleBind(Statement a,Statement b,Strategy* c,int buf,bool uni,bool conc) : head(a),body(b),itinerary(c),ara(buf),universal(uni),concrete(conc) {}
SingleBind SingleBind::deepcopy() const {
    SingleBind dup = SingleBind(head.deepcopy(),body.deepcopy(),new Strategy[ara],ara,universal,concrete);
    for (int h=0;h<ara;h++) dup.itinerary[h] = itinerary[h].deepcopy();
    return dup;
}


Binding::Binding(MetaBank* mb,Strategy* lt,int buf) : localtypes(lt), ara(buf) {
    tracks.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
}
Binding::Binding(Binding& other,Strategy* lt,int buf) {
    *this = other;
    delete[] localtypes;
    localtypes = lt;
    ara = buf;
}
Binding::Binding(ParameterContext& t,Strategy* lt,int buf) : tracks(t),localtypes(lt),ara(buf) {
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
}
Binding& Binding::operator = (const Binding& other) {
    ara = other.ara;
    if (ara) localtypes = new Strategy[ara];
    for (int u=0;u<ara;u++) localtypes[u] = other.localtypes[u].deepcopy();
    for (int j=0;j<other.binds.size();j++) binds.push_back(other.binds[j].deepcopy());
    if (other.tracks.dat.size()) {
        for (int f=0;f<other.tracks.dat.size()-1;f++) tracks.dat.push_back(other.tracks.dat[f]);
        tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
    }
    return *this;
}
Binding::Binding(const Binding& other) {
    *this = other;
}





void expandRange(int loc,int& ara,int gu,int dep,Statement*& rang,int cara,Strategy* charms) {
    Statement* r = rang;
    rang = new Statement[ara+cara];
    for (int j=0;j<ara;j++) rang[j] = r[j];
    for (int j=0;j<cara;j++) {
        rang[ara+j] = charms[j].snapshot(dep);
        rang[ara+j].idpush(loc+1,gu,0);
    }
    delete [] r;
    ara+=cara;
}
void expandRange(int loc,int& ara,Strategy*& rang,int cara,Strategy* charms) {
    Strategy* r = rang;
    rang = new Strategy[ara+cara];
    for (int j=0;j<ara;j++) rang[j] = r[j];
    for (int j=0;j<cara;j++) {
        rang[ara+j] = charms[j].deepcopy();
        rang[ara+j].idpush(loc+1,ara,0);
    }
    delete [] r;
    ara+=cara;
}
void expandLocs(int loc,Statement& ob,int gu,int dep,int cara,Strategy* charms) {
    for (int q=0;q<ob.ara;q++) expandLocs(loc,ob.args[q],gu,dep+1,cara,charms);
    if (ob.local==loc) expandRange(loc,ob.ara,gu,dep,ob.args,cara,charms);
}
void expandLocs(int loc,Strategy& ob,int gu,int cara,Strategy* charms) {
    expandLocs(loc,ob.type,gu,ob.local+2,cara,charms);
    for (int q=0;q<ob.ara;q++) expandLocs(loc,ob.args[q],gu,cara,charms);
    if (ob.local==loc) expandRange(loc,ob.ara,ob.args,cara,charms);
}






bool affrayBinding(Binding& l,int tar,Strategy& bind) {
    Strategy* nloctypes = new Strategy[l.ara+bind.ara];
    for (int f=0;f<l.ara;f++) nloctypes[f] = l.localtypes[f];
    delete[] l.localtypes;
    l.localtypes = nloctypes;
    l.tracks.dat[l.tracks.loc()].first = nloctypes;
    Statement oform = Statement(bind.id,bind.local,bind.ara);
    for (int g=0;g<bind.ara;g++) {
        ParameterContext cont = l.tracks.append(l.localtypes[tar]).append(bind.args[g]);
        oform.args[g] = cont.generateLocal(bind.args[g].type,l.tracks.loc(),l.ara+g,nloctypes);
        l.tracks.dat[l.tracks.loc()].second = l.ara+g+1;
    }
    l.ara+=bind.ara;
//    ParameterContext cont = l.tracks.append(l.localtypes[tar]);
//    Statement plain = l.localtypes[tar].snapshot(2);
//    return l.crossdecompose(plain,oform,cont);//) l.divide(out,-1);
    return l.bindonuniversal(tar,oform);
}

void multiaffray(MetaBank* mb,Binding& l,int tar,std::vector<Binding>& out) {
    for (int g=0;g<mb->ara;g++) {
        Binding dup = l;
        if (affrayBinding(dup,tar,mb->strategies[g])) dup.divide(out,-1);
    }
    for (int g=0;g<l.localtypes[tar].ara;g++) {
        Binding dup = l;
        if (affrayBinding(dup,tar,l.localtypes[tar].args[g])) dup.divide(out,-1);
    }
}

void affrayBinding(MetaBank* mb,Stitching& l,int tar,Strategy& bind,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,l.localtypes,l.ara);
    std::vector<Binding> buf;
    if (affrayBinding(bl,tar,bind)) bl.divide(buf,-1);
//    for (int v=0;v<buf.size();v++) out.push_back(Stitching(buf[v],l.caut,l.hook));
    for (int v=0;v<buf.size();v++) boilBinding(mb,out,buf[v],l.caut,l.hook);
}
void affrayBinding(MetaBank* mb,Stitching& l,int tar,Stitching& s,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,l.localtypes,l.ara);
    Binding sl = Binding(mb,s.localtypes,s.ara);
    Binding sgua = Binding(bl,sl,0,l.localtypes[tar].ara,l.localtypes[tar].args);
    std::vector<int> ncaut;
    for (int i=0;i<l.caut.size();i++) ncaut.push_back(l.caut[i]);
    for (int i=0;i<s.caut.size();i++) ncaut.push_back(s.caut[i]+l.ara);
    
    std::vector<Binding> buf;
//    ParameterContext cont = sgua.tracks.append(l.localtypes[tar]);
//    Statement female = l.localtypes[tar].snapshot(2);
    Statement male   = s.hook.deepcopy();
    male.idpush(1,l.ara,0);
    expandLocs(1,male,0,3,l.localtypes[tar].ara,l.localtypes[tar].args);
    if (sgua.bindonuniversal(tar,male)) {
        unsigned long long cmax = 1;
        for (int c=0;c<s.caut.size();c++) cmax*=(l.localtypes[tar].ara+1);
        for (unsigned long long c=0;c<cmax;c++) {
            Binding bn = sgua;
            unsigned long long ca=c;
            bool wrks = true;
            for (int a=0;a<s.caut.size();a++) {
                if (ca%(l.localtypes[tar].ara+1)) if (affrayBinding(bn,s.caut[a]+l.ara,
                bn.localtypes[s.caut[a]+l.ara].args[
                bn.localtypes[s.caut[a]+l.ara].ara-ca%(l.localtypes[tar].ara+1)
                ]))
                {wrks=false;break;}
            }
            if (wrks) bn.divide(buf,-1);
        }
    }
//    for (int v=0;v<buf.size();v++) out.push_back(Stitching(buf[v],ncaut,l.hook));
    for (int v=0;v<buf.size();v++) boilBinding(mb,out,buf[v],ncaut,l.hook);
}


void multiaffray(MetaBank* mb,std::vector<Stitching>& bank,int l,int tar) {
    for (int g=0;g<bank.size();g++) affrayBinding(mb,bank[l],tar,bank[g],bank);
    for (int g=0;g<bank[l].localtypes[tar].ara;g++) affrayBinding(mb,bank[l],tar,bank[l].localtypes[tar].args[g],bank);
    bank[l].caut.push_back(tar);
}


void allsolutions(Binding& inp,Strategy& a,Strategy& b,ParameterContext& pc,std::vector<Binding>& out) {
    inp.decompose(a.type,b.type,inp.tracks);
    std::vector<Binding> car1;
    std::vector<Binding> car2;
    inp.divide(car1,-1);
    #define CAR1 (x%2?car2:car1)
    #define CAR2 (x==a.ara-1?out:x%2?car1:car2)
    for (int x=0;x<a.ara;x++) {
        for (int h=0;h<car1.size();h++) {
            for (int y=0;y<b.ara;y++) {
                ParameterContext shift = pc.append(<#Strategy &#>);
                allsolutions(car1[h],b.args[y],a.args[y],car2);
            }
        }
        car1.clear();
    }
    #undef CAR1
    #undef CAR2
}

bool obsoletes(MetaBank* mb,Stitching& b,Stitching& a) {
    std::vector<Binding> obsconfigurations;
    ParameterContext comb;
    Strategy* dpa = new Strategy[a.ara];
    for (int i=0;i<a.ara;i++) dpa[i] = a.localtypes[i].bring_depth(1,1);
    comb.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
    comb.dat.push_back(std::pair<Strategy*,int>(dpa,a.ara));
    Binding l = Binding(comb,b.localtypes,b.ara);
    allsolutions(l, <#Strategy &a#>, <#Strategy &b#>,obsconfigurations);//targets????
    return obsconfigurations.size();
}
//    ParameterContext comb;throw;throw;throw;
//    Strategy* dpa = new Strategy[a.ara];
//    for (int i=0;i<a.ara;i++) dpa[i] = a.localtypes[i].bring_depth(1,1);
//    comb.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
//    comb.dat.push_back(std::pair<Strategy*,int>(dpa,a.ara));
//    Binding l = Binding(comb,b.localtypes,b.ara);
//    if (l.decompose(a.quicktype,b.quicktype,l.tracks)) {
//        bool found = true;
//        for (int g=0;g<b.ara;g++) {
//            bool skip=false;
//            for (int c=0;c<l.binds.size();c++) if (l.binds[c].head.id==g) {skip=true;break;}
//            if (skip) continue;
//            found = false;
//            Strategy insub = b.localtypes[g].substitute_single(l,l.tracks);
//            for (int h=0;h<a.ara;h++) if (judgemental_eq(insub,a.localtypes[h])) {found=true;break;}
//            if (!found) break;
//        }
//        return found;
//    }
//    return false;
//}



void MetaBank::offlineLearningStep() {
    int guard = stitchbank.size();
    for (int i=0;i<guard;i++) {
        while (stitchbank[i].open.size()) {
            stitchbank[i].open.erase(stitchbank[i].open.begin());
            multiaffray(this,stitchbank,stitchbank[i].open[0],i);
        }
    }
    for (int a=0;a<stitchbank.size()-1;a++) {
        for (int b=0;b<stitchbank.size();) if (a!=b) {
            if (obsoletes(this,stitchbank[a],stitchbank[b])) {
                stitchbank.erase(stitchbank.begin()+b);
                if (b<a) a--;
            } else b++;
        }
    }
}

//bool exclude(Statement& ob,std::vector<int>& ga) {
//
//}
//bool exclude(Strategy& ob,std::vector<int>& ga) {
//
//}

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




//void multiaffraySet(MetaBank* mb,Binding& l,std::vector<int>& tar,std::vector<Binding>& out) {
//    for (int c=0;c<)
//}


//void multiaffraySet(MetaBank* mb,Binding& l,std::vector<int>& tar,std::vector<Binding>& out) {
//    if (tar.size()==0) throw;
//    if (tar.size()==1) return multiaffray(mb,l,tar[0],out);
//    throw;//check this.
//    std::vector<Binding> car1;
//    std::vector<Binding> car2;
//    multiaffray(mb,l,tar[0],car1);
//    #define CAR1 (h%2?car1:car2)
//    #define CAR2 (h==tar.size()-1?out:h%2?car2:car1)
//    for (int h=1;h<tar.size();h++) {
//        for (int g=0;g<CAR1.size();g++) multiaffray(mb,CAR1[g],tar[h],CAR2);
//        CAR1.clear();
//    }
//    #undef CAR1
//    #undef CAR2
//}



//Binding::Binding(Binding& l,int tar,Strategy& bind) {
//}
//Binding::Binding(Binding& l,Binding& r) {
//    ara = l.ara+r.ara;
//    localtypes = new Strategy[ara];
//    for (int a=0;a<l.ara;a++) localtypes[a] = l.localtypes[a].deepcopy();
//    for (int a=0;a<r.ara;a++) localtypes[a+l.ara] = idpush(r.localtypes[a],l.ara,0);
//    for (int a=0;a<l.tracks.dat.size()-1;a++) tracks.dat.push_back(l.tracks.dat[a]);
//    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
//    for (int j=0;j<l.binds.size();j++) {
//        SingleBind dup = SingleBind(
//            l.binds[j].head.deepcopy(),
//            l.binds[j].body.deepcopy(),
//            new Strategy[l.binds[j].ara],l.binds[j].ara,l.binds[j].universal,l.binds[j].concrete);
//        for (int h=0;h<dup.ara;h++) dup.itinerary[h] = l.binds[j].itinerary[h].deepcopy();
//        binds.push_back(dup);
//    }
//    for (int j=0;j<r.binds.size();j++) {
//        SingleBind dup = SingleBind(
//            idpush(r.binds[j].head,l.ara,0),
//            idpush(r.binds[j].body,l.ara,0),
//            new Strategy[r.binds[j].ara],r.binds[j].ara,r.binds[j].universal,r.binds[j].concrete);
//        for (int h=0;h<dup.ara;h++) dup.itinerary[h] = idpush(r.binds[j].itinerary[h],l.ara,0),
//        binds.push_back(dup);
//    }
//}
//void Binding::boost(std::vector<Strategy>& charm) {
//    for (int d=0;d<ara;d++) {
//        Strategy garf = localtypes[d].deepcopy();
//        
//        localtypes[d] = idpush(localtypes[d],amt,com);
//    }
//    for (int d=0;d<binds.size();d++) {
//        binds[d].head = idpush(binds[d].head,amt,com);
//        binds[d].body = idpush(binds[d].body,amt,com);
//        for (int k=0;k<binds[d].ara;k++) {
//            binds[d].itinerary[d] = idpush(binds[d].itinerary[d],amt,com);
//        }
//    }
//    
//}



void Binding::gapinplace(int com,int amt) {
    for (int d=0;d<ara;d++) localtypes[d].idpush(tracks.loc(),amt,com);
    for (int d=0;d<binds.size();d++) {
        binds[d].head.idpush(tracks.loc(),amt,com);
        binds[d].body.idpush(tracks.loc(),amt,com);
        for (int k=0;k<binds[d].ara;k++) binds[d].itinerary[k].idpush(tracks.loc(),amt,com);
    }
}
void Binding::boostinplace(int cara,Strategy* charms) {
    for (int d=0;d<ara;d++) expandLocs(tracks.loc(),localtypes[d],localtypes[d].ara,cara,charms);
    for (int d=0;d<binds.size();d++) {
        expandLocs(tracks.loc(),binds[d].head,binds[d].ara,tracks.loc()+2,cara,charms);
        expandLocs(tracks.loc(),binds[d].body,binds[d].ara,tracks.loc()+2,cara,charms);
        expandRange(tracks.loc(),binds[d].ara,binds[d].itinerary,cara,charms);
    }
}

Binding::Binding(Binding& l,Binding& r,int common,int rcara,Strategy* rcharms) {
    ara = l.ara+r.ara-common;
    localtypes = new Strategy[ara];
    for (int a=0;a<l.ara;a++) localtypes[a] = l.localtypes[a].deepcopy();
    for (int a=common;a<r.ara;a++) {
        localtypes[a+l.ara-common] = r.localtypes[a].deepcopy();
        localtypes[a+l.ara-common].idpush(l.tracks.loc(),l.ara-common,common);
        if (rcara) expandLocs(l.tracks.loc(),localtypes[a+l.ara-common],localtypes[a+l.ara-common].ara,rcara,rcharms);
    }
    for (int a=0;a<l.tracks.dat.size()-1;a++) tracks.dat.push_back(l.tracks.dat[a]);
    tracks.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
    for (int j=0;j<l.binds.size();j++) binds.push_back(l.binds[j].deepcopy());
    for (int j=0;j<r.binds.size();j++) {
        binds.push_back(r.binds[j].deepcopy());
        binds[binds.size()-1].head.idpush(l.tracks.loc(),l.ara-common,common);
        binds[binds.size()-1].body.idpush(l.tracks.loc(),l.ara-common,common);
        for (int k=0;k<binds[binds.size()-1].ara;k++) binds[binds.size()-1].itinerary[k].idpush(tracks.loc(),l.ara-common,common);
        expandLocs(tracks.loc(),binds[binds.size()-1].head,binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        expandLocs(tracks.loc(),binds[binds.size()-1].body,binds[binds.size()-1].ara,tracks.loc()+2,rcara,rcharms);
        expandRange(tracks.loc(),binds[binds.size()-1].ara,binds[binds.size()-1].itinerary,rcara,rcharms);
    }
}

Binding::~Binding() {
    for (int j=0;j<ara;j++) localtypes[j].cleanup();
    if (ara) delete[] localtypes;
    for (int u=0;u<binds.size();u++) {
        binds[u].head.cleanup();
        binds[u].body.cleanup();
        for (int j=0;j<binds[u].ara;j++) binds[u].itinerary[j].cleanup();
        if (binds[u].ara) delete[] binds[u].itinerary;
    }
}
void Statement::cleanup() {
    for (int v=0;v<ara;v++) {
        args[v].cleanup();
    }
    local=9001;
    id=900;
    if (ara) delete[] args;
    ara = 0;
    args = 0;
}
void Strategy::cleanup() {
    for (int v=0;v<ara;v++) {
        args[v].cleanup();
    }
    type.cleanup();
    local=9001;
    id=900;
    ara = 90;
    delete[] args;
    args = 0;
}
Statement Statement::deepcopy() const {
    Statement res = Statement(id,local,ara);
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].deepcopy();
    }
    return res;
}
Strategy Strategy::deepcopy() const {
    Strategy res = Strategy(type.deepcopy(),id,local,ara);
    for (int q=0;q<ara;q++) {
        res.args[q] = args[q].deepcopy();
    }
    return res;
}
Statement::Statement() {}
Statement::Statement(int idr,int loc):local(loc),id(idr) {}
Statement::Statement(int idr,int loc,int buf):local(loc),id(idr),ara(buf) {
    if (buf) args = new Statement[buf];
}
Statement::Statement(int idr,int loc,Statement a):local(loc),id(idr),ara(1) {
    args = new Statement[1];
    args[0] = a;
}
Statement::Statement(int idr,int loc,Statement a,Statement b):local(loc),id(idr),ara(2) {
    args = new Statement[2];
    args[0] = a;args[1] = b;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c):local(loc),id(idr),ara(3) {
    args = new Statement[3];
    args[0] = a;args[1] = b;args[2] = c;
}
Statement::Statement(int idr,int loc,Statement a,Statement b,Statement c,Statement d):local(loc),id(idr),ara(4) {
    args = new Statement[4];
    args[0] = a;args[1] = b;args[2] = c;args[3] = d;
}
Strategy::Strategy() {}
Strategy::Strategy(Statement ty,int idr,int loc):local(loc),id(idr),type(ty) {}
Strategy::Strategy(Statement ty,int idr,int loc,int buf):local(loc),id(idr),type(ty),ara(buf) {
    if (buf) args = new Strategy[buf];
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a):local(loc),id(idr),type(ty),ara(1) {
    args = new Strategy[1];
    args[0] = a;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b):local(loc),id(idr),type(ty),ara(2) {
    args = new Strategy[2];
    args[0] = a;args[1] = b;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c):local(loc),id(idr),type(ty),ara(3) {
    args = new Strategy[3];
    args[0] = a;args[1] = b;args[2] = c;
}
Strategy::Strategy(Statement ty,int idr,int loc,Strategy a,Strategy b,Strategy c,Strategy d):local(loc),id(idr),type(ty),ara(4) {
    args = new Strategy[4];
    args[0] = a;args[1] = b;args[2] = c;args[3] = d;
}
int MetaBank::getAxiom(std::string ax) {
    for (int fs=0;fs<stratnames.size();fs++) {
        if (stratnames[fs]==ax) {
            return fs;
        }
    }
    std::cout<<"Cannot find axiom: "<<ax<<"\n";
    throw;
}

ParameterContext ParameterContext::append(Strategy& a) {return append(a.args,a.ara);}
ParameterContext ParameterContext::append(Strategy* l,int buf) {
    ParameterContext nn;
    nn.dat = dat;
    nn.dat.push_back(std::pair<Strategy*,int>(l,buf));
    return nn;
}
int ParameterContext::loc() {return dat.size()-1;}
Strategy ParameterContext::generateType(Statement& a) {
    if (a.id<0 or a.local<0) throw;
    if (a.local>=dat.size() or a.local<0) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
    if (dat[a.local].first[a.id].ara!=a.ara) throw;
    
    return dat[a.local].first[a.id].ndisp_sub(lloc+1,loc()-lloc,a.args,a.ara,0);
}
Strategy ParameterContext::generateTypeSection(Statement& a,int arg) {
    if (a.local>=dat.size() or a.local<0) throw;
    if (a.id>=dat[a.local].second) throw;
    int lloc = dat[a.local].first[a.id].local;
    return dat[a.local].first[a.id].args[arg].ndisp_sub(lloc+1,loc()-lloc,a.args,arg,1);
}
bool judgemental_eq(Strategy& a,Strategy& b) {
    if (a.id!=b.id or a.local!=b.local) return false;
    if (a.ara!=b.ara) return false;
    for (int k=0;k<a.ara;k++) {
        if (!judgemental_eq(a.args[k],b.args[k])) return false;
    }
    return judgemental_eq(a.type,b.type);
}
bool judgemental_eq(Statement& a,Statement& b) {
    if (a.local!=b.local or a.id!=b.id) return false;
    if (a.ara!=b.ara) throw;
    for (int u=0;u<a.ara;u++) {
        if (!judgemental_eq(a.args[u],b.args[u])) return false;
    }
    return true;
}
bool judgemental_eq(SingleBind& a,SingleBind& b) {
    if (a.ara!=b.ara) return false;
    for (int u=0;u<a.ara;u++) {
        if (!judgemental_eq(a.itinerary[u],b.itinerary[u])) return false;
    }
    return (judgemental_eq(a.head,b.head) and judgemental_eq(a.body,b.body)) or (judgemental_eq(a.head,b.body) and judgemental_eq(a.body,b.head));
}
//bool judgemental_eq(Stitch& a,Stitch& b) {
//    if (a.ara!=b.ara or a.vin!=b.vin) return false;
//    if (a.arav!=b.arav) throw;
//    for (int u=0;u<a.ara;u++)  if (!judgemental_eq(a.itinerary[u],b.itinerary[u])) return false;
//    for (int u=0;u<a.arav;u++) if (!judgemental_eq(a.bod[u],b.bod[u])) return false;
//    return judgemental_eq(a.head,b.head);
//}
bool judgemental_eq(Binding& a,Binding& b) {
    if (a.ara!=b.ara or a.binds.size()!=b.binds.size()) return false;
    for (int k=0;k<a.ara;k++) {
        if (!judgemental_eq(a.localtypes[k],b.localtypes[k])) return false;
    }
    for (int c=0;c<a.binds.size();c++) {
        bool found = false;
        for (int d=0;d<a.binds.size();d++) {
            if (judgemental_eq(a.binds[c],b.binds[c])) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}
//bool judgemental_eq(StitchLink& a,StitchLink& b) {
//    if (a.back!=b.back or a.locals.size()!=b.locals.size()) return false;
//    for (int g=0;g<a.locals.size();g++) if (a.locals[g]!=b.locals[g]) return false;
//    return true;
//}
bool semijudgeeq(Statement& a,Statement& b,std::map<int,Statement>& oa,std::map<int,Statement>& ob,
    std::vector<std::pair<int,int>>& bin,
    std::vector<std::pair<Statement,Statement>>& memo) {
    for (int h=0;h<memo.size();h++) if (judgemental_eq(memo[h].first,a) and judgemental_eq(memo[h].second,b)) return true;
    memo.push_back(std::pair<Statement,Statement>(a,b));
    if (a.local==1 and a.id!=-1 and b.local==1 and b.id!=-1) {
        if (!oa.count(a.id) and !ob.count(b.id)) {
            for (int h=0;h<bin.size();h++) {
                if (bin[h].first==a.id and bin[h].second==b.id) return true;
                if (bin[h].first==a.id or bin[h].second==b.id) return false;
            }
            bin.push_back(std::pair<int,int>(a.id,b.id));
        }
        if (!oa.count(a.id) or !ob.count(b.id)) return false;
        return semijudgeeq(oa[a.id],ob[b.id],oa,ob,bin,memo);
    }
    if (a.local==1 and a.id!=-1) {
        if (!oa.count(a.id)) return false;
        return semijudgeeq(oa[a.id],b,oa,ob,bin,memo);
    }
    if (b.local==1 and b.id!=-1) {
        if (!ob.count(b.id)) return false;
        return semijudgeeq(a,ob[b.id],oa,ob,bin,memo);
    }
    if (a.id!=b.id or a.local!=b.local) return false;
    for (int g=0;g<a.ara;g++) if (!semijudgeeq(a.args[g],b.args[g],oa,ob,bin,memo)) return false;
    return true;
}

//bool judgemental_eq(Restriction& a,Restriction& b) {
//    if (a.contmax!=b.contmax or a.contmin!=b.contmin) throw;
//    if (a.rest.size()!=b.rest.size()) return false;
//
////    for (int g=0;g<a.types.size();g++) if (!judgemental_eq(a.types[g],b.types[g])) return false;
//    
//    std::vector<std::pair<Statement,Statement>> memo;
//    std::vector<std::pair<int,int>> bin;
//    for (int h=a.contmin;h<a.contmax;h++) {
//        Statement l(h,1);
//        if (!semijudgeeq(l,l,a.rest,b.rest,bin,memo)) return false;
//    }
//    return true;
//}
//bool Restriction::isvalid() {return contmin!=-1;}
//Restriction::Restriction() : contmin(-1) {}
Statement restrictcombine(Statement&,Statement&,bool&,std::map<int,Statement>&,std::vector<std::pair<int,Statement>>&);
void restricthold(int a,Statement& b,bool& annihilate,std::map<int,Statement>& other,std::vector<std::pair<int,Statement>>& memo) {
    for (int j=0;j<memo.size();j++) if (memo[j].first==a and judgemental_eq(memo[j].second,b)) return;
    memo.push_back(std::pair<int,Statement>(a,b));
    Statement stupid = other.count(a)?restrictcombine(other[a],b,annihilate,other,memo):b;
    other[a] = stupid;
}
Statement restrictcombine(Statement& a,Statement& b,bool& annihilate,std::map<int,Statement>& other,std::vector<std::pair<int,Statement>>& memo) {
    if (a.local==1 and a.id!=-1) {
        restricthold(a.id,b,annihilate,other,memo);
        return a;
    }
    if (b.local==1 and b.id!=-1) {
        restricthold(b.id,a,annihilate,other,memo);
        return b;
    }
    if (a.id!=b.id or a.local!=b.local or a.id==-1) {annihilate=true;return Statement(-1,1);}
    Statement res = Statement(a.id,a.local,a.ara);
    for (int g=0;g<a.ara;g++) res.args[g] = restrictcombine(a.args[g],b.args[g],annihilate,other,memo);
    return res;
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
//Restriction::Restriction(Binding& bind,int cont) : contmin(cont), contmax(bind.ara) {
//    bool whocares=false;
//    for (int g=0;g<bind.ara;g++) types.push_back(restrictsantize(bind.localtypes[g].type));
//    for (int h=0;h<bind.binds.size();h++) {
//        Statement accounted = restrictsantize(bind.binds[h].body);
//        
//        std::vector<std::pair<int,Statement>> memo;
//        restricthold(bind.binds[h].head.id,accounted,whocares,rest,memo);
////        Statement stupid = rest.count(bind.binds[h].head.id)?restrictcombine(rest[bind.binds[h].head.id],accounted,whocares,rest):accounted;//order of ops?
////        rest[bind.binds[h].head.id] = stupid;
//    }
//    if (whocares) throw;
//}
//bool Restriction::combine(Restriction& other) {
//    bool whocares=false;
////    int oldlocmax = locmax;
////    for (int h=contmax;h<other.types.size();h++) types.push_back(increase(other.types[h],contmax,oldlocmax-contmax));
//    
//    for (auto it = other.rest.begin();it!=other.rest.end();it++) {
//        Statement accounted = increase(it->second,contmax,locmax-contmax);
//        
//        std::vector<std::pair<int,Statement>> memo;
//        restricthold(it->first,accounted,whocares,rest,memo);
//        
////        Statement stupid = rest.count(it->first)?restrictcombine(rest[it->first],accounted,whocares,rest):accounted;
////        rest[it->first] = stupid;
//    }
//    locmax+=other.locmax-contmax;
////    if (whocares) throw;
//    return whocares;
//}
//void Restriction::recurcombine(int rstart) {
//    bool whocares=false;
////    std::cout<<contmax-contmin<<" - "<<other.contmax-other.contmin<<"\n";
////    if (contmax-contmin!=other.contmax-other.contmin) throw;
////    int oldlocmax = locmax;
////    for (int h=other.contmax;h<other.types.size();h++) types.push_back(increase(other.types[h],other.contmax,oldlocmax-other.contmax));
//    for (int h=0;h<contmax-contmin;h++) {
//        Statement ldin(rstart+h,1);
//        Statement rdin(contmin+h,1);
//        std::vector<std::pair<int,Statement>> memo;
//        restrictcombine(ldin,rdin,whocares,rest,memo);
////        restrictcombine(types[other.contmin+h],types[contmin+h],whocares,rest,memo);
//    }
////    locmax+=
//}
//
//Stitching::Stitching(int s) : secretbacklink(s) {}
//Stitching::Stitching(Binding& b) : govern(b) , secretbacklink(-1) {}
//Restriction::Restriction(Restriction& memo,Restriction& inmate,bool& allowed) : contmax(inmate.contmax),contmin(inmate.contmin) {
//    if (inmate.contmax-inmate.contmin!=memo.contmax-memo.contmin) {allowed=false;return;}
//    for (int h=0;h<inmate.types.size();h++) types.push_back(inmate.types[h].deepcopy());
//    for (int h=0;h<memo.types.size();h++) types.push_back(increase(memo.types[h],0,inmate.types.size()));
//    for (auto it = inmate.rest.begin();it!=inmate.rest.end();it++) rest[it->first] = it->second.deepcopy();
//    for (auto it = memo.rest.begin();it!=memo.rest.end();it++) rest[it->first+inmate.types.size()] = increase(it->second,0,inmate.types.size());
//    for (int h=0;h<memo.contmax-memo.contmin;h++) {
//        Statement ldin(memo.contmin+h+inmate.types.size(),1);
//        Statement rdin(inmate.contmin+h,1);
//        restrictcombine(ldin,rdin,allowed,rest);
//        restrictcombine(types[memo.contmin+h+inmate.types.size()],types[inmate.contmin+h],allowed,rest);
//    }
//}
//bool restriction_collision(Restriction& a,Restriction& b) {
//    
//}
//bool judgemental_eq(std::vector<Stitch>& as,std::vector<Stitch>& bs) {
//    if (as.size()!=bs.size()) return false;
//    bool* stitchcheck = new bool[as.size()];
//    for (int c=0;c<as.size();c++) stitchcheck[c] = true;
//    for (int a=0;a<as.size();a++) {
//        bool ngr = true;
//        for (int b=0;b<as.size();b++) if (stitchcheck[b]) {
//            if (judgemental_eq(as[a],bs[b])) {
//                ngr = false;
//                stitchcheck[b] = false;
//            }
//        }
//        if (ngr) {
//            delete[] stitchcheck;
//            return false;
//        }
//    }
//    delete[] stitchcheck;
//    return true;
//}

//Vein vein_cartesian(Vein& va,Vein& vb) {
//    Vein res;
//    for (int a=0;a<va.size();a++) {
//        for (int b=0;b<vb.size();b++) {
//            res.push_back(Binding(va[a],vb[b]));
//        }
//    }
//    return res;
//}


//SingleBind emplace(Statement head,Statement body,int loc,ParameterContext& params) {
////    std::map<std::pair<int,int>,int> remap;
////    int mappoint = 0;
////    std::string placing = head.tostring()+"  "+body.tostring();
////    Statement modhead = head.depth_push(loc+1,1);
////    Statement modbody = body.depth_push(loc+1,1);
//    Statement modhead = params.morph(loc,head,params.loc());
//    Statement modbody = params.morph(loc,body,params.loc());
////    modhead.clip_upperbound(params.loc()+2,loc,remap,mappoint);
////    modhead = modhead.paste_upperbound(params.loc()+2,remap,loc).depth_push(loc+2,loc+1-params.loc()-1);
////    modbody = modbody.paste_upperbound(params.loc()+2,remap,loc).depth_push(loc+2,loc+1-params.loc()-1);
//////    std::string nplacing = modhead.tostring()+"  "+modbody.tostring();
////    Strategy* trash = 0;
////    int sz=-1;
////    if (modhead.is_valid() and modbody.is_valid()) {
////        trash = new Strategy[remap.size()];
////        sz=remap.size();
////        for (auto it=remap.begin();it!=remap.end();it++) {
////            if (it->first.first>=params.dat[it->first.second-1].second) throw;
////            int lloc = params.dat[it->first.second-1].first[it->first.first].local;
//////            throw;
////            trash[it->second] = params.dat[it->first.second-1].first[it->first.first].bring_depth(lloc,loc+1-lloc);//this shit
////            trash[it->second].id=it->second;//SUSPICIOUS
////        }
////    }
//    return SingleBind(modhead,modbody,params.compress(loc),params.index(loc),false,!modbody.contains(loc));
//}
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
bool Binding::crossdecompose(Statement& left,Statement& right,ParameterContext& params) {
    Strategy ltype = params.generateType(left);
    Strategy rtype = params.generateType(right);
    bool wrk = decompose(ltype.type,rtype.type,params);
    ltype.cleanup();
    rtype.cleanup();
    if (!wrk) return false;
    return decompose(left,right,params);
}
bool Binding::bindonuniversal(int tar,Statement& templ) {
    ParameterContext cont = tracks.append(localtypes[tar]);
    Statement plain = localtypes[tar].snapshot(tracks.loc()+1);
    Strategy htype = cont.generateType(plain);
    Strategy btype = cont.generateType(templ);
    bool wrk = decompose(htype.type,btype.type,cont);
    htype.cleanup();
    btype.cleanup();
    if (!wrk) return false;
    binds.push_back(SingleBind(plain,templ.deepcopy(),localtypes[tar].args,localtypes[tar].ara,true,!templ.contains(tracks.loc())));throw;//not memory safe.
    return true;
}
bool Binding::decompose(Statement& left,Statement& right,ParameterContext& params) {
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
        Strategy calctype = params.generateType(left);
        for (int u=0;u<left.ara;u++) {
            ParameterContext continued = params.append(calctype.args[u]);
            if (!decompose(left.args[u], right.args[u],continued)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
bool Binding::typebind(Statement& a,Statement& type,ParameterContext& params) {
//    if (a.local==0 and a.ara==0) return true;
    Strategy calctype = params.generateType(a);
//    calctype.type.loosecheck(params);
//    Statement passin = Statement(0,0);
//    typebind(type,passin,params);
//    loosecheck();
    if (!decompose(calctype.type,type,params)) return false;
//    loosecheck();
    if (calctype.ara!=a.ara) throw;
    for (int u=0;u<a.ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        typebind(a.args[u],calctype.args[u].type,nn);
    }
    return true;
}
Strategy ParameterContext::morph(int local,Strategy& a,int recur) {
    Strategy res = Strategy(morph(local,a.type,a.local-recur),a.id,local+1+recur,a.ara);
    for (int j=0;j<a.ara;j++) {
        res.args[j] = morph(local,a.args[j],recur+1);
    }
    return res;
}
Statement ParameterContext::morph(int local,Statement& a,int limit) {
    int nl=a.local;
    int ni=a.id;
    if (a.local>limit) {
        nl=a.local+local+1-limit;
    } else if (a.local>local) {
        nl=local+1;
        for (int j=local+1;j<a.local;j++) {
            ni+=dat[j].second;
        }
    }
    Statement res = Statement(ni,nl,a.ara);
    for (int j=0;j<a.ara;j++) {
        res.args[j] = morph(local,a.args[j],limit);
    }
    return res;
}
Strategy* ParameterContext::compress(int loc) {
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
//Statement intersperse(Strategy a,int l1,int l2) {
//    std::cout<<"this is hella sketch - ur really just gonna add one and alternate? thats wrong and u know it.\n";
////    throw;//this is hella sketch - ur really just gonna add one and alternate? thats wrong and u know it.
//    Statement res = Statement(a.id,l1,a.ara);
//    for (int k=0;k<a.ara;k++) {
//        res.args[k] = intersperse(a.args[k],l2+1,l1+1);
//    }
//    return res;
//}
Statement* ParameterContext::artifact(int local) {
    Statement* end = new Statement[index(local)];
    int k=0;
    for (int j=local+1;j<dat.size();j++) {
        for (int h=0;h<dat[j].second;h++) {
//            throw;//back and forths should devolve into +1 increments, not alternating.
//            end[k] = intersperse(dat[j].first[h],j,loc());
            end[k] = dat[j].first[h].snapshot(loc()+1);
            k++;
        }
    }
    return end;
}
int ParameterContext::index(int loc) {
    int k=0;
    for (int j=loc+1;j<dat.size();j++) {
        k+=dat[j].second;
    }
    return k;
}
Statement ParameterContext::generateLocal(Statement expected,int locl,int place,Strategy* types) {
    dat[locl].second = place;
    Strategy nloc = Strategy(morph(locl,expected,loc()),place,locl);
    Statement nstat = Statement(place,locl);
    nstat.ara=nloc.ara=index(locl);
    nloc.args=compress(locl);
    nstat.args=artifact(locl);
    types[place]=nloc;
    return nstat;
}
int primeTrans(Statement a,int loc) {
    if (a.local==loc) {
        return 1;
    } else {
        int sum=0;
        for (int u=0;u<a.ara;u++) sum+=primeTrans(a.args[u],loc);
        return sum;
    }
}
Statement godtrans(Statement a,Statement expected,int loc,ParameterContext& params) {
    if (a.local==loc) {
//        params.dat[loc] = std::pair<Strategy*,int>(types,place);
        return params.generateLocal(expected,loc,params.dat[loc].second++,params.dat[loc].first);
    } else {
        Statement res = Statement(a.id,a.local,a.ara);
        for (int u=0;u<a.ara;u++) {//now i understand
//            params.typecheck();
            Strategy subcalctype = params.generateTypeSection(res,u);
            ParameterContext nn = params.append(subcalctype);
            res.args[u] = godtrans(a.args[u],subcalctype.type,loc,nn);
        }
        return res;
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
Statement generateFormat(Statement** a,int fibers,ParameterContext& params,Statement exptype,int loc,int& place,Strategy* append,Statement* circuits) {
    bool ide=true;
//    for (int g=0;g<fibers;g++) if (a[g]->local!=params.loc()) ide=false;
    for (int i=0;i<fibers;i++) a[i]->tostring();
    for (int g=0;g<fibers;g++) if (a[g]->local<=loc+1) ide=false;
    if (ide or params.loc()==loc+1) {
        Statement res = Statement(a[0]->id,a[0]->local,a[0]->ara);
        Statement** nn;
        for (int j=0;j<res.ara;j++) {
            nn = new Statement*[fibers];
            for (int h=0;h<fibers;h++) nn[h] = &(a[h]->args[j]);
//            params.dat[loc+1] = std::pair<Strategy*,int>(append,place);
            Strategy jf = params.generateTypeSection(res,j);
            ParameterContext nncon = params.append(jf);
//            std::cout<<j<<"\n";
            res.args[j] = generateFormat(nn,fibers,nncon,jf.type,loc,place,append,circuits);
            
            delete[] nn;
        }
        return res;
    } else {
        for (int h=0;h<fibers;h++) circuits[place*fibers+h]=a[h]->deepcopy();
        return params.generateLocal(exptype,loc+1,place++,append);
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
    for (int u=0;u<a.ara;u++) {
        lazy_mutate_circuit(a.args[u],b.args[u],loc);
    }
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
bool Statement::contains(int i,int loc) const {
    if (id==i and local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(i,loc)) return true;
    return false;
}
bool Statement::contains(int loc) const {
    if (local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(loc)) return true;
    return false;
}
bool Binding::simplify() {
//    int check=0;
    for (int u=0;u<binds.size();u++) {
        ParameterContext tplusi = tracks.append(binds[u].itinerary,binds[u].ara);
        if (binds[u].body.local!=tracks.loc() and binds[u].body.contains(binds[u].head.id,tracks.loc())) return false;
        
        //universal->don't worry about nhead(unless there are two universal binds)
        //another flag->don't wory about nbod
        if (binds[u].universal) {
//            std::cout<<binds[u].head.tostring()<<" <-- "<<tracks.loc()<<"\n";
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            if (!judgemental_eq(nbod,binds[u].body)) {
                if (!decompose(binds[u].head,nbod,tplusi)) return false;
                binds.erase(binds.begin()+u);
                u=0;
            }
        } else if (binds[u].concrete) {
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            Statement nhead = binds[u].head.simp_substitute(*this,tplusi,u);
            if (!judgemental_eq(nbod,binds[u].body)) throw;
            if (!judgemental_eq(nhead,binds[u].head)) {
                if (!decompose(nhead,nbod,tplusi)) return false;
                binds.erase(binds.begin()+u);
                u=0;
            }
        } else {
            Statement nbod = binds[u].body.substitute_single(*this,tplusi);
            Statement nhead = binds[u].body.local==tracks.loc()?
            binds[u].head.substitute_single(*this,tplusi):
            binds[u].head.simp_substitute(*this,tplusi,u);
            if (!judgemental_eq(nhead,binds[u].head) or !judgemental_eq(nbod,binds[u].body)) {
                binds.erase(binds.begin()+u);
                if (!decompose(nhead,nbod,tplusi)) return false;
                u=0;
            }
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
Statement Binding::integrate(Statement& bod,Strategy* itinerary,int iara) {
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
    ParameterContext tplusempty = tracks;
    tplusempty.dat.push_back(std::pair<Strategy*,int>(0,0));
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
        Strategy* itinerary = new Strategy[itinerarysize];
        
        int pl=0;
        Statement format = generateFormat(mul1,out_b[h].size(),tplusempty,Statement(),tracks.loc(),pl,itinerary,circ);
        
        bool valid=true;
        for (int ha=0;ha<binds.size();ha++) if (judgemental_eq(binds[ha].head,format)) valid=false;
        if (!valid) continue;
        
        cartesian.push_back(std::vector<SingleBind>());
        
        generatePartialA(mul2,out_b[h].size(),itinerarysize,itinerary,circ,gather,0,tracks.loc());

        for (int v=0;v<gather.size();v++) {
            bool nuniv = true;
            for (int i=0;i<format.ara;i++) if (format.args[i].local!=tracks.loc()+1) nuniv=false;
            cartesian[cartesian.size()-1].push_back(SingleBind(format,gather[v],itinerary,itinerarysize,nuniv,!gather[v].contains(tracks.loc())));
        }
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

void capture(Statement&,Strategy*,std::map<int,int>&,std::vector<int>&);
void capture(Strategy&,Strategy*,std::map<int,int>&,std::vector<int>&);

void capture(Statement& ob,Strategy* lt,std::map<int,int>& map,std::vector<int>& rmap) {
    if (ob.local==1 and !map.count(ob.id)) {
        map[ob.id] = rmap.size();
        rmap.push_back(ob.id);
        capture(lt[ob.id],lt,map,rmap);
    }
    for (int a=0;a<ob.ara;a++) capture(ob.args[a],lt,map,rmap);
}
void capture(Strategy& ob,Strategy* lt,std::map<int,int>& map,std::vector<int>& rmap) {
    capture(ob.type,lt,map,rmap);
    for (int a=0;a<ob.ara;a++) capture(ob.args[a],lt,map,rmap);
}


void Statement::apply(int loc,std::map<int,int>& map) {
    if (local==loc) id=map[id];
    for (int a=0;a<ara;a++) args[a].apply(loc,map);
}
void Strategy::apply(int loc,std::map<int,int>& map) {
    if (local==loc) id=map[id];
    type.apply(loc,map);
    for (int a=0;a<ara;a++) args[a].apply(loc,map);
}



void boilBinding(MetaBank* mb,std::vector<Stitching>& out,Binding& bin,std::vector<int>& caut,Statement& khook) {
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
                for (int i=0;i<gather.size();i++) {
                    for (int f=0,limit=car.size();f<limit;f++) {
                        car.push_back(car[f]);
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].body.id].args,circref.ara);
                        car[car.size()-1].headargs.push_back(std::pair<int,Statement>(a,bod));
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].head.id].args[a]);
                        Statement incurer = bin.localtypes[bin.binds[0].head.id].snapshot(2);
                        Strategy calctype = conan.generateType(incurer);
                        if (!car[car.size()-1].center.typebind(bod,calctype.type,conan)) {car.erase(car.begin()+car.size()-1);}
                    }
                }
            }
            for (int b=0;b<bin.binds[0].body.ara;b++) {
                Statement circref = bin.binds[0].head.deepcopy();
                std::vector<Statement> gather;
                Statement* hack = bin.binds[0].body.args+b;
                generatePartialA(&hack,1,circref.ara,bin.localtypes[bin.binds[0].head.id].args,circref.args,gather,0,1);
                for (int i=0;i<gather.size();i++) {
                    for (int f=0,limit=car.size();f<limit;f++) {
                        car.push_back(car[f]);
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].head.id].args,circref.ara);
                        car[car.size()-1].bodyargs.push_back(std::pair<int,Statement>(b,bod));
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].body.id].args[b]);
                        Statement incurer = bin.localtypes[bin.binds[0].body.id].snapshot(2);
                        Strategy calctype = conan.generateType(incurer);
                        if (!car[car.size()-1].center.typebind(bod,calctype.type,conan)) {car.erase(car.begin()+car.size()-1);}
                    }
                }
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
                htype.cleanup();
                btype.cleanup();
                std::vector<int> ncaut;//symbound to caut means it is also caut.
                for (int n=0;n<caut.size();n++) {
                    if (caut[n]==bin.binds[0].head.id or caut[n]==bin.binds[0].body.id) ncaut.push_back(res.id+1);
                    else ncaut.push_back(caut[n]);
                }
                for (int i=0;i<dis.size();i++) boilBinding(mb,out,dis[i],caut,khook);
            }
            return;
        } else {
            if (!bin.binds[0].universal) throw;
            applyshort(bin,bin.binds[0].head.id,bin.binds[0].body);
            hook.inplace_sub(bin.binds[0].head.id,1,bin.binds[0].body,2);
        }
        bin.binds.erase(bin.binds.begin());
    }
    std::map<int,int> map;
    std::vector<int> rmap;
    capture(hook,bin.localtypes,map,rmap);
    Strategy* dgr = new Strategy[rmap.size()];
    for (int g=0;g<rmap.size();g++) {
        dgr[g] = bin.localtypes[rmap[g]].deepcopy();
        dgr[g].apply(1,map);
    }
    std::vector<int> ncaut;
    for (int n=0;n<caut.size();n++) if (map.count(caut[n])) ncaut.push_back(map[caut[n]]);
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

Stitching::Stitching(MetaBank* mb,Strategy& muse) : localtypes(muse.args),ara(muse.id),hook(muse.snapshot(1)) {calcopen(mb);}

Stitching::Stitching(MetaBank* mb,Strategy* gs,int a,std::vector<int>& ca,Statement& h) : localtypes(gs),ara(a),caut(ca),hook(h.deepcopy()) {
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









//        std::vector<SingleBind> soapbinds;
//        int sm=ara;
//        #define CAC cartesian[v][ca%cartesian[v].size()]
//        for (int v=0,ca=c;v<cartesian.size();v++) {sm+=primeTrans(CAC.body,tracks.loc());}
//        Strategy* nstrats = new Strategy[sm];
//        for (int w=0;w<ara;w++)  nstrats[w] = localtypes[w].deepcopy();
//        int pl=ara;
////        loosecheck();
//        for (int v=0,ca=c;v<cartesian.size();v++) {
//            ParameterContext ncon = tracks.append(CAC.itinerary,CAC.ara);
//            ncon.dat[tracks.loc()] = std::pair<Strategy*,int>(nstrats,ara);
//            Statement bod = godtrans(CAC.body,Statement(),tracks.loc(),ncon);
//            soapbinds.push_back(SingleBind(CAC.head,bod,CAC.itinerary,CAC.ara,CAC.universal,CAC.concrete));throw;//this isn't memory-safe. trash. use deepcopy.
//            ca/=cartesian[v].size();
//        }
////        if (pl!=sm) throw;
//        #undef CAC
//        bool valid=true;
//        Binding soap = Binding(*this,nstrats,sm);
////        Binding soap = Binding(tracks,nstrats,sm,binds,stitches);
//        for (int u=0;u<soapbinds.size();u++) {
//            ParameterContext conan = soap.tracks.append(soapbinds[u].itinerary,soapbinds[u].ara);
//            Strategy calctype = conan.generateType(soapbinds[u].head);
//            if (!soap.typebind(soapbinds[u].body,calctype.type,conan)) valid=false;
//            soap.binds.push_back(soapbinds[u]);
//        }


//typebind with partial generation?







//bool XBinding::decompose(Statement&,Statement&,std::vector<int>&,std::vector<int>&,ParameterContext&) {
//
//}
//bool XBinding::typebind(Statement&,Statement&,std::vector<int>&,int,ParameterContext&) {
//
//}
//void XBinding::divide(std::vector<XBinding>&,int) {
//
//}
//struct Option {
//    Binding bind;
//    std::vector<int> assign;
//    Option(MetaBank*,int);
//};
//struct Vein {
//    std::vector<Option> opts;
//    Vein();
//};
//struct ExBind {
//    std::vector<Vein> veins;
//    void create(MetaBank*,std::vector<Vein>,Vein);
//    ExBind(MetaBank*);
//};






//
//int findgap(Statement& ob,std::pair<int,int>* hoi,int ara) {
//    int res;
//    if (ob.local==1 and hoi[ob.id].first==-1) return ob.id;
//    for (int j=ob.ara-1;j>=0;j--) {
//        if ((res=findgap(ob.args[j],hoi,ara))!=-1) return res;
//    }
//    return -1;
//}
//void listentwine(Statement& ob,std::map<int,int>& entwine,int& remap) {
//    for (int a=0;a<ob.ara;a++) {
//        listentwine(ob.args[a],entwine,remap);
//    }
//    if (ob.local==1 and !entwine.count(ob.id)) {
//        entwine[ob.id] = remap++;
//    }
//}
//Statement repldecline(Statement& ob,std::vector<int>& unp,std::map<int,int>& tc,int h,Vein& top,Vein& bottom) {
//    Statement res = Statement(ob.id,ob.local,ob.ara);
//    for (int h=0;h<ob.ara;h++) {
//        res.args[h] = repldecline(ob.args[h],unp,tc,h,top,bottom);
//    }
//    if (res.local!=1) return res;
//    if (tc.count(ob.id)) {
//        res.id = tc[ob.id];
//        return res;
//    }
//    for (int j=0;j<bottom.downlink.size();j++) if (bottom.downlink[j]==res.id) {
//        res.id = top.downlink[j];
//        return res;
//    }
//    for (int j=0;j<bottom.uplink.size();j++) if (bottom.uplink[j]==res.id) {
//        res.id = top.uplink[j];
//        return res;
//    }
//    for (int b=0;b<unp.size();b++) if (unp[b]==ob.id) {
//        res.id = top.opts[h].ara+b;
//        return res;
//    }
//    res.id = top.opts[h].ara+unp.size();
//    unp.push_back(ob.id);
//    return res;
//}
//bool popudecline(Statement& obtop,Statement& ob,std::vector<int>& unp,std::map<int,int>& tc,std::vector<Statement>& annex,bool* assigned,int h,Vein& top,Vein& bottom) {
//    if (obtop.local==1) {
//        int wrk=-1;
//        for (int u=0;u<top.downlink.size();u++) {
//            if (top.downlink[u] == obtop.id) wrk=u;
//        }
//        if (wrk!=-1) {
//            Statement apitest = repldecline(ob,unp,tc,h,top,bottom);
//            if (!assigned[]) {
//                assigned = true;
//                //what if obtop contains parameters that appear in ob
//            }
//        }
//    }
//    for (int a=0;a<ob.ara;a++) {
//        if (!popudecline(obtop.args[a],ob.args[a],unp,tc,annex,assigned,h,top,bottom)) return false;
//    }
//    return true;
//}
//bool downdecline(Statement&,Statement&,std::map<int,int>&,int,Vein&,Vein&);
//bool apidowndecline(Statement& ob,int tar,Statement* args,std::map<int,int>& tc,int h,Vein& top,Vein& bottom) {
//    if (ob.local==1 and ob.id == tar) {
//        for (int a=0;a<ob.ara;a++) {
//            if (!downdecline(args[a],ob.args[a],tc,h,top,bottom)) return false;
//        }
//        return true;
//    }
//    for (int a=0;a<ob.ara;a++) {
//        if (!apidowndecline(ob.args[a],tar,args,tc,h,top,bottom)) return false;
//    }
//    return true;
//}


//vein params are implicitly contention
//what if vein parameters get bound? fuck ur right.

//vein parameters bind anything
//vein paramters bind something->split callers????

//V{     g(a) -> g(f(a))
//       g(f(a)) -> g(a)



//bool downdecline(Statement& obtop,Statement& ob,std::map<int,int>& tc,int h,Vein& top,Vein& bottom) {
//    
//    if (obtop.local>1 or ob.local>1) throw;
//    
//    if (obtop.local==1) {
//        int wrk=-1;
//        for (int u=0;u<top.downlink.size();u++) {
//            if (top.downlink[u] == obtop.id) wrk=u;
//        }
//        if (wrk==-1) {
//            if (ob.local!=1) return false;
//            
//            bool found=false;
//            for (int x=0;x<top.uplink.size();x++) {
//                if (top.uplink[x]==obtop.id and bottom.uplink[x]==ob.id) found=true;
//                else if (top.uplink[x]==obtop.id or bottom.uplink[x]==ob.id) return false;
//            }
//            if (!found) {
////                std::cout<<top.opts[h].centermap[obtop.id].first<<" , "<<top.opts[h].centermap[obtop.id].second<<" compared to "<<bottom.opts[h].centermap[ob.id].first<<" , "<<bottom.opts[h].centermap[ob.id].second<<"\n";
//                return true;
//                if (top.opts[h].centermap[obtop.id].second==bottom.opts[h].centermap[ob.id].second) {
//                    if (top.opts[h].centermap[obtop.id].first==-1 or bottom.opts[h].centermap[ob.id].first==-1) {
//                        return top.opts[h].centermap[obtop.id].first==-1 and bottom.opts[h].centermap[ob.id].first==-1;
//                    }
//                    return judgemental_eq(top.opts[h].centers[top.opts[h].centermap[obtop.id].first],bottom.opts[h].centers[bottom.opts[h].centermap[ob.id].first]);
//    //                throw;//unpopulated can be populated if its linked to a vein. educate urself.
//                   
//                }
//                
////                std::cout<<"unequal second centermap\n";
//                return false;
//            }
//        }
//        throw;//rewrite in terms of first locals god damn holy shit oh my god what the fuckkkk
////        if (assigned[wrk]) {
////            if (!judgemental_eq(annex[wrk],)) {
////            
////            }
//
//
//
//
////        } else {
////            assigned[wrk] = true;
////        }
//    }
//    if (obtop.local!=ob.local or obtop.id!=ob.id) return false;
//    for (int a=0;a<ob.ara;a++) {
//        if (!downdecline(obtop.args[a],ob.args[a],tc,h,top,bottom)) return false;
//    }
//    return true;
//}
//bool downdecline() {
//
//}
//void populateContention(std::vector<int>& star,Statement& ob) {
//    if (ob.local==1) {
//        for (int g=0;g<star.size();g++) if (star[g]==ob.id) return;
//        star.push_back(ob.id);
//        if (ob.ara) throw;
//    }//do arguments of local count as contention?
//    for (int a=0;a<ob.ara;a++) {
//        populateContention(star,ob.args[a]);
//    }
//}
//void populateDownlink(std::vector<int>& star,std::vector<Statement>& dl,Statement& ob) {
//    throw;
//    if (ob.local==1) {
//        for (int g=0;g<star.size();g++) if (star[g]==ob.id) return;
//        star.push_back(ob.id);
//        if (ob.ara) throw;
//    }//do arguments of local count as contention?
//    for (int a=0;a<ob.ara;a++) {
//        populateContention(star,ob.args[a]);
//    }
//}
//Strategy lipush(Strategy l,int amt) {
//
//}
//void MetaBank::printveins() {
//    throw;
////    for (int v=0;v<veins.size();v++) {
////        std::cout<<v<<" {\n";
////        for (int i=0;i<veins[v].uplink.size();i++) {
////            std::cout<<"\t"<<i<<" {\n";
////            for (int h=0;h<veins[v].opts.size();h++) {
////                ParameterContext growl = veins[v].opts[h].tracks.append(veins[v].opts[h].localtypes[veins[v].uplink[i]]);
////                Statement printout = veins[v].opts[h].localtypes[veins[v].uplink[i]].snapshot().substitute_single(veins[v].opts[h],growl);
////                std::cout<<"\t\t"<<printout.tostring()<<"\n";
////            }
////            std::cout<<"\t}\n";
////        }
////        std::cout<<"}\n";
////    }
//}
//void MetaBank::linkv(int v,Vein& vv) {
//    std::vector<Vein> subsq;
//    std::vector<std::vector<int>> contention;
//    for (int g=0;g<vv.opts.size();g++) {
//        std::vector<int> cons;
//        for (int l=0;l<vv.contention.size();l++) {
//            bool up=false;
//            for (int c=0;c<vv.opts[g].binds.size();c++) {
//                if (vv.opts[g].binds[c].head.id==vv.contention[l] or (vv.opts[g].binds[c].body.local==1 and vv.opts[g].binds[c].body.id==vv.contention[l])) {
//                    up=true;
//                    break;
//                }
//            }
//            if (!up) {
//                cons.push_back(l);
//            }
//        }
//        for (int h=0;h<contention.size();h++) {
//            if (contention[h]==cons) {
//                
//            }
//        }
//    }
//}
//left side binds
//just contantly check for double sided stitches.

//symmetric stitching?


//int findgap(Statement& ob,std::vector<Stitch>& got) {
//    int res=1;
//    for (int h=0;h<got.size();h++) if (got[h].head.local==1 and got[h].head.id==ob.id) res=0;
//    if (res==1 and ob.local==1) return ob.id;
//    for (int j=ob.ara-1;j>=0;j--) if ((res=findgap(ob.args[j],got))!=-1) return res;
//    return -1;
//}
//void charmsearch(Binding& vsearch,int loc) {
//    for (int b=0;b<vsearch.stitches.size();b++) {
//        if (vsearch.stitches[b].head.local==1 and vsearch.stitches[b].head.id==loc) {
//        
//        }
//    }
//}


//struct IEcombmemo {
//    int srcveina;
//    int srcveinb;
//    std::vector<std::pair<Statement,Statement>> pairsa;
//    std::vector<std::pair<Statement,Statement>> pairsb;
//    int res;
//};
//struct TriangularMemo {
//    Restriction res;
//    TriangularMemo(Binding& a,std::vector<int>& b,Restriction c) : in(a),foci(b),res(c) {}
//};






//struct InternalCarriage {
//    std::vector<Stitching> trees;
//    Restriction restriction;
////    std::vector<int> hdl;
//    std::vector<int> foci;
//    InternalCarriage(Binding& r,int c,std::vector<int>& f) : restriction(r,c),foci(f) {}
//};

//void idswitch(Statement& in,std::vector<int>& l,std::vector<int>& r) {
//    if (in.local==1) for (int g=0;g<l.size();g++) {
//        if (in.id==l[g]) {in.id=r[g];break;}
//        if (in.id==r[g]) {in.id=l[g];break;}
//    }
//    for (int a=0;a<in.ara;a++) idswitch(in.args[a],l,r);
//}
//void idswitch(Strategy& in,std::vector<int>& l,std::vector<int>& r) {
//    if (in.local==1) for (int g=0;g<l.size();g++) {
//        if (in.id==l[g]) {in.id=r[g];break;}
//        if (in.id==r[g]) {in.id=l[g];break;}
//    }
//    for (int a=0;a<in.ara;a++) idswitch(in.args[a],l,r);
//}

//Stitching Stitching::mono(std::vector<int>& fam,int ind) {
//    Stitching dup = *this;
//    dup.links.erase(dup.links.begin()+ind);
//    for (int g=0;g<fam.size();g++) {
//        dup.govern.localtypes[fam[g]].cleanup();
//        dup.govern.localtypes[links[ind].locals[g]].cleanup();
//        dup.govern.localtypes[fam[g]] = govern.localtypes[links[ind].locals[g]].deepcopy();
//        dup.govern.localtypes[links[ind].locals[g]] = govern.localtypes[fam[g]].deepcopy();
//    }
//    for (int g=0;g<govern.ara;g++) idswitch(dup.govern.localtypes[g],fam,links[ind].locals);
//    for (int b=0;b<govern.binds.size();b++) {
//        idswitch(dup.govern.binds[b].head,fam,links[ind].locals);
//        idswitch(dup.govern.binds[b].body,fam,links[ind].locals);
//        for (int c=0;c<dup.govern.binds[b].ara;c++) idswitch(dup.govern.binds[b].itinerary[c],fam,links[ind].locals);
//    }
//    for (int j=0;j<dup.links.size();j++) {
//        for (int f=0;f<dup.links[j].locals.size();f++) {
//            for (int g=0;g<fam.size();g++) {
//                if (dup.links[j].locals[f]==fam[g]) {dup.links[j].locals[f]=links[ind].locals[g];break;}
//                if (dup.links[j].locals[f]==links[ind].locals[g]) {dup.links[j].locals[f]=fam[g];break;}
//            }
//        }
//    }
//    for (int s=0;s<dup.charms.size();s++) {
//        for (int g=0;g<fam.size();g++) {
//            if (dup.charms[s]==fam[g]) {dup.charms[s]=links[ind].locals[g];break;}
//            if (dup.charms[s]==links[ind].locals[g]) {dup.charms[s]=fam[g];break;}
//        }
//    }
//    return dup;
//}

//Stitching veinExpansion(MetaBank* mb,Binding& inp,std::vector<Stitching>& dec,std::vector<TriangularMemo>& memo) {
//    
//}

//void extract(std::vector<int>&,std::vector<int>&,int,Strategy*);
//void decpull(Statement& du,int target,std::vector<std::vector<int>>& links) {
//    if (du.local==1) {
//        links[target].push_back(du.id);
//        links[du.id].push_back(target);
//    }
//    for (int a=0;a<du.ara;a++) decpull(du.args[a],target,links);
//}
//void decpull(Strategy& du,int target,std::vector<std::vector<int>>& links) {
//    decpull(du.type,target,links);
//    for (int a=0;a<du.ara;a++) decpull(du.args[a],target,links);
//}
//void extract(std::vector<int>& start,std::vector<int>& end,int target,std::vector<std::vector<int>>& llinks) {
//    for (int a=0;a<start.size();a++) if (start[a]==target) {
//        start.erase(start.begin()+a);
//        end.push_back(target);
//        for (int h=0;h<llinks[target].size();h++) extract(start,end,llinks[target][h],llinks);
//        return;
//    }
//}
//void cluster(std::vector<std::vector<int>>& end,Binding& extr,std::vector<int>& sk) {
//    std::vector<int> pool;
//    std::vector<std::vector<int>> llinks;
//    for (int a=0;a<sk.size();a++) {
//        pool.push_back(sk[a]);
//        decpull(extr.localtypes[sk[a]],sk[a],llinks);
//    }
//    for (int d=0;d<extr.binds.size();d++) {
//        for (int a=0;a<sk.size();a++) {
//            if (extr.binds[d].head.id==sk[a]) {decpull(extr.binds[d].body,extr.binds[d].head.id,llinks);break;}
//        }
//    }
//    while (pool.size()) {
//        end.push_back(std::vector<int>());
//        extract(pool,end[end.size()-1],pool[0],llinks);
//    }
//}
//bool cluster_eq(Statement& l,Statement& r,std::vector<int>& a,std::vector<int>& b) {
//    if (l.local!=r.local or l.ara!=r.ara) return false;
//    if (l.local==1) {
//        bool fnd=false;
//        for (int j=0;j<a.size();j++) if (a[j]==l.id and b[j]==r.id) fnd=true;
//        if (!fnd) return false;
//    } else if (l.id!=r.id) return false;
//    for (int g=0;g<l.ara;g++) if (!cluster_eq(l.args[g],r.args[g],a,b)) return false;
//    return true;
//}
//bool cluster_eq(Strategy& l,Strategy& r,std::vector<int>& a,std::vector<int>& b) {
//    if (l.ara!=r.ara or !cluster_eq(l.type,r.type,a,b)) return false;
//    for (int g=0;g<l.ara;g++) if (!cluster_eq(l.args[g],r.args[g],a,b)) return false;
//    return true;
//}
//bool cluster_subset(std::vector<int>& l,std::vector<int>& r,Binding& a,Binding& b) {
//    if (l.size()!=r.size()) return false;
//    for (int h=0;h<l.size();h++) if (!cluster_eq(a.localtypes[h],b.localtypes[h],l,r)) return false;
//    for (int k=0;k<a.binds.size();k++) {
//        bool isgroup=false;
//        for (int g=0;g<l.size();g++) if (a.binds[k].head.id==l[g]) isgroup=true;
//        if (isgroup) {
//            bool found=false;
//            for (int j=0;j<b.binds.size();j++) {
//                if (cluster_eq(a.binds[j].head,b.binds[j].head,l,r) and cluster_eq(a.binds[j].body,b.binds[j].body,l,r)) {
//                    found = true;
//                    break;
//                }
//            }
//            if (!found) return false;
//        }
//    }
//    return true;
//}

//Stitching::Stitching(Binding& inb) : govern(inb) {}

//void Stitching::makegap(int com,int amt) {
//    govern.gapinplace(com,amt);
//    for (int j=0;j<charms.size();j++) {
//        if (charms[j]>com) charms[j]+=amt;
//    }
//    for (int k=0;k<links.size();k++) {
//        for (int t=0;t<links[k].locals.size();t++) {
//            if (links[k].locals[t]>com) links[k].locals[t]+=amt;
//        }
//    }
//}
//
//void Stitching::combine(Stitching& r,int common) {
//    govern = Binding(govern,r.govern,common);
//    r.makegap(common,govern.ara-common);
//    for (int c=0;c<r.charms.size();c++) {
//        bool add=true;
//        for (int d=0;d<charms.size();d++) if (charms[d]==r.charms[c]) add=false;
//        if (add) charms.push_back(r.charms[c]);
//    }
//    for (int c=0;c<r.links.size();c++) {
//        bool add=true;
//        for (int d=0;d<links.size();d++) if (judgemental_eq(links[d],r.links[c])) add=false;
//        if (add) links.push_back(r.links[c]);
//    }
//    for (int c=0;c<r.monos.size();c++) monos.push_back(r.monos[c]);
//}
//bool Stitching::trivbind(int loc) {
//    for (int g=0;g<govern.binds.size();g++) {
//        if (govern.binds[g].head.id==loc and govern.binds[g].body.local!=1 and govern.binds[g].universal) {
//            throw;//binds arent explicitly marked as universal...
//            return true;
//        }
//    }
//    return false;
//}
//
//
//
//void stitchpopulate(std::vector<Stitching>& populate,MetaBank* mb,std::vector<int>& tar,Stitching& inp,
//        std::vector<Stitching*>& memo,std::vector<std::vector<int>*>& clustermemo,bool sq,int cq) {
//    std::vector<std::vector<int>> homeclusters;
//    cluster(homeclusters,inp.govern,tar);
//    std::vector<Stitching> car1;
//    std::vector<Stitching> car2;
//    car1.push_back(inp);
//    bool c=true;
//    #define CAR1 (c?car1:car2)
//    #define CAR2 (c?car2:car1)
//
//    for (int i=0;i<homeclusters.size();i++) {
//        int ilk=-1;
//        for (int dk=0,d=0;d<memo.size();dk=memo[d]->govern.ara,d++) if (cluster_subset(*clustermemo[d],homeclusters[i],memo[d]->govern,inp.govern)) {ilk=d;break;}
//        if (ilk!=-1) {
//            for (int g=0;g<CAR1.size();g++) {
//                CAR1[g].links.push_back(StitchLink(ilk,homeclusters[i]));
//            }
//            continue;
//        }
//        std::vector<Stitching> nexts;
//        std::vector<Stitching> conglom;
//        std::vector<Stitching> monos;
//        std::vector<std::vector<int>*> nclumemo = clustermemo;
//        std::vector<Stitching*> nmemo = memo;
//        nclumemo.push_back(&homeclusters[i]);
//        nmemo.push_back(&inp);
//        nexts.push_back(inp);
//        multiaffraySet(mb,homeclusters[i],nexts,sq,cq);
//        for (int j=0;j<nexts.size();j++) {
//            std::vector<int> grtar;
//            for (int b=inp.govern.ara;b<nexts[j].govern.ara;b++) grtar.push_back(b);
//            stitchpopulate(conglom,mb,grtar,nexts[j],nmemo,nclumemo,true,0);
//        }
//        nexts.clear();
//        while (true) {
//            for (int j=0;j<conglom.size();j++) {
//                bool selfdependant=false;
//                for (int t=0;t<conglom[j].links.size();t++) {
//                    if (conglom[j].links[t].back==clustermemo.size()) {
//                        selfdependant=true;
//                        monos.push_back(conglom[j].mono(homeclusters[i],t));
//                    }
//                }
//                if (!selfdependant) nexts.push_back(conglom[j]);
//            }
//            if (!monos.size()) break;
//            conglom.clear();
//            for (int j=0;j<nexts.size();j++) {
//                int precap = nexts[j].monos.size();
//                for (int k=0;k<monos.size();k++) {
//                    nexts[j].monos.push_back(monos[k]);
//                }
//                std::vector<int> postcharms = nexts[j].charms;
//                nexts[j].charms.clear();
//                stitchpopulate(conglom,mb,postcharms,nexts[j],nmemo,nclumemo,false,precap);
//            }
//            monos.clear();
//            nexts.clear();
//        }
//        for (int j=0;j<conglom.size();j++) {
//            for (int g=0;g<CAR1.size();g++) {
//                CAR2.push_back(CAR1[g]);
//                CAR2[CAR2.size()-1].combine(conglom[j],inp.govern.ara);
//            }
//        }
//        CAR1.clear();
//        c=!c;
//    }
//    for (int i=0;i<CAR1.size();i++) {
//        populate.push_back(CAR1[i]);
//    }
//    #undef CAR1
//    #undef CAR2
//}

//InternalCarriage multiplux(std::vector<Strategy>& bram,Binding& inp,std::vector<std::pair<int,int>>& drep,int cr,std::vector<int>& foci,std::vector<std::pair<int,int>>& memo) {
//    std::vector<Binding> car1;
//    std::vector<std::vector<std::pair<int,int>>> dcar1;
//    std::vector<Binding> car2;
//    std::vector<std::vector<std::pair<int,int>>> dcar2;
//    car1.push_back(inp);
//    dcar1.push_back(drep);
//    bool c1=true;
//    #define CAR1 (c1?car1:car2)
//    #define CAR2 (c1?car2:car1)
//    #define DCAR1 (c1?dcar1:dcar2)
//    #define DCAR2 (c1?dcar2:dcar1)
//    for (int d=0;d<foci.size();d++) {
//        for (int j=drep[foci[d]].first;j<bram.size();j++) {
//            for (int m=0;m<bram[j].ara;m++) {
//                for (int g=0;g<CAR1.size();g++) {
//                    if (affrayBinding(CAR1[g],foci[d],bram[j].args[m],CAR2)) {
//                        std::vector<std::pair<int,int>> transfer = DCAR1[g];
//                        for (int nu=0;nu<bram[j].args[m].ara;nu++) transfer.push_back(std::pair<int,int>(j,j?bram[j].args[m].args[nu].ara:0));
//                        DCAR2.push_back(transfer);
//                    }
//                }
//            }
//        }
//        for (int j=0;j<inp.localtypes[foci[d]].ara-drep[foci[d]].second;j++) {
//            for (int g=0;g<CAR1.size();g++) {
//                if (affrayBinding(CAR1[g],foci[d],inp.localtypes[foci[d]].args[j],CAR2)) {
//                    std::vector<std::pair<int,int>> transfer = DCAR1[g];
//                    for (int nu=0;nu<bram[j].args[m].ara;nu++) transfer.push_back(std::pair<int,int>(j));
//                    DCAR2.push_back(transfer);
//                }
//            }
//        }
//        CAR1.clear();
//        c1=!c1;
//    }
//    InternalCarriage dud(inp,cr,foci);
//    for (int h=0;h<CAR1.size();h++) {
//        std::vector<std::pair<int,int>> mem = memo;
//        Stitching HOO = veinConstruction(bram,CAR1[h],DCAR1[h],inp.ara,mem);
//        dud.trees.push_back(HOO);
//        dud.restriction.combine(HOO.hudguvt);
//    }
//    #undef CAR1
//    #undef CAR2
//    #undef DCAR1
//    #undef DCAR2
//    return dud;
//}
//Stitching veinConstruction(std::vector<Strategy>& bram,Binding& inp,std::vector<std::pair<int,int>>& drep,int foci,std::vector<std::pair<int,int>>& memo) {
//    for (int t=0;t<memo.size();t++) {
//        
//    }
////    std::cout<<"nostitch\n";
//    memo.push_back(std::pair<int,int>(foci,inp.ara));
//    
//    std::vector<InternalCarriage> internals;
//    for (int f=foci;f<inp.ara;f++) {
//        std::vector<int> du;
//        du.push_back(f);
//        internals.push_back(multiplux(bram,inp,drep,foci,du,memo));
//    }
//    for (int a=0;a<internals.size();a++) {
//        for (int b=a+1;b<internals.size();b++) {
//            Restriction tester = internals[a].restriction;//it doesn't matter that this is entangled.
//            
//            if (tester.combine(internals[b].restriction)) {
//            
//                std::vector<int> du;
//                for (int x=0;x<internals[a].foci.size();x++) du.push_back(internals[a].foci[x]);
//                for (int x=0;x<internals[b].foci.size();x++) du.push_back(internals[b].foci[x]);
//                internals.erase(internals.begin()+b);
//                internals[a] = multiplux(bram,inp,drep,foci,du,memo);
//                a=-1;break;
//            }
//        }
//    }
//    Stitching guar(inp,foci);
//    for (int a=0;a<internals.size();a++) {
//        guar.hudguvt.combine(internals[a].restriction);
//        guar.stitches.push_back(internals[a].trees);
//    }
//    
//    return guar;
//}

//struct BackBindMemo {
//    int id;
//    Statement head;
//    Binding bin;
//    BackBindMemo(int i,Statement& h,Binding& b) : id(i),head(h),bin(b) {};
//};
//bool deccompare(Statement& obs,std::vector<BackBindMemo>& visited,int v,Statement& concat,Binding& next) {
//    
//}
//bool MetaBank::backbinds(std::vector<BackBindMemo> visited,int id,Statement& head,Binding& bind) {
//    for (int y=0;y<visited.size();y++) {
//        if (visited[y].id==id and judgemental_eq(visited[y].head,head) and judgemental_eq(visited[y].bin,bind)) return false;
//    }
//    visited.push_back(BackBindMemo(id,head,bind));
//    for (int h=0;h<bind.stitches.size();h++) {
//        deccompare()
////        backbinds(visited, <#Statement &head#>, <#Binding &bind#>)
//    }
//}
//void MetaBank::filterV(std::map<std::vector<int>*,Filtermemo>& memo,std::vector<int>& vs,std::vector<Stitch>& stitches) {
//    for (auto it = memo.begin();it!=memo.end();it++) {
//        if (*it->first==vs and judgemental_eq(*it->second[0].second,stitches)) {
//            it->second.push_back(std::pair<std::vector<int>*,std::vector<Stitch>*>(&vs,&stitches));
//            return;
//        }
//    }
//    memo[&vs] = Filtermemo();
//    memo[&vs].push_back(std::pair<std::vector<int>*,std::vector<Stitch>*>(&vs,&stitches));
//    std::vector<int> nvs;
//    std::vector<Stitch> nstitch;
//    
//    
//    
//    
//    for (int d=0;d<memo[&vs].size();d++) {
//        *(memo[&vs][d].first) = nvs;
//        *(memo[&vs][d].second) = nstitch;
//    }
//    memo.erase(memo.find(&vs));
//    
//}
//int MetaBank::intercombineV(std::vector<IEcombmemo>& memo,int va,int vb,Statement& ua,Statement& ub,Statement& a,Statement& b) {
//    for (int p=0;p<memo.size();p++) {
//        if (memo[p].veina==va and judgemental_eq(memo[p].ua,ua) and judgemental_eq(memo[p].a,a) and
//            memo[p].veinb==vb and judgemental_eq(memo[p].ub,ub) and judgemental_eq(memo[p].b,b)) {
//            return memo[p].res;
//        }
//    }
//    Vein n;
//    
//    memo.push_back(IEcombmemo(va,vb,ua,a,ub,b,veins.size()));
//    veins.push_back(n);
//    return veins.size()-1;
//}
//void MetaBank::createv(Binding& nuev) {
//    for (int mem=0;mem<veinmemo.size();mem++) {
//        if (judgemental_eq(*veinmemo[mem][0],nuev)) {
//            veinmemo[mem].push_back(&nuev);
//            return;
//        }
//    }
//    nuev.vein = veins.size();
//    veins.push_back(Vein());
//    veinmemo.push_back(nuev);
//    veins[n].opts.push_back(Binding(this,0,0));
//    
////    for (int l=0;l<nuev.ara;l++) {
////        for (int j=0,maxp=n.opts.size();j<maxp;j++) {
////            if () {
////                for (int m=0;m<ara;m++) {
////                    std::vector<Stitch> nstitch;
////                    Strategy* gup = new Strategy[n.opts[j].ara+strategies[m].ara];
////                    for (int b=0;b<n.opts[j].ara;b++) gup[b] = n.opts[j].localtypes[b];
////                    for (int b=0;b<strategies[m].ara;b++) gup[b+n.opts[j].ara] = idpush(strategies[m].args[b],strategies[m].ara);
////                    Binding nxt = Binding(this,gup,n.opts[j].ara+strategies[m].ara,n.opts[j].binds,empt);
////                    ParameterContext fun = nxt.tracks.append(nxt.localtypes[l]);
////                    Statement source = nxt.localtypes[l].snapshot();
////                    Statement snapped = strategies[m].snapshot();
////                    Statement extension = idpush(snapped,strategies[m].ara);
////                    nxt.crossdecompose(source,extension,fun);
////                    nxt.divide(n.opts,-1);
////                }
////            }
////        }
////    }
//}

//    for (int h=0;h<nopts.size();h++) {
//        for (int )
//        Statement tat = o.substitute_single(veins[v].opts[h],veins[v].opts[h].tracks);
//        int gap = findgap(tat,veins[v].opts[h].stitches);
//        if (gap==-1) {h++;continue;}
//        veins[v].
//        Vein dennis;
//        for (int g=0;g<ara;g++) {
//            throw;//preserve assigned veins.
//            Strategy* localtypes = new Strategy[strategies[g].ara+veins[v].opts[h].ara];
//            for (int b=0;b<veins[v].opts[h].ara;b++) localtypes[b] = veins[v].opts[h].localtypes[b].deepcopy();
//            for (int b=0;b<strategies[g].ara;b++) localtypes[b+veins[v].opts[h].ara] = idpush(strategies[g].args[b],veins[v].opts[h].ara);
//            Binding create = Binding(this,localtypes,strategies[g].ara+veins[v].opts[h].ara,veins[v].opts[h].binds);
//            create.loosecheck();
//            
//            Statement consts = strategies[g].snapshot();
//            Statement consts2 = idpush(consts,veins[v].opts[h].ara);
//            Statement consts3 = consts2.depth_push(2,1);
//            Statement ls = veins[v].opts[h].localtypes[gap].snapshot();
//            ParameterContext tocompare = create.tracks.append(create.localtypes[gap]);
//            std::vector<Binding> after;
//            if (create.crossdecompose(ls,consts3,tocompare)) create.divide(after,0);
//            for (int f=0;f<after.size();f++) {
//                dennis.opts.push_back(after[f]);
//            }
//        }
//
//        
//        int nv=-1;
//        for (int y=0;y<veins.size();y++) {
//            if (dennis.opts.size()!=veins[y].opts.size()) continue;
////            if (dennis.contention.size()!=veins[y].contention.size()) continue;
//            std::vector<Statement> pmention;
//            bool* mention = new bool[dennis.contention];
//            for (int p=0;p<dennis.contention;p++) {
//                mention[r] = false;
//                pmention.push_back(dassign[r][p]);
//            }
//            bool works = true;
//            for (int x=0;x<veins[y].opts.size();x++) {
////                ParameterContext hurng = dennis.opts[x].tracks.append(kokovein[r].opts[x].localtypes[dennis.uplink[0]]);
////                Statement res = kokovein[r].opts[x].localtypes[kokovein[r].uplink[w]].snapshot().substitute_single(kokovein[r].opts[x],hurng);
//                Statement res = dennis.
////                ParameterContext althurng = veins[y].opts[x].tracks.append(veins[y].opts[x].localtypes[veins[y].uplink[w]]);
//                Statement altres = veins[y].opts[x].localtypes[veins[y].uplink[w]].snapshot().substitute_single(veins[y].opts[x],althurng);
//                if (!downdecline(altres,res,pmention,mention,x,veins[y],kokovein[r])) {works=false;break;}
//            }
//            if (works) {
//                dassign[r] = pmention;
//                nv = y;
//                break;
//            }
//        }
//        if (nv==-1) {
//            nv = veins.size();
//            veins.push_back(kokovein[r]);
//            createv(nv,gap);
//        }
//        
//        
//        std::vector<Vein> kokovein;
//        std::vector<std::vector<Statement>> dassign;
//        std::vector<int> contention;
//        Statement tattype = veins[v].opts[h].localtypes[gap].type;
//        contention.push_back(gap);
//        populateContention(contention,tattype);
//        for (int g=0;g<ara;g++) {
//            throw;//preserve assigned veins.
//            Strategy* localtypes = new Strategy[strategies[g].ara+veins[v].opts[h].ara];
//            for (int b=0;b<veins[v].opts[h].ara;b++) localtypes[b] = veins[v].opts[h].localtypes[b].deepcopy();
//            for (int b=0;b<strategies[g].ara;b++) localtypes[b+veins[v].opts[h].ara] = idpush(strategies[g].args[b],veins[v].opts[h].ara);
//            Binding create = Binding(this,localtypes,strategies[g].ara+veins[v].opts[h].ara,veins[v].opts[h].binds);
//            create.loosecheck();
//            
//            Statement consts = strategies[g].snapshot();
//            Statement consts2 = idpush(consts,veins[v].opts[h].ara);
//            Statement consts3 = consts2.depth_push(2,1);
//            Statement ls = veins[v].opts[h].localtypes[gap].snapshot();
//            ParameterContext tocompare = create.tracks.append(create.localtypes[gap]);
//            std::vector<Binding> after;
//            if (create.crossdecompose(ls,consts3,tocompare)) create.divide(after,0);
//            for (int f=0;f<after.size();f++) {
//                std::vector<int> uplink;
//                for (int l=0;l<contention.size();l++) {
//                    bool up=false;
//                    for (int c=0;c<after[f].binds.size();c++) {
//                        if (after[f].binds[c].head.id==contention[l] or (after[f].binds[c].body.local==1 and after[f].binds[c].body.id==contention[l])) {
//                            up=true;
//                            break;
//                        }
//                    }
//                    if (!up) {
//                        uplink.push_back(contention[l]);
//                    }
//                }
//                bool found=false;
//                for (int z=0;z<kokovein.size();z++) {
//                    if (uplink==kokovein[z].uplink) {
//                        kokovein[z].opts.push_back(after[f]);
//                        found=true;break;
//                    }
//                }
//                if (!found){
//                    kokovein.push_back(Vein());
//                    kokovein[kokovein.size()-1].opts.push_back(after[f]);
//                    kokovein[kokovein.size()-1].uplink = uplink;
//                    dassign.push_back(std::vector<Statement>());
//                    ParameterContext totat2 = create.tracks.append(veins[v].opts[h].localtypes[gap]);
//                    Statement tat2 = create.localtypes[gap].snapshot().substitute_single(create,totat2);
//                    populateDownlink(contention,dassign[dassign.size()-1],tat2);
//                }
//            }
//        }
//        std::vector<int> nvs;
//        for (int r=0;r<kokovein.size();r++) {
//            int nv=-1;
//            for (int y=0;y<veins.size();y++) {
//                if (kokovein[r].opts.size()!=veins[y].opts.size()) continue;
//                if (kokovein[r].uplink.size()!=veins[y].uplink.size()) continue;
//                std::vector<Statement> pmention;
//                bool* mention = new bool[dassign[r].size()];
//                for (int p=0;p<dassign[r].size();p++) {
//                    mention[r] = false;
//                    pmention.push_back(dassign[r][p]);
//                }
//                bool works = true;
//                for (int w=0;w<veins[y].uplink.size();w++) {
//                    for (int x=0;x<veins[y].opts.size();x++) {
//                        ParameterContext hurng = kokovein[r].opts[x].tracks.append(kokovein[r].opts[x].localtypes[kokovein[r].uplink[w]]);
//                        Statement res = kokovein[r].opts[x].localtypes[kokovein[r].uplink[w]].snapshot().substitute_single(kokovein[r].opts[x],hurng);
//                        ParameterContext althurng = veins[y].opts[x].tracks.append(veins[y].opts[x].localtypes[veins[y].uplink[w]]);
//                        Statement altres = veins[y].opts[x].localtypes[veins[y].uplink[w]].snapshot().substitute_single(veins[y].opts[x],althurng);
//                        if (!downdecline(altres,res,pmention,mention,x,veins[y],kokovein[r])) {works=false;break;}
//                    }
//                    if (!works) break;
//                }
//                if (works) {
//                    dassign[r] = pmention;
//                    nv = y;
//                    break;
//                }
//            }
//            if (nv==-1) {
//                nv = veins.size();
//                veins.push_back(kokovein[r]);
//                createv(nv,gap);
//            }
//            nvs.push_back(nv);
//        }
//        
//        
//        for (int e=0;e<nvs.size();e++) {
//            Binding transplant = veins[v].opts[h];
//            int ncenter = transplant.centers.size();
//            Statement newcenter = Statement(nvs[e],-1,dassign[e].size());
//            for (int a=0;a<newcenter.ara;a++) {
//                newcenter.args[a] = dassign[e][a];
//            }
//            transplant.centers.push_back(newcenter);
//            for (int d=0;d<contention.size();d++) {
//                transplant.centermap[contention[d]].first  = ncenter;
//                transplant.centermap[contention[d]].second = d;
//            }
//            std::cout<<"preinsert "<<&transplant<<"\n";
//            veins[v].opts.insert(veins[v].opts.begin()+h+1,transplant);
//            std::cout<<"postinsert\n";
//            
////            transplant.cleanup();
//        }
////        veins[v].opts[h].cleanup();   <------- this is the problem
//        veins[v].opts.erase(veins[v].opts.begin()+h);
////        for (int e=0;e<nvs.size();e++) {
////            veins[v].opts[h+e].loosecheck();
////        }
//        
//    }
//}
//
//ExBind::ExBind(MetaBank* mb) {
//
//}


//HypBinding::HypBinding(MetaBank* mb) {
//
//}
//Statement lambpush( ) {
//
//}
//Statement slsub(Statement& in,std::map<int,Statement>& carry,std::map<int,int>& vmap,int& maxl) {
//    throw;
//    Statement res = Statement(in.id,in.local,in.ara);
//    if (in.local==1 and in.id<maxp) {
//        if (carry.count(in.id)) {
//            delete [] res.args;
//            return slsub(carry[in.id],carry,vmap,maxp,maxl);
//        } else {
//            vmap[maxl] = vmap[in.id];
//            res.id=maxl++;
//        }
//    }
//    for (int j=0;j<in.ara;j++) {
//        res.args[j] = slsub(in.args[j],carry,vmap,maxp,maxl);
//    }
//    return res;
//}
//void lambcond(Statement& in,std::map<int,std::vector<Statement>>& empvain) {
//    for (int j=0;j<in.ara;j++) lambcond(in.args[j],empvain);
//    if (in.local==1) empvain[in.id];
//}
//bool hypBind(std::vector<std::vector<Statement>>& veins,std::map<int,int>& vmap,int& maxl,Statement& a,Statement& b,std::map<int,Statement>& carry) {
//    if (a.local==1 and carry.count(a.id)) hypBind(veins,vmap,maxl,carry[a.id],b,carry);
//    if (b.local==1 and carry.count(b.id)) hypBind(veins,vmap,maxl,a,carry[b.id],carry);
//    if (a.local==b.local and a.id==b.id) {
//        for (int j=0;j<a.ara;j++) {
//            if (!hypBind(veins,vmap,maxl,a.args[j],b.args[j],carry)) return false;
//        }
//        return true;
//    }
//    if (a.local==1 and b.local==1) {
//        std::vector<Statement> nvein;
//        std::map<int,int>
//        for (int l=0;l<veins[vmap[a.id]].size();l++) {
//            for (int h=0;h<veins[vmap[b.id]].size();h++) {
//                std::map<int,Statement> ndc;
//                std::map<int,int> vmc = vmap;
//                Statement va = lambpush()
//                Statement vb =
//                if (hypBind(veins,vmc,maxl,veins[vmap[a.id]][l],veins[vmap[b.id]][h],ndc)) {
//                    nvein.push_back(slsub(veins[vmap[a.id]][l],ndc,vmap,maxp,maxl));
//                }
//            }
//        }
//        carry[a.id] = Statement(maxl,1);
//        carry[b.id] = Statement(maxl,1);
//        vmap[maxl++] = veins.size();
//        veins.push_back(nvein);
//        return true;
//    }
//    if (a.local==1) {
//        std::map<int,std::vector<Statement>> vain;
//        lambcond(b,vain);
//        
//        std::vector<Statement> nvein;
//        for (int l=0;l<veins[vmap[a.id]].size();l++) {
//            std::map<int,Statement> ndc;
//            if (hypBind(veins,vmap,maxl,lambpush(veins[vmap[a.id]][l],maxl),b,ndc)) {
//                nvein.push_back(slsub(veins[vmap[a.id]][l],ndc,vmap,maxp,maxl));
//                for (auto it=vain.begin();it!=vain.end();it++) {
//                    if (ndc.count(it->first)) {
//                        vain[it->first].push_back(ndc[it->first]);
//                    } else {
//                        vain[it->first].push_back(Statement(it->first,1));
//                    }
//                }
//            }
//        }
//        for (auto it=vain.begin();it!=vain.end();it++) {
//            carry[it->first] = Statement(maxl,1);
//            vmap[maxl++] = veins.size();
//            veins.push_back(it->second);
//        }
//        carry[a.id] = Statement(maxl,1);
//        vmap[maxl++] = veins.size();
//        veins.push_back(nvein);
//        return true;
//    }
//    if (b.local==1) {
//        std::vector<Statement> nvein;
//        for (int l=0;l<veins[vmap[b.id]].size();l++) {
//            std::map<int,Statement> ndc;
//            int maxp=maxl;
//            if (hypBind(veins,vmap,maxl,veins[vmap[b.id]][l],a,ndc)) {
//                nvein.push_back(slsub(veins[vmap[b.id]][l],ndc,vmap,maxp,maxl));
//            }
//        }
//        carry[b.id] = Statement(maxl,1);
//        vmap[maxl++] = veins.size();
//        veins.push_back(nvein);
//        return true;
//    }
//    return false;
//}
//void hyptypebind(Statement& a,Statement& type,std::vector<std::vector<Statement>>& veins,std::map<int,int>& vmap,int& maxl,std::map<int,Statement>& carry,ParameterContext& params) {
//    Strategy calctype = params.generateType(a);
//    if (a.local==1) {
//        hypBind(veins,vmap,maxl,calctype.type,type,carry);
//    }
//    if (calctype.ara!=a.ara) throw;
//    for (int u=0;u<a.ara;u++) {
//        ParameterContext nn = params.append(calctype.args[u]);
//        hyptypebind(a.args[u],calctype.args[u].type,veins,vmap,maxl,carry,nn);
//    }
//}
//void hypPreOptimize() {
//    std::vector<std::vector<Statement>> veins;
//    std::vector<Statement> preop;
//    //pop preop
//    veins.push_back(preop);
//    
//}
//std::map<int,Statement> HypBinding::combine(Statement& a,Statement& b) {
////    std::map<int,Statement>
//    if (a.local==1) {
//        
//    } else if (b.local==1) {
//        
//    } else {
//        if (a.id!=b.id or a.local!=b.local) return false;
//        for (int j=0;j<a.ara;j++) {
//            if (!combine(a.args[j],b.args[j])) return false;
//        }
//        return std::map<int,Statement>();
//    }
//}


//bool ExBind::match(Statement& a,Statement& b,std::vector<int>& lassign,std::vector<int>& rassign,std::vector<int>& lpath,std::vector<int>& rpath,
//    std::vector<ExRelationship>& product,std::vector<std::pair<int,ExRelationship*>> known) {
//    if (a.id==b.id and a.local==b.local) {
//        if (a.local!=1) for (int j=0;j<a.ara;j++) {
//            match(a.args[j],b.args[j],lassign,rassign,lpath,rpath,product,known);
//        }
//        return true;
//    }
//    if (a.local==1) {
//        std::vector<int> lpathext = lpath;
//        lpathext.push_back(a.id);
//        ExRelationship gar = ExRelationship(lpathext,veins[lassign[a.id]].options.size());
//        for (int g=0;g<veins[a.id].options.size();g++) {
//            gar.cross[g].valid = match(veins[lassign[a.id]].options[g].contain,b,veins[lassign[a.id]].options[g].assign,rassign,lpathext,rpath,gar.cross[g].rels,known);
//        }
//        for (int f=0;f<known.size();f++) {
//            
//        }
//        //mergepush gar
//    } else if (b.local==1) {
//        std::vector<int> rpathext = rpath;
//        rpathext.push_back(b.id);
//        ExRelationship gar = ExRelationship(rpathext,veins[rassign[b.id]].options.size());
//        for (int g=0;g<veins[b.id].options.size();g++) {
//            gar.cross[g].valid = match(a,veins[rassign[b.id]].options[g].contain,lassign,veins[rassign[b.id]].options[g].assign,lpath,rpathext,gar.cross[g].rels,known);
//        }
//    }
//    return true;
//}
//bool pathcompare(std::vector<int> r,std::vector<int> l) {
//    if (r.size()>l.size()) return true;
//    for (int y=0;y<r.size();y++) {
//        if (r[y]>l[y]) return true;
//        if (r[y]<l[y]) return false;
//    }
//    throw;
//}
//bool ExBind::match(MetaBank* mb,Statement& a,Statement& b,std::vector<int>& lpath,std::vector<int>& rpath) {
//    if (b.local==1 and a.local!=1) return match(mb,b,a,rpath,lpath);
//    if (a.id==b.id and a.local==b.local) {
//        if (a.local!=1) for (int j=0;j<a.ara;j++) {
//            if (!match(mb,a.args[j],b.args[j],lpath,rpath)) return false;
//        }
//        return true;
//    }
//    if (a.local!=1) return false;
//    std::vector<int> lpathext = lpath;
//    lpathext.push_back(a.id);
//    if (links.count(lpathext)) {
//        Statement follow = mb->strategies[links[lpathext]].snapshot();
//        return match(mb,follow,b,lpathext,rpath);
//    }
//    for (int j=0;j<symlinks.size();j++) {
//        if (symlinks[j].first==lpathext) {
//            lpathext = symlinks[j].second;
//            j=0;
//        }
//    }
//    if (b.local==1) {
//        std::vector<int> rpathext = rpath;
//        rpathext.push_back(b.id);
//        for (int j=0;j<symlinks.size();j++) {
//            if (symlinks[j].first==rpathext) {
//                rpathext = symlinks[j].second;
//                j=0;
//            }
//        }
//        if (pathcompare(rpathext,lpathext)) symlinks.push_back(std::pair<std::vector<int>,std::vector<int>>(rpathext,lpathext));
//        else                                symlinks.push_back(std::pair<std::vector<int>,std::vector<int>>(lpathext,rpathext));
//        return true;
//    }
//    links[lpathext] = b.id;
//    Statement follow = mb->strategies[b.id].snapshot();
//    for (int j=0;j<symlinks.size();) {
//        if (symlinks[j].second==lpathext) {
//            std::vector<int> aside = symlinks[j].first;
//            Statement asid = Statement(aside[aside.size()-1],1);
//            aside.erase(aside.begin()+aside.size()-1);
//            if (!match(mb,asid,follow,aside,lpathext)) return false;
//            symlinks.erase(symlinks.begin()+j);
//        } else j++;
//    }
//    return match(mb,follow,b,lpathext,rpath);
//}
////void ExAABind::typematch(Statement& a,Statement& ty,int v,int vt,std::map<int,int>& veintypes,ParameterContext& params) {
////    if (a.local==1) {
////        //generate type for a.id vein
////        
////        Vein vtype;
////        
////        match(ty,type);
////    }
////    Strategy exptype = params.generateType(a);
////    for (int j=0;j<exptype.ara;j++) {
////        ParameterContext cont = params.append(exptype.args[j]);
////        typematch(a.args[j],exptype.args[j].type,v,vt,cont);
////    }
////}
////int ExAABind::resolve(int v,std::vector<int>& path) {
////    
////}
//Statement ExAABind::grom(MetaBank* mb,Option a,std::vector<int>& path,std::map<std::vector<int>,int>& seqmap,std::vector<int>& assign,ParameterContext&,std::vector<Statement>& types) {
//    if (a.bind.links.count(path)) {
//        Statement res = Statement(a.bind.links[path],0,mb->strategies[a.bind.links[path]].ara);
//        for (int j=0;j<res.ara;j++) {
//            std::vector<int> extpath = path;
//            extpath.push_back(j);
//            res.args[j] = grom(mb,a,extpath,seqmap,assign,types);
//        }
//        return res;
//    }
//    for (int j=0;j<a.bind.symlinks.size();j++) if (a.bind.symlinks[j].first==path) {
//        path = a.bind.symlinks[j].second;
//        j=0;
//    }
//    if (seqmap.count(path)) return Statement(seqmap[path],1);
//    int nv=a.assign[path[0]];
//    std::vector<int> parmap;
//    parmap.push_back(path[0]);
//    for (int g=1;g<path.size();g++) {
//        nv=veins[nv].opts[a.bind.links[parmap]].assign[path[g]];
//        parmap.push_back(path[g]);
//    }
//    seqmap[path] = assign.size();
//    assign.push_back(nv);
//    return Statement(assign.size()-1,1);
//}
//Option::Option(MetaBank* mb,int ind) {
//    for (int y=0;y<mb->strategies[ind].ara;y++) assign.push_back(0);
//    bind.links[std::vector<int>()]=ind;
//}
//void ExAABind::create(MetaBank* mb,std::vector<Vein> lveins,Vein copvein) {
//    for (int h=0;h<mb->ara;h++) {
//        std::map<std::vector<int>,int>& seqmap
//        std::vector<int>& assign
//        Statement c = grom(mb,copvein.opts[h],std::vector<int>(),);
//    }
//}
//ExAABind::ExAABind(MetaBank* mb) {
//    veins.push_back(Vein());
//    ParameterContext np;
//    np.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
//    for (int h=0;h<mb->ara;h++) {
//        veins[0].opts.push_back(Option(mb->strategies[h].ara));
//    }
//
////    typematch(<#Statement &a#>, <#ParameterContext &params#>)
//}



