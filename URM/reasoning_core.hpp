//
//  reasoning_core.hpp
//  URM
//
//  Created by Parker on 7/13/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#ifndef reasoning_core_hpp
#define reasoning_core_hpp

#define DUMP std::cout<<
#define TS ->tostring()
#define NLE <<"\n";

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
//#include <pair>

//#define safe_play on

struct MetaBank;
struct Statement;
struct Strategy;
struct Binding;

struct ParseResult;

struct Soln;
struct SolnLink;
struct Entry;
struct SolveInstance;


//Statement* parseTTFL(MetaBank*,const std::string&);
//Statement* convertTTML(MetaBank*,ParseResult*,int);
//Statement* convertTTFL(MetaBank*,ParseResult*);

struct Branches {
    std::vector<std::vector<int>> branches;
    Branches(int);
};
struct Statement {
    static Statement* universe;
    static Statement* gap;
    std::vector<Statement*> args;
    int local;
    int id;

    void clip_upperbound(int,int,std::map<std::pair<int,int>,int>&,int&);
    Statement* paste_upperbound(int,std::map<std::pair<int,int>,int>&,int);
    
    void obsolete(std::map<int,bool>&);
    void getsolvepoints(std::map<int,Statement*>&,std::map<int,bool>&,std::vector<std::vector<Strategy*>*>&,bool,bool);
    
    Statement* generate_type(std::vector<std::vector<Strategy*>*>&);
    
    
    Statement* substitute_level(std::vector<Statement*>*,int,int,int);
    void substitute(Binding*,std::vector<std::vector<Strategy*>*>&,std::vector<Statement*>&);
    void substitute(Binding*,std::vector<std::vector<Strategy*>*>&,std::vector<Statement*>&,int,bool&,bool);
    std::string substitute_tostring(Binding*);
    
    Statement* deepcopy();
    void cleanup();
    
    
    bool containsloop(int,int);
    bool is_complete();
    
    Statement* scramble(std::map<int,int>&,int&,int);
    void unscramble(std::map<int,int>&,int&,int);
    
    Statement* depth_push(int,int);
    
    bool observable(MetaBank* mb);
    double observe_affine(MetaBank* mb);
    bool observe_boolean(MetaBank* mb);
    
    void local_list(std::vector<Statement*>* list);
    

    void typecheck(Strategy*,std::vector<std::vector<Strategy*>*>);
    void constcheck(std::vector<std::vector<Strategy*>*>);


    std::string tostring();
    
    
    
    Statement(int,int);
    Statement(int,int,Statement*);
    Statement(int,int,Statement*,Statement*);
    Statement(int,int,Statement*,Statement*,Statement*);
    Statement(int,int,Statement*,Statement*,Statement*,Statement*);
};
struct Strategy {
    int local;
    int id;
    std::vector<Strategy*> args;
    Statement* type=0;
    Strategy* deepcopy();
    void cleanup();
    void typecheck(std::vector<std::vector<Strategy*>*>);
    Strategy* typechecksub(std::vector<Statement*>*,int,int,int);
    Statement* snapshot();
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,std::vector<Strategy*>&,std::vector<Statement*>&);
    
    
    Statement* locsnapshot(std::vector<Statement*>*,bool,int,std::vector<Strategy*>&,std::vector<Branches>&);
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,int&);
    
    Strategy* bring_depth(int,int);
    
    Strategy(Statement*,int,int);
    Strategy(Statement*,int,int,Strategy*);
    Strategy(Statement*,int,int,Strategy*,Strategy*);
    Strategy(Statement*,int,int,Strategy*,Strategy*,Strategy*);
    Strategy(Statement*,int,int,Strategy*,Strategy*,Strategy*,Strategy*);
    
    std::string tostring();
};
struct SingleBind {
    Statement* head;
    Statement* body;
    std::vector<Strategy*> itinerary;
    std::vector<Branches> principles;
    SingleBind(Statement*,Statement*,std::vector<Strategy*>&);
};
struct Binding {
    std::vector<Strategy*> localtypes;
    std::vector<Branches> principles;
    std::vector<std::vector<Strategy*>*> tracks;
    std::vector<Statement*> partials;
    std::vector<SingleBind> binds;
    
    bool decompose(Statement*,Statement*);
    bool decompose(Statement*,Statement*,std::vector<std::vector<Strategy*>*>&);
    bool decomposeverbal(Statement*,Statement*,std::vector<std::vector<Strategy*>*>&,int);
    bool simplify();
    
    void divide(std::vector<Binding>&);
//    void trimto(int,Binding&);
    Binding emptycopy();
    std::string tostring();
//    Statement* appendStrat();
//    int registerlocal(Strategy*);
//    Binding(MetaBank*);
//    Binding(std::vector<std::vector<Strategy*>*>&);
    Binding(MetaBank*,std::vector<Strategy*>&,std::vector<Branches>&);
    Binding(std::vector<std::vector<Strategy*>*>&,std::vector<Strategy*>&,std::vector<Branches>&);
//    Binding(std::vector<std::vector<Strategy*>*>&,int);
    ~Binding();
};
struct MetaBank {
    static MetaBank meta_prime;
    MetaBank();
    std::vector<Strategy*> strategies;
    std::vector<std::string> stratnames;
//    std::vector<MatchStructure> typefamilies;
//    Statement* solve(std::string);
    Statement* solve(Statement*,std::vector<Strategy*>&,std::vector<Branches>&);
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
    Entry(Binding);
    std::vector<Soln*> downstream;
//    bool asymmetricObsoletes(Entry* other,std::vector<Statement*>*);
    std::vector<Statement*> endpoints(MetaBank*,Statement*);
//    bool solved=false;
//    int ids;
};

struct Soln {
    std::vector<Entry*> bin;
    std::vector<Entry*> binqueue;
    std::vector<SolnLink> upstream;
    std::vector<Strategy*> loctypes;
    std::vector<Branches> principles;
    
    Statement* head;
    bool expanded=false;
    
    std::string tostring();

    Soln();
    Soln(Statement*);
    void remove(SolveInstance*,int);
    void expand(MetaBank*,SolveInstance*);
    bool recieveEnergy(MetaBank*,SolveInstance*,int);
    Statement* getsolution();
};

//Statement* invokeSolver(Statement*);
//Statement* invokeSolver(std::string);
//Statement* invokeSolver(std::map<std::string,Statement*>,std::string);


//std::pair<Statement*,Statement*> gentleSubstitute(Binding*,Statement*,Statement*,int);
bool amorphousmatch(Statement*,Statement*,std::map<int,int>&,std::map<int,int>&);






#endif /* reasoning_core_hpp */
