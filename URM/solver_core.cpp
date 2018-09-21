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


bool affrayBinding(Binding& l,int tar,const Strategy& bind) {
//    l.loosecheck();
    Statement oform = Statement(bind.id,bind.local,bind.ara);
    for (int g=0;g<bind.ara;g++) oform.args[g] = Statement(-1,l.tracks.loc());
    Statement sform = l.integrate(oform,Statement(),l.localtypes[tar].args,l.localtypes[tar].ara);
    
    bool wrk = l.bindonuniversal(tar,sform);
    oform.cleanup();
    sform.cleanup();
    return wrk;
}

void multiaffray(Strategy* bank,int ara,const Binding& l,int tar,std::vector<Binding>& out) {
    for (int g=0;g<ara;g++) {
        Binding dup = l;
        if (affrayBinding(dup,tar,bank[g])) dup.divide(out,-1);
    }
}
//void multiaffrayset(Strategy* bank,int ara,Binding& l,std::vector<int>& tar,std::vector<Binding>& out) {
//    unsigned long long cmax=1;
//    for (int c=0;c<tar.size();c++) cmax*=ara;
//    for (unsigned long long c=0;c<cmax;c++) {
//        unsigned long long ca=c;
//        bool valid=true;
//        Binding dup = l;
//        for (int v=0;v<tar.size();v++) {
//            if (!affrayBinding(dup,tar[v],bank[ca%ara])) {valid=false;break;}
//            ca/=ara;
//        }
//        if (valid) dup.divide(out,-1);
//    }
//}
void multiaffray(MetaBank* mb,const Binding& l,int tar,std::vector<Binding>& out) {
    multiaffray(mb->strategies,mb->ara,l,tar,out);
    multiaffray(l.localtypes[tar].args,l.localtypes[tar].ara,l,tar,out);
}

void affrayBinding(MetaBank* mb,const Stitching& l,int tar,const Strategy& bind,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,deepcopy(l.localtypes,l.ara),l.ara);
    std::vector<Binding> buf;
    if (affrayBinding(bl,tar,bind)) bl.divide(buf,-1);
    for (int v=0;v<buf.size();v++) boilBinding(mb,out,buf[v],l.caut,l.hook);
}
void affrayBinding(MetaBank* mb,const Stitching& l,int tar,const Stitching& s,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,deepcopy(l.localtypes,l.ara),l.ara);
    Binding sl = Binding(mb,deepcopy(s.localtypes,s.ara),s.ara);
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
    std::cout<<buf.size()<<"\n";
    for (int v=0;v<buf.size();v++) {
        boilBinding(mb,out,buf[v],ncaut,l.hook);
    }
}


void multiaffray(MetaBank* mb,int in,std::vector<Stitching>& bank,std::vector<Stitching>& out) {
    while (bank[in].open.size()>0) {
        std::cout<<in<<"__"<<bank[in].open.size()<<"\n";
        for (int g=0,guard=bank.size();g<guard;g++) affrayBinding(mb,bank[in],bank[in].open[0],bank[g],out);
        for (int g=0;g<bank[in].localtypes[bank[in].open[0]].ara;g++) affrayBinding(mb,bank[in],bank[in].open[0],bank[in].localtypes[bank[in].open[0]].args[g],out);
        bank[in].caut.push_back(bank[in].open[0]);
        bank[in].open.erase(bank[in].open.begin());
    }
}



void affrayOBS(Binding& bin,Strategy* bank,int ara,int avx,int tar,std::vector<Binding>& out) {
//    bin.loosecheck();
    std::vector<Binding> buf;
    multiaffray(bank,ara,bin,tar,buf);
    for (int h=0;h<buf.size();h++) {
//        buf[h].loosecheck();
        if (bin.ara==buf[h].ara) buf[h].divide(out,-1);
        std::vector<Binding> car1;
        std::vector<Binding> car2;
        affrayOBS(buf[h],buf[h].localtypes[tar].args+avx,buf[h].localtypes[tar].ara-avx,buf[h].localtypes[buf[h].ara-1].ara,buf[h].ara-1,car1);
        bool c=true;
        #define CAR1 (c?car1:car2)
        #define CAR2 (g==buf[h].ara-1?out:c?car2:car1)
        for (int g=buf[h].ara-2;g>=bin.ara;g--) {
            for (int b=0;b<CAR1.size();b++) {
                if (!CAR1[b].trivbound(b)) affrayOBS(CAR1[b],buf[h].localtypes[tar].args+avx,buf[h].localtypes[tar].ara-avx,buf[h].localtypes[g].ara,g,CAR2);
            }
            c=!c;
            CAR1.clear();
        }
        #undef CAR1
        #undef CAR2
    }
}


//void extaffrayOBS(int oldara,Binding& buf,Strategy* bank,int ara,int avx,int tar,std::vector<Binding>& out) {
//    if (oldara==buf.ara) buf.divide(out,-1);
//    std::vector<Binding> car1;
//    std::vector<Binding> car2;
//    std::vector<Binding> bufn;
//    multiaffray(bank,ara,buf,tar,bufn);
////    multiaffray(buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,buf,buf.ara-1,bufn);
//    for (int h=0;h<bufn.size();h++) {
//        extaffrayOBS(buf.ara,bufn[h],buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,buf.localtypes[buf.ara-1].ara,buf.ara-1,car1);
//    }
////    affrayOBS(buf,buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,buf.localtypes[buf.ara-1].ara,buf.ara-1,car1);
//    bool c=true;
//    #define CAR1 (c?car1:car2)
//    #define CAR2 (g==oldara?out:c?car2:car1)
//    for (int g=buf.ara-2;g>=oldara;g++) {
//        for (int b=0;b<CAR1.size();b++) {
//            if (!CAR1[b].trivbound(b)) {
//                std::vector<Binding> bufn;
//                multiaffray(buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,CAR1[b],g,bufn);
//                for (int h=0;h<bufn.size();h++) {
//                    extaffrayOBS(CAR1[b].ara,bufn[h],buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,buf.localtypes[g].ara,g,CAR2);
//                }
////                affrayOBS(CAR1[b],buf.localtypes[tar].args+avx,buf.localtypes[tar].ara-avx,buf.localtypes[g].ara,g,CAR2);
//            }
//        }
//        c=!c;
//        CAR1.clear();
//    }
//    #undef CAR1
//    #undef CAR2
//}

//void affrayOBS(Binding& buf,Strategy* bank,int ara,int avx,int tar,std::vector<Binding>& out) {
//    std::vector<Binding> bufn;
//    multiaffray(bank,ara,buf,tar,bufn);
//    for (int h=0;h<bufn.size();h++) {
//        extaffrayOBS(buf.ara,bufn[h],bank,ara,avx,tar,out);
//    }
//}

bool Stitching::primatch(MetaBank* mb,const Strategy& target) {
    Binding bl = Binding(mb,deepcopy(localtypes,ara),ara);
    int d=bl.ara;
    bl.pad(target.ara);
    for (int j=0;j<target.ara;j++) {
        bl.localtypes[d+j]=target.args[j];
        bl.localtypes[d+j].idpush(1,d,0);
    }
    Statement idpushed = target.type.deepcopy();
    idpushed.idpush(1,d,0);
    if (bl.decompose(quicktype,idpushed,bl.tracks)) {
    
        for (int v=0;v<caut.size();v++) {
            bool allvalid=false;
            Statement feedthu = localtypes[caut[v]].type.substitute_single(bl,bl.tracks);
            for (int g=0;g<target.ara;g++) {
                Statement feedarn = bl.localtypes[d+g].type.substitute_single(bl,bl.tracks);
                
                if (lazy_judgemental_eq(feedthu,feedarn,1)) allvalid=true;
            }
            if (!allvalid) return false;
        }
    } else return false;
    
    return true;
    throw;//memory neutrality
}

void privateaffraybinding(MetaBank* mb,const Stitching& l,int tar,const Stitching& s,std::vector<Stitching>& out) {
    Binding bl = Binding(mb,deepcopy(l.localtypes,l.ara),l.ara);
    Binding sl = Binding(mb,deepcopy(s.localtypes,s.ara),s.ara);
    Binding sgua = Binding(bl,sl,0,l.localtypes[tar].ara,l.localtypes[tar].args);
    std::vector<int> ncaut;
    
    std::vector<Binding> buf;
//    ParameterContext cont = sgua.tracks.append(l.localtypes[tar]);
//    Statement female = l.localtypes[tar].snapshot(2);
    Statement male   = s.hook.deepcopy();
    male.idpush(1,l.ara,0);
    male.expandLocs(1,0,3,l.localtypes[tar].ara,l.localtypes[tar].args);
    if (sgua.bindonuniversal(tar,male)) {
        unsigned long long cmax = 1;
        for (int c=0;c<s.caut.size();c++) cmax*=l.localtypes[tar].ara;
        for (unsigned long long c=0;c<cmax;c++) {
            Binding bn = sgua;
            unsigned long long ca=c;
            bool wrks = true;
            for (int a=0;a<s.caut.size();a++) {
                if (affrayBinding(bn,s.caut[a]+l.ara,bn.localtypes[s.caut[a]+l.ara].args[bn.localtypes[s.caut[a]+l.ara].ara-ca%l.localtypes[tar].ara])){
                    wrks=false;
                    break;
                }
                ca/=l.localtypes[tar].ara;
            }
            if (wrks) bn.divide(buf,-1);
        }
    }
    male.cleanup();
    for (int v=0;v<buf.size();v++) boilBinding(mb,out,buf[v],ncaut,l.hook);
}

bool obsoletes(MetaBank* mb,const Stitching& a,const Stitching& b) {
    if (!lazy_judgemental_eq(a.quicktype,b.quicktype,1)) return false;
    std::vector<Binding> obsconfigurations;
    Strategy* nmb = new Strategy[mb->ara+1];
    for (int j=0;j<mb->ara;j++) nmb[j]=mb->strategies[j];
    nmb[mb->ara] = Strategy(a.quicktype.deepcopy(),mb->ara,0,a.ara,deepcopy(a.localtypes,a.ara));
    ParameterContext comb;
    comb.dat.push_back({nmb,mb->ara+1});
    Strategy throwaway = Strategy(b.quicktype,0,0,b.ara,b.localtypes);
    Strategy* hoo = new Strategy;
    hoo[0] = throwaway.bring_depth(1,1);
    Binding l = Binding(comb,hoo,1);
    
    affrayOBS(l,nmb+mb->ara,1,0,0,obsconfigurations);
    return obsconfigurations.size();
}


void MetaBank::offlineLearningStep() {
    int guard = stitchbank.size();
    for (int i=0;i<guard;i++) multiaffray(this,i,stitchbank,stitchbank);
    cullobsolete(stitchbank);
}
Statement MetaBank::solveHare(const Strategy& target) {
    std::vector<Binding> buf1;
    std::vector<Binding> buf2;
    buf1.push_back(Binding(this,target.args,target.ara));
    #define B1 (flip?buf1:buf2)
    #define B2 (flip?buf2:buf1)
    bool flip=true;
    for (int safetries=0;safetries<5;safetries++) {
        for (int i=0;i<B1.size();i++) {
            multiaffray(this,B1[i], <#int tar#>, B2)
        }
        B1.clear();
        flip = !flip;
    }
    #undef B1
    #undef B2
    throw;//failed to achieve goal in given number of iterations.
}
Statement MetaBank::solveTortoise(const Strategy& target) {
    std::vector<Stitching> privatebank;
    privatebank.push_back(Stitching(this,target));
    int retcheck=0;
    
    for (int safetries=0;safetries<5;safetries++) {
        for (int i=0,guard=stitchbank.size();i<guard;i++) {
            for (int n=0;n<privatebank.size() and stitchbank[i].open.size();n++) {
                for (int m=0;m<privatebank[n].open.size();m++) {
                    if (stitchbank[i].primatch(this,privatebank[n].localtypes[privatebank[n].open[m]])) {multiaffray(this,i,stitchbank,stitchbank);break;}
                }
            }
            if (stitchbank[i].open.size()==0) {
                for (int n=0,g2=privatebank.size();n<g2;n++) {
                    for (int m=0;m<privatebank[n].open.size();m++) {
                        if (stitchbank[i].primatch(this,privatebank[n].localtypes[privatebank[n].open[m]])) privateaffraybinding(this,privatebank[n],privatebank[n].open[m],stitchbank[i],privatebank);
                    }
                }
            }
        }
        
        cullobsolete(stitchbank);
        cullobsolete(privatebank);
        for (;retcheck<privatebank.size();retcheck++) {
        
            if (privatebank[retcheck].open.size()==0) {
                return privatebank[retcheck].hook;
            }
        }
    }
    throw;//failed to achieve goal in given number of iterations.
}
void MetaBank::cullobsolete(std::vector<Stitching>& bank) {
    for (int a=0;a<bank.size()-1;a++) {
        for (int b=0;b<bank.size();) {
            if (b==a) b++;
            if (obsoletes(this,bank[a],bank[b])) {
                bank.erase(bank.begin()+b);
                if (b<a) a--;
            } else b++;
        }
    }
}

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

bool Binding::trivbound(int k) const {
    for (int b=0;b<binds.size();b++) {
        if (binds[b].head.id==k and binds[b].body.local!=tracks.loc()) return true;
    }
    return false;
}

void decpull(Statement& ob,std::vector<int>& refd) {
    if (ob.local==1) refd.push_back(ob.id);
    for (int a=0;a<ob.ara;a++) decpull(ob.args[a],refd);
}
void decpull(Strategy& ob,std::vector<int>& refd) {
    decpull(ob.type,refd);
    for (int a=0;a<ob.ara;a++) decpull(ob.args[a],refd);
}

Stitching::Stitching(MetaBank* mb,const Strategy& muse) : localtypes(deepcopy(muse.args,muse.ara)),ara(muse.ara),hook(muse.snapshot(1)),quicktype(muse.type.deepcopy()) {calcopen(mb);}

Stitching::Stitching(MetaBank* mb,Strategy* gs,int a,const std::vector<int>& ca,const Statement& h) : localtypes(gs),ara(a),caut(ca),hook(h) {
    ParameterContext cl;
    cl.dat.push_back({mb->strategies,mb->ara});
    cl.dat.push_back({gs,a});
    if (hook.id==-1) quicktype=Statement(-1,0);
    else {
        Strategy ga = cl.generateType(hook);
        quicktype = ga.type.deepcopy();
        ga.cleanup();
    }
    calcopen(mb);
}

void Stitching::calcopen(MetaBank* mb) {
    
//    ParameterContext checker;
//    checker.dat.push_back(std::pair<Strategy*,int>(mb->strategies,mb->ara));
//    checker.dat.push_back(std::pair<Strategy*,int>(localtypes,ara));
//    for (int l=0;l<ara;l++) localtypes[l].loosecheck(checker);
    
    
    
    for (int a=0;a<ara;a++) {
        bool found=false;
        for (int g=0;g<caut.size();g++) if (caut[g]==a) found=true;
        if (!found) open.push_back(a);
    }
    std::vector<int> instlinks;
    decpull(quicktype,instlinks);
    for (int a=0;a<open.size();a++) decpull(localtypes[open[a]],instlinks);
    for (int y=0;y<instlinks.size();y++) {
        for (int a=0;a<open.size();) {
            if (open[a]==instlinks[y]) open.erase(open.begin()+a);
            else a++;
        }
    }
    std::vector<int>* links = new std::vector<int>[ara];
    bool* ref = new bool[ara];
    for (int a=0;a<ara;a++) {decpull(localtypes[a],links[a]);ref[a]=false;}
    for (int z=0;z<ara;z++) for (int y=0;y<links[z].size();y++) ref[links[z][y]]=true;
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
        for (int a=0;a<open.size();a++) if (ref[open[a]] and links[open[a]].size()==0) {open.erase(open.begin()+a);hit=true;}
    }
    delete[] links;
    delete[] ref;
//    std::cout<<"-=-=-=-=-=-=-==-=\n[";
//    for (int v=0;v<ara;v++)std::cout<<localtypes[v].tostring()<<"|";std::cout<<"]"<<quicktype.tostring()<<"\n";
//    for (int h=0;h<open.size();h++) std::cout<<open[h]<<",";std::cout<<"\n";
    
    
    ParameterContext checker;
    checker.dat.push_back({mb->strategies,mb->ara});
    checker.dat.push_back({localtypes,ara});
    for (int l=0;l<ara;l++) localtypes[l].loosecheck(checker);
    if (hook.id>=0) hook.loosecheck(checker);
    for (int j=0;j<caut.size();j++)if (caut[j]>=ara)throw;
}

void generatePartialA(Statement**,int,int,Strategy*,Statement*,std::vector<Statement>&,std::vector<Statement*>*,int);


void boilBinding(MetaBank* mb,std::vector<Stitching>& out,Binding& bin,const std::vector<int>& caut,const Statement& khook) {
    Statement hook = khook.deepcopy();
    while (bin.binds.size()) {
        if (bin.binds[0].body.local==1) {
            std::vector<Binding> buffer;
            std::vector<StitchMPlexr> car;
            car.push_back(StitchMPlexr(bin));
            car[0].center.binds[0].cleanup();
            car[0].center.binds.erase(car[0].center.binds.begin());
            for (int a=0;a<bin.binds[0].head.ara;a++) {
                Statement circref = bin.binds[0].body.deepcopy();
                std::vector<Statement> gather;
                Statement* hack = bin.binds[0].head.args+a;
                generatePartialA(&hack,1,circref.ara,bin.localtypes[bin.binds[0].body.id].args,circref.args,gather,0,1);
                circref.cleanup();
                for (int i=0;i<gather.size();i++) {
                    for (int f=0,limit=car.size();f<limit;f++) {
                        car.push_back(car[f]);
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].head.id].args[a].type,bin.localtypes[bin.binds[0].body.id].args,circref.ara);
                        car[car.size()-1].headargs.push_back({a,bod});
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].body.id]);//conan is missing a step.
                        Statement incurer = bin.localtypes[bin.binds[0].head.id].args[a].snapshot(2);
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
                        Statement bod = car[car.size()-1].center.integrate(gather[i],bin.localtypes[bin.binds[0].body.id].args[b].type,bin.localtypes[bin.binds[0].head.id].args,circref.ara);
                        car[car.size()-1].bodyargs.push_back({b,bod});
                        
                        ParameterContext conan = car[car.size()-1].center.tracks.append(bin.localtypes[bin.binds[0].head.id]);
                        Statement incurer = bin.localtypes[bin.binds[0].body.id].args[b].snapshot(2);
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
                for (int i=0;i<dis.size();i++) boilBinding(mb,out,dis[i],caut,hook);
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
            int hid = bin.binds[0].head.id;
            Statement bode = bin.binds[0].body.deepcopy();
            bin.binds[0].cleanup();
            bin.binds.erase(bin.binds.begin());
            applyshort(bin,hid,bode);
            hook.inplace_sub(hid,1,bode,2);
        }
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
    hook.apply(1,rmap);
    bool valid=true;
    for (int v=0;v<rmap.size();v++) if (uprefs(dgr[v].type,v)) {valid=false;break;}
    if (valid) out.push_back(Stitching(mb,dgr,rmap.size(),ncaut,hook));
    else {
        hook.cleanup();
        for (int h=0;h<rmap.size();h++) dgr[h].cleanup();
        delete[] dgr;
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
    
    
//    Strategy* simstrats = new Strategy[2];
//    simstrats[0] = Strategy(Statement(0,0),0,1);
//    simstrats[1] = Strategy(Statement(0,1),1,1);
//    Binding aleph(this,simstrats,2);
    
//    for (int g=0;g<ara;g++) stitchbank.push_back(Stitching(this,strategies[g]));
    
//    cullobsolete(stitchbank);
    
//    offlineLearningStep();
    
}












