//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

bool Statement::is_complete(int loc) {
//    if (this==Statement::gap) return false;
    if (id==-1 and local==loc) return false;
    for (int u=0;u<ara;u++) {
        if (!args[u].is_complete(loc)) return false;
    }
    return true;
}

//bool Binding::substitute() {
//
//    std::vector<std::pair<Statement*,Statement*>> purify;
//    bool changeflag=false;
//    for (int u=0;u<binds.size();u++) {
//        purify.push_back(std::pair<Statement*,Statement*>(
//            binds[u].first->lam_substitute(this,2,1,u,changeflag),
//            binds[u].second->lam_substitute(this,2,1,u,changeflag)
//        ));
//    }
//    
////    std::cout<<"purify:\n"<<tostringheavy()<<"\n";
//    if (changeflag) {
//        binds.clear();
//        for (int u=0;u<purify.size();u++) {
//            if (!decompose(purify[u].first,purify[u].second,0)) return false;
//        }
//    }
////    std::cout<<"postpurify:\n"<<tostringheavy()<<"\n";
//    return true;
//}

SingleBind emplace(Statement head,Statement body,int tracksize,ParameterContext& params) {
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
    std::string placing = head.tostring()+"  "+body.tostring();
    
    
    Statement modhead = head.depth_push(tracksize,1);
    Statement modbody = body.depth_push(tracksize,1);
    modhead.clip_upperbound(params.loc()+2,tracksize+1,remap,mappoint);
    modhead = modhead.paste_upperbound(params.loc()+2,remap,tracksize).depth_push(tracksize+1,tracksize-params.loc()-1);
    modbody = modbody.paste_upperbound(params.loc()+2,remap,tracksize).depth_push(tracksize+1,tracksize-params.loc()-1);
    
    std::string nplacing = modhead.tostring()+"  "+modbody.tostring();
//    std::cout<<"\n-=-=-=-=-=-=-=-\n";
//    std::cout<<modhead->tostring()<<"\n";
//    std::cout<<modbody->tostring()<<"\n\n";
    
    Strategy* trash = 0;
    int sz=-1;
    if (modhead.is_valid() and modbody.is_valid()) {
        trash = new Strategy[remap.size()];
        sz=remap.size();
        int ii=0;
        for (auto it=remap.begin();it!=remap.end();it++) {
            if (it->first.first<params.dat[it->first.second].second) throw;
            trash[ii++] = params.dat[it->first.second].first[it->first.first].bring_depth(tracksize,tracksize-it->first.second);
        }
        
        
//        std::cout<<"BINDING: "<<modhead.tostring()<<" | "<<modbody.tostring()<<"\n";
    }
    
    
    return SingleBind(modhead,modbody,trash,sz);
}
//bool Binding::easydecompose(Statement* left,Statement* right,MetaBank* mb) {
//    std::vector<std::vector<Strategy*>*> p;
//    p.push_back(&mb->strategies);
//    p.push_back(&localtypes);
//    return decompose(left,right,0,p);
//}
void Binding::insert(SingleBind c) {
    if (partials[c.head.id].is_complete(tracks.loc()) and judgemental_eq(partials[c.head.id],c.body)) return;
    for (int j=0;j<binds.size();j++) {
        if (judgemental_eq(c.head,binds[j].head) and judgemental_eq(c.body,binds[j].body)) return;
    }
    binds.push_back(c);
}
bool Binding::decompose(Statement left,Statement right) {return decompose(left,right,tracks);}
bool Binding::decomposeverbal(Statement left,Statement right,ParameterContext& params,int tabs) {
    for (int j=0;j<tabs;j++) {
        std::cout<<"\t";
    }
    std::cout<<"Comparing: "<<left.tostring()<<" and "<<right.tostring()<<"\n";
    return decompose(left,right,params);
}
bool Binding::decompose(Statement left,Statement right,ParameterContext& params) {
//    left.constcheck(params);
//    right.constcheck(params);
    if (left.local==tracks.loc() or right.local==tracks.loc()) {
        if (left.id==-1 or right.id==-1) return true;
//        Statement ltype = left.generate_type(params);
//        Statement rtype = right.generate_type(params);
        Strategy ltype = params.generateType(left);
        Strategy rtype = params.generateType(right);
        
        if (!decompose(ltype.type,rtype.type,params)) return false;
        ltype.cleanup();
        rtype.cleanup();
        
        if (left.local==tracks.loc() and right.local==tracks.loc()) {
            SingleBind a = emplace(left,right,tracks.loc()+1,params);
            SingleBind b = emplace(right,left,tracks.loc()+1,params);
            if (a.ara==-1 or b.ara==-1) return false;
            insert(a);
        } else if (right.local!=tracks.loc()) {
            SingleBind c = emplace(left,right,tracks.loc()+1,params);
            if (c.ara==-1) return false;
            insert(c);
        } else if (left.local!=tracks.loc()) {
            SingleBind c = emplace(right,left,tracks.loc()+1,params);
            if (c.ara==-1) return false;
            insert(c);
        }
        return true;
    }
    if (left.local==right.local and left.id==right.id) {
        if (left.ara!=right.ara) throw;
        if (left.local==-1) return true;
//        if (left.id>=params[left.local].second) throw;
//        int lloc = params[left.local].first[left.id].local;
//        Strategy calctype = params[left.local].first[left.id].typechecksub(left.args,left.ara,lloc+1,(int)params.size(),1);
        Strategy calctype = params.generateType(left);
        for (int u=0;u<left.ara;u++) {
            ParameterContext continued = params.append(calctype.args[u]);
//            continued.push_back(std::pair<Strategy*,int>(calctype.args[u].args,calctype.args[u].ara));
            if (!decompose(left.args[u], right.args[u],continued)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

//Statement* trickplace(Statement* a,int target) {
//    if (a->local==target) return Statement::gap;
//    if (a->local<=0 and a->args.size()==0) {
//        return a;
//    }
//    Statement* res = new Statement(a->id,a->local);
//    for (int q=0;q<a->args.size();q++) {
//        res->args.push_back(trickplace(a->args[q],target));
//    }
//    return res;
//}

struct CartesianCarry {
    std::vector<Statement> partials;
    std::vector<Strategy> types;
    CartesianCarry(Statement* t,Strategy* g,int ara) {
        for (int a=0;a<ara;a++) {
            partials.push_back(t[a]);
            types.push_back(g[a]);
        }
    }
    CartesianCarry(std::vector<Statement> t,std::vector<Strategy> g) {
//        if (t.size()!=g.size()) throw;
        for (int a=0;a<g.size();a++) {
            types.push_back(g[a].deepcopy());
        }
        for (int a=0;a<t.size();a++) {
            partials.push_back(t[a].deepcopy());
        }
    }
};

//int idcheck(Statement* a,int target,int threshold) {
//    int max=0;
//    if (a->id>=threshold and a->local==target) {
//        max=a->id-threshold;
//    }
//    for (int u=0;u<a->args.size();u++) {
//        int t=idcheck(a->args[u],target,threshold);
//        if (t>max) max=t;
//    }
//    return max;
//}
//void idbump(Statement* a,int target,int threshold,int amt) {
//    if (a->local==target and a->id>=threshold) a->id+=amt;
//    for (int u=0;u<a->args.size();u++) {
//        idbump(a->args[u],target,threshold,amt);
//    }
//}
//Statement reduce(Statement in,int loc,int l1off,int l2off) {
//    Statement ret = Statement(in.id,in.local,in.ara);
//    if (in.local>=loc+2) {
//        ret.local-=2;
//        if (ret.local==loc)   ret.id+=l1off;
//        if (ret.local==loc+1) ret.id+=l2off;
//    }
//    for (int u=0;u<in.ara;u++) {
//        ret.args[u] = reduce(in.args[u],loc,l1off,l2off);
//    }
//    return ret;
//}
//Strategy reduce(Strategy in,int loc,int l1off,Strategy append,int pnum) {
//    Strategy ret = Strategy(reduce(in.type,loc,l1off,append.ara),in.id,in.local-2,in.local-2==loc?pnum+in.ara:in.ara);
//    
//    if (ret.local==loc) {
//        ret.id+=l1off;
//        for (int u=0;u<pnum;u++) {
//            ret.args[u] = append.args[u];
//        }
//    }
//    if (ret.local==loc+1) ret.id+=append.ara;
//    for (int u=0;u<in.ara;u++) {
//        ret.args[ret.local==loc?pnum+u:u] = reduce(in.args[u],loc,l1off,append,pnum);
//    }
//    return ret;
//}
//void idtrans(Statement& a,int target,std::vector<Strategy>& types,Strategy loctype,int loc,int pnum) {
//    int prp=0;
//    if (a.local==target) {
//        types.push_back(reduce(loctype.args[pnum].args[a.id],loc,types.size(),loctype,pnum));
//        
//        a.local=loc;
//        a.id=types.size()-1;
//        
//        Statement* oargs = a.args;
//        a.args = new Statement[pnum+a.ara];
//        for (int u=0;u<pnum;u++) {
//            a.args[u] = loctype.args[u].snapshot();
//        }
//        for (int u=0;u<a.ara;u++) {
//            a.args[pnum+u] = oargs[u];
//        }
//        a.ara+=pnum;
//        prp=pnum;
//        delete[] oargs;
//    }
//    
//}
Strategy ParameterContext::morph(int local,Strategy a,int recur) {//recur is strat depth
    Strategy res = Strategy(morph(local,a.type,a.local-recur),a.id,local+1+recur,a.ara);
    
    //limit is like 2.
    //then a new row of variables will be introduced which shall not be substituted out.
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
Statement intersperse(Strategy a,Strategy b) {
    Statement res = Statement(a.id,a.local,a.ara);
    if (a.ara!=b.ara) throw;
    for (int k=0;k<a.ara;k++) {
        res.args[k] = intersperse(b.args[k],a.args[k]);
    }
    return res;
}
Statement* ParameterContext::artifact(int local,Strategy* tra) {
    Statement* end = new Statement[index(local)];
    int k=0;
    for (int j=local+1;j<dat.size();j++) {
        for (int h=0;h<dat[j].second;h++) {
//            std::cout<<"interspersing: "<<dat[j].first[h].snapshot().tostring()<<" and "<<tra[k].snapshot().tostring()<<" result: "<<intersperse(dat[j].first[h],tra[k]).tostring()<<"\n";
            
            end[k] = intersperse(dat[j].first[h],tra[k].bring_depth(local+1,loc()-local-1));
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
void godtrans(Statement& a,int loc,std::vector<Strategy>& types,ParameterContext& params) {
    if (a.local==loc+1) {
        if (a.ara==-1) {
            int lloc = params.dat[a.local].first[a.id].local;
            Strategy ptype = params.dat[a.local].first[a.id].bring_depth(lloc+1,params.loc()-lloc);
//            int lloc = ;
            a.ara = ptype.ara;
            a.args = new Statement[a.ara];
            for (int k=0;k<a.ara;k++) {
//                for (int w=0;w<a.ara;w++) {
                Strategy nntype = ptype.args[k].typechecksub_1disp(a.args,k,params.loc()+1,1);
                ParameterContext nn = params.append(nntype);
//                }
                //[[NSTRAT = depth push k strat and sub 3->1
                //[[NSTRAT type may be required type, if not for local 2 id adjustments
                //[[append NSTRAT to params
                //[[params.compress()
                
                //[[[4]3]2]1
                Strategy nloc = Strategy(nn.morph(loc,ptype.type,ptype.local+2),types.size(),loc);
                Statement nstat = Statement(types.size(),loc);
                //add 4(transformed) givens.
                //subbed with the threes to locals.
                //fives refer to locals; become threes.
                
                
                //FINISHED: 0->0,1->1,2->itinerary,
                
                //FINISHED:  do not worry about this: 3->1 (id's strat will already have been visited)
                
                //2 givens are secretly the pre-existing parameters.
                
                nloc.ara=nn.index(loc);
                nloc.args=nn.compress(loc);//ensure params is updated with everything it needs.
                nstat.ara=nloc.ara;
                nstat.args=nn.artifact(loc,nloc.args);
                
                types.push_back(nloc);
                a.args[k] = nstat;
            }
        }
    } else {
        Strategy calctype = params.generateType(a);
        for (int u=0;u<a.ara;u++) {
            ParameterContext nn = params.append(calctype.args[u]);
            godtrans(a.args[u],loc,types,nn);
        }
    }
}
void generatePartial(Statement a,Statement partial,Strategy head,std::vector<Statement>& statout,CartesianCarry& opartials,int loc,bool& changed,ParameterContext& params) {
    if (a.local==loc) {
        if (a.id==-1) {
            statout.push_back(partial.deepcopy());
            return;
        }
        Statement jk = opartials.partials[a.id].depth_push(loc+1,loc-params.loc());
        Statement subbed = opartials.partials[a.id].substitute_level(a.args,a.ara,params.loc()+1,0);//this is unchecked.
        throw;
        generatePartial(subbed,partial,head,statout,opartials,loc,changed,params);//this screws up depth<-=--==-=-=--==--==-=--=-=-=-=-=-=-=-=-=
        subbed.cleanup();
        return;
    }
    if (partial.local==loc) {
        if (partial.id!=-1) throw;
        for (int u=0;u<head.ara;u++) {
//            statout.push_back(head.args[u].snapshot().depth_push(loc+2,params.loc()-loc-1));
            statout.push_back(Statement(u,loc+1,-1));
        }
        changed=true;
        
//        if (a->local!=loc+1) {
        Statement neg = Statement(a.id,a.local,a.ara);
        for (int y=0;y<a.ara;y++) {
            neg.args[y] = Statement(-1,loc);
        }
        generatePartial(a,neg,head,statout,opartials,loc,changed,params);
        delete[] neg.args;
//        } else throw;
    } else {
        if (partial.local==loc+1) {
            statout.push_back(partial.deepcopy());
            return;
        }
        if (a.id!=partial.id or a.local!=partial.local) return;
        if (a.ara==0) {
            statout.push_back(a.deepcopy());
            return;
        }
        Strategy calctype = params.generateType(a);
        std::vector<std::vector<Statement>> cartesian;
        for (int q=0;q<a.ara;q++) {
            ParameterContext continued = params.append(calctype.args[q]);
            cartesian.push_back(std::vector<Statement>());
            generatePartial(a.args[q],partial.args[q],head,cartesian[q],opartials,loc,changed,continued);
        }
        int cmax = 1;
        for (int v=0;v<a.ara;v++) {
            cmax*=cartesian[v].size();
        }
        for (int c=0;c<cmax;c++) {
            int ca = c;
            Statement nn = Statement(partial.id,partial.local,a.ara);
            for (int v=0;v<a.ara;v++) {
                nn.args[v] = cartesian[v][ca%cartesian[v].size()].deepcopy();
                ca/=cartesian[v].size();
            }
            statout.push_back(nn);
        }
        for (int v=0;v<a.ara;v++) {
            for (int y=0;y<cartesian[v].size();y++) {
                cartesian[v][y].cleanup();
            }
        }
    }
}
//Statement partialsub(Statement p,Binding* o,int rec,bool& changed) {
//    if ((p.local<=0 or p.id==-1) and p.ara==0) return p.deepcopy();
//    Statement res = Statement(p.id,p.local,p.ara);
//    for (int u=0;u<p.ara;u++) {
//        res.args[u] = partialsub(p.args[u],o,rec+1,changed);
//    }
//    if (p.local==o->tracks.loc()) {
//        if (p.id>=o->ara) throw;
//        if (o->partials[p.id].is_complete(o->tracks.loc())) {
//            changed=true;
//            if (p.ara!=0) throw;//keep boolean change flag.
//            return o->partials[res.id]
//            .depth_push(o->tracks.loc()+1,rec-o->tracks.loc()-1)
//            .substitute_level(res.args,res.ara,rec,0);
////            return o->partials[res.id].substitute_level(res.args,res.ara,o->tracks.loc()+1,rec,0);
//        }
//    }
//    return res;
//}
bool Binding::simplify() {
//    static int deg=0;
//    int thisdeg=deg++;
    std::vector<SingleBind> purify;
    bool changeflag=false;
//    std::cout<<"\n\n\n"<<tostring()<<"\n\n\n";
//    for (int u=0;u<ara;u++) {
//        partials[u] = partialsub(partials[u],this,tracks.loc()+2,changeflag);
//        
//    }
//    int oldbinds = binds.size();
//    std::string preserve = tostring();
    for (int u=0;u<binds.size();u++) {
        ParameterContext tplusi = tracks.append(binds[u].itinerary,binds[u].ara);
//        std::vector<std::pair<Strategy*,int>> tplusi=tracks;
//        tplusi.push_back(std::pair<Strategy*,int>(binds[u].itinerary,binds[u].ara));
        
        std::vector<Statement> s1;
        std::vector<Statement> s2;
        
        binds[u].head.substitute(this,tplusi,s1,binds[u].body.local==tracks.loc()?-1:u,changeflag);
        binds[u].body.substitute(this,tplusi,s2,-1,changeflag);
//        std::cout<<"-->"<<binds[u].head->tostring()<<"<-->"<<binds[u].body->tostring()<<"<--:\n";
        for (int x=0;x<s1.size();x++) {
            for (int y=0;y<s2.size();y++) {
//                std::cout<<"\t-->"<<s1[x].tostring()<<"<-->"<<s2[y].tostring()<<"<--:\n";
                purify.push_back(SingleBind(s1[x],s2[y],binds[u].itinerary,binds[u].ara));
            }
        }
    }
//    std::cout<<"SIMPLIFYING:\n"<<preserve<<"\n";
//    std::cout<<"TO:\n";
//    for (int j=0;j<purify.size();j++) {
//        std::cout<<purify[j].head.tostring()+"|"+purify[j].body.tostring()+"\n";
//    }
    if (changeflag) {
//        for (int h=0;h<thisdeg;h++) std::cout<<"\t";
//        std::cout<<"simplifying:\n"<<tostring(thisdeg)<<"\n";
//        std::cout<<"simplification:\n";
        binds.clear();
        for (int u=0;u<purify.size();u++) {
            ParameterContext tplusi = tracks.append(purify[u].itinerary,purify[u].ara);
            
//            std::vector<std::pair<Strategy*,int>> tplusi=tracks;
//            tplusi.push_back(std::pair<Strategy*,int>(purify[u].itinerary,purify[u].ara));
            if (!decompose(purify[u].head,purify[u].body,tplusi)) {
//                for (int h=0;h<thisdeg;h++) std::cout<<"\t";
//                std::cout<<"failure on: "<<purify[u].head.tostring()<<" | "<<purify[u].body.tostring()<<"\n\n";
//                deg--;
                return false;
            }
        }
//        std::cout<<tostring()<<"\n";
//        deg--;
        return simplify();
    }
//    for (int h=0;h<thisdeg;h++) std::cout<<"\t";
//    std::cout<<"end at:\n"<<tostring(thisdeg)<<"\n";
//    deg--;
    return true;
}
//void Binding::divide(std::vector<Binding>& list) {
//    if (!simplify()) return;
//    divide(list,true);
//}
void Binding::divide(std::vector<Binding>& list,int tabs) {
    if (!simplify()) return;
    std::cout<<tostring(tabs);
    std::vector<CartesianCarry> cartesian1;
    std::vector<CartesianCarry> cartesian2;
    bool changed=false;
    bool c1=true;
    std::vector<Statement> buffer;
    #define CAR1 (c1?cartesian1:cartesian2)
    #define CAR2 (c1?cartesian2:cartesian1)
    cartesian1.push_back(CartesianCarry(partials,localtypes,ara));
    
    for (int q=0;q<binds.size();q++) {
        ParameterContext continued = tracks.append(binds[q].itinerary,binds[q].ara);
        for (int u=0;u<CAR1.size();u++) {
            int bbinds = (int)CAR2.size();
//            std::cout<<"dissembling: "<<binds[q].body->tostring()<<" under type: "<<localtypes[binds[q].head->id]->tostring()<<" , partial: "<<CAR1[u].partials[binds[q].head->id]->tostring()<<"\n";
            if (binds[q].head.id>=ara) throw;
            generatePartial(binds[q].body,CAR1[u].partials[binds[q].head.id],localtypes[binds[q].head.id],buffer,CAR1[u],tracks.loc(),changed,continued);
            
//            for (int p=0;p<buffer.size();p++) {
//                std::cout<<buffer[p].tostring()<<"\n";
//            }
            ParameterContext ncon = tracks.append(localtypes[binds[q].head.id]);
            for (int p=0;p<buffer.size();p++) {
                std::vector<Strategy> stratbuffer = CAR1[u].types;
//                std::cout<<"godtrans for "<<buffer[p]->tostring()<<"\n";
                godtrans(buffer[p],tracks.loc(),stratbuffer,ncon);
//                std::cout<<buffer[p].tostring()<<" <---> "<<stratbuffer.size()<<"\n";
                
                CAR2.push_back(CartesianCarry(CAR1[u].partials,stratbuffer));
                CAR2[bbinds+p].partials[binds[q].head.id] = buffer[p].deepcopy();
                for (int j=CAR2[bbinds+p].partials.size();j<stratbuffer.size();j++) {
                    CAR2[bbinds+p].partials.push_back(Statement(-1,tracks.loc()));
                }
            }
            buffer.clear();
        }
        CAR1.clear();
        c1 = not c1;
        if (binds[q].body.local==tracks.loc()) {
            for (int u=0;u<CAR1.size();u++) {
                int bbinds = (int)CAR2.size();
                generatePartial(binds[q].body,CAR1[u].partials[binds[q].head.id],localtypes[binds[q].body.id],buffer,CAR1[u],tracks.loc(),changed,continued);
                ParameterContext ncon = tracks.append(localtypes[binds[q].head.id]);
                for (int p=0;p<buffer.size();p++) {
                    std::vector<Strategy> stratbuffer = CAR1[u].types;
                    godtrans(buffer[p],tracks.loc(),stratbuffer,ncon);
                    CAR2.push_back(CartesianCarry(CAR1[u].partials,stratbuffer));
                    CAR2[bbinds+p].partials[binds[q].body.id] = buffer[p].deepcopy();
                    for (int j=CAR2[bbinds+p].partials.size();j<stratbuffer.size();j++) {
                        CAR2[bbinds+p].partials.push_back(Statement(-1,tracks.loc()));
                    }
                }
                buffer.clear();
            }
            CAR1.clear();
            c1 = not c1;
        }
    }
    
    for (int u=0;u<CAR1.size();u++) {
        ParameterContext reverse = tracks;
        reverse.dat.erase(reverse.dat.begin()+reverse.loc());
        Binding soap(reverse,CAR1[u].types,CAR1[u].partials);
        for (int j=0;j<binds.size();j++) {
            Strategy* nn = new Strategy[binds[j].ara];
            for (int k=0;k<binds[j].ara;k++) {
                nn[k] = binds[j].itinerary[k].deepcopy();
            }
            soap.binds.push_back(SingleBind(binds[j].head.deepcopy(),binds[j].body.deepcopy(),nn,binds[j].ara));
        }
        bool valid=true;
        
        for (int w=0;w<soap.binds.size();w++) {
//            for (int z=0;z<tabs;z++) std::cout<<"\t";
//            std::cout<<"-=-=-=-\n";
            ParameterContext tplusi = soap.tracks.append(soap.binds[w].itinerary,soap.binds[w].ara);
//            std::cout<<soap.partials[soap.binds[w].head.id].tostring()<<"<--=-=-"<<soap.localtypes[soap.binds[w].head.id].tostring()<<"\n";
            Statement incpart = soap.partials[soap.binds[w].head.id].depth_push(tracks.loc()+1,1);
//            std::cout<<incpart.tostring()<<"\n";
            Statement compare = incpart.substitute_level(soap.binds[w].head.args,soap.binds[w].head.ara,tracks.loc()+2,0).depth_push(tracks.loc()+2,-1);
//            std::cout<<"\tCOMPARE: "<<compare.tostring()<<"\n";
            if (!soap.typebind(compare,Statement(-1,soap.tracks.loc()),tplusi)) {valid=false;break;}
//            if (soap.binds[w].head.id==4) {
//                for (int b=0;b<tabs;b++) std::cout<<"\t";
//                std::cout<<soap.binds[w].body.tostring()<<" | "<<soap.partials[soap.binds[w].head.id].tostring()<<"\n";
//            }
//                if (!soap.decomposeverbal(compare,soap.binds[w].body,tplusi,tabs)) {valid=false;break;}
//            } else {
            if (!soap.decompose(compare,soap.binds[w].body,tplusi)) {valid=false;break;}
//            }
            if (binds[w].body.local==tracks.loc()) {
                Statement incpart = soap.partials[soap.binds[w].body.id].depth_push(tracks.loc()+1,1);
                Statement compare = incpart.substitute_level(soap.binds[w].body.args,soap.binds[w].body.ara,tracks.loc()+2,0);
                if (!soap.typebind(compare,Statement(-1,soap.tracks.loc()),tplusi)) {valid=false;break;}
                if (!soap.decompose(compare,soap.binds[w].head,tplusi)) {valid=false;break;}
            }
        }
        
        if (valid) {
//            for (int y=0;y<tabs;y++) std::cout<<"\t";
//            std::cout<<"PUSHING: \n"<<soap.tostring(tabs+1)<<"\n";
            if (soap.binds.size()==binds.size() and not changed) {
                list.push_back(soap);
//                for (int y=0;y<tabs+1;y++) std::cout<<"\t";
//                std::cout<<"ACCEPTED\n";
            } else {
                soap.divide(list,tabs+1);
            }
        }
    }
    #undef CAR1
    #undef CAR2
}






bool Binding::typebind(Statement a,Statement type,ParameterContext& params) {
    if (a.local==0 and a.ara==0) return true;
    Strategy calctype = params.generateType(a);
    typebind(type,Statement(0,0),params);
    if (!decompose(calctype.type,type)) return false;
    if (calctype.ara!=a.ara) throw;
    for (int u=0;u<a.ara;u++) {
        ParameterContext nn = params.append(calctype.args[u]);
        typebind(a.args[u],calctype.args[u].type,nn);
    }
    return true;
}


