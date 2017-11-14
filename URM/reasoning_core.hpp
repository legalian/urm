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
//#include <pair>

#define safe_play on

struct MetaBank;
struct Statement;
struct Binding;

struct ParseResult;

struct Soln;
struct SolnLink;
struct Entry;
struct SolveInstance;


//Statement* parseTTFL(MetaBank*,const std::string&);
//Statement* convertTTML(MetaBank*,ParseResult*,int);
//Statement* convertTTFL(MetaBank*,ParseResult*);

struct Statement {
    static Statement* universe;
    int local;
    int id;
    int specifier=0;
    Statement* type=0;
    std::vector<Statement*> args;
    Statement(int,int);

//    void erase_deltasub();
    int maxloc(int);
    Statement* mapl(int,int);
    void clip_upperbound(int,bool,std::map<std::pair<int,int>,int>&,int&);
    Statement* paste_upperbound_prim(int,std::map<std::pair<int,int>,int>&,std::vector<Statement*>*,int,bool);
    Statement* paste_upperbound_sec(int,std::map<std::pair<int,int>,int>&,int);
    void obsolete(std::map<int,bool>&);
    void getsolvepoints(std::map<int,Statement*>&,std::map<int,bool>&,bool,bool);
    Statement* substitute(Binding*,int,int);
    Statement* deepcopy();
    void cleanup();
    Statement* symmetricbindavoid(int,int);
    
    bool containsloop(int);
    
    Statement* scramble(std::map<int,int>&,int&);
    void unscramble(std::map<int,int>&,int&);
    
    Statement* depth_push(int,int);
    
    bool observable(MetaBank* mb);
    double observe_affine(MetaBank* mb);
    bool observe_boolean(MetaBank* mb);
    
    void local_list(std::vector<Statement*>* list);
//    Statement* construction_substitute(std::vector<Statement*>*,int,int,int);
    
    
//    Statement* construction_substitute(std::vector<Statement*>*,int,int,int);
//    Statement* typeconstruction_sub(std::vector<Statement*>*,int,int,int);
    
#ifdef safe_play
//    int deltasub=0;
//    int debugdepth;
//    void enforcedepth(int);
    Statement* safe_substitute_level(std::vector<Statement*>*,int,int,int,std::string&);//deltasub may be obsolete. hopefully it is. test this later.
    Statement* typechecksub(std::vector<Statement*>*,int,int,int);
    void typecheck(Statement*);
    void globtypecheck();
    void typecheck(Statement*,std::map<int,std::vector<Statement*>*>,int,bool);
    void headlesstypecheck(std::map<int,std::vector<Statement*>*>,int);
#endif
    std::string tostringstrategy();
    std::string tostringheavy();
    std::string tostringdoubleheavy();
    std::string tostringrecursivedoubleheavy();
    
//    std::string tostringheavy(std::vector<std::vector<std::string>>*);
    
    Statement(Statement*,int,int);
    Statement(Statement*,int,int,Statement*);
    Statement(Statement*,int,int,Statement*,Statement*);
    Statement(Statement*,int,int,Statement*,Statement*,Statement*);
    Statement(Statement*,int,int,Statement*,Statement*,Statement*,Statement*);
};
struct Binding {
    std::vector<std::pair<Statement*,Statement*>> decoms;
    bool insert(Statement*,Statement*,int);
    bool decompose(Statement*,Statement*,int,int);
    bool ensureValidity(Statement*,Statement*,int,int);
    bool compare(Statement*,Statement*,Statement*,Statement*,int);
    
    void divide(std::vector<Binding>&,int,MetaBank*);
    bool functionalAnalysis(std::vector<Binding>&,Statement*,Statement*,int,int,int,Statement*,Statement*,Statement*,MetaBank*);
    Statement* typecomplete(Statement* body,int stmodif,int& curid,MetaBank* typechain);
    
    std::vector<Statement*> solvepoints(Statement*);
    
    std::string tostringheavy();
    Binding();
    Binding(Binding const &);
    Binding& operator=(Binding const &);
    ~Binding();
//    int locids=0;
};
//struct RecursivePattern {
//    Binding transformation;
//    MatchStructure* next;
//    RecursivePattern(Binding&,MatchStructure*);
//};
//struct MatchStructure {
//    Statement* type;
//    std::vector<Statement*> patterns;
//    std::vector<RecursivePattern> recursive;
//    void complete(std::vector<MatchStructure>&);
//    void match(std::vector<Binding>&,Statement*,Binding*);
//};

struct MetaBank {
    static MetaBank meta_prime;
    MetaBank();
    std::vector<Statement*> strategies;
    std::vector<std::string> stratnames;
//    std::vector<MatchStructure> typefamilies;
//    Statement* solve(std::string);
    Statement* solve(Statement*);
    int getAxiom(std::string);
};


struct SolveInstance {
    static std::vector<std::string> json;
    static std::vector<std::string> labels;
    std::vector<Soln*> solns;
    static std::string label;
    Soln* makeorcreate(Statement*);
    void increment(MetaBank*);
    void visualize();
    static void flushvisualizer();
    void heavyvisualize();
//    void remove(Soln*);
};
struct SolnLink {
    std::map<int,int> mapr;
    Soln* linked;
    Entry* container;
    SolnLink(Statement*,Statement*,Soln*,Entry*);
    Entry* transform(Entry*);
};
struct Entry {
    Binding bind;
//    std::vector<SolnLink> links;
    Entry(Binding,int);
    std::vector<Soln*> downstream;
    bool asymmetricObsoletes(Entry* other,std::vector<Statement*>*);
    std::vector<Statement*> endpoints(MetaBank*,Statement*);
//    bool solved=false;
    int ids;
};

struct Soln {
    std::vector<Entry*> bin;
    std::vector<Entry*> binqueue;
    std::vector<SolnLink> upstream;
    Statement* head;
    bool expanded=false;
    int ids=0;
//    int ids=0;
    std::string tostringheavy();

    Soln();
    Soln(Statement*);
    void remove(MetaBank*,SolveInstance*,int);
    void expand(MetaBank*,SolveInstance*);
    bool recieveEnergy(MetaBank*,SolveInstance*,int);
    Statement* getsolution();
};

Statement* invokeSolver(Statement*);
Statement* invokeSolver(std::string);
Statement* invokeSolver(std::map<std::string,Statement*>,std::string);


std::pair<Statement*,Statement*> gentleSubstitute(Binding*,Statement*,Statement*,int);
bool amorphousmatch(Statement*,Statement*,std::map<int,int>&,std::map<int,int>&);






#endif /* reasoning_core_hpp */
