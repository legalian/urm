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
    for (int u=0;u<args.size();u++) {
        if (!args[u]->is_complete(loc)) return false;
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

SingleBind emplace(Statement* head,Statement* body,int tracksize,std::vector<std::vector<Strategy*>*>& params) {
    std::map<std::pair<int,int>,int> remap;
    int mappoint = 0;
    
    head->clip_upperbound(params.size(),tracksize,remap,mappoint);
    Statement* modhead = head->paste_upperbound(params.size(),remap,tracksize);
    Statement* modbody = body->paste_upperbound(params.size(),remap,tracksize);
    
//    std::cout<<"\n-=-=-=-=-=-=-=-\n";
//    std::cout<<modhead->tostring()<<"\n";
//    std::cout<<modbody->tostring()<<"\n\n";
    
    if (modhead!=0 and modbody!=0) {
        std::vector<Strategy*> trash;
        for (auto it=remap.begin();it!=remap.end();it++) {
            trash.push_back((*params[it->first.second])[it->first.first]->bring_depth(tracksize,tracksize-it->first.second));
        }
        return SingleBind(modhead,modbody,trash);
    }
    if (modhead!=0) modhead->cleanup();
    if (modbody!=0) modbody->cleanup();
    std::vector<Strategy*> trash;
    return SingleBind(0,0,trash);
}
//bool Binding::easydecompose(Statement* left,Statement* right,MetaBank* mb) {
//    std::vector<std::vector<Strategy*>*> p;
//    p.push_back(&mb->strategies);
//    p.push_back(&localtypes);
//    return decompose(left,right,0,p);
//}
bool Binding::decompose(Statement* left,Statement* right) {return decompose(left,right,tracks);}
bool Binding::decomposeverbal(Statement* left,Statement* right,std::vector<std::vector<Strategy*>*>& params,int tabs) {
    for (int j=0;j<tabs;j++) {
        std::cout<<"\t";
    }
    std::cout<<"Comparing: "<<left->tostring()<<" and "<<right->tostring()<<" with track "<<tracks.size()<<" and param "<<params.size()<<"\n";
    return decompose(left,right,params);
}
bool Binding::decompose(Statement* left,Statement* right,std::vector<std::vector<Strategy*>*>& params) {
    left->constcheck(params);
    right->constcheck(params);
    if (left==0 or right==0) throw;
//    if (left==Statement::gap or right==Statement::gap) return true;
    if (left->local==tracks.size()-1 or right->local==tracks.size()-1) {
        if (left->id==-1 or right->id==-1) return true;
        Statement* ltype = left->generate_type(params);
        Statement* rtype = right->generate_type(params);
        if (!decompose(ltype,rtype,params)) return false;
        ltype->cleanup();
        rtype->cleanup();
        
        if (left->local==tracks.size()-1 and right->local==tracks.size()-1) {
            SingleBind a = emplace(left,right,tracks.size(),params);
            SingleBind b = emplace(right,left,tracks.size(),params);
            if (a.head==0 or b.head==0) return false;
            binds.push_back(a);
        } else if (right->local!=tracks.size()-1) {
            SingleBind c = emplace(left,right,tracks.size(),params);
            if (c.head==0) return false;
            binds.push_back(c);
        } else if (left->local!=tracks.size()-1) {
            SingleBind c = emplace(right,left,tracks.size(),params);
            if (c.head==0) return false;
            binds.push_back(c);
        }
        return true;
    }
    if (left->local==right->local and left->id==right->id) {
        if (left->args.size()!=right->args.size()) throw;
        if (left->local==-1) return true;
        Strategy* calctype = (*params[left->local])[left->id]->typechecksub(&left->args, (*params[left->local])[left->id]->local+1,(int)params.size(),1);
        for (int u=0;u<left->args.size();u++) {
            std::vector<std::vector<Strategy*>*> continued = params;
            continued.push_back(&calctype->args[u]->args);
            if (!decompose(left->args[u], right->args[u],continued)) {
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
    std::vector<Statement*> partials;
    std::vector<Strategy*> types;
    CartesianCarry(std::vector<Statement*>& p,std::vector<Strategy*>& t) : partials(p),types(t) {}
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
Statement* reduce(Statement* in,int loc,int l1off,int l2off) {
    Statement* ret = new Statement(in->id,in->local);
    if (in->local>=loc+2) {
        ret->local-=2;
        if (ret->local==loc)   ret->id+=l1off;
        if (ret->local==loc+1) ret->id+=l2off;
    }
    for (int u=0;u<in->args.size();u++) {
        ret->args.push_back(reduce(in->args[u],loc,l1off,l2off));
    }
    return ret;
}
Strategy* reduce(Strategy* in,int loc,int l1off,Strategy* append,int pnum) {
    Strategy* ret = new Strategy(reduce(in->type,loc,l1off,append->args.size()),in->id,in->local-2);
    
    if (ret->local==loc) {
        ret->id+=l1off;
        for (int u=0;u<pnum;u++) {
            ret->args.push_back(append->args[u]);
        }
    }
    if (ret->local==loc+1) ret->id+=append->args.size();
    for (int u=0;u<in->args.size();u++) {
        ret->args.push_back(reduce(in->args[u],loc,l1off,append,pnum));
    }
    return ret;
}
void idtrans(Statement* a,int target,std::vector<Strategy*>& types,Strategy* loctype,int loc,int pnum) {

    if (a->local==target) {
        types.push_back(reduce(loctype->args[pnum]->args[a->id],loc,types.size(),loctype,pnum));
        a->local=1;
        a->id=types.size()-1;
        
    }
    for (int u=0;u<pnum;u++) {
        a->args.insert(a->args.begin()+u,loctype->args[u]->snapshot());
    }
    for (int u=0;u<a->args.size();u++) {
        idtrans(a->args[u],target,types,loctype,loc,pnum);
    }
}
void godtrans(Statement* a,int loc,std::vector<Strategy*>& types,Strategy* type,int stat) {

    if (a->local==loc+1) {
        for (int u=0;u<a->args.size();u++) {
            idtrans(a->args[u],stat,types,type,loc,u);
        }
    }
    for (int u=0;u<a->args.size();u++) {
        godtrans(a->args[u],loc,types,type,stat+1);
    }
}
void generatePartial(Statement* a,Statement* partial,Strategy* head,std::vector<Statement*>& statout,CartesianCarry& opartials,int loc,bool& changed,std::vector<std::vector<Strategy*>*>& params) {
    if (a->local==loc) {
        if (a->id==-1) {
            statout.push_back(partial->deepcopy());
            return;
        }
        Statement* subbed = opartials.partials[a->id]->substitute_level(&a->args,loc+1,loc+1,params.size());
        generatePartial(subbed,partial,head,statout,opartials,loc,changed,params);//this screws up depth<-=--==-=-=--==--==-=--=-=-=-=-=-=-=-=-=
        subbed->cleanup();
        return;
    }
    if (partial->local==loc) {
        if (partial->id!=-1) throw;
        for (int u=0;u<head->args.size();u++) {
            statout.push_back(head->args[u]->snapshot()->depth_push(loc+2,params.size()-loc-2));
        }
        changed=true;
        
//        if (a->local!=loc+1) {
        Statement* neg=new Statement(a->id,a->local);
        for (int y=0;y<a->args.size();y++) {
            neg->args.push_back(new Statement(-1,loc));
        }
        generatePartial(a,neg,head,statout,opartials,loc,changed,params);
        delete neg;
//        } else throw;
    } else {
        if (partial->local==loc+1) {
            statout.push_back(partial->deepcopy());
            return;
        }
        if (a->id!=partial->id or a->local!=partial->local) return;
        if (a->args.size()==0) {
            statout.push_back(a->deepcopy());
            return;
        }
        std::vector<Statement*> buffer;
        Strategy* calctype = (*params[a->local])[a->id]->typechecksub(&a->args, (*params[a->local])[a->id]->local+1,(int)params.size(),1);
        std::vector<std::vector<Strategy*>*> continued0 = params;
        continued0.push_back(&calctype->args[0]->args);
        if (a->args.size()==1) {//depth push on substitute for bindings. actually just make sure depth works slotwise with both substitutions.
            generatePartial(a->args[0],partial->args[0],head,buffer,opartials,loc,changed,continued0);
            for (int g=0;g<buffer.size();g++) {
                statout.push_back(new Statement(a->id,a->local,buffer[g]));
            }
            return;
        }
        std::vector<Statement*> cartesian1;
        std::vector<Statement*> cartesian2;
        bool c1=true;
        #define CAR1 (c1?cartesian1:cartesian2)
        #define CAR2 (c1?cartesian2:cartesian1)
        generatePartial(a->args[0],partial->args[0],head,buffer,opartials,loc,changed,continued0);
        for (int g=0;g<buffer.size();g++) {
            cartesian1.push_back(new Statement(partial->id,partial->local,buffer[g]));
        }
        buffer.clear();
        for (int q=1;q<a->args.size();q++) {
            std::vector<std::vector<Strategy*>*> continued = params;
            continued.push_back(&calctype->args[q]->args);
            generatePartial(a->args[q],partial->args[q],head,buffer,opartials,loc,changed,continued);
            for (int u=0;u<CAR1.size();u++) {
                for (int p=0;p<buffer.size();p++) {
                    Statement* dup = CAR1[u]->deepcopy();
//                    int ju = idcheck(dup,loc,params[loc]->size());
                    dup->args.push_back(buffer[p]->deepcopy());
//                    idbump(dup->args[dup->args.size()-1],loc,params[loc]->size(),ju);
                    (q<a->args.size()-1?CAR2:statout).push_back(dup);
                }
            }
            buffer.clear();
            CAR1.clear();
            c1 = not c1;
        }
        #undef CAR1
        #undef CAR2
    }
}
Statement* partialsub(Statement* p,Binding* o,int rec) {
    if ((p->local<=0 or p->id==-1) and p->args.size()==0) return p;
    Statement* res = new Statement(p->id,p->local);
    for (int u=0;u<p->args.size();u++) {
        res->args.push_back(partialsub(p->args[u],o,rec+1));
    }
    if (res->local==o->tracks.size()-1) {
        if (o->partials[res->id]->is_complete(o->tracks.size()-1)) {
            return o->partials[res->id]->substitute_level(&res->args,o->tracks.size(),rec,1);
        }
    }
    return res;
}
bool Binding::simplify() {
    std::vector<SingleBind> purify;
    bool changeflag=false;
    for (int u=0;u<partials.size();u++) {
        partials[u] = partialsub(partials[u],this,3);
    }
//    std::cout<<"SIMPLIFYING:\n\n"<<tostring()<<"\n";
//    int oldbinds = binds.size();
    for (int u=0;u<binds.size();u++) {
        std::vector<std::vector<Strategy*>*> tplusi=tracks;
        tplusi.push_back(&binds[u].itinerary);
        std::vector<Statement*> s1;
        std::vector<Statement*> s2;
        
        binds[u].head->substitute(this,tplusi,s1,binds[u].body->local==tracks.size()-1?-1:u,changeflag);
        binds[u].body->substitute(this,tplusi,s2,-1,changeflag);
//        std::cout<<"-->"<<binds[u].head->tostring()<<"<-->"<<binds[u].body->tostring()<<"<--:\n";
        for (int x=0;x<s1.size();x++) {
            for (int y=0;y<s2.size();y++) {
//                std::cout<<"\t-->"<<s1[x]->tostring()<<"<-->"<<s2[y]->tostring()<<"<--:\n";
                purify.push_back(SingleBind(s1[x],s2[y],binds[u].itinerary));
            }
        }
    }
    if (changeflag) {
//        std::cout<<"simplification:\n";
        binds.clear();
        for (int u=0;u<purify.size();u++) {
            
            std::vector<std::vector<Strategy*>*> tplusi=tracks;
            tplusi.push_back(&purify[u].itinerary);
            if (!decompose(purify[u].head,purify[u].body,tplusi)) return false;
        }
        return simplify();
    }
    return true;
}
//void Binding::divide(std::vector<Binding>& list) {
//    if (!simplify()) return;
//    divide(list,true);
//}
void Binding::divide(std::vector<Binding>& list) {
    if (!simplify()) return;
    std::vector<CartesianCarry> cartesian1;
    std::vector<CartesianCarry> cartesian2;
    bool changed=false;
    bool c1=true;
    std::vector<Statement*> buffer;
    #define CAR1 (c1?cartesian1:cartesian2)
    #define CAR2 (c1?cartesian2:cartesian1)
    cartesian1.push_back(CartesianCarry(partials,localtypes));
    
    for (int q=0;q<binds.size();q++) {
        std::vector<std::vector<Strategy*>*> continued = tracks;
        continued.push_back(&binds[q].itinerary);
        for (int u=0;u<CAR1.size();u++) {
            int bbinds = (int)CAR2.size();
//            std::cout<<"dissembling: "<<binds[q].body->tostring()<<" under type: "<<localtypes[binds[q].head->id]->tostring()<<" , partial: "<<CAR1[u].partials[binds[q].head->id]->tostring()<<"\n";
            generatePartial(binds[q].body,CAR1[u].partials[binds[q].head->id],localtypes[binds[q].head->id],buffer,CAR1[u],tracks.size()-1,changed,continued);
            for (int p=0;p<buffer.size();p++) {
                std::vector<Strategy*> stratbuffer = CAR1[u].types;
//                std::cout<<"godtrans for "<<buffer[p]->tostring()<<"\n";
                godtrans(buffer[p],tracks.size()-1,stratbuffer,localtypes[binds[q].head->id],tracks.size()+1);
                
                
                
        std::vector<std::vector<Strategy*>*> debugtra = tracks;
        debugtra.push_back(&localtypes[binds[q].head->id]->args);
        debugtra[tracks.size()-1] = &stratbuffer;
        
        
                
                buffer[p]->constcheck(debugtra);
//                std::cout<<"created: "<<buffer[p]->tostring()<<"\n";
                
                CAR2.push_back(CartesianCarry(CAR1[u].partials,stratbuffer));
                CAR2[bbinds+p].partials[binds[q].head->id] = buffer[p]->deepcopy();
                for (int j=CAR2[bbinds+p].partials.size();j<stratbuffer.size();j++) {
                    CAR2[bbinds+p].partials.push_back(new Statement(-1,tracks.size()-1));
                }
            }
            buffer.clear();
        }
        CAR1.clear();
        c1 = not c1;
        if (binds[q].body->local==tracks.size()-1) {
            for (int u=0;u<CAR1.size();u++) {
                int bbinds = (int)CAR2.size();
                generatePartial(binds[q].body,CAR1[u].partials[binds[q].head->id],localtypes[binds[q].body->id],buffer,CAR1[u],tracks.size()-1,changed,continued);
                for (int p=0;p<buffer.size();p++) {
                    std::vector<Strategy*> stratbuffer = CAR1[u].types;
                    godtrans(buffer[p],tracks.size()-1,stratbuffer,localtypes[binds[q].body->id],tracks.size()+1);
                    CAR2.push_back(CartesianCarry(CAR1[u].partials,stratbuffer));
                    CAR2[bbinds+p].partials[binds[q].body->id] = buffer[p]->deepcopy();
                }
                buffer.clear();
            }
            CAR1.clear();
            c1 = not c1;
        }
    }
//    std::cout<<"--===--=-=--=--=-=-==-=--==--=-===---=\n"<<tostring()<<"\n";
//    for (int u=0;u<CAR1.size();u++) {
//        Binding soap = *this;
//        soap.partials = CAR1[u].partials;
//        soap.localtypes = CAR1[u].types;
//        soap.tracks[soap.tracks.size()-1] = &soap.localtypes;
//        std::cout<<soap.tostring()<<"\n";
//    }
    
    for (int u=0;u<CAR1.size();u++) {
        Binding soap = *this;
        soap.partials = CAR1[u].partials;
        soap.localtypes = CAR1[u].types;
        soap.tracks[soap.tracks.size()-1] = &soap.localtypes;
        bool valid=true;
        
        for (int u=0;u<binds.size();u++) {
            std::vector<std::vector<Strategy*>*> tplusi=soap.tracks;
            tplusi.push_back(&soap.binds[u].itinerary);//which itinerary
            Statement* compare = soap.partials[soap.binds[u].head->id]->substitute_level(&soap.binds[u].head->args,tracks.size(),tracks.size()+1,1);
            if (!soap.decomposeverbal(compare,soap.binds[u].body,tplusi,1)) valid=false;
            if (binds[u].body->local==tracks.size()-1) {
                Statement* compare = soap.partials[soap.binds[u].body->id]->substitute_level(&soap.binds[u].body->args,tracks.size(),tracks.size()+1,1);
                if (!soap.decomposeverbal(compare,soap.binds[u].head,tplusi,1)) valid=false;
            }
        }
        
        
        if (valid) {
            std::cout<<"PUSHING: \n"<<soap.tostring()<<"\n";
            if (soap.binds.size()==binds.size() and not changed) {
                list.push_back(soap);
            } else {
                soap.divide(list);
            }
        }
    }
    #undef CAR1
    #undef CAR2
}








