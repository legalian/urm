//
//  reasoning_core.hpp
//  URM
//
//  Created by Parker on 7/13/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#ifndef reasoning_core_hpp
#define reasoning_core_hpp

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>

struct MetaBank;
struct Statement;
struct Strategy;
struct Binding;
struct Stitching;

struct ParseResult;
struct ParameterContext;

struct Statement {//checked
    Statement* args=0;
    int local=99999;
    int id=99999;
    int ara=0;

    Statement ndisp_sub(int,int,Statement*,int,int) const;
    Statement substitute_single(const Binding&,const ParameterContext&) const;
    Statement simp_substitute(const Binding&,const ParameterContext&,int) const;
    Statement depth_push(int,int) const;
    Statement deepcopy() const;
    void cleanup();
    void idpush(int,int,int);
    void apply(int,std::vector<int>&);
    void inplace_sub(int,int,const Statement&,int);
    void expandLocs(int,int,int,int,Strategy*);
    
    bool contains(int,int) const;
    bool contains(int) const;
    
    void typecheck(const Statement&,const ParameterContext&) const;
    void loosecheck(const ParameterContext&) const;
    void constcheck(const ParameterContext&) const;

    std::string tostring() const;

    Statement();
    Statement(int,int);
    Statement(int,int,int);
    Statement(int,int,int,Statement*);
    Statement(int,int,Statement);
    Statement(int,int,Statement,Statement);
    Statement(int,int,Statement,Statement,Statement);
    Statement(int,int,Statement,Statement,Statement,Statement);
};
bool judgemental_eq(const Statement&,const Statement&);
bool judgemental_eq(const Strategy&,const Strategy&);

struct Strategy {//checked
    Strategy* args=0;
    int local=963369;
    int id=963369;
    int ara=0;
    Statement type;
    
    Strategy deepcopy() const;
    Strategy ndisp_sub(int,int,Statement*,int,int) const;
    Strategy substitute_single(const Binding&,const ParameterContext&) const;
    Strategy bring_depth(int,int) const;
    
    Statement snapshot(int) const;
    
    void cleanup();
    void inplace_sub(int,int,const Statement&);
    void apply(int,std::vector<int>&);
    void idpush(int,int,int);
    void expandLocs(int,int,int,Strategy*);
    
    void typecheck(const ParameterContext&) const;
    void loosecheck(const ParameterContext&) const;
    
    std::string tostring() const;
    std::string tostringheavy() const;
    
    Strategy();
    Strategy(Statement,int,int);
    Strategy(Statement,int,int,int);
    Strategy(Statement,int,int,int,Strategy*);
    Strategy(Statement,int,int,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy,Strategy,Strategy);
};
struct ParameterContext {//checked
    std::vector<std::pair<Strategy*,int>> dat;
    ParameterContext append(const Strategy&) const;
    ParameterContext append(Strategy*,int) const;
    Strategy generateType(const Statement&) const;
//    Strategy generateTypeSection(const Statement&,int) const;
    Statement partialgenerateLocal(int,int);
    Statement generateLocal(const Statement&,int);
    Strategy morph(int,const Strategy&,int) const;
    Statement morph(int,const Statement&,int) const;
    Strategy* compress(int) const;
    Statement* artifact(int) const;
    int index(int) const;
    int loc() const;
};
struct SingleBind {
    Statement head;
    Statement body;
    Strategy* itinerary;
    int ara;
    bool universal;
    bool concrete;
    SingleBind deepcopy() const;
    SingleBind(Statement,Statement,Strategy*,int,bool,bool);
};



struct Binding {
    int ara=0;
    Strategy* localtypes=0;
    std::vector<SingleBind> binds;
    ParameterContext tracks;
    
    Statement integrate(const Statement&,Strategy*,int);
    
    bool decompose(const Statement&,const Statement&,const ParameterContext&);
    bool crossdecompose(const Statement&,const Statement&,const ParameterContext&);
    bool bindonuniversal(int,const Statement&);
    bool typebind(const Statement&,const Statement&,const ParameterContext&);
    bool simplify();
    
    void divide(std::vector<Binding>&,int);
    void gapinplace(int,int);
    void boostinplace(int,Strategy*);
    void pad(int);
    void insert(SingleBind);
    
    void loosecheck() const;
    
    std::string tostring(int) const;

    Binding(const Binding&,const Binding&,int,int,Strategy*);
    Binding(MetaBank*,Strategy*,int);
    Binding(const ParameterContext&,Strategy*,int);
    Binding(const Binding&);
    Binding& operator = (const Binding&);
    ~Binding();
};

struct Stitching {
    int ara;
    Strategy* localtypes;
    std::vector<int> open;
    std::vector<int> caut;
    
    Statement quicktype;
    Statement hook;
    
    void calcopen(MetaBank*);
    Stitching(MetaBank*,const Strategy&);
    Stitching(MetaBank*,Strategy*,int,std::vector<int>&,const Statement&);
};

void boilBinding(MetaBank*,std::vector<Stitching>&,Binding&,std::vector<int>&,const Statement&);

struct MetaBank {
    static MetaBank meta_prime;
    int ara;
    Strategy* strategies;
    std::vector<std::string> stratnames;
    std::vector<Stitching> stitchbank;
    std::vector<int> easytypes;
    
    Statement solve(const Strategy&);
    void offlineLearningStep();
    void onlineLearningStep(const Strategy&);
    
    int getAxiom(std::string) const;
    MetaBank(const std::string&);
};

void ntabprint(std::string,int);

Statement* deepcopy(Statement*,int);
Strategy* deepcopy(Strategy*,int);

void expandRange(int,int&,int,int,Statement*&,int,Strategy*);
void expandRange(int,int&,Strategy*&,int,Strategy*);

bool affrayBinding(Binding&,int,Strategy&);
void multiaffray(MetaBank*,Binding&,int,std::vector<Binding>&);
void multiaffray(Strategy*,int,Binding&,int,std::vector<Binding>&);
void multiaffrayset(Strategy*,int,Binding&,std::vector<int>&,std::vector<Binding>&);
void affrayBinding(MetaBank*,Stitching&,int,Strategy&,std::vector<Stitching>&);
void affrayBinding(MetaBank*,Stitching&,int,Stitching&,std::vector<Stitching>&);
void multiaffray(MetaBank*,int,int,std::vector<Stitching>&);
//void multiaffrayset(MetaBank*,int,std::vector<int>,std::vector<Stitching>&);


#endif /* reasoning_core_hpp */
