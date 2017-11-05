//
//  reasoning_interface.cpp
//  URM
//
//  Created by Parker on 10/22/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include "reasoning_core.hpp"
#include "parser_core.hpp"


Statement* invokeSolver(Statement* a) {return MetaBank::meta_prime.solve(a);}
Statement* invokeSolver(std::string parse) {return MetaBank::meta_prime.solve(parse_TTML(parse,0));}
Statement* invokeSolver(std::map<std::string,Statement*> varbank,std::string parse) {return MetaBank::meta_prime.solve(parse_TTML(parse,0,varbank));}