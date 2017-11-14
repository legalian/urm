//
//  reasoning_core_extension.cpp
//  URM
//
//  Created by Parker on 11/11/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"

//struct RecursivePattern {
//    Binding transformation;
//    MatchStructure* next;
//    void match(std::vector<Binding>&,Statement*);
//};
//struct Pattern {
//    Statement* raw;
//    std::vector<RecursivePattern> recursive;
//};
//struct MatchStructure {
//    Statement* type;
//    std::vector<Pattern> patterns;
//    void complete(std::vector<MatchStructure>&);
//};
//RecursivePattern::RecursivePattern(Binding& b,MatchStructure* m) : transformation(b),next(m) {}
//
//void MatchStructure::complete(std::vector<MatchStructure>& typefamilies) {
//    for (int p=0;p<patterns.size();p++) {
//        for (int f=0;f<typefamilies.size();f++) {
//            Binding bind;
//            if (bind.decompose(typefamilies[f].type,patterns[p],2,0)) {
//                recursive.push_back(RecursivePattern(bind,&typefamilies[f]));
//            }
//        }
//    }
//    //each type family:
//    ////T =o= [this] -> append T to this. (infinite)
//    for (int r=0;r<recursive.size();r++) {
//        std::vector<Binding> ways;
//        match(ways,<the pattern that the recursive is satisfying>,&recursive[r].transformation);
//        for (int w=0;w<ways.size();w++) {
////            recursive[recursive.size()-1].next->match(<#std::vector<Binding> &#>, <#Statement *#>, <#Binding *#>);
//        }
//    }
//}
//void MatchStructure::match(std::vector<Binding>& deposit,Statement* other,Binding* subs) {
//    for (int p=0;p<patterns.size();p++) {
//        Binding bind;
//        if (bind.decompose(patterns[p]->substitute(subs,2,0),other,2,0)) {
//            deposit.push_back(bind);
//        }
//    }
//    for (int u=0;u<recursive.size();u++) {
//        Binding bind = recursive[u].transformation.substitute(subs);
//        recursive[u].next->match(deposit,other,&bind);
//    }
//}
//





