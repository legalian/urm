//
//  parser_core.hpp
//  URM
//
//  Created by Parker on 8/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#ifndef parser_core_hpp
#define parser_core_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "reasoning_core.hpp"

struct ParseSpecifier;
struct LineInfo;
struct ParseResult;
struct Conversion;
struct ConversionChoice;
struct ParseArtifact;
struct ParseVariant;
struct ParseStructure;

struct LineInfo {
    long begin;
    long end;
};
struct ErrorReport {
    long begin;
    long end;
    int errdepth=-1;
    long bigbegin;
    long bigend;
    int bigerrdepth=-1;
    std::string message;
    std::string bigmessage;
    void failure(std::string,long,long,int);
    void bigfailure(std::string,long,long,int);
};
struct ParseResult {
    int struc=-1;
    int var=-1;
    int order=-1;
    LineInfo lineinfo;
    std::string endpoint="";
    std::vector<ParseResult*> children;
    ParseResult();
    ParseResult(int,int,int);
    ParseResult(std::string,int);
    std::string recode(ParseSpecifier*);
    void cleanup();
    int measureLeftBalance();
//    ParseResult* rotate(ParseSpecifier*,int,int);
};
struct ConstructArgReference {
    std::vector<int> path;
    std::string tostringheavy();
};
struct DictEntry;
struct Construction {
    int strucLocal;
    int varID;
//    int specifier;
    int reconstruct;
    int tokargs;
    std::string ifget;
    std::vector<DictEntry> given;
    ConstructArgReference alt;
    std::vector<Construction> children;
    std::string tostringheavy();
    Statement* convert(ParseSpecifier*,std::vector<DictEntry>,std::map<int,std::vector<Statement*>*>&,int&,int,Statement*,std::vector<Statement*>&,int);
    Construction reference(ConstructArgReference);
    Construction();
    Construction(ParseSpecifier*,ParseResult*);
//    Construction uproot(int);
//    Construction reroot(int);
//    Construction(Statement*);
    Construction replace(Construction*,int depth);
//    Construction depthadjust(Construction,int);
};
struct DictEntry {
    std::string channel;
    Construction a;
    Construction b;
};
struct Conversion {
    std::vector<ConversionChoice> choices;
    Construction elapse;
    std::string tostringheavy(int tabs);
    Statement* convert(ParseSpecifier*,std::vector<DictEntry>,Construction*,std::map<int,std::vector<Statement*>*>& params,int&,int,ParseVariant&,std::vector<Statement*>&,int);
};
struct ConversionChoice {
    ConstructArgReference head;
    int id;
    Conversion body;
    std::string tostringheavy(int tabs);
};

struct ParseArtifact {
    int tok;
    bool obj;
    ParseArtifact(int,bool);
    std::string tostringheavy(ParseSpecifier*);
};
struct ParseVariant {
    int struc;
    int order;
    std::vector<ParseArtifact> arguments;
    ParseVariant(int);
    ParseVariant& then(std::vector<int>);
    ParseVariant& then(int);
//    bool matchSubset(ParseSpecifier*,long&,long,int,std::vector<ParseResult*>*);
    ParseResult* match(ParseSpecifier*,long&,long,ErrorReport&,int);
    Conversion converter;
    std::string tostringheavy(ParseSpecifier*);
    std::string tostringlight(ParseSpecifier*);
};
struct ParseStructure {
    int struc;
    int maxorder=0;
    std::string englishname;
    std::map<int,bool> flippedorders;
    bool verbose;
    Statement* type=0;
//    std::vector<std::string> varnames;
    std::vector<ParseVariant> variants;
    ParseStructure();
    ParseStructure(int);
    ParseStructure& variant(ParseVariant);
    ParseResult* match(ParseSpecifier*,long&,long,ErrorReport&,int);
    std::string tostringheavy(ParseSpecifier*);
};
struct ParseSpecifier {
//    int tablesize=0;
    int keysize=0;
    std::map<int,ParseStructure> table;
    std::vector<std::string> tokenkey;
    std::vector<int> tokens;
    std::vector<std::string> varnames;
    
    std::vector<std::pair<std::string,std::string>> comments;
    std::vector<std::pair<int,int>> literals;
//    int registerStructure();
    int registerToken(std::string);
    void registerComment(std::string,std::string);
    void registerLiteral(std::string,std::string);
    std::vector<int> tz(const std::string&);
    std::string untokenize(long start,long end);
    std::string tostringheavy();
    
    void tokenize(std::vector<int>* tokens,const std::string&);
    ParseResult* parse(int,const std::string&);
    Statement* fullconvert(const std::string&);
};
ParseSpecifier& getGlobParser();
ParseSpecifier& getLangFile(const std::string&);

Statement* parse_TTML(const std::string&,int);
Statement* parse_TTML(const std::string&,int,std::map<std::string,Statement*>);
Statement* parse_TTML(const std::string&,int,std::map<std::string,Statement*>,std::vector<std::string>*);
Statement* parse_TTML(const std::string&,int,std::vector<std::string>*);
ParseSpecifier parse_parser(std::map<std::string,std::string>,std::map<std::string,std::string>,const std::string&,const std::string&);



//Statement* indexedStrategyConvert(MetaBank*,ParseResult*,std::map<std::string,Statement*>&,int,int);






#endif /* parser_core_hpp */
