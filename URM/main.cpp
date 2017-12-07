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
    std::cout<<"\n\nbegin:\n";


//    std::vector<Strategy*> loctypes;
//    loctypes.push_back(new Strategy(Statement::universe,0,1,new Strategy(Statement::universe,0,2)));
//    loctypes.push_back(new Strategy(Statement::universe,1,1,new Strategy(Statement::universe,0,2)));
//    loctypes.push_back(new Strategy(Statement::universe,2,1,new Strategy(Statement::universe,0,2)));
//    Binding test = Binding(&MetaBank::meta_prime,loctypes);
//    
//    Statement* test1 = new Statement(0,1,Statement::universe);
//    Statement* test2 = new Statement(1,1,new Statement(0,1,new Statement(2,1,Statement::universe)));
//    Statement* test3 = new Statement(2,1,Statement::universe);
//    test.decompose(test1,new Statement(1,0));
//    test.decompose(test2,new Statement(1,0));
//    test.decompose(test3,Statement::universe);
    
    std::vector<Strategy*> loctypes;
    std::vector<Branches> locprin;
    loctypes.push_back(new Strategy(new Statement(1,0),0,1,new Strategy(new Statement(1,0),0,2,new Strategy(new Statement(1,0),0,3))));
    loctypes.push_back(new Strategy(new Statement(1,0),1,1));
    loctypes.push_back(new Strategy(new Statement(1,0),2,1,new Strategy(new Statement(1,0),0,2)));
    Branches loc1(1);
    loc1.branches[0].push_back(1);
    locprin.push_back(Branches(0));
    locprin.push_back(Branches(1));
    Binding test = Binding(&MetaBank::meta_prime,loctypes,locprin);
    
    Statement* test1 = //new Statement(5,0,new Statement(5,-1),
//                        new Statement(5,0,new Statement(4,-1),
//                         new Statement(5,0,new Statement(3,-1),
                          new Statement(5,0,new Statement(2,-1),
                           new Statement(1,-1)
//                          )
//                         )
//                        )
                       );
    test.decompose(test1,new Statement(0,1,new Statement(2,1,new Statement(0,2))));
//    test.decompose(test1,new Statement(0,1,new Statement(1,1,new Statement(2,-1))));
//    test.decompose(test1,new Statement(0,1,new Statement(0,2)));
//    test.decompose(test1,new Statement(0,1,new Statement(2,-1)));
    
    
//    test.decompose(test1,new Statement(0,1,new Statement(1,1)));
    
    std::vector<Binding> bindings;
    std::cout<<test.tostring()<<"\n";
    
    test.divide(bindings);
    
    std::string res="BINDINGS:\n";
    for (int it=0;it<bindings.size();it++) {
        res+="BINDING "+std::to_string(it)+":\n";
        res+=bindings[it].tostring()+"\n";
    }
    std::cout<<res;



    //f(2:0(3:0)) = 2:0(<2/0>)


    
//    Statement* solution;
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    SolveInstance::label="Test 1";
//    solution = invokeSolver("[EQ(AFF,<2/1:AFF>,<2/1:AFF>)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    SolveInstance::label="Test 2";
//    solution = invokeSolver("[type:U|a:type|EQ(type,a,a)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();

//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    SolveInstance::label="Test 3";
//    solution = invokeSolver("[[a:AFF]b:AFF|[c:AFF]EQ(AFF,b(c),c)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//    
//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    SolveInstance::label="Test 4";
//    solution = invokeSolver("[type:U|[a:type]b:type|[c:type]EQ(type,b(c),c)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//    SolveInstance::flushvisualizer();

//    std::vector<std::string> parsingbenchmarks;
//    parsingbenchmarks.push_back(
//        "affine x;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "x=1;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "x=x;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "if (x>5) "
//        "x=5;"
//        "x=5;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "if (x>x) "
//        "x=x;"
//        "x=x;"
//    );
//    parsingbenchmarks.push_back(
//        "while (2>5)"
//        "pass"
//    );
//    parsingbenchmarks.push_back(
//        "while (2>5)"
//        "pass "
//        "affine x;"
//    );
//    parsingbenchmarks.push_back(
//        "while (2>5)"
//        "pass "
//        "while (2>5)"
//        "pass "
//    );
//    parsingbenchmarks.push_back(
//        "while (2>5)"
//        "while (2>5)"
//        "pass "
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "while (x>5)"
//        "pass"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "while (x>5)"
//        "x=5;"
//    );
//    parsingbenchmarks.push_back(
//        "while (3>5) "
//        "pass "
//        "affine x;"
//        "affine y;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "while (x>5)"
//        "x=5;"
//        "x=5;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "while (x>x)"
//        "x=x;"
//        "x=x;"
//    );
//    parsingbenchmarks.push_back(
//        "affine x;"
//        "if (6/x>6) pass "
//        "if (6/x>6) pass "
//    );
//    for (int r=0;r<parsingbenchmarks.size();r++) {
//        Statement* result = getLangFile("ttfl.ilang").fullconvert(parsingbenchmarks[r]);
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"<<result->tostringrecursivedoubleheavy()<<"\n";
//        Statement* expectedtype = new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1,
//            new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1)
//        );
//        result->typecheck(expectedtype);
//        result->cleanup();
//    }
    
    

    
    
    
//    std::vector<std::string> validitybenchmarks;
//    validitybenchmarks.push_back(
//        "affine x;"
//        "x=2/9;"
//    );
//    for (int r=0;r<validitybenchmarks.size();r++) {
//        Statement* result = getLangFile("ttfl.ilang").fullconvert(validitybenchmarks[r]);
//        Statement* expectedtype = new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1,
//            new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1)
//        );
//        result->typecheck(expectedtype);
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"<<result->tostringheavy()<<"\n";
//        Statement* solved = invokeSolver(result);
//        std::cout<<solved->tostringheavy()<<"\n";
//        solved->cleanup();
//    }
    
    
    
    
    
    
    
    return 0;
}

















