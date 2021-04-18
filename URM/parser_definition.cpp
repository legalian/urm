//
//  parser_definition.cpp
//  URM
//
//  Created by Parker on 10/21/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "parser_core.hpp"
#include <streambuf>
#include <fstream>

using std::vector;
using std::pair;
using std::string;
using std::map;
using std::stoi;

#define p_parse 0
#define p_variant 1
#define p_pattern 2


#define p_conversion 6
#define p_function
#define p_varg 7



#define p_statement 8
#define p_arguments 9


#define p_accessible
#define p_named
#define p_token 10
#define p_tokargs 11


#define p_dictkeys 12
#define p_dictionary 13



#define p_purestatement 14
#define p_purearguments 15
#define p_purestrategy 16
#define p_puremodifiers 17
#define p_stringlist 18

ParseSpecifier& getGlobParser() {
    static ParseSpecifier globParser;
    static bool created=false;
    if (!created) {
        created=true;
        #define TOK(x) globParser.registerToken(x);
        TOK("{") TOK("}") TOK("\\") TOK("\\\\") TOK("-") TOK("~") TOK(",") TOK(".")
        TOK(":") TOK("<") TOK(">") TOK("(") TOK(")") TOK("[") TOK("]") TOK("null") TOK("flipped") TOK("=")
        #undef TOK
        #define VARIANT(x) .variant(ParseVariant(x)
        #define S(x) .then(globParser.tz(x))
        #define OBJ(x) .then(x)
        #define END )
        globParser.registerLiteral("\\","\\");
        globParser.registerComment("#","\n");
        globParser.table[p_parse] = ParseStructure(p_parse)
            VARIANT(0) S("[") OBJ(p_deepstringlist) S("]") OBJ(-2) OBJ(p_stringlist) S("{") OBJ(p_variant) S("}") OBJ(p_parse) END
            VARIANT(1) OBJ(-2) OBJ(p_purestrategy) S("{") OBJ(p_variant) S("}") OBJ(p_parse) END
            VARIANT(2) END
        ;
        globParser.table[p_variant] = ParseStructure(p_variant)
            VARIANT(0) S("->") OBJ(-2) OBJ(p_pattern) S("flipped") OBJ(p_conversion) OBJ(p_variant) END
            VARIANT(1) S("->") OBJ(-2) OBJ(p_pattern)              OBJ(p_conversion) OBJ(p_variant) END
            VARIANT(2) S( ">") OBJ(-2) OBJ(p_pattern)              OBJ(p_conversion) OBJ(p_variant) END
            VARIANT(3) END
        ;
        globParser.table[p_pattern] = ParseStructure(p_pattern)
            VARIANT(0) S("\\") OBJ(-3) S("\\") OBJ(p_pattern) END
            VARIANT(1) OBJ(-2) S(":") OBJ(p_tokdef) OBJ(p_pattern) END
            VARIANT(2) OBJ(p_tokdef) OBJ(p_pattern) END
            VARIANT(3) END
        ;
        
        
        globParser.table[p_tokdef] = ParseStructure(p_tokdef)
//            VARIANT(0) S("~") END
            VARIANT(1) OBJ(-2) S("(") OBJ(p_tokdefargs) S(")") END
            VARIANT(2) OBJ(-2) END
        ;
        globParser.table[p_tokdefargs] = ParseStructure(p_tokdefargs)
            VARIANT(0) S("[") OBJ(p_stringlist) S("]") OBJ(p_tokdef) S(",") OBJ(p_tokdefargs) END
            VARIANT(1) S("[") OBJ(p_stringlist) S("]") OBJ(p_tokdef) END
            VARIANT(2) OBJ(p_tokdef) S(",") OBJ(p_tokdefargs) END
            VARIANT(3) OBJ(p_tokdef) END
        ;
        
        globParser.table[p_conversion] = ParseStructure(p_conversion)
            VARIANT(0) OBJ(p_convarg) S("{") OBJ(p_conversion) S("}") OBJ(p_conversion) END
            VARIANT(4) S("null") END
            VARIANT(5) OBJ(p_statement) END
        ;
        globParser.table[p_convarg] = ParseStructure(p_convarg)
            VARIANT(0) S("-[") OBJ(p_dictkeys) S("]") OBJ(p_convarg) END
            VARIANT(1) S("-") OBJ(-2) S("~") OBJ(-2) OBJ(p_convarg) END
            VARIANT(2) S("-") OBJ(-2) S("~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END
            VARIANT(3) S("-") OBJ(-2) S("(") OBJ(p_stringlist) S(")") OBJ(p_convarg) END
            VARIANT(4) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) OBJ(p_convarg) END
            VARIANT(5) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END
            VARIANT(6) S("-[") OBJ(p_dictkeys) S("](") OBJ(p_stringlist) S(")") OBJ(p_convarg) END
            VARIANT(7) S("-") OBJ(-2) S("=(") OBJ(-2) S(")") OBJ(p_convarg) END
            VARIANT(8) S("-") OBJ(-2) S("=") OBJ(-2) OBJ(p_convarg) END
            VARIANT(9) END
        ;
        
        
        
        
        globParser.table[p_statement] = ParseStructure(p_statement)
            VARIANT(0) OBJ(p_tokdef) OBJ(p_token) S("[") OBJ(p_dictionary) S("]") OBJ(-2) END
            VARIANT(1) OBJ(p_tokdef) OBJ(p_token) S("[") OBJ(p_dictionary) S("]") OBJ(-2) END
            VARIANT(4) OBJ(-2) S("(") OBJ(p_arguments) S(")") END
            VARIANT(5) OBJ(-2) END
        ;
        globParser.table[p_arguments] = ParseStructure(p_arguments)
            VARIANT(0) S("[") OBJ(p_stringlist) S("]") OBJ(p_statement) S(",") OBJ(p_arguments) END
            VARIANT(1) S("[") OBJ(p_stringlist) S("]") OBJ(p_statement) END
            VARIANT(2) OBJ(p_statement) S(",") OBJ(p_arguments) END
            VARIANT(3) OBJ(p_statement) END
        ;
        
        
        
        globParser.table[p_invertoken] = ParseStructure(p_invertoken)
            VARIANT(0) S(">") OBJ(-2) S("(") OBJ(p_tokargs) S(")") END
            VARIANT(1) S(">") OBJ(-2) END
            VARIANT(2) S("\\") OBJ(-3) S("\\") END
            VARIANT(4) S("[") OBJ(p_dictkeys) S("]") END
            VARIANT(3) OBJ(-2) END
        ;
        
        
        globParser.table[p_invertokargs] = ParseStructure(p_invertokargs)
            VARIANT(0) OBJ(p_token) S(",") OBJ(p_tokargs) END
            VARIANT(1) OBJ(p_token) END
        ;
        
        
        
        globParser.table[p_token] = ParseStructure(p_token)
            VARIANT(0) S(">") OBJ(-2) S("(") OBJ(p_tokargs) S(")") END
            VARIANT(0) S(">") OBJ(-2) S("(") OBJ(p_tokargs) S(")") END
            VARIANT(1) S(">") OBJ(-2) END
            VARIANT(1) S(">") OBJ(-2) END
            VARIANT(2) S("\\") OBJ(-3) S("\\") END
            VARIANT(4) S("[") OBJ(p_dictkeys) S("]") END
            VARIANT(3) OBJ(-2) END
        ;
        globParser.table[p_tokargs] = ParseStructure(p_tokargs)
            VARIANT(0) OBJ(p_token) S(",") OBJ(p_tokargs) END
            VARIANT(1) OBJ(p_token) END
        ;
        globParser.table[p_dictkeys] = ParseStructure(p_dictkeys)
            VARIANT(0) S("(") OBJ(-2) S("),") OBJ(p_dictkeys) END
            VARIANT(1) S("(") OBJ(-2) S(")") END
            VARIANT(2) OBJ(-2) S(",") OBJ(p_dictkeys) END
            VARIANT(3) OBJ(-2) END
        ;
        globParser.table[p_dictionary] = ParseStructure(p_dictionary)
            VARIANT(0) S("[") OBJ(p_dictkeys) S("]=") OBJ(p_statement) OBJ(p_dictionary) END
            VARIANT(1) END
        ;
        
        
        
        globParser.table[p_purestatement] = ParseStructure(p_purestatement)
            VARIANT(0) OBJ(-2) S("(") OBJ(p_purearguments) S(")") END
            VARIANT(1) OBJ(-2) END
        ;
        globParser.table[p_purearguments] = ParseStructure(p_purearguments)
            VARIANT(0) S("[") OBJ(p_stringlist) S("]") OBJ(p_purestatement) S(",") OBJ(p_purearguments) END
            VARIANT(1) S("[") OBJ(p_stringlist) S("]") OBJ(p_purestatement) END
            VARIANT(2) OBJ(p_purestatement) S(",") OBJ(p_purearguments) END
            VARIANT(3) OBJ(p_purestatement) END
        ;
        globParser.table[p_purestrategy] = ParseStructure(p_purestrategy)
            VARIANT(5) OBJ(p_purestatement) END
            VARIANT(4) OBJ(-2) S(":") OBJ(p_purestatement) END
            VARIANT(2) S("[") OBJ(p_puremodifiers) S("]") OBJ(p_purestatement) END
            VARIANT(1) S("[") OBJ(p_puremodifiers) S("]") OBJ(-2) S(":") OBJ(p_purestatement) END
            VARIANT(3) S("{") OBJ(p_puremodifiers) S("}") END
            VARIANT(0) S("[") OBJ(p_puremodifiers) S("]{") OBJ(p_puremodifiers) S("}") END
        ;
        globParser.table[p_puremodifiers] = ParseStructure(p_puremodifiers)
            VARIANT(0) OBJ(p_purestrategy) S(",") OBJ(p_puremodifiers) END
            VARIANT(1) OBJ(p_purestrategy) END
        ;
        
        
        globParser.table[p_stringlist] = ParseStructure(p_stringlist)
            VARIANT(0) OBJ(-2) S(",") OBJ(p_stringlist) END
            VARIANT(1) OBJ(-2) END
        ;
        globParser.table[p_deepstringlist] = ParseStructure(p_deepstringlist)
            VARIANT(0) S("[") OBJ(p_deepstringlist) S("]") OBJ(-2) S(",") OBJ(p_deepstringlist) END
            VARIANT(1) S("[") OBJ(p_deepstringlist) S("]") OBJ(-2) END
            VARIANT(2) OBJ(-2) S(",") OBJ(p_deepstringlist) END
            VARIANT(3) OBJ(-2) END
        ;
        #undef VARIANT
        #undef S
        #undef OBJ
        #undef END
        globParser.table[p_parse].englishname=     "Structures";
        globParser.table[p_variant].englishname=        "Variant";
        globParser.table[p_pattern].englishname=   "Pattern";
        globParser.table[p_tokdef].englishname=    "TokenDefinition";
        globParser.table[p_tokdefargs].englishname="TokenDefinitionArguments";
        globParser.table[p_conversion].englishname=     "Conversion";
        globParser.table[p_convarg].englishname=     "ConversionArguments";
        globParser.table[p_statement].englishname= "Statement";
        globParser.table[p_arguments].englishname= "Arguments";
        globParser.table[p_token].englishname= "Token";
        globParser.table[p_tokargs].englishname="TokenArguments";
        globParser.table[p_dictkeys].englishname= "DictionaryKeys";
        globParser.table[p_dictionary].englishname="Dictionary";
        globParser.table[p_purestatement].englishname="PureStatement";
        globParser.table[p_purearguments].englishname="PureArguments";
        globParser.table[p_purestrategy].englishname= "PureStrategy";
        globParser.table[p_puremodifiers].englishname="PureModifiers";
        globParser.table[p_stringlist].englishname="StringList";
        globParser.table[p_deepstringlist].englishname="TemplateFormat";
    }
    return globParser;
}
struct FullScope;
DictKeyset indexedKeysetConvert(ParseResult*,FullScope&);

void indexedLabelExtraction(vector<string>& ref,int& insertion,ParseResult* change) {
    if (change->struc==p_puremodifiers) {
        indexedLabelExtraction(ref,insertion,change->children[0]);
        if (change->var==0) indexedLabelExtraction(ref,insertion,change->children[1]);
    } else if (change->struc==p_purestrategy) {
        if      (change->var==1) {ref.insert(ref.begin()+insertion,change->children[0]->endpoint);insertion++;}
        else if (change->var==3) {ref.insert(ref.begin()+insertion,change->children[1]->endpoint);insertion++;}
        else if (change->var==4) indexedLabelExtraction(ref,insertion,change->children[0]);
        else if (change->var==5) indexedLabelExtraction(ref,insertion,change->children[1]);
        else {ref.push_back("");insertion++;}
    } else throw;
}
void indexedLabelExtraction(vector<string>& ref,ParseResult* change) {
    int idu=ref.size();
    return indexedLabelExtraction(ref,idu,change);
}
void indexedSubLabelExtraction(vector<string>* ref,int& pow,ParseResult* change) {
    if (change->struc==p_puremodifiers) {
        indexedSubLabelExtraction(ref,pow,change->children[0]);
        if (change->var==0) indexedSubLabelExtraction(ref,pow,change->children[1]);
    } else if (change->struc==p_purestrategy) {
        if (change->var==3||change->var==2) indexedLabelExtraction(ref[pow],change->children[0]);
        if (change->var==4) indexedSubLabelExtraction(ref,pow,change->children[0]);
        else if (change->var==5) {
            int du = pow;
            indexedSubLabelExtraction(ref,pow,change->children[1]);
            for (int h=pow;h<du;h++) {
                int idu = du;
                indexedLabelExtraction(ref[h],idu,change->children[0]);
            }
        } else pow++;
    } else throw;
}
void indexedSubLabelExtraction(vector<string>* ref,ParseResult* change) {
    int idu=0;
    return indexedSubLabelExtraction(ref,idu,change);
}

vector<string> parseStringList(ParseResult* o) {
    vector<string> golist;
    if (o->struc==p_stringlist) {
        while (true) {
            golist.push_back(o->children[0]->endpoint);
            if (o->var==1) break;
            o = o->children[1];
        }
    } else if (o->struc==p_pattern) {
        while (true) {
            if (o->var==1) golist.push_back(o->children[0]->endpoint);
            if (o->var==2) golist.push_back("");
            if (o->var==3) break;
            o = o->children[o->var==1?2:1];
        }
    } else if (o->struc==p_deepstringlist) {
        while (true) {
            if (o->var<2) golist.push_back(o->children[1]->endpoint);
            else golist.push_back(o->children[0]->endpoint);
            if (o->var==0) o = o->children[2];
            else if (o->var==2) o = o->children[1];
            else break;
        }
    } else if (o->struc==p_parse) {
        while (true) {
            if (o->var==0)      {golist.push_back(o->children[1]->endpoint);o=o->children[4];}
            else if (o->var==1) {golist.push_back(o->children[0]->endpoint);o=o->children[3];}
            else break;
        }
    } else throw;
    return golist;
}
inline int indexARA(ParseResult* change) {
    int amt=0;
    switch(change->struc) {
        case p_deepstringlist:      while(true) {amt++;if(change->var&1) return amt;change = change->children[change->var?1:2];}
        case p_tokdefargs:      while(true) {amt++;if(change->var&1) return amt;change = change->children[change->var?1:2];}
        case p_variant:         while(true) {if(change->var<3) return amt;amt++;change = change->children[3];}
        case p_arguments:       while(true) {amt++;if(change->var&1) return amt;change = change->children[change->var?1:2];}
        case p_dictionary:      while(true) {amt++;if(change->var)   return amt;change = change->children[3];}
        case p_purearguments:   while(true) {amt++;if(change->var&1) return amt;change = change->children[change->var?1:2];}
        case p_puremodifiers:   while(true) {amt++;if(change->var)   return amt;change = change->children[1];}
        case p_stringlist:      while(true) {amt++;if(change->var)   return amt;change = change->children[1];}
        case p_convarg:         while(true) {amt++;if(change->var>=10) return amt;change = change->children[change->var==1?3:2];}
    }
    throw;
}

template <typename T> class SmartPointer {
private:
    T*    pData;
    int* reference;
public:
    SmartPointer(T* pValue) : pData(pValue), reference(new int(1)) {}
    SmartPointer(const SmartPointer<T>& sp) : pData(sp.pData), reference(sp.reference) {(*reference)++;}
    ~SmartPointer() {if (--(*reference)==0) {delete pData;delete reference;}}
    SmartPointer<T>& operator = (const SmartPointer<T>& sp) {
        if (reference!=sp.reference) {
            if (--(*reference)==0) {delete pData;delete reference;}
            pData = sp.pData;
            reference = sp.reference;
            (*reference)++;
        }
        return *this;
    }
    T& operator*  (){return *pData;}
    T* operator-> (){return pData;}
    SmartPointer deepcopy() {return SmartPointer(new T(*pData));}
};
struct SingleScope {
    SmartPointer<map<string,pair<int,int>>> tokens;
    int level=0;
    SingleScope(SmartPointer<map<string,pair<int,int>>> vb,int l) :tokens(vb),level(l) {}
    SingleScope():tokens(new map<string,pair<int,int>>()){}
    SingleScope(const vector<string>& da) : tokens(new map<string,pair<int,int>>()),level(1) {
        for (int id=0;id<da.size();id++) (*tokens)[da[id]] = {id,0};
    }
    SingleScope levelup() {return SingleScope(tokens,level+1);}
    SingleScope levelup(ParseResult* pr) {
        return levelup(parseStringList(pr));
    }
    SingleScope levelup(const vector<string>& da) {
        SingleScope sc = SingleScope(tokens.deepcopy(),level+1);
        for (int id=0;id<da.size();id++) (*tokens)[da[id]] = {id,level};
        return sc;
    }
    int id(ParseResult* pr)  {return (*tokens)[pr->endpoint].first;}
    int loc(ParseResult* pr) {return (*tokens)[pr->endpoint].second;}
    pair<int,int> idloc(ParseResult* pr) {return (*tokens)[pr->endpoint];}
};
struct FullScope {
    SmartPointer<map<string,vector<int>>> gorman;
    SmartPointer<map<string,StatPath>> dedman;
    SingleScope tok;
    SingleScope stat;
    ParseSpecifier* parser;
    FullScope(MetaBank* mb,ParseSpecifier* p,const vector<string>& ips) : parser(p),gorman(new map<string,vector<int>>()),dedman(new map<string,StatPath>()),stat(mb->stratnames),tok(ips) {
        (*tok.tokens)["NUMBER"] = {0,-1};
        (*tok.tokens)["STRING"] = {1,-1};
        (*tok.tokens)["LITERAL"] = {2,-1};
    }
    FullScope(SingleScope t,SingleScope s,SmartPointer<map<string,vector<int>>> c,SmartPointer<map<string,StatPath>> d,ParseSpecifier* p) : tok(t),stat(s),gorman(c),dedman(d),parser(p) {}
    FullScope levelupStat()                {return FullScope(tok,stat.levelup(  ),gorman,dedman,parser);}
    FullScope levelupStat(ParseResult* pr) {return FullScope(tok,stat.levelup(pr),gorman,dedman,parser);}
    FullScope levelupStat(const vector<string>& da) {return FullScope(tok,stat.levelup(da),gorman,dedman,parser);}
    FullScope levelupTok()                {return FullScope(tok.levelup(  ),stat,gorman,dedman,parser);}
    FullScope levelupTok(ParseResult* pr) {return FullScope(tok.levelup(pr),stat,gorman,dedman,parser);}
    FullScope levelupTok(const vector<string>& da) {return FullScope(tok.levelup(da),stat,gorman,dedman,parser);}
    vector<int> paraRef(ParseResult* pr) {return (*gorman)[pr->endpoint];}
    StatPath statRef(ParseResult* pr) {return (*dedman)[pr->endpoint];}
    FullScope introduce(ParseResult* pr) {
        if (pr->struc!=p_convarg) throw;
        if (pr->var==0) return introduce(pr->children[1]);
        if (pr->var==1) return introduce(pr->children[2]);
        if (pr->var==2) {
            vector<string> da = parseStringList(pr->children[2]);
            if (gorman->count(pr->children[0]->endpoint)) {
                FullScope sc = FullScope(tok,stat,gorman.deepcopy(),dedman,parser);
                vector<int> idloc = paraRef(pr->children[0]);
                for (int id=0;id<da.size();id++) {
                    (*sc.gorman)[da[id]] = idloc;
                    (*sc.gorman)[da[id]].push_back(id);
                }
                return sc.introduce(pr->children[3]);
            } else if (dedman->count(pr->children[0]->endpoint)) {
                FullScope sc = FullScope(tok,stat,gorman,dedman.deepcopy(),parser);
                StatPath idloc = statRef(pr->children[0]);
                for (int id=0;id<da.size();id++) {
                    (*sc.dedman)[da[id]] = idloc;
                    (*sc.dedman)[da[id]].pan.push_back(id);
                }
                return sc.introduce(pr->children[3]);
            } else throw;
        }
        if (pr->var==3) return introduce(pr->children[2]);//when type safety is added, this line must do more
        if (pr->var==4) return introduce(pr->children[2]);
        if (pr->var==5) {
            vector<string> da = parseStringList(pr->children[2]);
            FullScope sc = FullScope(tok,stat,gorman,dedman.deepcopy(),parser);
            DictKeyset idloc = indexedKeysetConvert(pr->children[0],*this);
            for (int id=0;id<da.size();id++) {
                (*sc.dedman)[da[id]] = StatPath(idloc,{id});
            }
            return sc.introduce(pr->children[3]);
        }
        if (pr->var==6) return introduce(pr->children[2]);
        if (pr->var==7) return introduce(pr->children[2]);
        if (pr->var==8) return introduce(pr->children[2]);
        return *this;
    }
    FullScope introduce(string s,vector<int> v) {
        FullScope sc = FullScope(tok,stat,gorman.deepcopy(),dedman,parser);
        (*sc.gorman)[s] = v;
        return sc;
    }
};






Statement indexedPureStatementConvert(ParseResult* change,SingleScope scope) {
    if (change->struc!=p_purestatement) throw;
    if (change->var==0) {
        Statement head = Statement(scope.id(change->children[0]),scope.loc(change->children[0]),indexARA(change->children[1]));
        int n=0;
        for (ParseResult* pa=change->children[1];;) {
            if (pa->var<2) head.args[n++] = indexedPureStatementConvert(pa->children[1],scope.levelup(pa->children[0]));
            else           head.args[n++] = indexedPureStatementConvert(pa->children[0],scope.levelup());
            if      (pa->var==0) pa=pa->children[2];
            else if (pa->var==2) pa=pa->children[1];
            else break;
        }
        return head;
    } else {
        return Statement(scope.id(change->children[0]),scope.loc(change->children[0]));
    }
}
void indexedPureStrategyConvert(ParseResult* change,vector<Strategy>& exargs,vector<Strategy>& list,SingleScope scope,int paint) {//pass one greater than you were
    if (change->struc!=p_purestrategy) throw;
    switch (change->var) {
        case 0: {
            list.push_back(Strategy(indexedPureStatementConvert(change->children[1],scope.levelup()),paint,scope.level-1));
            break;
        } case 1: {
            (*scope.tokens)[change->children[0]->endpoint] = {paint,scope.level-1};
            list.push_back(Strategy(indexedPureStatementConvert(change->children[1],scope.levelup()),paint,scope.level-1));
            break;
        } case 2: {
            vector<Strategy> fargs = exargs;
            vector<Strategy> trash;
            ParseResult* pa = change->children[0];
            for (int npa=0;;pa=pa->children[1]) {
                indexedPureStrategyConvert(pa->children[0],trash,fargs,scope.levelup(),npa++);
                if (pa->var!=0) break;
            }
            Strategy res = Strategy(indexedPureStatementConvert(change->children[1],scope.levelup()),paint,scope.level-1,fargs.size());
            for (int j=0;j<fargs.size();j++) {
                res.args[j] = fargs[j];
            }
            list.push_back(res);
            break;
        } case 3: {
            vector<Strategy> fargs = exargs;
            vector<Strategy> trash;
            ParseResult* pa = change->children[0];
            for (int npa=0;;pa=pa->children[1]) {
                indexedPureStrategyConvert(pa->children[0],trash,fargs,scope.levelup(),npa++);
                if (pa->var!=0) break;
            }
            Strategy res = Strategy(indexedPureStatementConvert(change->children[2],scope.levelup()),paint,scope.level-1,fargs.size());
            for (int j=0;j<fargs.size();j++) {
                res.args[j] = fargs[j];
            }
            (*scope.tokens)[change->children[1]->endpoint] = {paint,scope.level-1};
            list.push_back(res);
            break;
        } case 4: {
            ParseResult* pa = change->children[0];
            for (int npa=paint;;pa=pa->children[1]) {
                indexedPureStrategyConvert(pa->children[0],exargs,list,scope,npa++);
                if (pa->var!=0) break;
            }
            break;
        } case 5: {
            vector<Strategy> fargs = exargs;
            vector<Strategy> trash;
            ParseResult* pa = change->children[0];
            for (int npa=0;;pa=pa->children[1]) {
                indexedPureStrategyConvert(pa->children[0],trash,fargs,scope.levelup(),npa++);
                if (pa->var!=0) break;
            }
            pa = change->children[1];
            for (int npa=paint;;pa=pa->children[1]) {
                indexedPureStrategyConvert(pa->children[0],fargs,list,scope,npa++);
                if (pa->var!=0) break;
            }
            break;
        }
    }
}
ParseArtifact indexedTokdefConvert(ParseResult* change,SingleScope scope) {
    if (change->struc!=p_tokdef) throw;
    if (change->var==0) return ParseArtifact(p_statement,0,0,0);
    int ara=0;
    ParseArtifact* args=0;
    if (change->var==1) {
        ara = indexARA(change->children[1]);
        args = new ParseArtifact[ara];
        int mt = 0;
        for (ParseResult* pa = change->children[1];;) {
            if (change->var<2) args[mt++] = indexedTokdefConvert(pa->children[1],scope.levelup(pa->children[0]));
            else               args[mt++] = indexedTokdefConvert(pa->children[0],scope.levelup());
            if(change->var&1) break;
            pa = pa->children[change->var?1:2];
        }
    }
    return ParseArtifact(scope.id(change->children[0]),scope.loc(change->children[0]),ara,args);
}







//        globParser.table[p_convarg] = ParseStructure(p_convarg)
//            VARIANT(0) S("-[") OBJ(p_dictkeys) S("]") OBJ(p_convarg) END
//            VARIANT(1) S("-") OBJ(-2) S("~") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(2) S("-") OBJ(-2) S("~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END
//            VARIANT(3) S("-") OBJ(-2) S("(") OBJ(p_stringlist) S(")") OBJ(p_convarg) END
//            VARIANT(4) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(5) S("-[") OBJ(p_dictkeys) S("]~") OBJ(-2) S("[") OBJ(p_stringlist) S("]") OBJ(p_convarg) END
//            VARIANT(6) S("-[") OBJ(p_dictkeys) S("](") OBJ(p_stringlist) S(")") OBJ(p_convarg) END
//            VARIANT(7) S("-") OBJ(-2) S("=(") OBJ(-2) S(")") OBJ(p_convarg) END
//            VARIANT(8) S("-") OBJ(-2) S("=") OBJ(-2) OBJ(p_convarg) END
//            VARIANT(9) END
//        ;


DictKeyset indexedKeysetConvert(ParseResult* change,FullScope& scope) {
    if (change->struc!=p_dictkeys) throw;
    DictKeyset ah;
    while (true) {
        if (change->var<2) ah.gan.push_back({change->children[0]->endpoint,{}});
        else               ah.gan.push_back({"",scope.paraRef(change->children[0])});
        if (change->var%2==0) change = change->children[1];
    }
    return ah;
}
TokenConstruction indexedTokenConvert(ParseResult* change,FullScope& scope) {
    if (change->struc!=p_token) throw;
    if (change->var == 2) return TokenConstruction(change->children[0]->endpoint);
    if (change->var == 3) return TokenConstruction(scope.paraRef(change->children[0]));
    int ara = 0;
    TokenConstruction* args = 0;
    if (change->var == 0) {
        ParseResult* pa = change->children[1];
        ara = indexARA(pa);
        args = new TokenConstruction[ara];
        for (int n=0;;n++) {
            args[n] = indexedTokenConvert(pa->children[0],scope);
            if (pa->var==0) pa=pa->children[1];
            else break;
        }
    }
    return TokenConstruction(change->children[0]->endpoint,ara,args);
}
Construction indexedStatementConvert(ParseResult* change,FullScope scope) {
    if (change->struc!=p_statement) throw;
    int ara = 0;
    Construction* args = 0;
    if (change->var%2==0) {
        ParseResult* pa = change->children[change->var==0?4:1];
        ara = indexARA(pa);
        args = new Construction[ara];
        for (int n=0;;n++) {
            if (pa->var<2) args[n] = indexedStatementConvert(pa->children[1],scope.levelupStat(pa->children[0]));
            else           args[n] = indexedStatementConvert(pa->children[0],scope.levelupStat());
            if      (pa->var==0) pa=pa->children[2];
            else if (pa->var==2) pa=pa->children[1];
            else break;
        }
    }
    if (change->var<2) {
        ParseResult* pa = change->children[2];
        int dara = indexARA(pa);
        DictEntry* dargs = new DictEntry[dara];
        for (int dn=0;;dn++) {
            if (pa->var==0) {
                dargs[dn] = DictEntry(indexedKeysetConvert(pa->children[0],scope),indexedStatementConvert(pa->children[1],scope));
                pa = pa->children[2];
            } else break;
        }
        return Construction(indexedTokdefConvert(change->children[0],scope.tok),
                            indexedTokenConvert(change->children[1],scope),
                            dara,dargs,
                            change->children[3]->endpoint,
                            ara,args);
    } else if (change->var<4) return Construction(indexedKeysetConvert(change->children[0],scope),ara,args);
    else {
        if (scope.stat.tokens->count(change->children[0]->endpoint)) {
            return Construction(scope.stat.id(change->children[0]),scope.stat.loc(change->children[0]),ara,args);
        } else if (scope.dedman->count(change->children[0]->endpoint)) {
            return Construction(scope.statRef(change->children[0]),ara,args);
        } else throw;
    }
}
Conversion makeconvert(ParseResult* change,FullScope scope) {
    if (change->struc!=p_conversion) throw;
    if (change->var==0) {
        ParseResult* pa = change->children[0];
        int ara = indexARA(pa);
        ConversionValidator* args = new ConversionValidator[ara];
        for (int n=0;;n++) {
            if (pa->var%10<2) args[n] = ConversionValidator(scope.paraRef(pa->children[0]),pa->children[1]->endpoint);
            else if (pa->var%10==2 || pa->var%10==6) args[n] = ConversionValidator(pa->children[0]->endpoint     ,scope.paraRef(pa->children[1]),pa->var%10>5);
            else if (pa->var%10==3 || pa->var%10==7) args[n] = ConversionValidator(pa->children[0]->endpoint     ,pa->children[1]->endpoint     ,pa->var%10>5);
            else if (pa->var%10==4 || pa->var%10==8) args[n] = ConversionValidator(scope.paraRef(pa->children[0]),pa->children[1]->endpoint     ,pa->var%10>5);
            else if (pa->var%10==5 || pa->var%10==9) args[n] = ConversionValidator(scope.paraRef(pa->children[0]),scope.paraRef(pa->children[1]),pa->var%10>5);
            if      (pa->var==1) pa=pa->children[3];
            else if (pa->var<10) pa=pa->children[2];
            else break;
        }
        Conversion bot = makeconvert(change->children[2],scope);
        ConversionChoice android(makeconvert(change->children[1],scope.introduce(change->children[0])),ara,args);
        bot.choices.insert(bot.choices.begin(),android);
        return bot;
    }
    if (change->var==1) return Conversion();
    if (change->var==2) return Conversion(indexedStatementConvert(change->children[0],scope));
    throw;
}
void definePattern(ParseResult* change,FullScope& scope,ParseVariant& addin) {
    if (change->struc!=p_pattern) throw;
    if (change->var==0) addin.then(scope.parser->tz(change->children[0]->endpoint));
    if (change->var==1) {
        scope = scope.introduce(change->children[0]->endpoint,{(int)addin.argmappings.size()});
        addin.then(indexedTokdefConvert(change->children[1],scope.tok));
    }
    if (change->var==2) addin.then(indexedTokdefConvert(change->children[0],scope.tok));
    if (change->var!=3) definePattern(change->children[change->var==1?2:1],scope,addin);
}
void listvariants(ParseResult* change,FullScope scope,ParseStructure& addin,int order,vector<string>* namespaces,int cosn) {
    if (change->struc!=p_variant) throw;
    if (change->var!=3) {
        order = change->var==2?order:order+1;
        if (change->var==0) addin.flippedorders[order]=true;
        ParseVariant newvar = ParseVariant(order,change->children[0]->endpoint);
        definePattern(change->children[1],scope,newvar);
        ParseResult* pa = change->children[2];
        if (indexARA(pa)!=cosn) throw;
        int n=0;
        while (pa->var==1) {
            FullScope nexcope = scope.levelupStat(namespaces[n]);//should be two stat levels
            newvar.converters.push_back(makeconvert(pa->children[0],nexcope));
            pa=pa->children[1];
            n++;
        }
        addin.variant(newvar);
        listvariants(change->children[2],scope,addin,order,namespaces,cosn);
    }
}
void liststrucs(ParseResult* change,FullScope scope,int resid) {
    if (change->struc!=p_parse) throw;
    if (change->var==2) return;
    scope.parser->table[resid] = ParseStructure(resid);
    scope.parser->table[resid].englishname = change->children[change->var==0?1:0]->endpoint;

    vector<Strategy> trash;
    indexedPureStrategyConvert(change->children[change->var==0?1:0],trash,scope.parser->table[resid].types,scope.stat,0);
    indexedLabelExtraction(scope.parser->table[resid].typenames,change->children[change->var==0?1:0]);
    if (change->var==0) scope = scope.levelupTok(change->children[0]);
    vector<string>* scopematrix = new vector<string>[scope.parser->table[resid].types.size()];
    indexedSubLabelExtraction(scopematrix,change->children[change->var==0?2:1]);
    
    //should be extension of current table? ~ refers to statement, then. Index here starts later on. Copy this thing with it's time create a new one
    listvariants(change->children[2],scope,scope.parser->table[resid],0,scopematrix,scope.parser->table[resid].types.size());
    liststrucs(change->children[3],scope,resid+1);
}

ParseSpecifier& getLangFile(const string& filename) {
    static map<string,ParseSpecifier> globparsers;
    if (globparsers.find(filename)==globparsers.end()) {
        map<string,string> comments;
        map<string,string> literals;
        string tokenlist="";
        std::ifstream file(filename);
        string strn((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
        bool hithash=false;
        bool key=false;
        int line=0;
        string buf="";
        string cumul="";
        for (int u=0;u<strn.size();u++) {
            if (hithash) {
                if (line==2 and strn[u]!='\n') tokenlist+=strn[u];
                else {
                    if (strn[u]==':') {
                        if (key) throw;
                        buf=cumul;
                        cumul="";
                        key=true;
                    } else if (strn[u]==',') {
                        if (!key) throw;
                        if (line==0) {
                            comments[buf]=cumul;
                        } else if (line==1) {
                            literals[buf]=cumul;
                        }
                        cumul="";
                        key=false;
                    } else if (strn[u]!='\n' and strn[u]!=' ') {
                        cumul+=strn[u];
                    }
                }
                if (strn[u]=='\n') {
                    hithash=false;
                    if (line++==3) break;
                    if (!key and cumul.size()) {
                        if (line==1) {
                            comments[cumul]="\n";
                        } else throw;
                    } else {
                        if (line==0) {
                            comments[buf]=cumul;
                        } else if (line==1) {
                            literals[buf]=cumul;
                        }
                    }
                    key=false;
                    cumul="";
                    continue;
                }
            } else if (strn[u]=='#') hithash=true;
        }
        globparsers[filename] = parse_parser(comments,literals,tokenlist,strn);
    }
    return globparsers[filename];
}
ParseSpecifier parse_parser(map<string,string> comments,map<string,string> literals,const string & tokenlist,const string & parse) {
    ParseResult* tokenized = getGlobParser().parse(p_parse, parse);
    ParseSpecifier result = getGlobParser();
    string cur="";
    for (int i=0;i<=tokenlist.length();i++) {
        char cc=i==tokenlist.length()?' ':tokenlist[i];
        if ((cc==' ' or cc=='\n') and cur.length()>0) {result.registerToken(cur);cur="";}
        else if (cc!=' ' and cc!='\n') cur+=cc;
    }
    for (auto it=comments.begin();it!=comments.end();it++) result.registerComment(it->first,it->second);
    for (auto it=literals.begin();it!=literals.end();it++) result.registerLiteral(it->first,it->second);
    
    FullScope hra(&MetaBank::meta_prime,&result,parseStringList(tokenized));
    liststrucs(tokenized,hra,getGlobParser().maxSpec());
    tokenized->cleanup();
    return result;
}

vector<Strategy> parse_TTML(const string & parse,const vector<vector<string>>& scope,vector<string>* pop) {
    ParseResult* tokenized = getGlobParser().parse(p_purestrategy, parse);
    if (pop) indexedLabelExtraction(*pop,tokenized->children[0]);
    SingleScope barb;
    for (int y=0;y<scope.size();y++) barb = barb.levelup(scope[y]);
    vector<Strategy> result;
    vector<Strategy> trash;
    indexedPureStrategyConvert(tokenized,trash,result,barb,0);
    return result;
}
vector<Strategy> parse_TTML(const string& parse,const vector<vector<string>>& scope) {
    return parse_TTML(parse,scope,0);
}
vector<Strategy> parse_TTML(const string & parse,vector<string>* pop) {
    return parse_TTML(parse,{},pop);
}
vector<Strategy> parse_TTML(const string& parse) {
    return parse_TTML(parse,{},0);
}

#undef p_parse
#undef p_variant
#undef p_pattern
#undef p_tokdef
#undef p_tokdefargs
#undef p_conversion
#undef p_convarg
#undef p_statement
#undef p_arguments
#undef p_token
#undef p_tokargs
#undef p_dictkeys
#undef p_dictionary
#undef p_purestatement
#undef p_purearguments
#undef p_purestrategy
#undef p_puremodifiers
#undef p_stringlist
#undef p_deepstringlist
