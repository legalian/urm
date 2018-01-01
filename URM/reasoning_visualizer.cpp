//
//  reasoning_visualizer.cpp
//  URM
//
//  Created by Parker on 11/7/17.
//  Copyright © 2017 Parker. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "reasoning_core.hpp"

std::string SolveInstance::label="Unassigned";
std::vector<std::string> SolveInstance::labels;
std::vector<std::string> SolveInstance::json;

void SolveInstance::flushvisualizer() {
    std::ofstream vfile;
    vfile.open("visualizer/Visualizer.html");
    vfile<<"<html><body><script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.min.js\"></script><script src=\"springy.js\"></script><script src=\"springyui.js\"></script><script>var jsonGraphs = [";
    for (int u=0;u<json.size();u++) {
        vfile<<json[u];
        if (u!=json.size()-1) vfile<<",";
    }
    vfile<<"];var graph;jQuery(function(){graph = new Springy.Graph();var springy = jQuery('#springydemo').springy({graph: graph});});function displaygraph(n) {for(var i=graph.nodes.length-1;i>=0;i--){graph.removeNode(graph.nodes[i]);}graph.loadJSON(jsonGraphs[n]);}</script><div style='border:1px solid black;position:absolute;left:0px;right:200px;top:0px;bottom:0px'><canvas id=\"springydemo\" height=\"700\" width=\"1200\"/></div><div style='border:1px solid black;position:absolute;width:200px;right:0px;top:0px;bottom:0px'>";
    int c=0;
    std::string carry="";
    for (int u=0;u<json.size();u++) {
        vfile<<"<a href='javascript:displaygraph("<<u<<")'>"<<labels[u]<<" "<<u<<"</a><br>";
        if (carry==labels[u]) c++;
        else c=0;
    }
    vfile<<"</div></body></html>";
    vfile.close();
}
void SolveInstance::visualize() {
    std::string js="{\"nodes\": [";
    for (int i=0;i<solns.size();i++) {
        js+="\""+solns[i]->head.tostring()+"\"";
        if (i!=solns.size()-1) js+=",";
    }
    js+="],\"edges\":[\n";
    for (int i=0;i<solns.size();i++) {
        for (int o=0;o<solns[i]->upstream.size();o++) {
            Binding& interm =solns[i]->upstream[o].container->bind;
            std::string linkname=solns[i]->upstream[o].linked->head.substitute_tostring(&interm);
            js+="[\""+solns[i]->head.tostring()+"\",\""+solns[i]->upstream[o].linked->head.tostring()+"\",{label:'"+
            linkname
            +"'}]";
            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
        }
    }
    js+="]}";
    json.push_back(js);
    labels.push_back(label);
}
void SolveInstance::heavyvisualize() {
//    std::string js="{\"nodes\": [";
//    for (int i=0;i<solns.size();i++) {
//        js+="\"["+solns[i]->head.tostring()+"]\"";
//        js+=",";
//    }
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->bin.size();o++) {
//            Binding& interm =solns[i]->bin[o]->bind;
//            std::string linkname=solns[i]->head.substitute_tostring(&interm);
//            js+="\""+linkname+"\"";
//            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
//        }
//    }
//    js+="],\"edges\":[\n";
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->bin.size();o++) {
//            Binding& interm =solns[i]->bin[o]->bind;
//            std::string linkname=solns[i]->head.substitute_tostring(&interm);
//            js+="[\"["+solns[i]->head.tostring()+"]\",\""+linkname+"\",{color:'#0000FF',label:'"+std::to_string(o)+"'}]";
//            js+=",";
//        }
//    }
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->upstream.size();o++) {
//            Binding& interm =solns[i]->upstream[o].container->bind;
//            std::string linkname=solns[i]->upstream[o].linked->head.substitute_tostring(&interm);
//            int rm=0;
//            Statement target=solns[i]->head.scramble(solns[i]->upstream[o].mapr,rm,1);
//            js+="[\""+linkname+"\",\"["+solns[i]->head.tostring()+"]\",{color:'#FF0000',label:'"+target.tostring()+"'}]";
//            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
//        }
//    }
//    js+="]}";
//    json.push_back(js);
//    labels.push_back(label);
}





