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
struct ParameterContext;

//Statement* parseTTFL(MetaBank*,const std::string&);
//Statement* convertTTML(MetaBank*,ParseResult*,int);
//Statement* convertTTFL(MetaBank*,ParseResult*);

struct Statement {
    Statement* args=0;
    int local=99999;
    int id=99999;
    int ara=0;
    
    bool is_valid() {return local!=99999 and id!=99999;}
    

    void clip_upperbound(int,int,std::map<std::pair<int,int>,int>&,int&);
    Statement paste_upperbound(int,std::map<std::pair<int,int>,int>&,int);
    
    void obsolete(std::map<int,bool>&);
    void getsolvepoints(std::map<int,Statement>&,std::map<int,bool>&,ParameterContext&,bool,bool);
    
//    Statement generate_type(ParameterContext&);
    
    
//    Statement substitute_level_1disp(Statement*,int,int,int);
    Statement substitute_level(Statement*,int,int,int);
    Statement substitute_level_verbal(Statement*,int,int,int,int);
    void substitute(Binding*,ParameterContext&,std::vector<Statement>&);
    void substitute(Binding*,ParameterContext&,std::vector<Statement>&,int,bool&);
    std::string substitute_tostring(Binding*);
    
    Statement deepcopy();
    void cleanup();
    
    
//    bool containsloop(int,int);
    bool is_complete(int);
    
    Statement apply(std::vector<int>&);
    Statement applyreverse(std::vector<int>&);
    void extract(std::vector<int>&,ParameterContext&);
//    Statement scramble(std::map<int,int>&,int&,int);
//    void unscramble(std::map<int,int>&,int&,int);
    
    Statement depth_push(int,int);
    
//    bool observable(MetaBank* mb);
//    double observe_affine(MetaBank* mb);
//    bool observe_boolean(MetaBank* mb);
    
//    void local_list(std::vector<Statement*>* list);
    

    void typecheck(Statement,ParameterContext&);
    void constcheck(ParameterContext);


    std::string tostring() const;
    
    
    Statement();
    Statement(int,int);
    Statement(int,int,int);
    Statement(int,int,Statement);
    Statement(int,int,Statement,Statement);
    Statement(int,int,Statement,Statement,Statement);
    Statement(int,int,Statement,Statement,Statement,Statement);
};
bool judgemental_eq(Statement,Statement);
bool judgemental_eq(Strategy,Strategy);

struct Strategy {
    Strategy* args=0;
    int local=963369;
    int id=963369;
    int ara=0;
    Statement type;
    
    Strategy deepcopy();
    
    void cleanup();
    void typecheck(ParameterContext);
    Strategy typechecksub_1disp(Statement*,int,int,int);
    Statement snapshot();
    
    Strategy apply(std::vector<int>&);
    Strategy applyreverse(std::vector<int>&);
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,std::vector<Strategy*>&,std::vector<Statement*>&);
    
    
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,std::vector<Strategy*>&);
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,int&);
    
    Strategy bring_depth(int,int);
    
    Strategy();
    Strategy(Statement,int,int);
    Strategy(Statement,int,int,int);
    Strategy(Statement,int,int,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy,Strategy);
    Strategy(Statement,int,int,Strategy,Strategy,Strategy,Strategy);
    
    std::string tostring();
};
struct ParameterContext {
    std::vector<std::pair<Strategy*,int>> dat;
    Strategy generateType(Statement);
    ParameterContext append(Strategy);
    ParameterContext append(Strategy*,int);
    Strategy* compress(int);
    Statement* artifact(int,Strategy*);
    int index(int);
    Strategy morph(int,Strategy,int);
    Statement morph(int,Statement,int);
    int loc();
};
struct SingleBind {
    Statement head;
    Statement body;
    Strategy* itinerary;
    int ara;
    SingleBind(Statement,Statement,Strategy*,int);
};
struct Binding {
    ParameterContext tracks;
    int ara;
    Strategy* localtypes;
    Statement* partials;
    std::vector<SingleBind> binds;
    
    void insert(SingleBind);
    bool decompose(Statement,Statement);
    bool decompose(Statement,Statement,ParameterContext&);
    bool decomposeverbal(Statement,Statement,ParameterContext&,int);
    bool typebind(Statement,Statement,ParameterContext&);
    bool simplify();
    
    void divide(std::vector<Binding>&,int);
//    void divide(std::vector<Binding>&,bool);
//    void trimto(int,Binding&);
    Binding emptycopy();
    std::string tostring(int);
//    Statement* appendStrat();
//    int registerlocal(Strategy*);
//    Binding(const Binding&) {throw;}
//    Binding(MetaBank*);
//    Binding(std::vector<std::vector<Strategy*>*>&);
    Binding(MetaBank*,Strategy*,int);
//    Binding(int);
    Binding(const ParameterContext&,Strategy*,int);
    Binding(const ParameterContext&,std::vector<Strategy>&,std::vector<Statement>&);
//    Binding(std::vector<std::vector<Strategy*>*>&,int);
    ~Binding();
};
struct MetaBank {
    static MetaBank meta_prime;
    MetaBank();
    int ara;
    Strategy* strategies;
    std::vector<std::string> stratnames;
//    std::vector<MatchStructure> typefamilies;
//    Statement* solve(std::string);
    Statement solve(Statement,Strategy*,int);
    int getAxiom(std::string);
};



struct SolnLink {
    std::vector<int> mapr;
    Soln* linked;
    Entry* container;
    SolnLink(std::vector<int>,Soln*,Entry*);
    Entry* transform(Entry*);
};
struct Entry {
    Binding bind;
//    std::vector<SolnLink> links;
    Entry(Binding);
    std::vector<Soln*> downstream;
//    bool asymmetricObsoletes(Entry* other,std::vector<Statement*>*);
    std::vector<Statement> endpoints(MetaBank*,Statement);
//    bool solved=false;
//    int ids;
};

struct Soln {
    std::vector<Entry*> bin;
    std::vector<Entry*> binqueue;
    std::vector<SolnLink> upstream;
    Strategy* loctypes;
    int ara;
    
    Statement head;
    bool expanded=false;
    
    std::string tostring();

//    Soln();
    Soln(Statement,Strategy*,int);
    void remove(SolveInstance*,int);
    void expand(MetaBank*,SolveInstance*);
    bool recieveEnergy(MetaBank*,SolveInstance*,int);
};
struct SolveInstance {
    static std::vector<std::string> json;
    static std::vector<std::string> labels;
    std::vector<Soln*> solns;
    Soln mainsoln;
    static std::string label;
    Soln* makeorcreate(Statement,Strategy*,int);
    void increment(MetaBank*);
    void visualize();
    static void flushvisualizer();
    void heavyvisualize();
    SolveInstance(Statement,Strategy*,int);
//    void remove(Soln*);
};

//Statement* invokeSolver(Statement*);
//Statement* invokeSolver(std::string);
//Statement* invokeSolver(std::map<std::string,Statement*>,std::string);


//std::pair<Statement*,Statement*> gentleSubstitute(Binding*,Statement*,Statement*,int);
//bool amorphousmatch(Statement,Statement,std::map<int,int>&,std::map<int,int>&);






#endif /* reasoning_core_hpp */
