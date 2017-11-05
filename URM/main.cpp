//
//  main.cpp
//  URM
//
//  Created by Parker on 6/22/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include "reasoning_core.hpp"
#include "parser_core.hpp"

int main(int argc, const char * argv[]) {

    std::vector<std::string> parsingbenchmarks;
    parsingbenchmarks.push_back(
        "affine x;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "x=1;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "x=x;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "if (x>5) "
        "x=5;"
        "x=5;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "if (x>x) "
        "x=x;"
        "x=x;"
    );
    parsingbenchmarks.push_back(
        "while (2>5)"
        "pass"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "while (x>5)"
        "pass"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "while (x>5)"
        "x=5;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "while (x>5)"
        "x=5;"
        "x=5;"
    );
    parsingbenchmarks.push_back(
        "affine x;"
        "while (x>x)"
        "x=x;"
        "x=x;"
    );
//    std::string phase2 =
////    "affine x;"
////    "while(x!=0)"
////    "x=x;";
//
//
//
//
////    "affine x;"
////    "affine y;"
////    "x=15;"
////    "y=0;"
////    "y=y;";
////    "y=15/x;";
//
//
////    "affine x;"
////    "affine y;"
//////    "x=15;"
//////    "y=0;"
////    "while (y<15) {"
////    "   x=x+1;"
//////    "   y=y+1;"
////    "}"
////    "y=15/x;";
//
//
//
//    "affine x;"
////    "x=15;"
////    "y=0;"
////    "while (x<15) {"
////    "   x=2;"
////    "   y=y+1;"
////    "}"
////    "if (x==0)"
//    "x=3;";
//
//
//
////    "affine x;"
////    "x=15/x;"
////    "x=15/x;";
    for (int r=0;r<parsingbenchmarks.size();r++)
    {
//        int r=5;
        Statement* result = getLangFile("ttfl.ilang").fullconvert(parsingbenchmarks[r]);
        
        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"<<result->tostringrecursivedoubleheavy()<<"\n";
        Statement* expectedtype = new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1,
            new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1)
        );
        result->typecheck(expectedtype);
        if (expectedtype->maxloc(1)) {
            throw;
        }
        result->cleanup();
    }
    
    

//    Statement* solution;
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    solution = invokeSolver("[EQ(AFF,<2/1:AFF>,<2/1:AFF>)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    solution = invokeSolver("[type:U|a:type|EQ(type,a,a)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    solution = invokeSolver("[[a:AFF]b:AFF|[c:AFF]EQ(AFF,b(c),c)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    solution = invokeSolver("[type:U|[a:type]b:type|[c:type]EQ(type,b(c),c)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
    
    return 0;
}





















