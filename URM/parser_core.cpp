//
//  parser_core.cpp
//  URM
//
//  Created by Parker on 8/19/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include "parser_core.hpp"


//ParseArtifact::ParseArtifact(int id,std::string del) : obj(id), delim(del) {}
ParseArtifact::ParseArtifact(int id,bool isobj) : tok(id), obj(isobj) {}

void ErrorReport::failure(std::string mes,long a,long b,int c) {

    if (c>=errdepth) {
        errdepth=c;
        begin=a;
        end=b;
        message=mes;
    }
}
void ErrorReport::bigfailure(std::string mes,long a,long b,int c) {

    if (c>=bigerrdepth) {
        bigerrdepth=c;
        bigbegin=a;
        bigend=b;
        bigmessage=mes;
    }
}


int ParseSpecifier::registerToken(std::string name) {
    tokenkey.push_back(name);
    return keysize++;
}
void ParseSpecifier::registerComment(std::string begin,std::string end) {
    comments.push_back(std::pair<std::string,std::string>(begin,end));
}
void ParseSpecifier::registerLiteral(std::string begin,std::string end) {
    std::vector<int> collector;
    tokenize(&collector,begin);
    if (collector.size()==0) throw;
    int first=collector[0];
    tokenize(&collector,end);
    if (collector.size()==0) throw;
    int second=collector[0];
    literals.push_back(std::pair<int,int>(first,second));
}
ParseResult* ParseSpecifier::parse(int n,const std::string& target) {
    tokens.clear();
    tokenize(&tokens,target);
    long start=0;
    ErrorReport attempt;
    ParseResult* result = table[n].match(this,start,tokens.size(),attempt,0);
    if (result and start!=tokens.size()) {
        attempt.failure("Extra nonsense after token.",start,tokens.size(),0);
        result->cleanup();
        result=0;
    }
    if (result==0) {
        std::cout<<attempt.message<<"\n";
        int surround=0;
        std::cout<<untokenize(std::max((int)attempt.begin-surround,0),std::min((int)attempt.end+surround+1,(int)tokens.size()))<<"\n";
        throw;
    }
    return result;
}

bool iswhitespace(char var) {return var==' ' or var=='\t' or var=='\n';}
bool isnumeric(char var)    {return (var-'0'>=0 and var-'0'<=9);}
bool validvarchar(char var) {return (var-'0'>=0 and var-'0'<=9) or (var-'a'>=0 and var-'a'<=25) or (var-'A'>=0 and var-'A'<=25) or var=='_';}
bool isvalidsymbl(std::vector<std::string>* names,std::string partial) {
    for (int y=0;y<names->size();y++) {
        bool succeed=true;
        for (int x=0;x<partial.length();x++) {
            if (x>=(*names)[y].length() or (*names)[y][x]!=partial[x]) {
                succeed = false;
                break;
            }
        }
        if (succeed) {
            return true;
        }
    }
    return false;
}
int isvalidfinalsymbl(std::vector<std::string>* names,std::string partial) {
    for (int y=0;y<names->size();y++) {
        if ((*names)[y]==partial) {
            return y;
        }
    }
    return -1;
}
std::vector<int> ParseSpecifier::tz(const std::string& in) {
    std::vector<int> res;
    tokenize(&res,in);
    return res;
}
std::string ParseSpecifier::untokenize(long start,long end) {
    std::string res="";
    for (long r=start;r<end;r++) {
        if (tokens[r]<keysize) {
            res+= tokenkey[tokens[r]] + " ";
        } else {
            res+= ""+varnames[tokens[r]-keysize]+" ";
        }
    }
    return res;
}
void ParseSpecifier::tokenize(std::vector<int>* tokens,const std::string& target) {
    std::string cumul="";
    int cumultype=0;
    std::string subcumul="";
    int ender=-1;
    bool incomment=false;
    for (int q=0;q<=target.length();q++) {
        if (incomment) {
            for (int com=0;com<comments.size();com++) {
                if (q<=target.length()-comments[com].second.size()) {
                    bool matched=true;
                    for (int z=0;z<comments[com].second.size();z++) {
                        if (comments[com].second[z]!=target[q+z]) {
                            matched=false;
                        }
                    }
                    if (matched) incomment=false;
                }
            }
        } else if (ender==-1) {
            for (int com=0;com<comments.size();com++) {
                if (q<=target.length()-comments[com].first.size()) {
                    bool matched=true;
                    for (int z=0;z<comments[com].first.size();z++) {
                        if (comments[com].first[z]!=target[q+z]) {
                            matched=false;
                        }
                    }
                    if (matched) incomment=true;
                }
            }
        }
        char cc = q==target.length() or incomment?' ':target[q];
        if ((cumultype==1 and !isnumeric(cc)) or (cumultype==2 and !validvarchar(cc))) {
            bool found=false;
            int tind = isvalidfinalsymbl(&tokenkey,cumul);
            if (tind!=-1 and tind!=-2 and tind!=-3) {
                if (ender==-1) tokens->push_back(tind);
            } else {
                for (int u=0;u<varnames.size();u++) {
                    if (varnames[u]==cumul) {
                        if (ender==-1)tokens->push_back(u+keysize);
                        found=true;
                        break;
                    }
                }
                if (!found) {
                    if (ender==-1)tokens->push_back(keysize+(int)varnames.size());
                    if (ender==-1)varnames.push_back(cumul);
                }
            }
            cumultype=0;
            cumul="";
        }
        if (cumultype==3) {
            if (!isvalidsymbl(&tokenkey,cumul+cc)) {
                int tind = isvalidfinalsymbl(&tokenkey,cumul);
//                if (tokenkey[tind]!=cumul) throw;
//                if (tind<0) throw;
                if (ender==tind and tind>=0) {
                    tokens->push_back(keysize+(int)varnames.size());
                    std::string subsubcumul="";
                    for (int k=0;k<subcumul.length()-cumul.length();k++) {
                        subsubcumul+=subcumul[k];
                    }
                    varnames.push_back(subsubcumul);
                    ender=-1;
                    tokens->push_back(tind);
                } else if (ender==-1) {
                    tokens->push_back(tind);
                    subcumul="";
                    for (int b=0;b<literals.size();b++) {
                        if (literals[b].first==tind) {
                            ender=literals[b].second;
                        }
                    }
                }
                cumultype=0;
                cumul="";
            }
        }
        subcumul+=cc;
        if (!iswhitespace(cc)) {
            cumul+=cc;
            if (cumultype==0) {
                if (isnumeric(cc)) {
                    cumultype=1;
                } else if (validvarchar(cc)) {
                    cumultype=2;
                } else {
                    if (!isvalidsymbl(&tokenkey,cumul)) {if (ender==-1) throw;}//be prepared to convert between expecting symbol and expecting var name.
                    cumultype=3;
                }
            }
        }
    }
}


ParseVariant::ParseVariant(int up) : order(up) {}
ParseStructure::ParseStructure() : struc(-1) {}
ParseStructure::ParseStructure(int s) : struc(s) {}
ParseStructure& ParseStructure::variant(ParseVariant add) {
    add.struc=struc;
    variants.push_back(add);
    if (maxorder<=add.order) {
        maxorder=add.order+1;
    }
    return *this;
}
ParseVariant& ParseVariant::then(int obj) {
    arguments.push_back(ParseArtifact(obj,true));
    return *this;
}
ParseVariant& ParseVariant::then(std::vector<int> objs) {
    for (int j=0;j<objs.size();j++) {
        arguments.push_back(ParseArtifact(objs[j],false));
    }
    return *this;
}
//ParseVariant& ParseVariant::multi(int obj,bool isobj) {
//    arguments.push_back(ParseArtifact(obj,isobj,true));
//    return *this;
//}
ParseResult::ParseResult(int a,int b,int c) : struc(a), var(b), order(c) {}
ParseResult::ParseResult(std::string c,int ord) : endpoint(c), order(ord) {}
ParseResult::ParseResult() {}
int ParseResult::measureLeftBalance() {
    if (children.size()) {
        return 1+children[0]->measureLeftBalance();
    }
    return 1;
}
void ParseResult::cleanup() {
    for (int v=0;v<children.size();v++) {
        children[v]->cleanup();
    }
    delete this;
}
std::string ParseResult::recode(ParseSpecifier* glob) {
    if (struc==-1) {
        return endpoint;
    }
    std::vector<ParseArtifact> arts = glob->table[struc].variants[var].arguments;
    int ar=0;
    std::string res="";
    for (int w=0;w<arts.size();w++) {
        if (!arts[w].obj) {
            res+=glob->tokenkey[arts[w].tok];
        } else {
            res+=children[ar++]->recode(glob);
        }
        if (w!=arts.size()-1) {
            res+=" ";
        }
    }
    return res;
}
ParseResult* ParseStructure::match(ParseSpecifier* glob,long& offset,long end,ErrorReport& debug,int errdepth) {
    for (int w=0;w<maxorder;w++) {
        ParseResult* result=0;
        int lastpri=0;
        int asv=-1;
        long after=offset;
        for (int v=0;v<variants.size();v++) {
            if (variants[v].order==w) {
                long adjuster=offset;
                ParseResult* last = variants[v].match(glob,adjuster,end,debug,errdepth+1);
                if (last) {
                    int leftbalance = last->measureLeftBalance();
                    if (leftbalance>lastpri) {
                        after=adjuster;
                        result=last;
                        lastpri=leftbalance;
                        asv=v;
                    } else {
                        last->cleanup();
                    }
                }
            }
        }
        if (result) {
//            std::cout<<debug<<" depth: "<<struc<<" and "<<w<<" matched "<<glob->untokenize(offset,after)<<"\n";
//            if (variants[asv].arguments.size()==0) {
//                std::cout<<struc<<" <========== "<<glob->untokenize(offset,end)<<"\n";
//            }
            offset=after;
            result->var=asv;
            return result;
        }
    }
//    std::cout<<struc<<" <---------------- "<<glob->untokenize(offset,end)<<"\n";
    debug.failure(englishname+" did not match template.",offset,end,errdepth);
    return 0;
}

bool contains(std::vector<int>* tokens,int token,long& start,long end) {
    long tent=start;
    while (end>tent and (*tokens)[tent]!=token) tent++;
    if (tent>=end) {
        return false;
    }
    start=tent;
    return true;
}
ParseResult* ParseVariant::match(ParseSpecifier* glob,long& offset,long end,ErrorReport& debug,int errdepth) {
    long tentative=offset;
    ParseResult* result = new ParseResult(struc,-1,order);
    for (int index=0;index<arguments.size();index++) {
        if (arguments[index].obj) {
            ParseResult* res;
            if (arguments[index].tok==struc) {
                if (arguments.size()==index+1 or index!=0) {
                    res = glob->table[arguments[index].tok].match(glob,tentative,end,debug,errdepth+1);
                    if (!res) {
                        result->cleanup();
                        return 0;
                    }
                } else {
                    if (arguments[index+1].obj) {throw;}
                    long unanchor=tentative;
                    long anchor=tentative;
                    while (true) {
                        anchor=unanchor;
                        if (!contains(&glob->tokens,arguments[index+1].tok,tentative,end)) {
                            debug.failure("Could not find next "+glob->tokenkey[arguments[index+1].tok]+" in given range.",anchor,tentative,errdepth);
                            return 0;
                        }
                        res = glob->table[struc].match(glob,anchor,tentative,debug,errdepth+1);
                        tentative++;
                        if (res) {
                            if (anchor!=tentative-1) {
                                debug.failure("Unexpected content after "+glob->table[arguments[index].tok].englishname+".",anchor,tentative,errdepth);
                                result->cleanup();
                                return 0;
                            }
                            index++;
                            break;
                        }
                    }
                }
            } else if (arguments[index].tok==-1 or arguments[index].tok==-2 or arguments[index].tok==-3) {
                if (tentative>=end){
                    debug.failure("Ran out of range.",offset,end,errdepth);
                    result->cleanup();
                    return 0;
                }
//                if (arguments[index].tok==-1 and glob->tokens[tentative]<glob->keysize and glob->tokenkey[glob->tokens[tentative]]=="-") {
//                    
//                    if (tentative+1>=end or glob->tokens[tentative+1]<glob->keysize){
//                        debug.failure("Interpreted negative sign for number but then next character was invalid.",tentative,end,errdepth);
//                        result->cleanup();
//                        return 0;
//                    }
//                    if (!isnumeric(glob->varnames[glob->tokens[tentative+1]-glob->keysize][0])) {
//                        debug.failure("Attempted to interpret non-numeric character for number.",tentative,end,errdepth);
//                        result->cleanup();
//                        return 0;
//                    }
//                    res = new ParseResult(glob->varnames[glob->tokens[tentative+1]-glob->keysize],order);
//                    tentative+=2;
//                } else {
                    if (glob->tokens[tentative]<glob->keysize) {
//                        debug.failure(glob->tokenkey[glob->tokens[tentative]]+" cannot be number, string, or literal.",offset,end,errdepth);
                        result->cleanup();
                        return 0;
                    }
                    if (arguments[index].tok==-1 and !isnumeric(glob->varnames[glob->tokens[tentative]-glob->keysize][0])) {
                        debug.failure("String can't be number.",offset,end,errdepth);
                        result->cleanup();
                        return 0;
                    }
                    if (arguments[index].tok==-2 and isnumeric(glob->varnames[glob->tokens[tentative]-glob->keysize][0])) {
                        debug.failure("Number can't be string.",offset,end,errdepth);
                        result->cleanup();
                        return 0;
                    }
                    res = new ParseResult(glob->varnames[glob->tokens[tentative]-glob->keysize],order);
                    tentative++;
//                }
            } else {
                res = glob->table[arguments[index].tok].match(glob,tentative,end,debug,errdepth+1);
                if (!res) {
                    result->cleanup();
                    return 0;
                }
            }
            result->children.push_back(res);
        } else {
            if (tentative>=end or glob->tokens[tentative]!=arguments[index].tok) {
                if (glob->tokens[tentative]<glob->keysize) {
//                    debug.failure("Token mismatch:"+glob->tokenkey[arguments[index].tok]+" and "+glob->tokenkey[glob->tokens[tentative]]+".",offset,end,errdepth);
                } else {
//                    debug.failure("Token mismatch:"+glob->tokenkey[arguments[index].tok]+" and "+glob->varnames[glob->tokens[tentative]-glob->keysize]+".",offset,end,errdepth);
                }
                result->cleanup();
                return 0;
            }
            tentative++;
        }
    }
    offset=tentative;
    return result;
}








DictEntry replace(DictEntry b,Construction* c,int depth) {
    DictEntry a;
    a.channel=b.channel;
    a.a=b.a.replace(c,depth);
    a.b=b.b.replace(c,depth);
    return a;
}

Construction::Construction(){}
Construction::Construction(ParseSpecifier* spec,ParseResult* token) {
    strucLocal = token->struc;
    varID = token->var;
    reconstruct=1;
    ifget=token->endpoint;
    tokargs=(int)token->children.size();
    if (!spec->table[token->struc].flippedorders[token->order]) {
        if (token->children.size()>1 and token->children[0]->struc==token->struc and token->children[token->children.size()-1]->struc==token->struc) {
            ParseResult* child = token->children[token->children.size()-1];
            if (token->order==child->order) {
                if (child->children.size()>1 and child->children[0]->struc==child->struc and child->children[child->children.size()-1]->struc==child->struc) {
                    token->children[token->children.size()-1]=child->children[0];
                    child->children[0]=token;
                    *this=Construction(spec,child);
                    return;
                }
            }
        }
    }
    for (int r=0;r<token->children.size();r++) {
        children.push_back(Construction(spec,token->children[r]));
    }
}
Construction Construction::replace(Construction* a,int depth) {
    if (reconstruct==2) {
        Construction ref = a->reference(alt);
        for (int y=tokargs;y<children.size();y++) {
            ref.children.push_back(children[y].replace(a,depth));//<----=-=--==--=-=-=-=-===--==--=
        }
        for (int j=0;j<given.size();j++) {
            ref.given.push_back(::replace(given[j],a,depth));
        }
        return ref;
    }
    Construction res;
    res.ifget=ifget;
    res.tokargs=tokargs;
    res.varID=varID;
    for (int j=0;j<given.size();j++) {
        res.given.push_back(::replace(given[j],a,depth));
    }
    if (reconstruct==6) {
        res.strucLocal=strucLocal+depth+1;
        res.reconstruct=0;
    } else {
        res.strucLocal=strucLocal;
        res.reconstruct=reconstruct;
    }
    for (int y=0;y<children.size();y++) {
        res.children.push_back(children[y].replace(a,depth));
    }
    return res;
}

Construction Construction::reference(ConstructArgReference e) {
    if (e.path.size()==0) return *this;
    if (reconstruct!=1) throw;
    int p=e.path[0];
    e.path.erase(e.path.begin());
    if (children.size()<=p) throw;
    Construction a= children[p].reference(e);
    return a;
}
Statement Construction::convert(ParseSpecifier* parser,std::vector<DictEntry> space,int& locid,int stat,std::vector<Statement>& varref,int olddepth) {
    switch (reconstruct) {
        case 0: {
            Statement ret = Statement(varID,strucLocal,children.size());
            for (int e=0;e<children.size();e++) {
                ret.args[e] = children[e].convert(parser,space,locid,stat+1,varref,olddepth);
            }
            return ret;
        } case 1:
            if (strucLocal==-1) {
                return Statement(std::stoi(ifget),-1);
            }
            for (int u=0;u<given.size();u++) {
                space.push_back(given[u]);
            }
            return parser->table[strucLocal].variants[varID].converter.convert(parser,space,this,locid,stat,varref,olddepth);
        case 4:
            for (int u=0;u<space.size();u++) {
                if (space[u].channel==ifget and space[u].a.ifget==children[0].ifget) {
                    return space[u].b.convert(parser,space,locid,stat,varref,olddepth);
                }
            }
            throw;
        case 5:
            return Statement(locid++,1);
        case 7:
            if (children.size()) throw;
            return varref[varID].depth_push(olddepth+1,stat-olddepth);
    }
    throw;
}
Statement Conversion::convert(ParseSpecifier* parser,std::vector<DictEntry> space,Construction* token,int& locid,int stat,std::vector<Statement>& varref,int olddepth) {
    for (int y=0;y<choices.size();y++) {
        Construction party = token->reference(choices[y].head);
        if (party.varID==choices[y].id) {
            return choices[y].body.convert(parser,space,token,locid,stat,varref,olddepth);
        }
    }
    std::vector<Statement> fargs;
    for (int y=token->tokargs;y<token->children.size();y++) {
        fargs.push_back(token->children[y].convert(parser,space,locid,stat,varref,olddepth));
    }
    
    return elapse.replace(token,stat).convert(parser,space,locid,stat,fargs,stat);
}
Statement ParseSpecifier::fullconvert(const std::string& input) {
    ParseResult* tokens = parse(0,input);
    
    std::vector<DictEntry> carry;
    std::vector<Statement> upset;
    int locid=0;
    
    Construction a = Construction(this,tokens);
    for (int d=0;d<table[0].type.ara;d++) {
        a.children.push_back(Construction());
        a.children[a.tokargs+d].strucLocal=2;
        a.children[a.tokargs+d].varID=d;
        a.children[a.tokargs+d].reconstruct=0;
        upset.push_back(table[0].type.args[d].snapshot(99));
        throw;//depth guessed
    }
//    std::cout<<a.tostringheavy()<<"\n";
//    int reassign=0;
    std::vector<int> remap;
    Statement res = a.convert(this,carry,locid,2,upset,2);
    throw;//make sure this works
    
//    res.unscramble(remap,reassign,1);
    return res;
}









