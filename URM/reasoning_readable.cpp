//
//  readable_output.cpp
//  URM
//
//  Created by Parker on 9/14/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"
#include "parser_core.hpp"

std::string Binding::tostringheavy() {
    std::string res="";
    for (auto it=0;it<decoms.size();it++) {
        res+=decoms[it].first->tostringdoubleheavy()+"|"+decoms[it].second->tostringdoubleheavy()+"\n";
    }
    return res;
}

std::string Soln::tostringheavy() {
    std::string res="";
    res+=head->tostringdoubleheavy()+";\n";
    for (int t=0;t<bin.size();t++) {
        Statement* subbed = head->substitute(&bin[t]->bind,2,0);
        res+="\t"+subbed->tostringdoubleheavy()+"\n";
        for (int i=0;i<bin[t]->downstream.size();i++) {
            res+="\t\t"+bin[t]->downstream[i]->head->tostringdoubleheavy()+"\n";
        }
    }
    return res;
}
//std::string Soln::getdebugpath() {
//    std::string res="";
//    for (int y=0;y<debugpath.size();y++) {
//        res+=std::to_string(debugpath[y]);
//        if (y!=debugpath.size()-1) {
//            res+=",";
//        }
//    }
//    return res;
//}


std::string Statement::tostringdoubleheavy() {
    if (type) return tostringheavy()+":"+type->tostringheavy();
    return tostringheavy()+":NULL";
}
//std::string Statement::tostringrecursivedoubleheavy() {
//    if (type) return tostringrecursivedoubleheavy()+":"+type->tostringrecursivedoubleheavy();
//    return tostringrecursivedoubleheavy()+":NULL";
//}
std::string Statement::tostringrecursivedoubleheavy() {
    std::string ret = std::to_string(local)+":"+std::to_string(id);
    if (local==1) ret = "["+std::to_string(specifier)+"]:"+std::to_string(id);
    else if (specifier!=0) ret = "<"+std::to_string(specifier)+"/"+std::to_string(id)+">";
    else if (0==local and MetaBank::meta_prime.stratnames.size()>id) {
        ret = MetaBank::meta_prime.stratnames[id];
    }
    
    if (args.size()) {
        ret += "(";
        for (int e=0;e<args.size();e++) {
            if (args[e]==0) {
                ret = ret+"NULL";
            } else {
                ret = ret+args[e]->tostringrecursivedoubleheavy();
            }
            if (e<args.size()-1) {
                ret = ret+",";
            }
        }
        ret = ret+")";
    }
    if (type==0) ret+=":NULL";
    else if (type!=Statement::universe) ret+=":"+type->tostringrecursivedoubleheavy();
    return ret;
}
std::string Statement::tostringheavy() {
    std::string ret = std::to_string(local)+":"+std::to_string(id);
    if (local==1) ret = "["+std::to_string(specifier)+"]:"+std::to_string(id);
    else if (specifier!=0) ret = "<"+std::to_string(specifier)+"/"+std::to_string(id)+">";
    else if (0==local and MetaBank::meta_prime.stratnames.size()>id) {
        ret = MetaBank::meta_prime.stratnames[id];
    }
    
    if (args.size()) {
        ret += "(";
        for (int e=0;e<args.size();e++) {
            if (args[e]==0) {
                ret = ret+"NULL";
            } else {
                ret = ret+args[e]->tostringheavy();
            }
            if (e<args.size()-1) {
                ret = ret+",";
            }
        }
        ret = ret+")";
    }
    return ret;
}


std::string ntabs(int tabs) {
    std::string res="";
    for (int nu=0;nu<tabs;nu++) res+="\t";
    return res;
}
std::string ConstructArgReference::tostringheavy() {
    std::string res=".";
    for (int u=0;u<path.size();u++) {
        res+=std::to_string(path[u])+".";
    }
    return res;
}
std::string Construction::tostringheavy() {
    std::string ret;
    switch (reconstruct) {
        case 6:
        case 0:
            ret = std::to_string(strucLocal)+":"+std::to_string(varID);
            if (strucLocal==1) ret = "["+std::to_string(specifier)+"]:"+std::to_string(varID);
            else if (specifier!=0) ret = "<"+std::to_string(specifier)+"/"+std::to_string(varID)+">";
            else if (strucLocal==0) {
                ret = MetaBank::meta_prime.stratnames[varID];
            }
            for (int r=0;r<given.size();r++) {
                ret+=";;";
            }
            if (children.size()) {
                ret += "(";
                for (int e=0;e<children.size();e++) {
                    ret = ret+children[e].tostringheavy();
                    if (e<children.size()-1) {
                        ret = ret+",";
                    }
                }
                ret = ret+")";
            }
            return ret;
        case 1:
            ret+="[("+std::to_string(strucLocal)+") "+std::to_string(varID)+"|";
            for (int r=0;r<given.size();r++) {
                ret+=";;";
            }
            for (int e=0;e<children.size();e++) {
                ret = ret+children[e].tostringheavy();
                if (e<children.size()-1) {
                    ret = ret+",";
                }
            }
            return ret+"]";
        case 2:
        
            for (int r=0;r<given.size();r++) {
                ret+=";;";
            }
            return ret+alt.tostringheavy();
        case 3:
            if (strucLocal==-1) return "null";
            return std::to_string(strucLocal);
        case 4:
            return "["+this->ifget+":"+children[0].tostringheavy()+"]";
        case 5:
            return "[[]]";
            
    }
    throw;
}
std::string ConversionChoice::tostringheavy(int tabs) {
    return ntabs(tabs)+"-"+head.tostringheavy()+"~"+std::to_string(id)+body.tostringheavy(tabs);
}
std::string Conversion::tostringheavy(int tabs) {
    std::string res = "{\n";
    for (int y=0;y<choices.size();y++) {
        res+=choices[y].tostringheavy(tabs+1);
    }
    res+=ntabs(tabs+1)+elapse.tostringheavy()+"\n";
    return res+ntabs(tabs)+"}\n";
}
std::string ParseArtifact::tostringheavy(ParseSpecifier* ref) {
    if (obj) {
        return "("+std::to_string(tok)+")";
    } else {
        return +"\\"+ref->tokenkey[tok]+"\\";
    }
}
std::string ParseVariant::tostringheavy(ParseSpecifier* ref) {
    std::string res="\t\t";
    for (int j=0;j<arguments.size();j++) {
        res+=arguments[j].tostringheavy(ref);
    }
    res+=":"+std::to_string(order)+converter.tostringheavy(2);
    return res;
}
std::string ParseVariant::tostringlight(ParseSpecifier* ref) {
    std::string res="\t\t";
    for (int j=0;j<arguments.size();j++) {
        res+=arguments[j].tostringheavy(ref);
    }
    res+=":"+std::to_string(order)+";\n";
    return res;
}
std::string ParseStructure::tostringheavy(ParseSpecifier* ref) {
    std::string res="\t"+std::to_string(struc)+"\n";
    if (verbose) {
        for (int it=0;it<variants.size();it++) {
            res+=variants[it].tostringheavy(ref);
        }
    } else {
        for (int it=0;it<variants.size();it++) {
            res+=variants[it].tostringlight(ref);
        }
    }
    return res;
}
std::string ParseSpecifier::tostringheavy() {
    std::string res="Specifier:\n";
    for (auto it = table.begin();it!=table.end();it++) {
        res+=it->second.tostringheavy(this);
    }
    return res;
}










