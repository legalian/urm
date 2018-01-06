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
    
    Strategy* loctypes = new Strategy[6];
    loctypes[0] = Strategy(Statement(1,0),0,1,Strategy(Statement(1,0),0,2,Strategy(Statement(1,0),0,3)));
    loctypes[1] = Strategy(Statement(1,0),1,1,Strategy(Statement(1,0),0,2));
    loctypes[2] = Strategy(Statement(0,2),2,1,Strategy(Statement(0,0),0,2));
    loctypes[3] = Strategy(Statement(0,0),3,1,Strategy(Statement(0,0),0,2));
    loctypes[4] = Strategy(Statement(0,0),4,1,Strategy(Statement(1,0),0,2,Strategy(Statement(1,0),0,3)));
    loctypes[5] = Strategy(Statement(0,0),5,1);

#define AX(x) MetaBank::meta_prime.getAxiom(x),0
#define CASE(x,y) testcases.push_back(std::pair<Statement,Statement>(x,y))
#define S_add(x,y) Statement(MetaBank::meta_prime.getAxiom("ADD"),0,x,y)
#define S_and(x,y) Statement(MetaBank::meta_prime.getAxiom("AND"),0,x,y)
#define S_not(x) Statement(MetaBank::meta_prime.getAxiom("NOT"),0,x)
#define S_test(x) Statement(MetaBank::meta_prime.getAxiom("test"),0,x)
#define S_NUM(x) Statement(x,-1)

    std::vector<std::pair<Statement,Statement>> testcases;
    CASE(Statement(0,1,S_NUM(2)),                      S_add(S_NUM(2),S_NUM(1)));
    CASE(Statement(0,1,Statement(0,2)),                S_add(S_NUM(2),S_NUM(1)));
    CASE(Statement(0,1,Statement(1,1,S_NUM(2))),       S_add(S_NUM(2),S_NUM(1)));
    CASE(Statement(0,1,Statement(1,1,Statement(0,2))), S_add(S_NUM(2),S_NUM(1)));
    
    CASE(Statement(2,1,S_test(S_add(Statement(0,3),S_NUM(1)))),Statement(AX("test2"),S_add(Statement(0,2),S_NUM(1))));
    
    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_test(S_add(Statement(0,2),S_NUM(1))));
    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_and(S_test(S_add(Statement(0,3),S_NUM(1))),S_test(S_add(Statement(0,3),S_NUM(1)))));
    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
    CASE(Statement(3,1,Statement(4,1,S_add(Statement(0,3),S_NUM(1)))), S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
    CASE(Statement(3,1,Statement(4,1,Statement(0,3))),                 S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
    
    CASE(S_not(Statement(5,1)),S_not(S_test(S_add(Statement(0,3),S_NUM(1)))));
    
    CASE(S_test(Statement(3,1,Statement(0,2))),S_test(S_add(Statement(0,2),S_NUM(1))));
    
#undef AX
#undef CASE
#undef S_add
#undef S_and
#undef S_not
#undef S_test
#undef S_NUM

//    for (int y=5;y<8;y++) {
    int y=0;
        Binding test = Binding(&MetaBank::meta_prime,loctypes,6);
        testcases[y].first.constcheck(test.tracks);
        testcases[y].second.constcheck(test.tracks);
        if (!test.decomposeverbal(testcases[y].first,testcases[y].second,test.tracks,2)) throw;
        std::vector<Binding> bindings;
        std::cout<<test.tostring(0)<<"\n";
        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        test.divide(bindings,0);
        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
        std::string res="BINDINGS:\n";
        for (int it=0;it<bindings.size();it++) {
            res+="BINDING "+std::to_string(it)+":\n";
            res+=bindings[it].tostring(0)+"\n";
        }
        std::cout<<res;
//    }


//Statement MetaBank::solve(Statement target,Strategy* loc,int buf) {

    
//    Statement solution;
    
    
    
    
    
//                        std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//                        SolveInstance::label="Test 1";
//                        
//                        Strategy* loctypes = new Strategy[2];
//                        loctypes[0] = Strategy(Statement(2,0,Statement(1,0),Statement(1,-1),Statement(1,-1)),0,1);
//                        
//                        Statement target = Statement(0,1);
//                        
//                        Statement solution = MetaBank::meta_prime.solve(target,loctypes,1);
//                        
//                        std::cout<<"\t"<<solution.tostring()<<"\n";
//                        solution.cleanup();




//    std::cout<<"-=-=-=-=-=-=-=-=-=-\n";
//    SolveInstance::label="Test 2";
//    solution = invokeSolver("[type:U|a:type|EQ(type,a,a)]");
//    for (int d=0;d<solution->args.size();d++) {
//        std::cout<<"\t"<<solution->args[d]->tostringheavy()<<"\n";
//    }
//    solution->cleanup();
//
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
    SolveInstance::flushvisualizer();

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
//        Statement result = getLangFile("ttfl.ilang").fullconvert(parsingbenchmarks[r]);
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"<<result.tostring()<<"\n";
////        Statement expectedtype = Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1,
////            new Statement(new Statement(Statement::universe,MetaBank::meta_prime.getAxiom("HEAP"),0),-1,-1)
////        );
////        result.typecheck(expectedtype);
//        result.cleanup();
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

















