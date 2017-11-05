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

#define p_parse 0
#define p_variant 1
#define p_nocvariant 2
#define p_conversion 3
#define p_statement 4
#define p_arguments 5
#define p_pattern 6
#define p_token 7
#define p_tokstring 8
#define p_tokargs 9
#define p_dictionary 10
#define p_purestrategy 11
#define p_purestatement 12
#define p_purearguments 13
#define p_puremodifiers 14

ParseSpecifier& getGlobParser() {
    static ParseSpecifier globParser;
    static bool created=false;
    if (!created) {
        created=true;
        #define TOK(x) globParser.registerToken(x);
        TOK("{") TOK("}") TOK("\\") TOK("\\\\") TOK("-") TOK("~") TOK("/") TOK(",") TOK(".") TOK("enum") TOK(";")
        TOK(":") TOK("<") TOK(">") TOK("(") TOK(")") TOK("[") TOK("]") TOK("|") TOK("null") TOK("flipped") TOK("=")
        #undef TOK
        #define VARIANT(x) .variant(ParseVariant(x)
        #define S(x) .then(globParser.tz(x))
        #define OBJ(x) .then(x)
        #define END )
        globParser.registerLiteral("\\","\\");
        globParser.registerComment("#","\n");
        globParser.table[p_parse] = ParseStructure(p_parse)
            VARIANT(0) OBJ(-2) S(":enum{") OBJ(p_variant) S("}") OBJ(p_parse) END
            VARIANT(1) OBJ(-2) S(":") OBJ(p_purestrategy) S("{") OBJ(p_variant) S("}") OBJ(p_parse) END
            VARIANT(2) OBJ(-2) S("{") OBJ(p_nocvariant) S("}") OBJ(p_parse) END
            VARIANT(3) END
        ;
        globParser.table[p_variant] = ParseStructure(p_variant)
            VARIANT(0) S("-") OBJ(p_pattern) S("{") OBJ(p_conversion) S("}") OBJ(p_variant) END
            VARIANT(1) S("-") OBJ(p_pattern) S("flipped{") OBJ(p_conversion) S("}") OBJ(p_variant) END
            VARIANT(2) OBJ(p_pattern) S("{") OBJ(p_conversion) S("}") OBJ(p_variant) END
            VARIANT(3) END
        ;
        globParser.table[p_nocvariant] = ParseStructure(p_nocvariant)
            VARIANT(0) S("-") OBJ(p_pattern) S(";") OBJ(p_nocvariant) END
            VARIANT(1) S("-") OBJ(p_pattern) S("flipped;") OBJ(p_nocvariant) END
            VARIANT(2) OBJ(p_pattern) S(";") OBJ(p_nocvariant) END
            VARIANT(3) END
        ;
        globParser.table[p_conversion] = ParseStructure(p_conversion)
            VARIANT(0) S("-") OBJ(p_tokstring) S("~") OBJ(-1) S("{") OBJ(p_conversion) S("}") OBJ(p_conversion) END
            VARIANT(1) S("null") END
            VARIANT(2) OBJ(-1) END
            VARIANT(3) OBJ(p_statement) END
        ;
        globParser.table[p_statement] = ParseStructure(p_statement)
            VARIANT(0) S("{") OBJ(-1) S("/") OBJ(-1) S("}") S("(") OBJ(p_arguments) S(")") END
            VARIANT(1) S("{") OBJ(-1) S("/") OBJ(-1) S("}") END
            VARIANT(2) S("<") OBJ(-1) S("/") OBJ(-1) S(">") S("(") OBJ(p_arguments) S(")") END
            VARIANT(3) S("<") OBJ(-1) S("/") OBJ(-1) S(">") END
            VARIANT(4) S("[[]]") S("(") OBJ(p_arguments) S(")") END
            VARIANT(5) S("[[]]") END
            VARIANT(6) OBJ(-2) S("(") OBJ(p_arguments) S(")") END
            VARIANT(7) OBJ(-2) END
            VARIANT(8) OBJ(p_token) S("(") OBJ(p_arguments) S(")") END
            VARIANT(9) OBJ(p_token) END
        ;
        globParser.table[p_token] = ParseStructure(p_token)
            VARIANT(0) S("[") OBJ(-2) OBJ(-1) OBJ(p_dictionary) S("|") OBJ(p_tokargs) S("]") END
            VARIANT(1) S("[") OBJ(-2) OBJ(-1) OBJ(p_dictionary) S("]") END
            VARIANT(3) S("[") OBJ(p_tokstring)  OBJ(p_dictionary) S("]") END
            VARIANT(2) S("[") OBJ(-2) S(":") OBJ(p_token) S("]") END
            VARIANT(4) OBJ(p_tokstring) END
        ;
        globParser.table[p_pattern] = ParseStructure(p_pattern)
            VARIANT(0) S("\\") OBJ(-3) S("\\") OBJ(p_pattern) END
            VARIANT(1) OBJ(-2) OBJ(p_pattern) END
            VARIANT(2) END
        ;
        globParser.table[p_dictionary] = ParseStructure(p_dictionary)
            VARIANT(0) S(";") OBJ(-2) OBJ(p_token) S("=") OBJ(p_statement) OBJ(p_dictionary) END
            VARIANT(1) END
        ;
        globParser.table[p_tokstring] = ParseStructure(p_tokstring)
            VARIANT(0) S(".") OBJ(-1) OBJ(p_tokstring) END
            VARIANT(1) S(".") END
        ;
        globParser.table[p_arguments] = ParseStructure(p_arguments)
            VARIANT(0) OBJ(p_statement) S(",") OBJ(p_arguments) END
            VARIANT(1) OBJ(p_statement) END
        ;
        globParser.table[p_tokargs] = ParseStructure(p_tokargs)
            VARIANT(0) OBJ(p_token) S(",") OBJ(p_tokargs) END
            VARIANT(1) OBJ(p_token) END
        ;
        globParser.table[p_purestrategy] = ParseStructure(p_purestrategy)
            VARIANT(6) OBJ(p_purestatement) END
            VARIANT(5) OBJ(-2) S(":") OBJ(p_purestatement) END
            VARIANT(3) S("[") OBJ(p_puremodifiers) S("]") OBJ(p_purestatement) END
            VARIANT(2) S("[") OBJ(p_puremodifiers) S("]") OBJ(-2) S(":") OBJ(p_purestatement) END
            VARIANT(4) S("[") OBJ(p_puremodifiers) S("]") END
        ;
        globParser.table[p_purestatement] = ParseStructure(p_purestatement)
            VARIANT(0) S("{") OBJ(-1) S("/") OBJ(-1) S(":") OBJ(p_purestatement) S("}") S("(") OBJ(p_purearguments) S(")") END
            VARIANT(1) S("{") OBJ(-1) S("/") OBJ(-1) S(":") OBJ(p_purestatement) S("}") END
            VARIANT(3) OBJ(-2) S("(") OBJ(p_purearguments) S(")") END
            VARIANT(4) OBJ(-2) END
            VARIANT(2) S("<") OBJ(-1) S("/") OBJ(-1) S(":") OBJ(p_purestatement) S(">") END
        ;
        globParser.table[p_puremodifiers] = ParseStructure(p_puremodifiers)
            VARIANT(0) OBJ(p_purestrategy) S("|") OBJ(p_puremodifiers) END
            VARIANT(1) OBJ(p_purestrategy) END
        ;
        globParser.table[p_purearguments] = ParseStructure(p_purearguments)
            VARIANT(0) OBJ(p_purestatement) S(",") OBJ(p_purearguments) END
            VARIANT(1) OBJ(p_purestatement) END
        ;
        #undef VARIANT
        #undef S
        #undef OBJ
        #undef END
        globParser.table[p_parse].englishname=        "Structures";
        globParser.table[p_variant].englishname=      "Variant";
        globParser.table[p_nocvariant].englishname=   "NoConversionVariant";
        globParser.table[p_conversion].englishname=   "Conversion";
        globParser.table[p_statement].englishname=    "Statement";
        globParser.table[p_arguments].englishname=    "Arguments";
        globParser.table[p_pattern].englishname=      "Pattern";
        globParser.table[p_token].englishname=        "Token";
        globParser.table[p_tokstring].englishname=    "TokenString";
        globParser.table[p_tokargs].englishname=      "TokenArguments";
        globParser.table[p_dictionary].englishname=   "Dictionary";
        globParser.table[p_purestrategy].englishname= "PureStrategy";
        globParser.table[p_purestatement].englishname="PureStatement";
        globParser.table[p_purearguments].englishname="PureArguments";
        globParser.table[p_puremodifiers].englishname="PureModifiers";
    }
    return globParser;
}





Statement* indexedPureStatementConvert(MetaBank*,ParseResult*,std::map<std::string,Statement*>&,int);
void indexedPureCoalesceStatements(MetaBank* mb,ParseResult* change,std::map<std::string,Statement*>& varbank,std::vector<Statement*>* list,int stat) {
    list->push_back(indexedPureStatementConvert(mb,change->children[0],varbank,stat));
    if (change->var==0) {
        indexedPureCoalesceStatements(mb,change->children[1],varbank,list,stat);
    }
}
Statement* indexedPureStatementConvert(MetaBank* mb,ParseResult* change,std::map<std::string,Statement*>& varbank,int stat) {
    if (change->struc!=p_purestatement) throw;
    Statement* head=0;
    if (change->var<2) {
        head = new Statement(std::stoi(change->children[0]->endpoint),std::stoi(change->children[1]->endpoint));
        head->type = indexedPureStatementConvert(mb,change->children[2],varbank,stat+1);
        if (change->var==0) {
            indexedPureCoalesceStatements(mb,change->children[3],varbank,&head->args,stat+1);
        }
        return head;
    } else if (change->var<4) {
    
        int lp1 = varbank[change->children[0]->endpoint]->local+1;
        Statement* roottype = varbank[change->children[0]->endpoint]->depth_push(lp1,stat-(lp1),0);
        if (change->var==2) {
//            head = new Statement(roottype->id,roottype->local);
            std::vector<Statement*> fargs;
            indexedPureCoalesceStatements(mb,change->children[1],varbank,&fargs,stat+1);
            std::string traceback="";
            
            head = roottype->safe_substitute_level(&fargs,stat,stat+1,0,0,true,traceback);
            if (head==0) {
                std::cout<<"\nTraceback:\n"<<roottype->tostringheavy()<<"\n"<<traceback<<"\n";
                throw;
            }
            head->erase_deltasub();
//            std::cout<<"-=-=-=-=-="<<stat<<"\n";
//            for (int e=0;e<fargs.size();e++) {
//                std::cout<<"\t"<<fargs[e]->tostringdoubleheavy()<<"\n";
//            }
//            std::cout<<roottype->tostringdoubleheavy()<<" --> "<<head->tostringdoubleheavy()<<"\n";
            return head;
        } else {
            return roottype;
        }
    } else if (change->var==4) {
        head = new Statement(std::stoi(change->children[1]->endpoint),0);
        head->specifier = std::stoi(change->children[0]->endpoint);
        head->type = indexedPureStatementConvert(mb,change->children[2],varbank,stat+1);
        return head;
    }
    throw;
}



Statement* indexedPureStrategyConvert(MetaBank*,ParseResult*,std::map<std::string,Statement*>&,int paint,int depth);
void indexedPureCoalesceStrategies(MetaBank* mb,ParseResult* change,std::map<std::string,Statement*>& varbank,std::vector<Statement*>* list,int paint,int depth) {
    Statement* uwu = indexedPureStrategyConvert(mb,change->children[0],varbank,paint,depth);
    list->push_back(uwu);
    if (change->var==0) {
        indexedPureCoalesceStrategies(mb,change->children[1],varbank,list,paint+1,depth);
    }
}
Statement* indexedPureStrategyConvert(MetaBank* mb,ParseResult* change,std::map<std::string,Statement*>& varbank,int paint,int depth) {
    if (change->struc!=p_purestrategy) throw;
    Statement* res;
    switch (change->var) {
        case 0:
            res = new Statement(paint,depth);
            res->type = indexedPureStatementConvert(mb,change->children[0],varbank,depth+2);
            return res;
        case 1:
            if (depth==0) {
                mb->stratnames.push_back(change->children[0]->endpoint);
            }
            res = new Statement(paint,depth);
            res->type = indexedPureStatementConvert(mb,change->children[1],varbank,depth+2);
            varbank[change->children[0]->endpoint] = res;
//            std::cout<<change->children[0]->endpoint<<" = "<<res->tostringdoubleheavy()<<"\n";
            return res;
        case 2:
            res = new Statement(paint,depth);
            indexedPureCoalesceStrategies(mb,change->children[0],varbank,&res->args,0,depth+1);
            res->type = indexedPureStatementConvert(mb,change->children[1],varbank,depth+2);
            return res;
        case 3:
            if (depth==0) {
                mb->stratnames.push_back(change->children[1]->endpoint);
            }
            res = new Statement(paint,depth);
            indexedPureCoalesceStrategies(mb,change->children[0],varbank,&res->args,0,depth+1);
            res->type = indexedPureStatementConvert(mb,change->children[2],varbank,depth+2);
            varbank[change->children[1]->endpoint] = res;
//            std::cout<<change->children[1]->endpoint<<" = "<<res->tostringdoubleheavy()<<"\n";
            return res;
        case 4:
            res = new Statement(-1,depth);
            res->type = 0;
            indexedPureCoalesceStrategies(mb,change->children[0],varbank,&res->args,0,depth+1);
            return res;
    }
    throw;
}

Construction indexedStatementConvert(MetaBank*,ParseResult*,std::map<std::string,int>& handle);
void indexedCoalesceArgReference(ParseResult* tokenized,std::vector<int>* list) {
    if (tokenized->var==0) {
        list->push_back(std::stoi(tokenized->children[0]->endpoint));
        indexedCoalesceArgReference(tokenized->children[1],list);
    }
}
ConstructArgReference makeargreference(ParseResult* tokenized) {
    if (tokenized->struc!=p_tokstring) throw;
    ConstructArgReference res;
    indexedCoalesceArgReference(tokenized,&res.path);
    return res;
}
Construction indexedTokenConvert(MetaBank*,ParseResult*,std::map<std::string,int>& handle);
void indexedCoalesceTokens(MetaBank* mb,ParseResult* change,std::vector<Construction>* list,std::map<std::string,int>& handle) {
    list->push_back(indexedTokenConvert(mb,change->children[0],handle));
    if (change->var==0) {
        indexedCoalesceTokens(mb,change->children[1],list,handle);
    }
}
void indexedCoalesceDictionary(MetaBank* mb,ParseResult* change,std::vector<DictEntry>* entries,std::map<std::string,int>& handle) {
    if (change->var==0) {
        DictEntry n;
        n.channel = change->children[0]->endpoint;
        n.a = indexedTokenConvert(mb,change->children[1],handle);
        n.b = indexedStatementConvert(mb,change->children[2],handle);
        entries->push_back(n);
        
        indexedCoalesceDictionary(mb,change->children[3],entries,handle);
    }
}
Construction indexedTokenConvert(MetaBank* mb,ParseResult* change,std::map<std::string,int>& handle) {
    if (change->struc!=p_token) throw;
    Construction token;
    if (change->var==0) {
        if (handle.find(change->children[0]->endpoint)==handle.end()) throw;
        token.strucLocal = handle[change->children[0]->endpoint];
        token.varID = std::stoi(change->children[1]->endpoint);
        token.reconstruct=1;
        indexedCoalesceDictionary(mb,change->children[2],&token.given, handle);
        indexedCoalesceTokens(mb,change->children[3],&token.children,handle);
    }
    if (change->var==1) {
        if (handle.find(change->children[0]->endpoint)==handle.end()) throw;
        token.strucLocal = handle[change->children[0]->endpoint];
        token.varID = std::stoi(change->children[1]->endpoint);
        token.reconstruct=1;
        indexedCoalesceDictionary(mb,change->children[2],&token.given, handle);
    }
    if (change->var==2) {
        token.alt = makeargreference(change->children[0]);
        token.reconstruct=2;
        indexedCoalesceDictionary(mb,change->children[1],&token.given, handle);
    }
    if (change->var==3) {
        token.reconstruct=4;
        token.ifget = change->children[0]->endpoint;
        token.children.push_back(indexedTokenConvert(mb,change->children[1], handle));
    }
    if (change->var==4) {
        token.alt = makeargreference(change->children[0]);
        token.reconstruct=2;
    }
    token.tokargs=(int)token.children.size();
    return token;
}





void indexedCoalesceStatements(MetaBank* mb,ParseResult* change,std::vector<Construction>* list,std::map<std::string,int>& handle) {
    list->push_back(indexedStatementConvert(mb,change->children[0],handle));
    if (change->var==0) {
        indexedCoalesceStatements(mb,change->children[1],list,handle);
    }
}

Construction indexedStatementConvert(MetaBank* mb,ParseResult* change,std::map<std::string,int>& handle) {
    if (change->struc!=p_statement) throw;
    Construction head;
    head.specifier=0;
    head.reconstruct=0;
    if (change->var<2) {
        head.varID = std::stoi(change->children[0]->endpoint);
        head.strucLocal = std::stoi(change->children[1]->endpoint);
        head.reconstruct=6;
        if (change->var%2==0) indexedCoalesceStatements(mb,change->children[2],&head.children,handle);
    } else if (change->var<4) {
        head.varID = std::stoi(change->children[1]->endpoint);
        head.strucLocal = 0;
        head.specifier = std::stoi(change->children[0]->endpoint);
        if (change->var%2==0) indexedCoalesceStatements(mb,change->children[2],&head.children,handle);
    } else if (change->var<6) {
        head.reconstruct=5;
        if (change->var%2==0) indexedCoalesceStatements(mb,change->children[0],&head.children,handle);
    } else if (change->var<8) {
        head.varID = mb->getAxiom(change->children[0]->endpoint);
        head.strucLocal = 0;
        if (change->var%2==0) indexedCoalesceStatements(mb,change->children[1],&head.children,handle);
    } else if (change->var<10) {
        head = indexedTokenConvert(mb,change->children[0],handle);
        if (change->var%2==0) indexedCoalesceStatements(mb,change->children[1],&head.children,handle);
    }
    return head;
}

Conversion makeconvert(MetaBank* mb,ParseResult* tokenized,std::map<std::string,int>& handle) {
    Conversion bot;
    if (tokenized->struc!=p_conversion) throw;
    if (tokenized->var==0) {
        bot = makeconvert(mb,tokenized->children[3],handle);
        ConversionChoice android;
        android.body = makeconvert(mb,tokenized->children[2],handle);
        android.head = makeargreference(tokenized->children[0]);
        android.id = std::stoi(tokenized->children[1]->endpoint);
        bot.choices.insert(bot.choices.begin(),android);
    }
    if (tokenized->var==1) {
        bot.elapse.reconstruct=3;
        bot.elapse.strucLocal=-1;
    }
    if (tokenized->var==2) {
        bot.elapse.reconstruct=3;
        bot.elapse.strucLocal=std::stoi(tokenized->children[0]->endpoint);
    }
    if (tokenized->var==3) {
        bot.elapse = indexedStatementConvert(mb,tokenized->children[0],handle);
    }
    return bot;
}


void definePattern(MetaBank* mb,std::map<std::string,int>& handle,ParseSpecifier& parser,ParseResult* tokenized,ParseVariant& addin) {
    if (tokenized->struc!=p_pattern) throw;
    if (tokenized->var==0) {
        addin.then(parser.tz(tokenized->children[0]->endpoint));
        definePattern(mb,handle,parser,tokenized->children[1],addin);
    }
    if (tokenized->var==1) {
        if (handle.find(tokenized->children[0]->endpoint)==handle.end()) throw;
        addin.then(handle[tokenized->children[0]->endpoint]);
        definePattern(mb,handle,parser,tokenized->children[1],addin);
    }
}
void listvariants(MetaBank* mb,std::map<std::string,int>& handle,ParseSpecifier& parser,ParseResult* tokenized,ParseStructure& addin,int order) {
    if (tokenized->struc!=p_variant) throw;
    if (tokenized->var!=3) {
        ParseVariant newvar = ParseVariant(order);
        newvar.converter = makeconvert(mb,tokenized->children[1],handle);
        definePattern(mb,handle,parser, tokenized->children[0], newvar);
        addin.variant(newvar);
        listvariants(mb,handle,parser,tokenized->children[2],addin,tokenized->var!=2?order+1:order);
    }
}
void listnocvariants(MetaBank* mb,std::map<std::string,int>& handle,ParseSpecifier& parser,ParseResult* tokenized,ParseStructure& addin,int order) {
    if (tokenized->struc!=p_nocvariant) throw;
    if (tokenized->var!=3) {
        if (tokenized->var==1) addin.flippedorders[order]=true;
        ParseVariant newvar = ParseVariant(order);
        definePattern(mb,handle,parser, tokenized->children[0], newvar);
        addin.variant(newvar);
        listnocvariants(mb,handle,parser,tokenized->children[1],addin,tokenized->var!=2?order+1:order);
    }
}
void liststrucs(MetaBank* mb,std::map<std::string,int>& handle,ParseSpecifier& parser,ParseResult* tokenized,int resid) {
    if (tokenized->struc!=p_parse) throw;
    if (tokenized->var==3) return;
    parser.table[resid] = ParseStructure(resid);
    
    parser.table[resid].englishname=tokenized->children[0]->endpoint;
    
    parser.table[resid].verbose=tokenized->var!=2;
    if (tokenized->var==1) {
        std::map<std::string,Statement*> typevarbank;
        for (int a=0;a<mb->strategies.size();a++) {
            typevarbank[mb->stratnames[a]]=mb->strategies[a];
        }
        parser.table[resid].type = indexedPureStrategyConvert(mb,tokenized->children[1],typevarbank,0,0);
        listvariants(mb,handle,parser,tokenized->children[2],parser.table[resid],0);
        liststrucs(mb,handle,parser,tokenized->children[3],resid+1);
    } else liststrucs(mb,handle,parser,tokenized->children[2],resid+1);
    
    if (tokenized->var==0) listvariants(mb,handle,parser,tokenized->children[1],parser.table[resid],0);
    if (tokenized->var==2) listnocvariants(mb,handle,parser,tokenized->children[1],parser.table[resid],0);
}
void generateHandle(std::map<std::string,int>& handle,ParseResult* tokenized,int resid) {
    if (tokenized->struc!=p_parse) throw;
    if (tokenized->var!=3) {
        handle[tokenized->children[0]->endpoint] = resid;
    }
    if (tokenized->var==0) generateHandle(handle,tokenized->children[2],resid+1);
    if (tokenized->var==1) generateHandle(handle,tokenized->children[3],resid+1);
    if (tokenized->var==2) generateHandle(handle,tokenized->children[2],resid+1);
}





ParseSpecifier& getLangFile(const std::string& filename) {
    static std::map<std::string,ParseSpecifier> globparsers;
    if (globparsers.find(filename)==globparsers.end()) {
        std::map<std::string,std::string> comments;
        std::map<std::string,std::string> literals;
        std::string tokenlist="";
        std::ifstream file(filename);
        std::string strn((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
        bool hithash=false;
        bool key=false;
        int line=0;
        std::string buf="";
        std::string cumul="";
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

ParseSpecifier parse_parser(std::map<std::string,std::string> comments,std::map<std::string,std::string> literals,
                            const std::string & tokenlist,const std::string & parse) {
    ParseResult* tokenized = getGlobParser().parse(p_parse, parse);
    ParseSpecifier result;
    std::string cur="";
    for (int i=0;i<=tokenlist.length();i++) {
        char cc=i==tokenlist.length()?' ':tokenlist[i];
        if ((cc==' ' or cc=='\n') and cur.length()>0) {
            result.registerToken(cur);
            cur="";
        } else if (cc!=' ' and cc!='\n') {
            cur+=cc;
        }
    }
    for (auto it=comments.begin();it!=comments.end();it++) {
        result.registerComment(it->first,it->second);
    }
    for (auto it=literals.begin();it!=literals.end();it++) {
        result.registerLiteral(it->first,it->second);
    }
    std::map<std::string,int> handle;
    handle["NUMBER"] = -1;
    handle["STRING"] = -2;
    handle["LITERAL"] = -3;
    generateHandle(handle,tokenized,0);
    liststrucs(&MetaBank::meta_prime,handle,result,tokenized,0);
    tokenized->cleanup();
    return result;
}
Statement* parse_TTML(const std::string & parse,int tdepth) {
    ParseResult* tokenized = getGlobParser().parse(p_purestrategy, parse);
    std::map<std::string,Statement*> varbank;
    MetaBank* mb = &MetaBank::meta_prime;
    varbank["U"]=Statement::universe;
    for (int a=0;a<mb->strategies.size();a++) {
        varbank[mb->stratnames[a]]=mb->strategies[a];
    }
    Statement* result = indexedPureStrategyConvert(mb,tokenized,varbank,0,tdepth);
    tokenized->cleanup();
    return result;
}
Statement* parse_TTML(const std::string & parse,int tdepth,std::map<std::string,Statement*> varbank) {
    ParseResult* tokenized = getGlobParser().parse(p_purestrategy, parse);

    MetaBank* mb = &MetaBank::meta_prime;
    varbank["U"]=Statement::universe;
    for (int a=0;a<mb->strategies.size();a++) {
        varbank[mb->stratnames[a]]=mb->strategies[a];
    }
    Statement* result = indexedPureStrategyConvert(mb,tokenized,varbank,0,tdepth);
    tokenized->cleanup();
    return result;
}

#undef p_parse
#undef p_variant
#undef p_nocvariant
#undef p_conversion
#undef p_statement
#undef p_arguments
#undef p_pattern
#undef p_token
#undef p_tokstring
#undef p_tokargs
#undef p_dictionary
#undef p_purestrategy
#undef p_purestatement
#undef p_purearguments
#undef p_puremodifiers


