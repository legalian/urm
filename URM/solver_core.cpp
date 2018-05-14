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


bool affrayBinding(Binding& l,int tar,Strategy& bind) {
    int lara=l.ara;
    l.pad(bind.ara);
    Statement oform = Statement(bind.id,bind.local,bind.ara);
    for (int g=0;g<bind.ara;g++) {
        ParameterContext cont = l.tracks.append(l.localtypes[tar]).append(bind.args[g]);
        cont.dat[l.tracks.loc()].second = lara+g;
        oform.args[g] = cont.generateLocal(bind.args[g].type,l.tracks.loc());
    }
    bool wrk = l.bindonuniversal(tar,oform);
    oform.cleanup();
    return wrk;
}

void multiaffray(Strategy* bank,int ara,Binding& l,int tar,std::vector<Binding>& out) {
    for (int g=0;g<ara;g++) {
        Binding dup = l;
        if (affrayBinding(dup,tar,bank[g])) dup.divide(out,-1);
    }
}
void multiaffrayset(Strategy* bank,int ara,Binding& l,std::vector<int>& tar,std::vector<Binding>& out) {
    unsigned long long cmax=1;
    for (int c=0;c<tar.size();c++) cmax*=ara;
    for (unsigned long long c=0;c<cmax;c++) {
        unsigned long long ca=c;
        bool valid=true;
        Binding dup = l;
        for (int v=0;v<tar.size();v++) {
            if (!affrayBinding(dup,tar[v],bank[ca%ara])) {valid=false;break;}
            ca/=ara;
        }
        if (valid) dup.divide(out,-1);
    }
}
void multiaffray(MetaBank* mb,Binding& l,int tar,std::vector<Binding>& out) {
    multiaffray(mb->strategies,mb->ara,l,tar,out);
    multiaffray(l.localtypes[tar].args,l.localtypes[tar].ara,l,tar,out);
}

void affrayBinding(MetaBank* mb,Stitching& l,int tar,Strategy& bind,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,l.localtypes,l.ara);
    std::vector<Binding> buf;
    if (affrayBinding(bl,tar,bind)) bl.divide(buf,-1);
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
    male.expandLocs(1,0,3,l.localtypes[tar].ara,l.localtypes[tar].args);
    if (sgua.bindonuniversal(tar,male)) {
        unsigned long long cmax = 1;
        for (int c=0;c<s.caut.size();c++) cmax*=(l.localtypes[tar].ara+1);
        for (unsigned long long c=0;c<cmax;c++) {
            Binding bn = sgua;
            unsigned long long ca=c;
            bool wrks = true;
            for (int a=0;a<s.caut.size();a++) {
                if (ca%(l.localtypes[tar].ara+1)) {
                    if (affrayBinding(bn,s.caut[a]+l.ara,bn.localtypes[s.caut[a]+l.ara].args[bn.localtypes[s.caut[a]+l.ara].ara-ca%(l.localtypes[tar].ara+1)])){
                        wrks=false;
                        break;
                    }
                }
                ca/=(l.localtypes[tar].ara+1);
            }
            if (wrks) bn.divide(buf,-1);
        }
    }
    male.cleanup();
    for (int v=0;v<buf.size();v++) boilBinding(mb,out,buf[v],ncaut,l.hook);
}


void multiaffray(MetaBank* mb,int l,int tar,std::vector<Stitching>& bank) {
    for (int g=0;g<bank.size();g++) affrayBinding(mb,bank[l],tar,bank[g],bank);
    for (int g=0;g<bank[l].localtypes[tar].ara;g++) affrayBinding(mb,bank[l],tar,bank[l].localtypes[tar].args[g],bank);
    bank[l].caut.push_back(tar);
}



void affrayOBS(Binding& bin,Strategy* bank,int ara,int avx,int tar,std::vector<Binding>& out) {
    std::vector<Binding> buf;
    multiaffray(bank,ara,bin,tar,buf);
    for (int h=0;h<buf.size();h++) {
        if (bin.ara==buf[h].ara) bin.divide(out,-1);
        std::vector<Binding> car1;
        std::vector<Binding> car2;
        affrayOBS(buf[h],buf[h].localtypes[tar].args+avx,buf[h].localtypes[tar].ara-avx,buf[h].localtypes[bin.ara].ara,bin.ara,car1);
        bool c=true;
        #define CAR1 (c?car1:car2)
        #define CAR2 (g==buf[h].ara-1?out:c?car2:car1)
        for (int g=bin.ara+1;g<buf[h].ara;g++) {
            for (int b=0;b<CAR1.size();b++) {
                affrayOBS(CAR1[b],buf[h].localtypes[tar].args+avx,buf[h].localtypes[tar].ara-avx,buf[h].localtypes[g].ara,g,CAR2);
            }
            c=!c;
            CAR1.clear();
        }
        #undef CAR1
        #undef CAR2
    }
}



bool obsoletes(MetaBank* mb,const Stitching& a,const Stitching& b) {
    std::vector<Binding> obsconfigurations;
    ParameterContext comb;
    Strategy* dpa = new Strategy[a.ara];
    for (int i=0;i<a.ara;i++) dpa[i] = a.localtypes[i].bring_depth(1,1);
    comb.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
    comb.dat.push_back(std::pair<Strategy*,int>(b.localtypes,b.ara));
    Binding l = Binding(comb,dpa,a.ara);
    Statement apush = a.quicktype.depth_push(1,1);
    if (!l.decompose(apush,b.quicktype,l.tracks)) return false;
    apush.cleanup();
    
    if (!a.open.size()) l.divide(obsconfigurations,-1);
    else {
        std::vector<Binding> car1;
        std::vector<Binding> car2;
        affrayOBS(l,b.localtypes,b.ara,0,a.open[0],car1);
        bool c=true;
        #define CAR1 (c?car1:car2)
        #define CAR2 (j==a.open.size()-1?obsconfigurations:c?car2:car1)
        for (int j=1;j<a.open.size();j++) {
            for (int v=0;v<CAR1.size();v++) {
                affrayOBS(CAR1[v],b.localtypes,b.ara,0,a.open[j],CAR2);
            }
            c=!c;
            CAR1.clear();
        }
        #undef CAR1
        #undef CAR2
    }
    
    return obsconfigurations.size();
}


void MetaBank::offlineLearningStep() {
    int guard = stitchbank.size();
    for (int i=0;i<guard;i++) {
        while (stitchbank[i].open.size()) {
            stitchbank[i].open.erase(stitchbank[i].open.begin());
            multiaffray(this,stitchbank[i].open[0],i,stitchbank);
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
    
    for (int g=0;g<ara;g++) stitchbank.push_back(Stitching(this,strategies[g]));
    
    offlineLearningStep();
}












