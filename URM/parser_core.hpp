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
//struct LineInfo;
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

struct DictEntry;
struct DictKeyset;
struct StatPath;

struct TokenConstruction {
    TokenConstruction();
    TokenConstruction(std::string);
    TokenConstruction(std::string,int,TokenConstruction*);
    TokenConstruction(std::vector<int>);
};

struct Construction {
    
    
//    std::string ifget;
//
//    std::vector<DictEntry> given;
//    std::vector<Construction> children;
    
    std::string tostringheavy();
    Statement convert(ParseSpecifier*,std::vector<DictEntry>,int&,int,std::vector<Statement>&,int);
    Construction reference(std::vector<int>);
    
    Construction();
    
    Construction(ParseArtifact,TokenConstruction,int,DictEntry*,std::string,int,Construction*);
    Construction(DictKeyset,int,Construction*);//dynamic dict access
    Construction(StatPath,int,Construction*);
    Construction(int,int,int,Construction*);//statement
    
    
//    Construction(ParseSpecifier*,ParseResult*);  <=-=-=-=-=-=-= this right here is something to keep an eye on
    Construction replace(Construction*,int depth);
};
struct DictKeyset {
    std::vector<std::pair<std::string,std::vector<int>>> gan;
};
struct StatPath {
    DictKeyset han;
    std::vector<int> pan;
    StatPath();
    StatPath(DictKeyset,std::vector<int>);
};
struct DictEntry {
    DictKeyset a;
    Construction b;
    DictEntry();
    DictEntry(DictKeyset,Construction);
};
struct Conversion {
    std::vector<ConversionChoice> choices;
    Construction elapse;
    std::string tostringheavy(int tabs);
    
    Conversion();
    Conversion(Construction);
//    Conversion(std::vector<Conversion)
    Statement convert(ParseSpecifier*,std::vector<DictEntry>,Construction*,int&,int,std::vector<Statement>&,int);
};

struct ConversionValidator {
    std::vector<int> head;
    std::vector<int> ocomp;
    std::string key;
    std::string dict;
    int id;
    bool inverted;
    
    ConversionValidator();
    
    
    
//        globParser.table[p_convarg] = ParseStructure(p_convarg)
//            VARIANT(0) S("-") OBJ(-2) S("~") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(1) S("-") OBJ(-2) S("~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END


//            VARIANT(2) S("-[") OBJ(p_dictkeys) S("]") OBJ(p_convarg) END
//            VARIANT(3) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(3) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END


//            VARIANT(4) S("-") OBJ(-2) S("(") OBJ(p_stringlist) S(")") OBJ(p_convarg) END
//            VARIANT(6) S("-") OBJ(-2) S("=(") OBJ(-2) S(")") OBJ(p_convarg) END
//            VARIANT(7) S("-") OBJ(-2) S("=") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(8) END
//        ;
    ConversionValidator(std::vector<int>,std::string);
    
    
    ConversionValidator(DictKeyset);
    ConversionValidator(DictKeyset,int);
    
    
    ConversionValidator(DictKeyset,int);
    
    ConversionValidator(std::vector<int>,std::string);
    ConversionValidator(std::vector<int>,std::vector<int>);
};
struct ConversionChoice {
    ConversionValidator* conds;
    int ara;
    Conversion body;
    ConversionChoice(Conversion,int,ConversionValidator*);
    std::string tostringheavy(int tabs);
};

struct ParseArtifact {
    int tok=0;
    int obj=0;
    ParseArtifact* args=0;
    int ara=0;
    ParseArtifact();
    ParseArtifact(int,int,int,ParseArtifact*);//-1 means parser string, 0 means root level, 1 means introduced at root level, etc...
    std::string tostringheavy(ParseSpecifier*);
//    ParseStructure(ParseSpecifier)
};
struct ParseVariant {
    int order;
    std::string name;
    std::vector<int> argmappings;
    std::vector<ParseArtifact> arguments;
    std::vector<Conversion> converters;
    
    
    ParseVariant(int);
    ParseVariant(int,std::string);
    
    ParseVariant& then(std::vector<int>);
    ParseVariant& then(ParseArtifact);
    ParseVariant& then(int);
//    bool matchSubset(ParseSpecifier*,long&,long,int,std::vector<ParseResult*>*);
    ParseResult* match(ParseSpecifier*,long&,long,ErrorReport&,int);
    
    std::string tostringheavy(ParseSpecifier*);
    std::string tostringlight(ParseSpecifier*);
};
struct ParseStructure {
    int maxorder=0;
    std::string englishname;
    std::map<int,bool> flippedorders;
    std::vector<ParseVariant> variants;
    std::vector<std::string> typenames;
    std::vector<Strategy> types;
    
    ParseStructure(int);
    ParseStructure();
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
    Statement fullconvert(const std::string&);
    int maxSpec();
};
ParseSpecifier& getGlobParser();
ParseSpecifier& getLangFile(const std::string&);

std::vector<Strategy> parse_TTML(const std::string&,const std::vector<std::vector<std::string>>&,std::vector<std::string>*);
std::vector<Strategy> parse_TTML(const std::string&,const std::vector<std::vector<std::string>>&);
std::vector<Strategy> parse_TTML(const std::string&,std::vector<std::string>*);
std::vector<Strategy> parse_TTML(const std::string&);
ParseSpecifier parse_parser(std::map<std::string,std::string>,std::map<std::string,std::string>,const std::string&,const std::string&);



#endif /* parser_core_hpp */
