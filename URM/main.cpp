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

int test(int a,int b) {return 6;}
int main(int argc, const char * argv[]) {
//    std::cout<<"\n\nbegin:\n";
    
//    Strategy* loctypes = new Strategy[6];
//    loctypes[0] = Strategy(Statement(1,0),0,1,Strategy(Statement(1,0),0,2,Strategy(Statement(1,0),0,3)));
//    loctypes[1] = Strategy(Statement(1,0),1,1,Strategy(Statement(1,0),0,2));
//    loctypes[2] = Strategy(Statement(0,2),2,1,Strategy(Statement(0,0),0,2));
//    loctypes[3] = Strategy(Statement(0,0),3,1,Strategy(Statement(0,0),0,2));
//    loctypes[4] = Strategy(Statement(0,0),4,1,Strategy(Statement(1,0),0,2,Strategy(Statement(1,0),0,3)));
//    loctypes[5] = Strategy(Statement(0,0),5,1);
//
//#define AX(x) MetaBank::meta_prime.getAxiom(x),0
//#define CASE(x,y) testcases.push_back(std::pair<Statement,Statement>(x,y))
//#define S_add(x,y) Statement(MetaBank::meta_prime.getAxiom("ADD"),0,x,y)
//#define S_and(x,y) Statement(MetaBank::meta_prime.getAxiom("AND"),0,x,y)
//#define S_not(x) Statement(MetaBank::meta_prime.getAxiom("NOT"),0,x)
//#define S_test(x) Statement(MetaBank::meta_prime.getAxiom("test"),0,x)
//#define S_NUM(x) Statement(x,-1)
//
//    std::vector<std::pair<Statement,Statement>> testcases;
//    CASE(Statement(0,1,S_NUM(2)),                      S_add(S_NUM(2),S_NUM(1)));
//    CASE(Statement(0,1,Statement(0,2)),                S_add(S_NUM(2),S_NUM(1)));
//    CASE(Statement(0,1,Statement(1,1,S_NUM(2))),       S_add(S_NUM(2),S_NUM(1)));
//    CASE(Statement(0,1,Statement(1,1,Statement(0,2))), S_add(S_NUM(2),S_NUM(1)));
//    
//    CASE(Statement(2,1,S_test(S_add(Statement(0,3),S_NUM(1)))),Statement(AX("test2"),S_add(Statement(0,2),S_NUM(1))));
//    
//    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_test(S_add(Statement(0,2),S_NUM(1))));
//    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_and(S_test(S_add(Statement(0,3),S_NUM(1))),S_test(S_add(Statement(0,3),S_NUM(1)))));
//    CASE(Statement(3,1,S_test(S_add(Statement(0,3),S_NUM(1)))),        S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
//    CASE(Statement(3,1,Statement(4,1,S_add(Statement(0,3),S_NUM(1)))), S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
//    CASE(Statement(3,1,Statement(4,1,Statement(0,3))),                 S_not(S_and(S_test(S_add(Statement(0,4),S_NUM(1))),S_test(S_add(Statement(0,4),S_NUM(1))))));
//    
//    CASE(S_not(Statement(5,1)),S_not(S_test(S_add(Statement(0,3),S_NUM(1)))));
//    
//    CASE(S_test(Statement(3,1,Statement(0,2))),S_test(S_add(Statement(0,2),S_NUM(1))));
//    
//#undef AX
//#undef CASE
//#undef S_add
//#undef S_and
//#undef S_not
//#undef S_test
//#undef S_NUM
//
//    for (int y=0;y<12;y++) {
////    int y=0;
//        Binding test = Binding(&MetaBank::meta_prime,loctypes,6);
//        testcases[y].first.constcheck(test.tracks);
//        testcases[y].second.constcheck(test.tracks);
//        if (!test.decomposeverbal(testcases[y].first,testcases[y].second,test.tracks,2)) throw;
//        std::vector<Binding> bindings;
//        std::cout<<test.tostring(0)<<"\n";
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
//        test.divide(bindings,0);
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
//        std::string res="BINDINGS:\n";
//        for (int it=0;it<bindings.size();it++) {
//            res+="BINDING "+std::to_string(it)+":\n";
//            res+=bindings[it].tostring(0)+"\n";
//        }
//        std::cout<<res;
//    }
//    int waste_time=0;
//    for (int d=0;d<160;d++) {
//        for (int w=0;w<9000000;w++) waste_time+=w;
////        std::cout<<"el\n";
//    }
//    
//    std::cout<<"elapse\n";

    std::vector<std::string> solvertestcases;
//    solvertestcases.push_back("[A]");
//    solvertestcases.push_back("[[g(a(a(a(a(aa)))))]g(aa)]");
    solvertestcases.push_back("[EQ(AFF,ONE,ONE)]");
    solvertestcases.push_back("[A:U|a:A|EQ(A,a,a)]");
    solvertestcases.push_back("[[a:AFF]b:AFF|[c:AFF]EQ(AFF,b(c),c)]");
    solvertestcases.push_back("[type:U|[a:type]b:type|[c:type]EQ(type,b(c),c)]");
//    solvertestcases.push_back("["
//    "[U|U]B2:U|"
//    "[U|U]C2:U|"
//    "[A:U|B:U|C:U|x:A|y:A|z:A]EQ(A,DEC(A,B,x,DEC(A,C,y,z)),DEC(A,B2(B,C),DEC(A,C2(B,C),x,y),z))]"
//    );
    
    for (int h=0;h<solvertestcases.size();h++) {
        Strategy engulf = parse_TTML(solvertestcases[h],0);
        engulf.id=-1;
        Statement solution = MetaBank::meta_prime.solveHare(engulf);
        std::cout<<solvertestcases[h]<<" result:\n";
        std::cout<<"\t"<<solution.tostring()<<"\n";
        solution.cleanup();
    }

    

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
//    int x,y;
//    for (;;) ;
//    int x,y;
//    x=5,y=3,
//    std::cout<<"hahhah",x=8,
//    if () {
//        
//    }
//    for (;;);
    

    //commas
    
//    std::vector<std::string> validitybenchmarks;
//    validitybenchmarks.push_back(
//        "affine x;"
//        "x=2/9;"
//    );
//    for (int r=0;r<validitybenchmarks.size();r++) {
//        Statement result = getLangFile("ttfl.ilang").fullconvert(validitybenchmarks[r]);
//        std::cout<<"-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"<<result.tostringheavy()<<"\n";
////        Statement* solved = invokeSolver(result);
//        std::cout<<solved->tostringheavy()<<"\n";
//        solved->cleanup();
//    }
    
    
    
    
    
    return 0;
}

















