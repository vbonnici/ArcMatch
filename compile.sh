#!/bin/bash


#Nomenclature
#archmatch_
#[
#FR													custom file reader (w.r.t. RI versions). always on
#_	
#NODE_D_CONVERGENCE=0(inactive),1(active)			NODE_D_CONV is on for 1
#EDGE_D_CONVERGENCE=0(inactive),1(active)			EDGE_D_CONV is on for 1
#_
#]
#1|0|FC|AC|NS|NSL									   mama type
#_
#0|ED|DP|LF											solver type
#[
#_
#RE|NRE  (reduce or not edge domains)				REDUCE_EDGES is on for RE
#]

#//#define MDEBUG

#//#define PRINT_MATCHES

#//#define FIRST_MATCH_ONLY  //if setted, the searching process stops at the first found match
#//#define FIRST_100k_MATCHES //stop at the first 100k matches

#define NODE_D_CONV	//refine node domains until convergence
#define EDGE_D_CONV	//refine edge domains until convergence

#//#define REDUCE_EDGES

#//#define MAMA_1 //original order
#//#define MAMA_NS //the real original order with node sets for flags
#define MAMA_NSL //the real original order with node sets for flags, with disjoint leafs at the end of the ordering

#//#define SOLVER_0 //simple solver with edge domains
#//#define SOLVER_ED //simple solver which exploits edge domains
#//#define SOLVER_DP //solver with dynamic parent selection
#define SOLVER_LF //solver with dynamic parent selection and leafs

#define PATH_LENGTH 6
#//#define PATH_LENGTH 9


additional=""


t1="-DMAMA_1 -DSOLVER_0"
t2="-DMAMA_1 -DSOLVER_ED"
t3="-DMAMA_1 -DSOLVER_DP"
t4="-DMAMA_NS -DSOLVER_DP"
t5="-DREDUCE_EDGES -DPATH_LENGTH=6 -DMAMA_NS -DSOLVER_DP"
t6="-DMAMA_NSL -DSOLVER_LF"
t7="-DREDUCE_EDGES -DPATH_LENGTH=6 -DMAMA_NSL -DSOLVER_LF"
t8="-DNODE_D_CONV -DMAMA_1 -DSOLVER_0"
t9="-DNODE_D_CONV -DMAMA_1 -DSOLVER_ED"
t10="-DNODE_D_CONV -DMAMA_1 -DSOLVER_DP"
t11="-DNODE_D_CONV -DMAMA_NS -DSOLVER_DP"
t12="-DNODE_D_CONV -DREDUCE_EDGES -DPATH_LENGTH=6 -DMAMA_1 -DSOLVER_0"
t13="-DNODE_D_CONV -DMAMA_NSL -DSOLVER_LF"
t14="-DNODE_D_CONV -DREDUCE_EDGES -DPATH_LENGTH=6 -DMAMA_NSL -DSOLVER_LF"
t15="-DNODE_D_CONV -DEDGE_D_CONV -DMAMA_NS -DSOLVER_DP"
t16="-DNODE_D_CONV -DEDGE_D_CONV -DMAMA_NSL -DSOLVER_LF"

g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t1 $additional -o arcmatch-1
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t2 $additional -o arcmatch-2
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t3 $additional -o arcmatch-3 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t4 $additional -o arcmatch-4 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t5 $additional -o arcmatch-5 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t6 $additional -o arcmatch-6 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t7 $additional -o arcmatch-7
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t8 $additional -o arcmatch-8 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t9 $additional -o arcmatch-9 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t10 $additional -o arcmatch-10 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t11 $additional -o arcmatch-11 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t12 $additional -o arcmatch-12 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t13 $additional -o arcmatch-13 
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t14 $additional -o arcmatch-14
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t15 $additional -o arcmatch-15
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t16 $additional -o arcmatch-16 

cp arcmatch-7 arcmatch
cp arcmatch-13 arcmatch-lt


additional="-DPRINT_MATCHES"

g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t7 $additional  -o arcmatch-matches
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t13 $additional -o arcmatch-lt-matches


additional="-DFIRST_100k_MATCHES"

g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t7 $additional  -o arcmatch-100k
g++ -std=c++11 -O3  arcmatch_template.cpp -I ./rilib/ -I ./include/  $t13 $additional -o arcmatch-lt-100k