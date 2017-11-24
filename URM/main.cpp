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
    Binding test(1);
//    Statement* test1 = new Statement(Statement::universe,1,1,new Statement(Statement::universe,1,0));
//    Statement* test2 = new Statement(Statement::universe,0,1);
//    if (!test.decompose(test1,test2,0)) throw;

    Statement* partial1 = new Statement(Statement::universe,0,1,new Statement(Statement::universe,0,2));
    Statement* partial2 = new Statement(Statement::universe,1,1,new Statement(Statement::universe,0,2));
    Statement* partial3 = new Statement(Statement::universe,2,1,new Statement(Statement::universe,0,2));
//    test.binds.push_back();
//    test.binds.push_back();
//    test.binds.push_back();
    test.partials[0]=std::pair<Statement*,Statement*>(partial1,Statement::gap);
    test.partials[1]=std::pair<Statement*,Statement*>(partial2,Statement::gap);
    test.partials[2]=std::pair<Statement*,Statement*>(partial3,Statement::gap);
    
    Statement* test1 = new Statement(Statement::universe,0,1,Statement::universe);
    Statement* test2 = new Statement(Statement::universe,1,1,new Statement(Statement::universe,0,1,new Statement(Statement::universe,2,1,Statement::universe)));
    Statement* test3 = new Statement(Statement::universe,2,1,Statement::universe);
    test.decompose(test1,new Statement(Statement::universe,1,0),0);
//    std::cout<<test.tostringheavy()<<"\n";
    test.decompose(test2,new Statement(Statement::universe,1,0),0);
//    std::cout<<test.tostringheavy()<<"\n";
    test.decompose(test3,Statement::universe,0);
    std::vector<Binding> bindings;
    std::cout<<test.tostringheavy()<<"\n";
    
//    std::vector<std::vector<Statement*>*> required;
//    required.push_back(&MetaBank::meta_prime.strategies);
    
    test.divide(bindings,&MetaBank::meta_prime);
    
    std::string res="BINDINGS:\n";
    for (int it=0;it<bindings.size();it++) {
        res+="BINDING "+std::to_string(it)+":\n";
        res+=bindings[it].tostringheavy()+"\n";
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





















