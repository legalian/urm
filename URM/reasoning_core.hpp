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

//#define safe_play on

struct MetaBank;
struct Statement;
struct Strategy;
struct Binding;

struct ParseResult;

//struct Lemma;
//struct Soln;
//struct SolnLink;
//struct Entry;
//struct SolveInstance;
struct ParameterContext;
//struct Vein;
//typedef std::vector<Binding> Vein;

//Statement* parseTTFL(MetaBank*,const std::string&);
//Statement* convertTTML(MetaBank*,ParseResult*,int);
//Statement* convertTTFL(MetaBank*,ParseResult*);

struct Statement {
    Statement* args=0;
    int local=99999;
    int id=99999;
    int ara=0;
    

//    void clip_upperbound(int,int,std::map<std::pair<int,int>,int>&,int&);
//    Statement paste_upperbound(int,std::map<std::pair<int,int>,int>&,int);
    
//    void obsoletesolvepoints(std::map<int,bool>&);
//    void getsolvepoints(std::map<int,bool>&);
    
//    Statement generate_type(ParameterContext&);
    
    
//    Statement substitute_level(Statement*,int,int,int);
//    Statement substitute_level_verbal(Statement*,int,int,int,int);
    Statement ndisp_sub(int,int,Statement*,int,int);
    Statement substitute_single(Binding&,ParameterContext&);
    Statement simp_substitute(Binding&,ParameterContext&,int);
    
//    std::string substitute_tostring(Binding*);
    
    Statement deepcopy() const;
    void cleanup();
    
    void idpush(int,int,int);
    
    bool contains(int,int) const;
    bool contains(int) const;
//    bool is_complete(int);
    
    void apply(int,std::map<int,int>&);

//    Statement apply(std::vector<int>&,int) const;
//    Statement applyreverse(std::vector<int>&) const;
//    void extract(std::vector<int>&,Binding&);
//    Statement scramble(std::map<int,int>&,int&,int);
//    void unscramble(std::map<int,int>&,int&,int);
    
    Statement depth_push(int,int) const;
    
    void inplace_sub(int,int,Statement&,int);
    
//    bool observable(MetaBank* mb);
//    double observe_affine(MetaBank* mb);
//    bool observe_boolean(MetaBank* mb);
    
//    void local_list(std::vector<Statement*>* list);
    

    void typecheck(Statement&,ParameterContext&);
    void loosecheck(ParameterContext&);
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
bool judgemental_eq(Statement&,Statement&);
//bool lazy_judgemental_eq(Statement&,Statement&,int,int);
bool judgemental_eq(Strategy&,Strategy&);

struct Strategy {
    Strategy* args=0;
    int local=963369;
    int id=963369;
    int ara=0;
    Statement type;
    
    Strategy deepcopy() const;
    
    void cleanup();
    void typecheck(ParameterContext&);
    void loosecheck(ParameterContext&);
//    Strategy typechecksub_1disp(Statement*,int,int,int);
    Strategy ndisp_sub(int,int,Statement*,int,int);
    Strategy substitute_single(Binding&,ParameterContext&);
    Statement snapshot(int);
    
    void inplace_sub(int,int,Statement&);
    
    void apply(int,std::map<int,int>&);
//    Strategy apply(std::vector<int>&,int);
//    Strategy applyreverse(std::vector<int>&);
//    Statement* locsnapshot(std::vector<Statement*>*,bool,int,std::vector<Strategy*>&,std::vector<Statement*>&);
//    void extract(std::vector<int>&,Binding&);
    
    
    void obsoletesolvepoints(std::map<int,bool>&);
    
    void idpush(int,int,int);
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
    std::string tostringheavy();
};
struct ParameterContext {
    std::vector<std::pair<Strategy*,int>> dat;
    Strategy generateType(Statement&);
    Strategy generateTypeSection(Statement&,int);
    Statement generateLocal(Statement,int,int,Strategy*);
    ParameterContext append(Strategy&);
    ParameterContext append(Strategy*,int);
    Strategy* compress(int);
    Statement* artifact(int);
    int index(int);
    Strategy morph(int,Strategy&,int);
    Statement morph(int,Statement&,int);
    int loc();
//    void typecheck();
    
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
    
    void insert(SingleBind);
    bool decompose(Statement&,Statement&,ParameterContext&);
    bool crossdecompose(Statement&,Statement&,ParameterContext&);
    bool bindonuniversal(int,Statement&);
//    bool type()
//    bool crossdecomposeverbal(Statement&,Statement&,ParameterContext&,int);
//    bool decomposeverbal(Statement&,Statement&,ParameterContext&,int);
    bool typebind(Statement&,Statement&,ParameterContext&);
    bool simplify();
    void divide(std::vector<Binding>&,int);
    std::string tostring(int);
    void loosecheck();
    void gapinplace(int,int);
    void boostinplace(int,Strategy*);
    
    void pad(int);
    Statement integrate(Statement&,Strategy*,int);
    
//    void boost(std::vector<Strategy>&);
//    Binding() {}
//    Binding(Binding&,Binding&);

    Binding(Binding&,Binding&,int,int,Strategy*);
//    Binding(Binding&,int,Strategy&);
    Binding(MetaBank*,Strategy*,int);
    Binding(Binding&,Strategy*,int);
    Binding(ParameterContext&,Strategy*,int);
//    Binding(Binding&,std::vector<Statement>&);
//    Binding(ParameterContext&,Strategy*,int,Binding&);
    Binding(const Binding&);
    Binding& operator = (const Binding&);//free existing memory
//    Binding(Binding&);
//    Binding& operator = (Binding&);
//    Binding(Binding&);
//    Binding(Binding&) = delete;
//    void cleanup();
    ~Binding();
    
//    void tap();
};

//struct SingleStitch {
//    Statement head;
//    Statement* bod;
//    Strategy* itinerary;
//    int vin;
//    int ara;
//    int arav;
//    SingleStitch(Statement,Statement*,Strategy*,int,int,int);
//};
//struct Restriction {
//    int contmin;
//    int contmax;
//    int locmax;
////    std::vector<Statement> types;
//    std::map<int,Statement> rest;
//    Restriction(Binding&,int);
////    Restriction(Restriction&,Restriction&,bool& allowed);
//    bool combine(Restriction&);//get upper contentive range
//    void recurcombine(int);
//    Restriction(const Restriction&);
//    std::string tostring();
//};
//struct StitchLink {
//    int back;
//    std::vector<int> locals;
//    StitchLink(int,std::vector<int>&);
////    StitchLink(int,std::vector<int>&,std::vector<int>&,Binding&,Binding&);
//};
struct Stitching {
    int ara;
    Strategy* localtypes;
    std::vector<int> open;
    std::vector<int> caut;
    
//    Strategy simstrat;
    
    Statement quicktype;
    Statement hook;
    
    Stitching(MetaBank*,Strategy&);
    Stitching(MetaBank*,Strategy*,int,std::vector<int>&,Statement&);
    void calcopen(MetaBank*);
};
void boilBinding(MetaBank*,std::vector<Stitching>&,Binding&,std::vector<int>&,Statement&);





//void stitchpopulate(std::vector<Stitching>&,MetaBank*,Stitching&,std::vector<Binding*>,std::vector<std::vector<int>*>&);
//Stitching veinConstruction(MetaBank*,Binding&,int,std::vector<Binding*>&,std::vector<int>&);

//typedef std::vector<std::pair<std::vector<int>*,std::vector<Stitch>*>> Filtermemo;
//    std::vector<int> vs;
//    std::vector<Stitch> stitches;
//    std::vector<int> vsfinal;
//    std::vector<Stitch> stitchesfinal;
//    Filtermemo(int v,Statement& dun,Statement& run,int r) : srcvein(v),filter(dun),site(run),res(r) {}
//};

struct MetaBank {
    static MetaBank meta_prime;
    MetaBank(const std::string&);
    int ara;
    Strategy* strategies;
    std::vector<std::string> stratnames;
    int getAxiom(std::string);
    std::vector<Stitching> stitchbank;
    std::vector<int> easytypes;
    void offlineLearningStep();
    void onlineLearningStep(Strategy&);
    
    Statement solve(Strategy&);
    
    
    
    
//    bool backbinds(std::vector<std::pair<int,int>>&,int,int);
//    void filterV(std::map<std::vector<int>*,Filtermemo>&,std::vector<int>&,std::vector<Stitch>&);
//    int intracombineV(int,Statement&,Statement&);
//    int intercombineV(int,int,Statement&,Statement&);
//    void createv(Binding&);
//    void linkv(int,Vein&);
//    void printveins();
};

//struct ExBind {
//    std::map<std::vector<int>,int> links;
//    std::vector<std::pair<std::vector<int>,std::vector<int>>> symlinks;
//    bool match(MetaBank*,Statement&,Statement&,std::vector<int>&,std::vector<int>&);
//};

//struct ExBind {
//    std::vector<Vein> veins;
//    void create(MetaBank*,int);
//    ExBind(MetaBank*);
//};
//



//struct SolnLink {
//    std::vector<int> mapr;
//    Soln* linked;
//    Entry* container;
//    SolnLink(std::vector<int>,Soln*,Entry*);
//    void transform(Binding,std::vector<Binding>&);
//};
//struct Entry {
//    Binding bind;
////    std::vector<SolnLink> links;
//    Entry(Binding);
//    std::vector<Soln*> downstream;
////    bool asymmetricObsoletes(Entry* other,std::vector<Statement*>*);
//    
////    bool solved=false;
////    int ids;
//};

//struct Soln {
//    std::vector<Entry*> bin;
//    std::vector<Binding> binqueue;
//    std::vector<SolnLink> upstream;
//    
//    Binding initial;
//    int head;
//    bool expanded=false;
//    
//    std::string tostring();
//
//    Soln(Binding&,int);
////    void remove(SolveInstance*,int);
//    void expand(MetaBank*,SolveInstance*);
//    bool recieveEnergy(MetaBank*,SolveInstance*,int);
////    void typecheck();
//};
//struct SolveInstance {
//    static std::vector<std::string> json;
//    static std::vector<std::string> labels;
////    static std::vector<std::string> gephi;
//    std::vector<Soln*> solns;
////    Soln mainsoln;
//    static std::string label;
//    Soln* makeorcreate(Binding&,int);
//    void increment(MetaBank*);
//    void browser_visualize();
//    void browser_visualizeheavy();
//    void gephi_visualize();
//    static void browser_flushvisualizer();
////    static void gephi_flushvisualizer();
////    void heavyvisualize();
//    SolveInstance(Binding&,int);
////    void remove(Soln*);
//};
//struct Lemma {
//    Statement shortype;
//    int ara;
//    Strategy* internal;
//    Statement payload;
//    Strategy* engageitinerary(int,Strategy*,int);
//    Statement engagepayload(int,Strategy*,int);
//};
//Statement* invokeSolver(Statement*);
//Statement* invokeSolver(std::string);
//Statement* invokeSolver(std::map<std::string,Statement*>,std::string);


//std::pair<Statement*,Statement*> gentleSubstitute(Binding*,Statement*,Statement*,int);
//bool amorphousmatch(Statement,Statement,std::map<int,int>&,std::map<int,int>&);

void ntabprint(std::string,int);




#endif /* reasoning_core_hpp */
