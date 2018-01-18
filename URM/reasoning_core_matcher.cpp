//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

SingleBind emplace(Statement head,Statement body,int loc,ParameterContext& params) {
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
//    std::string placing = head.tostring()+"  "+body.tostring();
    Statement modhead = head.depth_push(loc+1,1);
    Statement modbody = body.depth_push(loc+1,1);
    modhead.clip_upperbound(params.loc()+2,loc,remap,mappoint);
    modhead = modhead.paste_upperbound(params.loc()+2,remap,loc).depth_push(loc+2,loc+1-params.loc()-1);
    modbody = modbody.paste_upperbound(params.loc()+2,remap,loc).depth_push(loc+2,loc+1-params.loc()-1);
//    std::string nplacing = modhead.tostring()+"  "+modbody.tostring();
    Strategy* trash = 0;
    int sz=-1;
    if (modhead.is_valid() and modbody.is_valid()) {
        trash = new Strategy[remap.size()];
        sz=remap.size();
        for (auto it=remap.begin();it!=remap.end();it++) {
            if (it->first.first>=params.dat[it->first.second-1].second) throw;
            int lloc = params.dat[it->first.second-1].first[it->first.first].local;
//            throw;
            trash[it->second] = params.dat[it->first.second-1].first[it->first.first].bring_depth(lloc,loc+1-lloc);//this shit
            trash[it->second].id=it->second;//SUSPICIOUS
        }
    }
    return SingleBind(modhead,modbody,trash,sz);
}
void Binding::insert(SingleBind c) {
    for (int j=0;j<binds.size();j++) {
        if (judgemental_eq(c.head,binds[j].head) and judgemental_eq(c.body,binds[j].body)) return;
    }
    binds.push_back(c);
}

bool Binding::crossdecomposeverbal(Statement left,Statement right,ParameterContext& params,int tabs) {
    for (int j=0;j<tabs;j++) std::cout<<"\t";
    std::cout<<"Cross-Comparing: "<<left.tostring()<<" and "<<right.tostring()<<"\n";
    return crossdecompose(left,right,params);

//    Strategy ltype = params.generateType(left);
//    Strategy rtype = params.generateType(right);
//    if (!decomposeverbal(ltype.type,rtype.type,params,tabs)) return false;
//    ltype.cleanup();
//    rtype.cleanup();
//    return decomposeverbal(left,right,params,tabs);
}
bool Binding::decomposeverbal(Statement left,Statement right,ParameterContext& params,int tabs) {
    for (int j=0;j<tabs;j++) std::cout<<"\t";
    std::cout<<"Comparing: "<<left.tostring()<<" and "<<right.tostring()<<"\n";
    return decompose(left,right,params);
}
bool Binding::crossdecompose(Statement left,Statement right,ParameterContext& params) {
    Strategy ltype = params.generateType(left);
    Strategy rtype = params.generateType(right);
    if (!decompose(ltype.type,rtype.type,params)) return false;
    ltype.cleanup();
    rtype.cleanup();
    return decompose(left,right,params);
}
bool Binding::decompose(Statement left,Statement right,ParameterContext& params) {
    if ((left.local==tracks.loc() and left.id==-1) or (right.local==tracks.loc() and right.id==-1)) return true;
    if (left.local==tracks.loc() or right.local==tracks.loc()) {
//        if (left.id==-1 or right.id==-1) throw;
        if (judgemental_eq(left,right)) return true;
//        Strategy ltype = params.generateType(left);
//        Strategy rtype = params.generateType(right);
//        if (!decompose(ltype.type,rtype.type,params)) return false;
//        ltype.cleanup();
//        rtype.cleanup();
//        std::string comparar = left.tostring()+" "+right.tostring();
//        std::cout<<"-=-=--=-=--==--\n"<<tostring(1);
        if (left.local==tracks.loc() and right.local==tracks.loc()) {
            SingleBind a = emplace(left,right,tracks.loc(),params);
            SingleBind b = emplace(right,left,tracks.loc(),params);
            if (a.ara==-1 or b.ara==-1) return false;
            insert(a);
        } else if (right.local!=tracks.loc()) {
            SingleBind c = emplace(left,right,tracks.loc(),params);
            if (c.ara==-1) return false;
            insert(c);
        } else if (left.local!=tracks.loc()) {
            SingleBind c = emplace(right,left,tracks.loc(),params);
            if (c.ara==-1) return false;
            insert(c);
        }
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
bool Binding::typebind(Statement a,Statement type,ParameterContext& params) {
    if (a.local==0 and a.ara==0) return true;
    Strategy calctype = params.generateType(a);
    typebind(type,Statement(0,0),params);
    if (!decompose(calctype.type,type,params)) return false;
    if (calctype.ara!=a.ara) throw;
    for (int u=0;u<a.ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        typebind(a.args[u],calctype.args[u].type,nn);
    }
    return true;
}
Strategy ParameterContext::morph(int local,Strategy a,int recur) {
    Strategy res = Strategy(morph(local,a.type,a.local-recur),a.id,local+1+recur,a.ara);
    for (int j=0;j<a.ara;j++) {
        res.args[j] = morph(local,a.args[j],recur+1);
    }
    return res;
}
Statement ParameterContext::morph(int local,Statement a,int limit) {
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
Statement intersperse(Strategy a,int l1,int l2) {
    Statement res = Statement(a.id,l1,a.ara);
    for (int k=0;k<a.ara;k++) {
        res.args[k] = intersperse(a.args[k],l2+1,l1+1);
    }
    return res;
}
Statement* ParameterContext::artifact(int local) {
    Statement* end = new Statement[index(local)];
    int k=0;
    for (int j=local+1;j<dat.size();j++) {
        for (int h=0;h<dat[j].second;h++) {
            end[k] = intersperse(dat[j].first[h],j,loc());
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
Statement generateLocal(ParameterContext& params,Statement expected,int loc,int place,Strategy* types) {
    Strategy nloc = Strategy(expected,place,loc);
    Statement nstat = Statement(place,loc);
    nstat.ara=nloc.ara=params.index(loc);
    nloc.args=params.compress(loc);
    nstat.args=params.artifact(loc);
//    std::cout<<nloc.tostring()<<"<--="<<place<<"\n";
    
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
Statement godtrans(Statement a,Statement expected,int loc,int& place,Strategy* types,ParameterContext& params) {
    if (a.local==loc) {
        return generateLocal(params,expected,loc,place++,types);
    } else {
        Statement res = Statement(a.id,a.local,a.ara);
        for (int u=0;u<a.ara;u++) {//now i understand
            Strategy subcalctype = params.generateTypeSection(res,u);
            ParameterContext nn = params.append(subcalctype);
            res.args[u] = godtrans(a.args[u],subcalctype.type,loc,place,types,nn);
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
    
            Strategy jf = params.generateTypeSection(res,j);
            ParameterContext nncon = params.append(jf);
//            std::cout<<j<<"\n";
            res.args[j] = generateFormat(nn,fibers,nncon,jf.type,loc,place,append,circuits);
            
            delete[] nn;
        }
        return res;
    } else {
        for (int h=0;h<fibers;h++) circuits[place*fibers+h]=*a[h];
        return generateLocal(params,exptype,loc+1,place++,append);
    }
}

//void generatePartialA(Statement** a,int fibers,int ara,Strategy* itinerary,Statement* circuits,std::vector<Statement>& statout,int loc) {
//    bool n_loc=false,d_eli=true;
//    int gi=-1,gl=-1,ga=-1;
//    for (int g=0;g<fibers;g++) {
//        if (a[g]->local==loc+1) {d_eli=false;n_loc=true;}
//        else if (a[g]->local!=loc) {
//            if (!n_loc) {gi=a[g]->id;gl=a[g]->local;ga=a[g]->ara;}
//            else if (gi!=a[g]->id or gl!=a[g]->local) {d_eli=false;}
//            n_loc=true;
//        }
//    }
//    if (!n_loc) {statout.push_back(Statement(-1,loc,0));return;}
//    for (int i=0;i<ara;i++) {
//        bool valid=true;
//        for (int g=0;g<fibers;g++) if (!lazy_judgemental_eq(circuits[g+fibers*i],*(a[g]),loc,loc+1)) valid=false;
//        if (valid) {
//            Statement nxt = Statement(i,loc+1,itinerary[i].ara);
//            for (int q=0;q<itinerary[i].ara;q++) {
//                nxt.args[q] = Statement(-1,loc,0);
//            }
//            statout.push_back(nxt);
//        }
//    }
//    if (d_eli) {
//        std::vector<std::vector<Statement>> cartesian;
//        std::vector<std::vector<Binding>> bindesian;
//        Statement** nnr;
//        for (int q=0;q<ga;q++) {
//            nnr = new Statement*[fibers];
//            for (int o=0;o<fibers;o++) {
//                if (a[o]->local==loc) nnr[o]=a[o];
//                else nnr[o]=&(a[o]->args[q]);
//            }
//            cartesian.push_back(std::vector<Statement>());
//            generatePartialA(nnr,fibers,ara,itinerary,circuits,cartesian[q],loc);
//            delete nnr;
//        }
//        int cmax = 1;
//        for (int v=0;v<ga;v++) cmax*=cartesian[v].size();
//        for (int c=0;c<cmax;c++) {
//            Statement nn = Statement(gi,gl,ga);
//            for (int v=0,ca=c;v<ga;v++) {
//                nn.args[v] = cartesian[v][ca%cartesian[v].size()].deepcopy();
//                ca/=cartesian[v].size();
//            }
//            statout.push_back(nn);
//        }
//        for (int v=0;v<ga;v++) {
//            for (int y=0;y<cartesian[v].size();y++) {
//                cartesian[v][y].cleanup();
//            }
//        }
//    }
//}
void generatePartialB(Statement** a,int fibers,int ara,Strategy* itinerary,Statement* circuits,ParameterContext& params,Statement exptype,std::vector<Statement>& statout,std::vector<Binding>* bindout,Binding& ref,int loc) {
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
        if (bindout) bindout->push_back(ref);
        statout.push_back(Statement(-1,loc,0));
        return;
    }
    for (int i=0;i<ara;i++) {
        bool valid=true;
        for (int g=0;g<fibers;g++) if (!lazy_judgemental_eq(circuits[g+fibers*i],*(a[g]),loc,loc+1)) valid=false;
        if (valid) {
        
            Statement nxt = Statement(i,loc+1,itinerary[i].ara);
            for (int q=0;q<itinerary[i].ara;q++) {
                nxt.args[q] = Statement(-1,loc,0);
            }
            Binding lift = ref;
            if (lift.decompose(params.generateType(nxt).type,exptype,params)) {
                if (bindout) bindout->push_back(lift);
                statout.push_back(nxt);
            }
        }
    }
    if (d_eli) {
        std::vector<Statement*> cartesian1;
        std::vector<Binding> bindesian1;
        std::vector<Statement*> cartesian2;
        std::vector<Binding> bindesian2;
        Statement** nnr;
        cartesian1.push_back(0);
        bindesian1.push_back(ref);
        bool c=true;
        #define CAR1 (c?cartesian1:cartesian2)
        #define CAR2 (c?cartesian2:cartesian1)
        #define BCAR1 (c?bindesian1:bindesian2)
        #define BCAR2 (c?bindesian2:bindesian1)
        for (int q=0;q<ga;q++) {
            for (int h=0;h<CAR1.size();h++) {
                std::vector<Statement> buffer;
                nnr = new Statement*[fibers];
                for (int o=0;o<fibers;o++) {
                    if (a[o]->local==loc) nnr[o]=a[o];
                    else nnr[o]=&(a[o]->args[q]);
                }
                Statement ggg = Statement(gi,gl);
                ggg.args = CAR1[h];ggg.ara=q;
                Strategy nnn = params.generateTypeSection(ggg,q);
                ParameterContext nnnn = params.append(nnn);
                
                generatePartialB(nnr,fibers,ara,itinerary,circuits,nnnn,nnn.type,buffer,&BCAR2,BCAR1[h],loc);
                for (int y=0;y<buffer.size();y++) {
                    Statement* ng = new Statement[q+1];
                    for (int x=0;x<q;x++) ng[x]=CAR1[h][x];
                    ng[q] = buffer[y];
                    CAR2.push_back(ng);
                }
                delete nnr;
            }
            CAR1.clear();
            c=!c;
        }
        for (int h=0;h<CAR1.size();h++) {
            Statement ggg = Statement(gi,gl);
            ggg.args = CAR1[h];ggg.ara=ga;
            if (BCAR1[h].simplify()) {
                if (bindout) bindout->push_back(BCAR1[h]);
                statout.push_back(ggg);
            }
        }
        #undef CAR1
        #undef CAR2
        #undef BCAR1
        #undef BCAR2
    }
}
bool lazy_judgemental_eq(Statement a,Statement b,int loc,int limit) {
    if (a.local==loc or a.local>limit or b.local==loc or b.local>limit) return true;
    if (a.id!=b.id or a.local!=b.local) return false;
    for (int u=0;u<a.ara;u++) {
        if (!lazy_judgemental_eq(a.args[u],b.args[u],loc,limit)) return false;
    }
    return true;
}
//Statement lazy_template_create(Statement a,Statement b,int loc) {
//    if (a.local==loc or a.local>loc+1 or b.local==loc or b.local>loc+1 or a.id!=b.id or a.local!=b.local) return Statement(-1,loc);
//    Statement res = Statement(a.id,a.local,a.ara);
//    for (int u=0;u<a.ara;u++) {
//        res.args[u] = lazy_template_create(a.args[u],b.args[u],loc);
//    }
//    return res;
//}
//void shortCircuitList(Statement format,Statement** head,int strand,int loc,Statement* circuits) {
//    if (format.local==loc+1) {
//        for (int h=0;h<strand;h++) {
//            circuits[format.id*strand+h]=*head[h];
//        }
//    }
//    Statement** nnr;
//    for (int q=0;q<format.ara;q++) {
//        nnr = new Statement*[strand];
//        for (int o=0;o<strand;o++) {
//            nnr[o]=&head[o]->args[q];
//        }
//        shortCircuitList(format.args[q],nnr,strand,loc,circuits);
//        delete nnr;
//    }
//}
bool Statement::contains(int i,int loc) {
    if (id==i and local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(i,loc)) return true;
    return false;
}
bool Binding::simplify() {
//    int check=0;
    for (int u=0;u<binds.size();u++) {
        if (binds[u].head.local!=tracks.loc()) throw;
        ParameterContext tplusi = tracks.append(binds[u].itinerary,binds[u].ara);
        if (binds[u].body.local!=tracks.loc() and binds[u].body.contains(binds[u].head.id,tracks.loc())) return false;
        Statement nbod = binds[u].body.substitute_single(*this,tplusi);
        Statement nhead = binds[u].body.local==tracks.loc()?
        binds[u].head.substitute_single(*this,tplusi):
        binds[u].head.simp_substitute(*this,tplusi,u);
        if (!judgemental_eq(nhead,binds[u].head) or !judgemental_eq(nbod,binds[u].body)) {
            binds.erase(binds.begin()+u);
            if (!decompose(nhead,nbod,tplusi)) return false;
//            std::cout<<"checkpoint "<<check++<<"\n";
            u=0;
        }
    }
    return true;
}

void Binding::divide(std::vector<Binding>& list,int tabs) {
    if (!simplify()) return;
//    std::cout<<"checkpoint "<<tabs<<"\n";
    std::vector<std::vector<SingleBind>> cartesian;
    ParameterContext tplusempty = tracks;
    tplusempty.dat.push_back(std::pair<Strategy*,int>(0,0));
//    for (int x=0;x<ara;x++) {
//        if (localtypes[x].ara>0) {
//    std::vector<SingleBind> mab;
//    for (int y=0;y<binds.size();y++) {
//        if (binds[y].head.id==x and binds[y].body.local!=tracks.loc()) {
//            mab.push_back(binds[y]);
//        }
//    }
//    if (mab.size()) {
    std::vector<int> in_b;
    std::vector<std::vector<int>> out_b;
    for (int o=0;o<binds.size();o++) {
        if (binds[o].body.local!=tracks.loc()) {
            bool eliminate=true;
            for (int h=0;h<binds[o].head.ara;h++) {
                if (binds[o].head.args[h].local!=tracks.loc()+1 or binds[o].head.args[h].id!=h) eliminate=false;
            }
            if (eliminate) continue;
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
//        std::vector<Strategy> newitin;
        std::vector<Statement> gather;
        int itinerarysize = primeFormat(mul1,out_b[h].size(),tracks.loc(),true);
        
        Statement* circ     = new Statement[itinerarysize*out_b[h].size()];
        Strategy* itinerary = new Strategy[itinerarysize];
        
        int pl=0;
//        ParameterContext inhead
        Statement format = generateFormat(mul1,out_b[h].size(),tplusempty,Statement(),tracks.loc(),pl,itinerary,circ);
//        if (pl!=itinerarysize) throw;
//        for (int uu=0;uu<itinerarysize;uu++) std::cout<<itinerary[uu].tostringheavy()<<"\n";
        
        bool valid=true;
        for (int ha=0;ha<binds.size();ha++) {
            if (judgemental_eq(binds[ha].head,format)) valid=false;
        }
        if (!valid) continue;
        
        cartesian.push_back(std::vector<SingleBind>());
        
//        bool heavyalgorithm=false;
////        throw;
//        for (int j=0;j<itinerarysize;j++) {
//            if (itinerary[j].type.local==tracks.loc()) heavyalgorithm=true;
////            for (int hr=0;hr<out_b[h].size();hr++) {
//////                if (circ[j*out_b[h].size()+hr].local==tracks.loc()) heavyalgorithm=true;
////            }
//        }
//        if (!heavyalgorithm) throw;
        ParameterContext extension = tracks.append(itinerary,itinerarysize);
        Strategy ltype = extension.generateType(format);
        generatePartialB(mul2,out_b[h].size(),itinerarysize,itinerary,circ,extension,ltype.type,gather,0,*this,tracks.loc());
//        } else {
//            std::cout<<"non-local division.\n";
//            generatePartialA(mul2,out_b[h].size(),itinerarysize,itinerary,circ,gather,tracks.loc());
//        }
        
        for (int v=0;v<gather.size();v++) {
            cartesian[cartesian.size()-1].push_back(SingleBind(format,gather[v],itinerary,itinerarysize));
        }
        delete[] mul1;
        delete[] mul2;
    }
//    }
//        }
//    }
    
    if (!cartesian.size()) {
        list.push_back(*this);
        return;
    }
    int cmax = 1;
    for (int v=0;v<cartesian.size();v++) cmax*=cartesian[v].size();
    
    for (int c=0;c<cmax;c++) {
//        std::vector<Strategy> stratbuffer;

        std::vector<SingleBind> soapbinds = binds;
        int sm=ara;
        #define CAC cartesian[v][ca%cartesian[v].size()]
        for (int v=0,ca=c;v<cartesian.size();v++) {sm+=primeTrans(CAC.body,tracks.loc());}
        Strategy* nstrats = new Strategy[sm];
        for (int w=0;w<ara;w++) nstrats[w] = localtypes[w];
        int pl=ara;
        for (int v=0,ca=c;v<cartesian.size();v++) {
            ParameterContext ncon = tracks.append(CAC.itinerary,CAC.ara);
            //technically the expected type is ncon.generateType(CAC.head).type, but it would never be needed on the first step anyway.
            soapbinds.push_back(SingleBind(CAC.head,godtrans(CAC.body,Statement(),tracks.loc(),pl,nstrats,ncon),CAC.itinerary,CAC.ara));
            ca/=cartesian[v].size();
        }
            #undef CAC
        bool valid=true;
        Binding soap = Binding(tracks,nstrats,sm,soapbinds);
        for (int u=0;u<soapbinds.size();u++) {
            ParameterContext conan = soap.tracks.append(soapbinds[u].itinerary,soapbinds[u].ara);
            Strategy calctype = conan.generateType(soapbinds[u].head);
            if (!soap.typebind(soapbinds[u].body,calctype.type,conan)) valid=false;
            soap.binds.push_back(soapbinds[u]);
        }
        if (valid) soap.divide(list,tabs>=0?tabs+1:-1);
    }
}




