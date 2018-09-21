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

//std::string SolveInstance::label="Unassigned";
//std::vector<std::string> SolveInstance::labels;
//std::vector<std::string> SolveInstance::json;
//std::vector<std::string> SolveInstance::gephi;



//void SolveInstance::gephi_flushvisualizer() {
//    std::ofstream vfile;
//    vfile.open("visualizer/Graph.tlp");
//    for (int r=0;r<gephi.size();r++) {
//        vfile<<gephi[r];
//    }
//    vfile.close();
//}
//void SolveInstance::gephi_visualize() {




//    <node id="0" label="Gephi" start="0"/>
//    <node id="1" label="Network" start="5"/>
//    <node id="2" label="Visualization" start="5"/>
//    <node id="3" label="Graph" start="0"/>
//
//    <edge id="0" source="0" target="1"/>
//    <edge id="1" source="0" target="2"/>
//    <edge id="2" source="1" target="0"/>
//    <edge id="3" source="2" target="1"/>
//
//    std::ofstream vfile;
//    vfile.open("visualizer/Graph.gexf");
//
//    vfile<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
//    "<gexf xmlns=\"http://www.gexf.net/1.2draft\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd\" version=\"1.2\"><graph mode=\"dynamic\" defaultedgetype=\"directed\" timeformat=\"date\"><nodes>";
//
//    for (int i=0;i<solns.size();i++) {
//        vfile<<"<node id=\""<<i<<"\" label=\""+solns[i]->tostring()<<"\"/>";
//    }
//    vfile<<"</nodes><edges>";
//    int edges=0;
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->upstream.size();o++) {
//            int other = -1;
//            for (int ff=0;ff<solns.size();ff++) if (solns[ff]==solns[i]->upstream[o].linked) other=ff;
//            if (other==-1) throw;
//            vfile<<"<edge id=\""<<edges++<<"\" source=\""<<i<<"\" target=\""<<other<<"\"/>";
//        }
//    }
//    vfile<<"</edges></graph></gexf>";


//    std::string nw="(nb_nodes "+std::to_string(solns.size())+")\n(nodes 0.."+std::to_string(solns.size()-1)+")\n";
//    int edges=0;
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->upstream.size();o++) {
//            int other = -1;
//            for (int ff=0;ff<solns.size();ff++) {
//                if (solns[ff]==solns[i]->upstream[o].linked) {
//                    other=ff;
//                    break;
//                }
//            }
//            if (other==-1) throw;
//            nw+="(edge "+std::to_string(edges++)+" "+std::to_string(i)+" "+std::to_string(other)+")\n";
//        }
//    }
////    (property 0 string "viewLabel"
////      (default "" "" )
////      (node 1 "Hello")
////      (node 2 "Bonjour")
////      (node 3 "Bye")
////      (edge 2 "Aurevoir")
////    )
//
//    nw+="(property 0 string \"viewLabel\"(default \"\" \"\" )\n";
//    for (int i=0;i<solns.size();i++) nw+="\t(node "+std::to_string(i)+" \""+solns[i]->tostring()+"\")\n";
//    nw+=")";


//    vfile.close();
//    
//    
//    
//}
//
//
//
//
//void SolveInstance::browser_flushvisualizer() {
//    std::ofstream vfile;
//    vfile.open("visualizer/Visualizer.html");
//    vfile<<"<html><body><script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.min.js\"></script><script src=\"springy.js\"></script><script src=\"springyui.js\"></script><script>var jsonGraphs = [";
//    for (int u=0;u<json.size();u++) {
//        vfile<<json[u];
//        if (u!=json.size()-1) vfile<<",";
//    }
//    vfile<<"];var graph;jQuery(function(){graph = new Springy.Graph();var springy = jQuery('#springydemo').springy({graph: graph});});function displaygraph(n) {for(var i=graph.nodes.length-1;i>=0;i--){graph.removeNode(graph.nodes[i]);}graph.loadJSON(jsonGraphs[n]);}</script><div style='border:1px solid black;position:absolute;left:0px;right:200px;top:0px;bottom:0px'><canvas id=\"springydemo\" height=\"700\" width=\"1200\"/></div><div style='border:1px solid black;position:absolute;width:200px;right:0px;top:0px;bottom:0px'>";
//    int c=0;
//    std::string carry="";
//    for (int u=0;u<json.size();u++) {
//        vfile<<"<a href='javascript:displaygraph("<<u<<")'>"<<labels[u]<<" "<<u<<"</a><br>";
//        if (carry==labels[u]) c++;
//        else c=0;
//    }
//    vfile<<"</div></body></html>";
//    vfile.close();
//}
//void SolveInstance::browser_visualize() {
//    std::string js="{\"nodes\": [";
//    for (int i=0;i<solns.size();i++) {
//        js+="\""+solns[i]->tostring()+"\",";
//    }
//    js+="],\"edges\":[\n";
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->upstream.size();o++) {
//            std::string linkname1 = solns[i]->tostring();
//            std::string linkname2 = solns[i]->upstream[o].linked->tostring();
//            js+="[\""+linkname1+"\",\""+linkname2+"\"]";
//            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
//        }
//    }
//    js+="]}";
//    json.push_back(js);
//    labels.push_back(label);
//}
//void SolveInstance::browser_visualizeheavy() {
//    std::string js="{\"nodes\": [";
//    for (int i=0;i<solns.size();i++) {
//        js+="\"["+solns[i]->tostring()+"]\",";
//    }
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->bin.size();o++) {
//            Binding& interm =solns[i]->bin[o]->bind;
//            js+="\""+solns[i]->initial.localtypes[solns[i]->head].snapshot().substitute_single(interm,interm.tracks).tostring()+"\"";
//            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
//        }
//    }
//    js+="],\"edges\":[\n";
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->bin.size();o++) {
//            Binding& interm =solns[i]->bin[o]->bind;
//            std::string linkname1 = solns[i]->tostring();
//            std::string linkname2 = solns[i]->initial.localtypes[solns[i]->head].snapshot().substitute_single(interm,interm.tracks).tostring();
//            js+="[\"["+linkname1+"]\",\""+linkname2+"\",{color:'#0000FF'}]";
//            js+=",";
//        }
//    }
//    for (int i=0;i<solns.size();i++) {
//        for (int o=0;o<solns[i]->upstream.size();o++) {
//            Binding& interm =solns[i]->upstream[o].container->bind;
//            std::string linkname1 = solns[i]->upstream[o].linked->initial.localtypes[solns[i]->upstream[o].linked->head].snapshot().substitute_single(interm,interm.tracks).tostring();
//            std::string linkname2 = solns[i]->tostring();
//            js+="[\""+linkname1+"\",\"["+linkname2+"]\",{color:'#FF0000'}]";
//            if (i!=solns.size()-1 or o!=solns[i]->upstream.size()) js+=",";
//        }
//    }
//    js+="]}";
//    json.push_back(js);
//    labels.push_back(label);
//}
//
//
//
//
//
