//
//  reasoning_observable.cpp
//  URM
//
//  Created by Parker on 9/25/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

//bool Statement::observable(MetaBank* mb) {
//    if (local<0) return true;
//    if (local==0) {
//        if (
//            id==mb->getAxiom("ADD") or
//            id==mb->getAxiom("SUBTRACT") or
//            id==mb->getAxiom("MULTIPLY") or
//            id==mb->getAxiom("DIVIDE") or
//            id==mb->getAxiom("MODULO") or
//            id==mb->getAxiom("NOT") or
//            id==mb->getAxiom("GT") or
//            id==mb->getAxiom("EQ") or
//            id==mb->getAxiom("AFF")
//        ) {
//            for (int j=0;j<args.size();j++) {
//                if (!args[j]->observable(mb)) return false;
//            }
//            return true;
//        }
//        else return false;
//    }
//    return false;
//}
//double Statement::observe_affine(MetaBank* mb) {
//    if (local==-1) return id;
//    if (local==0) {
//        if (id==mb->getAxiom("ADD")) {     return args[0]->observe_affine(mb)+args[1]->observe_affine(mb);}
//        if (id==mb->getAxiom("SUBTRACT")) {return args[0]->observe_affine(mb)-args[1]->observe_affine(mb);}
//        if (id==mb->getAxiom("MULTIPLY")) {return args[0]->observe_affine(mb)*args[1]->observe_affine(mb);}
//        if (id==mb->getAxiom("DIVIDE")) {  return args[2]->observe_affine(mb)/args[0]->observe_affine(mb);}
//        if (id==mb->getAxiom("MODULO")) {  throw;}//return args[2]->observe_affine(mb)%args[0]->observe_affine(mb);}
//    }
//    throw;
//}
//bool Statement::observe_boolean(MetaBank* mb) {
//    if (local==0) {
//        if (id==mb->getAxiom("EQ")) { return args[1]->observe_affine(mb)==args[2]->observe_affine(mb);}
//        if (id==mb->getAxiom("GT")) { return args[0]->observe_affine(mb)>args[1]->observe_affine(mb);}
//        if (id==mb->getAxiom("NOT")) {return !args[0]->observe_boolean(mb);}
//    }
//    throw;
//}










