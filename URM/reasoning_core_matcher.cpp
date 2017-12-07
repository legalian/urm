//
//  reasoning_core_matcher.cpp
//  URM
//
//  Created by Parker on 10/1/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

bool Statement::is_complete() {
    if (this==Statement::gap) return false;
    for (int u=0;u<args.size();u++) {
        if (!args[u]->is_complete()) return false;
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
    if (left==Statement::gap or right==Statement::gap) return true;
    if (left->local==tracks.size()-1 or right->local==tracks.size()-1) {
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
//bool Binding::compare(Statement* head1,Statement* head2,Statement* body1,Statement* body2) {
//    head2=head2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1))->localswitch(stmodif+1);
//    body2=body2->symmetricbindavoid(stmodif+1,head1->maxloc(stmodif+1))->localswitch(stmodif+1);
//    head1=head1->localswitch(stmodif+1);
//    body1=body1->localswitch(stmodif+1);
//    std::vector<std::pair<Statement*,Statement*>> pairs;
//    for (int i=0;i<head1->args.size();i++) {
//        pairs.push_back(std::pair<Statement*,Statement*>(head1->args[i],head2->args[i]));
//    }
////    std::cout<<"COMPARING:\n";
////    std::cout<<head1->tostringheavy()<<" | "<<body1->tostringheavy()<<"\n";
////    std::cout<<head2->tostringheavy()<<" | "<<body2->tostringheavy()<<"\n";
////    std::cout<<"\n";
//    bool res = decompose(body1,body2,2,&pairs);
//    head2->cleanup();
//    body2->cleanup();
//    head1->cleanup();
//    body1->cleanup();
//    return res;
//}
//Statement* trimrip(Statement* a,int trim,int target) {
//    if (a->local<=0 and a->args.size()==0) return a;
//    if (a->local==target and a->id>=trim) return Statement::gap;
//    Statement* res = new Statement(a->id,a->local);
//    for (int q=0;q<a->args.size();q++) {
//        res->args.push_back(trimrip(a->args[q],trim,target));
//    }
//    return res;
//}
//void Binding::trimto(int trim,Binding& other) {
////    std::vector<SingleBind> purify;
////    for (int u=0;u<binds.size();u++) {
////        purify.push_back(SingleBind(
////            ,
////            trimrip(binds[u].body,trim,tracks.size()-1),
////            binds[u].itinerary
////        ));
////    }
////    for (int y=trim;y<localtypes.size();y++) {
////        partials.erase(y);
////    }
////    localtypes.erase(localtypes.begin()+trim,localtypes.end());
//    
//    for (int u=0;u<binds.size();u++) {
//        std::vector<std::vector<Strategy*>*> tplusi;
//        tplusi.push_back(&binds[u].itinerary);
//        decompose(trimrip(binds[u].head,trim,tracks.size()-1),trimrip(binds[u].body,trim,tracks.size()-1),tracks);
//    }
//
//    
//}
Statement* trickplace(Statement* a,int target) {
    if (a->local==target) return Statement::gap;
    if (a->local<=0 and a->args.size()==0) {
        return a;
    }
    Statement* res = new Statement(a->id,a->local);
    for (int q=0;q<a->args.size();q++) {
        res->args.push_back(trickplace(a->args[q],target));
    }
    return res;
}
//void checkPartialDescent(Statement* a,Statement* partial,Statement* head,
//                        Binding& in,std::vector<Statement*>& statout,std::vector<Binding>& bindout,
//                        bool& changed,std::vector<std::vector<Strategy*>*>& params) {//this will need to accept parameter types and match them.
//    if (a==Statement::gap) {
//        statout.push_back(partial->deepcopy());//sub args beforehand.
//        bindout.push_back(in);
//    }
//    if (a->local==in.tracks.size()-1) {
//        Statement* subbed = in.partials[a->id]->substitute_level(&a->args,in.tracks.size(),in.tracks.size(),params.size());
//        checkPartialDescent(subbed,partial,head,in,statout,bindout,changed,params);//this screws up depth<-=--==-=-=--==--==-=--=-=-=-=-=-=-=-=-=
//        subbed->cleanup();
//        return;
//    }
//    if (partial==Statement::gap) {
//        for (int p=0;p<head->args.size();p++) {
//            changed=true;
//            statout.push_back(<#const_reference __x#>);
////            Statement* pushed = head->args[p]->substitute_level(&in.principles[head->id]->args[p]->args,in.tracks.size()+1,in.tracks.size(),1);
////            pushed = pushed->depth_push(in.tracks.size()+2,params.size()-(in.tracks.size()+2));
////            Binding soap = in;
////            if (soap.decompose(pushed,a)) {
////                statout.push_back(in.principles[head->id]->args[p]);
////                bindout.push_back(soap);
////            }
////            pushed=pushed->substitute_level(&in.principles[head->id]->args[p]->args,in.tracks.size()+1,, );
////            Statement* h = trickplace(pushed,in.tracks.size()+1);
////            Binding grime = in.emptycopy();
////            std::vector<Binding> firstsplit;
//////            std::cout<<h->tostring()<<",,,,,,,,,,,\n";
////            if (grime.decompose(h,a,params)) grime.divide(firstsplit);
////            for (int x=0;x<firstsplit.size();x++) {
//////                std::cout<<"---====1>"<<pushed->tostring()<<"\n";
////                Statement* f = pushed->substitute_level(&firstsplit[x].partials,in.tracks.size()-1,firstsplit[x].tracks.size(),params.size());
//////                std::cout<<firstsplit[x].tostringheavy();
////                std::vector<std::vector<Strategy*>*> nextbinder = in.tracks;
////                nextbinder.push_back(params[nextbinder.size()]);
////                Binding filth = Binding(nextbinder,in.localtypes[head->id]->args[p]->args);
////                std::vector<Binding> secondsplit;
//////                std::cout<<"---====2>"<<f->tostring()<<"\n";
////                if (filth.decompose(f,a)) filth.divide(secondsplit);
////                for (int y=0;y<secondsplit.size();y++) {
////                    Statement* l = pushed->substitute_level(&secondsplit[y].partials,in.tracks.size()+1,secondsplit[y].tracks.size(),params.size());
////                    Binding scum = in;
////                    scum.partials = firstsplit[x].partials;
//////                    std::cout<<"COMPARING("<<in.tracks.size()<<"): "<<pushed->tostring()<<" then "<<l->tostring()<<" and "<<a->tostring()<<"\n";
////                    if (scum.decompose(l,a)) {
//////                        std::cout<<scum.tostringheavy()<<"\n";
////                        bindout.push_back(scum);
////                        Statement* snap = in.localtypes[head->id]->args[p]->snapshot();
////                        snap=snap->depth_push(in.tracks.size()+1,params.size()-in.tracks.size());//secondsplit is not parameters
//////                        std::cout<<"<-----========0=0=0=0=0=0=0=0=0=----------   "<<snap->tostring()<<"\n";
////
////                        snap=snap->substitute_level(&secondsplit[y].partials,params.size()+1,secondsplit[y].tracks.size(), 1);
////                        
//////                        std::cout<<"<------------0000==00=0=00=0=0=0=0=0=0=0=0=0=0=0=---   "<<snap->tostring()<<"\n";
////                        
////                        statout.push_back(snap);
////                    }
////                }
//            
//        }
//        Statement* neg=new Statement(a->id,a->local);
//        for (int y=0;y<a->args.size();y++) {
//            neg->args.push_back(Statement::gap);
//        }
//        changed=true;
//        checkPartialDescent(a,neg,head,in,statout,bindout,changed,params);
//        delete neg;
//    } else if (partial->local==in.tracks.size()) {
//        Binding grime=in;
////        std::cout<<"COMPARING "<<partial->substitute_level(&head->args,in.tracks.size(),in.tracks.size(),1)->tostring()<<" and "<<a<<"\n";
////->substitute_level(&head->args,in.tracks.size(),in.tracks.size()+1,in.tracks.size()-1)
//        if (grime.decompose(partial,a,params)) {
////            changed=true;
//            statout.push_back(partial->deepcopy());
//            bindout.push_back(grime);
//        }
//    } else if (a->local==in.tracks.size()-1) {
//        Binding grime=in;
//        if (grime.decompose(a,partial,params)) {
//            changed=true;
//            statout.push_back(partial->deepcopy());
//            bindout.push_back(grime);
//        }
//    } else {
//        if (a->id!=partial->id or a->local!=partial->local) return;
//        if (a->args.size()==0) {
//            statout.push_back(a->deepcopy());
//            bindout.push_back(in);
//            return;
//        }
//        std::vector<Statement*> buffer;
//        Strategy* calctype = (*params[a->local])[a->id]->typechecksub(&a->args, (*params[a->local])[a->id]->local+1,(int)params.size(),1);
//        std::vector<std::vector<Strategy*>*> continued0 = params;
//        continued0.push_back(&calctype->args[0]->args);
//        if (a->args.size()==1) {//depth push on substitute for bindings. actually just make sure depth works slotwise with both substitutions.
//            checkPartialDescent(a->args[0],partial->args[0],head,in,buffer,bindout,changed,continued0);
//            for (int g=0;g<buffer.size();g++) {
//                statout.push_back(new Statement(a->id,a->local,buffer[g]));
//            }
//            return;
//        }
//        std::vector<Statement*> cartesian1;
//        std::vector<Statement*> cartesian2;
//        std::vector<Binding> bcartesian1;
//        std::vector<Binding> bcartesian2;
//        bool c1=true;
//        #define CAR1 (c1?cartesian1:cartesian2)
//        #define CAR2 (c1?cartesian2:cartesian1)
//        #define BCAR1 (c1?bcartesian1:bcartesian2)
//        #define BCAR2 (c1?bcartesian2:bcartesian1)
//        checkPartialDescent(a->args[0],partial->args[0],head,in,buffer,bcartesian1,changed,continued0);
//        for (int g=0;g<buffer.size();g++) {
//            cartesian1.push_back(new Statement(partial->id,partial->local,buffer[g]));
//        }
//        buffer.clear();
//        for (int q=1;q<a->args.size();q++) {
//            std::vector<std::vector<Strategy*>*> continued = params;
//            continued.push_back(&calctype->args[q]->args);
//            for (int u=0;u<CAR1.size();u++) {
//                checkPartialDescent(a->args[q],partial->args[q],head,BCAR1[u],buffer,(q<a->args.size()-1?BCAR2:bindout),changed,continued);
//                for (int p=0;p<buffer.size();p++) {
//                    Statement* dup = CAR1[u]->deepcopy();
//                    dup->args.push_back(buffer[p]);
//                    (q<a->args.size()-1?CAR2:statout).push_back(dup);
//                }
//                buffer.clear();
//            }
//            CAR1.clear();
//            c1 = not c1;
//        }
//        #undef CAR1
//        #undef CAR2
//        #undef BCAR1
//        #undef BCAR2
//    }
//}

void generatePartial(Statement* a,Statement* partial,Strategy* head,std::vector<Statement*>& statout,std::vector<Statement*>& opartials,int loc,
                        bool& changed,std::vector<std::vector<Strategy*>*>& params) {//this will need to accept parameter types and match them.
    if (a==Statement::gap) {
        statout.push_back(partial->deepcopy());//sub args beforehand.
    }
    if (a->local==loc) {
        Statement* subbed = opartials[a->id]->substitute_level(&a->args,loc+1,loc+1,params.size());
        generatePartial(subbed,partial,head,statout,opartials,loc,changed,params);//this screws up depth<-=--==-=-=--==--==-=--=-=-=-=-=-=-=-=-=
        subbed->cleanup();
        return;
    }
    if (partial==Statement::gap) {
        for (int u=0;u<head->args.size();u++) {
            statout.push_back(head->args[u]->snapshot());
        }
        Statement* neg=new Statement(a->id,a->local);
        for (int y=0;y<a->args.size();y++) {
            neg->args.push_back(Statement::gap);
        }
        changed=true;
        generatePartial(a,neg,head,statout,opartials,loc,changed,params);
        delete neg;
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
                    dup->args.push_back(buffer[p]);
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

bool Binding::simplify() {
    std::vector<SingleBind> purify;
    bool changeflag=false;
    for (int u=0;u<binds.size();u++) {
        std::vector<std::vector<Strategy*>*> tplusi=tracks;
        tplusi.push_back(&binds[u].itinerary);
        std::vector<Statement*> s1;
        std::vector<Statement*> s2;
        binds[u].head->substitute(this,tplusi,s1,binds[u].body->local==tracks.size()-1?-1:u,changeflag,true);
        binds[u].body->substitute(this,tplusi,s2,-1,changeflag,true);
        std::cout<<s1.size()<<","<<s2.size()<<"\n";
        for (int x=0;x<s1.size();x++) {
            for (int y=0;y<s2.size();y++) {
                purify.push_back(SingleBind(s1[x],s2[y],binds[u].itinerary));
            }
        }
    }
    if (changeflag) {
        std::cout<<"printing:\n";
        binds.clear();
        for (int u=0;u<purify.size();u++) {
            
            std::vector<std::vector<Strategy*>*> tplusi=tracks;
            tplusi.push_back(&purify[u].itinerary);
            if (!decomposeverbal(purify[u].head,purify[u].body,tracks,1)) return false;
        }
    }
    return true;
}


void Binding::divide(std::vector<Binding>& list) {
    if (!simplify()) return;
    std::vector<std::vector<Statement*>> cartesian1;
    std::vector<std::vector<Statement*>> cartesian2;
    bool changed=false;
    bool c1=true;
    std::vector<Statement*> buffer;
    #define CAR1 (c1?cartesian1:cartesian2)
    #define CAR2 (c1?cartesian2:cartesian1)
    cartesian1.push_back(partials);
    
    for (int q=0;q<binds.size();q++) {
        std::vector<std::vector<Strategy*>*> continued = tracks;
        continued.push_back(&binds[q].itinerary);
        for (int u=0;u<CAR1.size();u++) {
            int bbinds = (int)CAR2.size();
            generatePartial(binds[q].body,CAR1[u][binds[q].head->id],localtypes[q],buffer,CAR1[u],tracks.size()-1,changed,continued);
            for (int p=0;p<buffer.size();p++) {
                std::cout<<"sugguestd: "<<buffer[p]->tostring()<<"\n";
                CAR2.push_back(CAR1[u]);
                CAR2[bbinds+p][binds[q].head->id] = buffer[p]->deepcopy();
            }
            buffer.clear();
        }
        CAR1.clear();
        c1 = not c1;
        if (binds[q].body->local==tracks.size()-1) {
            for (int u=0;u<CAR1.size();u++) {
                int bbinds = (int)CAR2.size();
                generatePartial(binds[q].body,CAR1[u][binds[q].head->id],localtypes[q],buffer,CAR1[u],tracks.size()-1,changed,continued);
                for (int p=0;p<buffer.size();p++) {
                    CAR2.push_back(CAR1[u]);
                    CAR2[bbinds+p][binds[q].head->id] = buffer[p]->deepcopy();
                }
                buffer.clear();
            }
            CAR1.clear();
            c1 = not c1;
        }
    }
    for (int u=0;u<CAR1.size();u++) {
        Binding soap = *this;
        soap.partials = CAR1[u];
        if (soap.binds.size()==binds.size() and not changed) {
            if (soap.simplify()) list.push_back(soap);
        } else {
            soap.divide(list);
        }
    }
    #undef CAR1
    #undef CAR2
}








