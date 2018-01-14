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
        int ii=0;
        for (auto it=remap.begin();it!=remap.end();it++) {
            if (it->first.first>=params.dat[it->first.second-1].second) throw;
            int lloc = params.dat[it->first.second-1].first[it->first.first].local;
            trash[ii++] = params.dat[it->first.second-1].first[it->first.first].bring_depth(lloc,loc+1-lloc);
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
Statement godtrans(Statement a,Statement expected,int loc,std::vector<Strategy>& types,ParameterContext& params) {
    if (a.local==loc) {
        Strategy nloc = Strategy(expected,types.size(),loc);
        Statement nstat = Statement(types.size(),loc);
        nstat.ara=nloc.ara=params.index(loc);
        nloc.args=params.compress(loc);
        nstat.args=params.artifact(loc);
        types.push_back(nloc);
        return nstat;
    } else {
        Statement res = Statement(a.id,a.local,a.ara);
        for (int u=0;u<a.ara;u++) {//now i understand
            Strategy subcalctype = params.generateTypeSection(res,u);
            ParameterContext nn = params.append(subcalctype);
            res.args[u] = godtrans(a.args[u],subcalctype.type,loc,types,nn);
        }
        return res;
    }
}
void separatePartials(Statement* a,std::vector<int> b,Strategy head,int loc,int depth,std::vector<std::vector<int>>& out) {
    bool ide=true;
    for (int g=0;g<b.size();g++) if (a[g].local!=loc+depth+1) ide=false;
    std::vector<std::pair<std::vector<int>,Strategy>> ws;
    if (depth==0) ws.push_back(std::pair<std::vector<int>,Strategy>(b,head));
    else if (ide) {
        for (int k=0;k<head.ara;k++) {
            std::vector<int> w;
            for (int g=0;g<b.size();g++) if (a[g].id==k) w.push_back(b[g]);
            if (w.size()) ws.push_back(std::pair<std::vector<int>,Strategy>(w,head.args[k]));
        }
    } else out.push_back(b);
    for (int o=0;o<ws.size();o++) {
        std::vector<std::vector<int>> mplex1;
        std::vector<std::vector<int>> mplex2;
        mplex1.push_back(ws[o].first);
        bool c=true;
        #define MP1 (c?mplex1:mplex2)
        #define MP2 (c?mplex2:mplex1)
        for (int j=0;j<ws[o].second.ara;j++) {
            for (int r=0;r<MP1.size();r++) {
                Statement* mul = new Statement[MP1[r].size()];
                for (int g=0,s=0;s<MP1[r].size();) {
                    if (b[g]==MP1[r][s]) mul[s++]=a[g].args[j];
                    else g++;
                }
                separatePartials(mul,MP1[r],ws[o].second.args[j],loc,depth+1,MP2);
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
Statement generateFormat(Statement* a,int fibers,ParameterContext params,Strategy expected,int loc,std::vector<Strategy>& append) {
    bool ide=true;
    for (int g=0;g<fibers;g++) if (a[g].local!=params.loc()) ide=false;
    if (ide or params.loc()==loc+1) {
        Statement res = Statement(a[0].id,a[0].local,a[0].ara);
        Statement* nn;
        for (int j=0;j<res.ara;j++) {
            nn = new Statement[fibers];
            for (int h=0;h<fibers;h++) nn[h] = a[h].args[j];
            Strategy jf = params.generateTypeSection(res,j);
            ParameterContext nncon = params.append(jf);
            res.args[j] = generateFormat(nn,fibers,nncon,jf,loc,append);
            delete[] nn;
        }
        return res;
    } else {
        append.push_back(expected.bring_depth(expected.local,loc+1-expected.local));
        return intersperse(expected,loc+1,params.loc()-1);
    }
}
void generatePartial(Statement* a,int fibers,int ara,Strategy* itinerary,std::vector<Statement>& statout,int loc) {
    bool n_loc=false,d_eli=true;
    int gi=-1,gl=-1,ga=-1;
    for (int g=0;g<fibers;g++) {
        if (a[g].local==loc+1) {d_eli=false;n_loc=true;}
        else if (a[g].local!=loc) {
            if (!n_loc) {gi=a[g].id;gl=a[g].local;ga=a[g].ara;}
            else if (gi!=a[g].id or gl!=a[g].local) {d_eli=false;}
            n_loc=true;
        }
    }
    if (!n_loc) {statout.push_back(Statement(-1,loc,0));return;}
    for (int i=0;i<ara;i++) {
        Statement nxt = Statement(i,loc+1,itinerary[i].ara);
        for (int q=0;q<itinerary[i].ara;q++) {
            nxt.args[q] = Statement(-1,loc,0);
        }
        statout.push_back(nxt);
    }
    if (d_eli) {
        std::vector<std::vector<Statement>> cartesian;
        Statement* nnr;
        for (int q=0;q<ga;q++) {
            nnr = new Statement[fibers];
            for (int o=0;o<fibers;o++) {
                if (a[o].local==loc) nnr[o]=a[o];
                else nnr[o]=a[o].args[q];
            }
            cartesian.push_back(std::vector<Statement>());
            generatePartial(nnr,fibers,ara,itinerary,cartesian[q],loc);
            delete nnr;
        }
        int cmax = 1;
        for (int v=0;v<ga;v++) cmax*=cartesian[v].size();
        for (int c=0;c<cmax;c++) {
            Statement nn = Statement(gi,gl,ga);
            for (int v=0,ca=c;v<ga;v++) {
                nn.args[v] = cartesian[v][ca%cartesian[v].size()].deepcopy();
                ca/=cartesian[v].size();
            }
            statout.push_back(nn);
        }
        for (int v=0;v<ga;v++) {
            for (int y=0;y<cartesian[v].size();y++) {
                cartesian[v][y].cleanup();
            }
        }
    }
}
bool Statement::contains(int i,int loc) {
    if (id==i and local==loc) return true;
    for (int u=0;u<ara;u++) if (args[u].contains(i,loc)) return true;
    return false;
}
bool Binding::simplify() {
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
            u=0;
        }
    }
    return true;
}
void Binding::divide(std::vector<Binding>& list,int tabs) {
    if (!simplify()) return;
    std::vector<std::vector<SingleBind>> cartesian;
    ParameterContext tplusempty = tracks;
    tplusempty.dat.push_back(std::pair<Strategy*,int>(0,0));
    for (int x=0;x<ara;x++) {
        if (localtypes[x].ara>0) {
            std::vector<SingleBind> mab;
            for (int y=0;y<binds.size();y++) {
                if (binds[y].head.id==x and binds[y].body.local!=tracks.loc()) {
                    mab.push_back(binds[y]);
                }
            }
            if (mab.size()) {
                std::vector<int> in_b;
                std::vector<std::vector<int>> out_b;
                Statement* in_a = new Statement[mab.size()];
                for (int o=0;o<mab.size();o++) {
                    in_b.push_back(o);
                    in_a[o] = mab[o].head;
                }
                separatePartials(in_a,in_b,localtypes[x],tracks.loc(),0,out_b);
                delete[] in_a;
                for (int h=0;h<out_b.size();h++) {
                    Statement* mul1 = new Statement[out_b[h].size()];
                    Statement* mul2 = new Statement[out_b[h].size()];
                    for (int s=0;s<out_b[h].size();s++) {
                        mul1[s]=mab[out_b[h][s]].head;
                        mul2[s]=mab[out_b[h][s]].body;
                    }
                    std::vector<Strategy> newitin;
                    std::vector<Statement> gather;
                    Statement format = generateFormat(mul1,out_b[h].size(),tplusempty,Strategy(),tracks.loc(),newitin);
                    bool valid=true;
                    for (int ha=0;ha<binds.size();ha++) {
                        if (judgemental_eq(binds[ha].head,format)) valid=false;
                    }
                    if (!valid) continue;
                    cartesian.push_back(std::vector<SingleBind>());
                    Strategy* itinerary = new Strategy[newitin.size()];
                    for (int j=0;j<newitin.size();j++) itinerary[j] = newitin[j];
                    generatePartial(mul2,out_b[h].size(),newitin.size(),itinerary,gather,tracks.loc());
                    for (int v=0;v<gather.size();v++) {
                        cartesian[cartesian.size()-1].push_back(SingleBind(format,gather[v],itinerary,newitin.size()));
                    }
                    delete[] mul1;
                    delete[] mul2;
                }
            }
        }
    }
    if (!cartesian.size()) {
        list.push_back(*this);
        return;
    }
    int cmax = 1;
    for (int v=0;v<cartesian.size();v++) cmax*=cartesian[v].size();
    for (int c=0;c<cmax;c++) {
        std::vector<Strategy> stratbuffer;
        for (int w=0;w<ara;w++) stratbuffer.push_back(localtypes[w]);
        std::vector<SingleBind> soapbinds = binds;
        for (int v=0,ca=c;v<cartesian.size();v++) {
            #define CAC cartesian[v][ca%cartesian[v].size()]
            ParameterContext ncon = tracks.append(CAC.itinerary,CAC.ara);
            //technically the expected type is ncon.generateType(CAC.head).type, but it would never be needed on the first step anyway.
            soapbinds.push_back(SingleBind(CAC.head,godtrans(CAC.body,Statement(),tracks.loc(),stratbuffer,ncon),CAC.itinerary,CAC.ara));
            ca/=cartesian[v].size();
            #undef CAC
        }
        bool valid=true;
        Binding soap = Binding(tracks,stratbuffer,soapbinds,valid);
        if (valid) soap.divide(list,tabs>=0?tabs+1:-1);
    }
}




