/*
 * ri3.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: vbonnici
 */

/*

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



/*
Nomenclature
rids4_
[
FR													custom file reader
_	
NODE_D_CONVERGENCE=0(inactive),1(active)			NODE_D_CONV is on for 1
EDGE_D_CONVERGENCE=0(inactive),1(active)			EDGE_D_CONV is on for 1
_
]
1|0|FC|AC|NS|NSç												MAMA_0 is on for 0
_
0|ED|DP												SOLVER_0 is on for 0
[
_
RE|NRE  (reduce or not edge domains)				REDUCE_EDGES is on for RE
]
*/

//#define MDEBUG

//#define PRINT_MATCHES
//#define CSV_FORMAT

//#define FIRST_MATCH_ONLY  //if setted, the searching process stops at the first found match

#define NODE_D_CONV	//refine node domains until convergence
#define EDGE_D_CONV	//refine edge domains until convergence

//#define REDUCE_EDGES


//#define MAMA_1 //original order
//#define MAMA_0 //simple matching machine with edge weigths set to 1
//#define MAMA_FC //mama by centrality flooding
//#define MAMA_AC //matching machine with angular coefficient
//#define MAMA_NS //the real original order with node sets for flags
#define MAMA_NSL //the real original order with node sets for flags, with disjoint leafs at the end of the ordering
//#define MAMA_CC //ordering by taking into account core compatibility

//#define SOLVER_0 //simple solver with edge domains
//#define SOLVER_ED //simple solver which exploits edge domains
#define SOLVER_DP //solver with dynamic parent selection

#define PATH_LENGTH 6
//#define PATH_LENGTH 9




#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>



#include "fr_textdb_driver.h"
#include "timer.h"


#include "AttributeComparator.h"
#include "Graph.h"
#include "MatchingMachine.h"
#include "MaMaConstrFirstDs.h"
#include "MaMaConstrFirstEDs.h"
#include "MaMaFloodCore.h"
#include "MaMaAngularCoefficient.h"
#include "MaMaConstrFirstNodeSets.h"
#include "MaMaConstrFirstNodeSetsLeafs.h"
#include "MaMaConstrFirstNSCC.h"


#include "MatchListener.h"

//#define FIRST_MATCH_ONLY  //if setted, the searching process stops at the first found match
#include "Solver.h"
#include "SubGISolver.h"
#include "InducedSubGISolver.h"
#include "Domains.h"


/*#include <unordered_set>
#include <tuple>
namespace rilib{
struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return v.first*31+v.second;
    }
};
typedef std::unordered_set< std::pair<int,int>, pair_hash> unordered_edge_set;
}
*/


using namespace rilib;

enum MATCH_TYPE {
	MT_ISO,		//isomprhism
	MT_INDSUB, //induced sub-isomorphism
	MT_MONO		//monomorphism
};

void usage(char* args0);
int match(MATCH_TYPE matchtype, GRAPH_FILE_TYPE filetype,	std::string& referencefile,	std::string& queryfile);


int main(int argc, char* argv[]){

	/*
	unordered_edge_set* domains;
	domains = new unordered_edge_set[10];
	domains[0].insert( std::pair<int,int>(0,1) );
	*/

	if(argc!=5){
			usage(argv[0]);
			return -1;
		}

	MATCH_TYPE matchtype;
	GRAPH_FILE_TYPE filetype;
	std::string reference;
	std::string query;

	std::string par = argv[1];
	if(par=="iso"){
		matchtype = MT_ISO;
	}
	else if(par=="ind"){
		matchtype = MT_INDSUB;
	}
	else
	if(par=="mono"){
		matchtype = MT_MONO;
	}
	else{
		usage(argv[0]);
		return -1;
	}

	par = argv[2];
	if(par=="gfu"){
		filetype = GFT_GFU;		//undirected type
	}
	else if(par=="gfd"){
		filetype = GFT_GFD;		//directed type
	}
	else if(par=="geu"){
		filetype = GFT_EGFU;	//undirect type with labels on edges
	}
	else if(par=="ged"){
		filetype = GFT_EGFD;	//direct type with labels on edges
	}
	//if no labels, domains are unuseful
//	else if(par=="vfu"){
//		filetype = GFT_VFU;
//	}
	else{
		usage(argv[0]);
		return -1;
	}

	reference = argv[3];
	query = argv[4];

	return match(matchtype, filetype, reference, query);
};





void usage(char* args0){
	std::cout<<"usage "<<args0<<" [iso ind mono] [gfu gfd geu ged] reference query\n";
	std::cout<<"\tmatch type:\n";
	std::cout<<"\t\tiso = isomorphism\n";
	std::cout<<"\t\tind = induced subisomorphism\n";
	std::cout<<"\t\tmono = monomorphism\n";
	std::cout<<"\tgraph input format:\n";
	std::cout<<"\t\tgfu = undirect graphs with labels on nodes\n";
	std::cout<<"\t\tgfd = direct graphs with labels on nodes\n";
	std::cout<<"\t\tgeu = undirect graphs with labels both on nodes and edges\n";
	std::cout<<"\t\tged = direct graphs with labels both on nodes and edges\n";
	std::cout<<"\treference file contains one ormore reference graphs\n";
	std::cout<<"\tquery contains the query graph (just one)\n";

};


int match(
		MATCH_TYPE 			matchtype,
		GRAPH_FILE_TYPE 	filetype,
		std::string& 		referencefile,
		std::string& 	queryfile){
	bool doBijIso = (matchtype == MT_ISO);

	TIMEHANDLE load_s, load_s_q, make_mama_s, match_s, total_s, s_tmp;
	double load_t=0;double load_t_q=0; double make_mama_t=0; double match_t=0; double total_t=0; double t_tmp;


	total_s=start_time();

	int rret;

	AttributeComparator* nodeComparator;	//to compare node labels
	AttributeComparator* edgeComparator;	//to compare edges labels
	switch(filetype){
		case GFT_GFU:
		case GFT_GFD:
			//for these formats, labels are only on nodes and they are strings
			nodeComparator = new StringAttrComparator();
			//nodeComparator = new DefaultAttrComparator();
			edgeComparator = new DefaultAttrComparator();
			break;
		case GFT_EGFU:
		case GFT_EGFD:
			//labels both on nodes and edges
			nodeComparator = new StringAttrComparator();
			edgeComparator = new StringAttrComparator();
			break;
//		case GFT_VFU:
//			//no labels
//			nodeComparator = new DefaultAttrComparator();
//			edgeComparator = new DefaultAttrComparator();
//			break;
	}


	TIMEHANDLE tt_start;
	double tt_end;

#ifdef MDEBUG
	std::cout<<"reading query...\n";
#endif

	//read the query
	load_s_q=start_time();
	Graph *query = new Graph();
	rret = read_graph(queryfile.c_str(), query, filetype);
	load_t_q+=end_time(load_s_q);
	if(rret !=0){
		std::cout<<"error on reading query graph\n";
	}

#ifdef MDEBUG
	query->print();
#endif

	//delete dquery;
	load_t_q+=end_time(load_s_q);

	long 	steps = 0,				//total number of steps of the backtracking phase
			triedcouples = 0, 		//nof tried pair (query node, reference node)
			matchcount = 0, 		//nof found matches
			matchedcouples = 0;		//nof mathed pair (during partial solutions)
	long tsteps = 0, ttriedcouples = 0, tmatchedcouples = 0;

	FileReader *fd = open_file(referencefile.c_str(), filetype);
	if(fd != NULL){
#ifdef PRINT_MATCHES
		//if you want to print found matches on screen
		MatchListener* matchListener=new ConsoleMatchListener();
#else
		//do not print matches
		MatchListener* matchListener=new EmptyMatchListener();
#endif

		int i=0;
		bool rreaded = true;
		do{	//for each reference graph in the file
			load_s=start_time();
			Graph * rrg = new Graph();
			//read the graph
#ifdef MDEBUG
	std::cout<<"reading reference...\n";
#endif
			int rret = read_dbgraph(referencefile.c_str(), fd, rrg, filetype);
			rreaded = fd->is_valid();
			load_t+=end_time(load_s);


			if(rreaded){
				if(!doBijIso ||
					(doBijIso && (query->nof_nodes == rrg->nof_nodes))){

					//initialize domains
					sbitset *domains = new sbitset[query->nof_nodes];
					match_s=start_time();
#ifdef MDEBUG
	std::cout<<"initializing domain...\n";
#endif
					s_tmp = start_time();
					bool domok = init_domains(*rrg, *query, *nodeComparator, *edgeComparator, domains, doBijIso);
					t_tmp = end_time(s_tmp);
					std::cout<<":time: init domains "<<t_tmp<<"\n";

					match_t+=end_time(match_s);

					//if domain constraints are satisfied (at least one compatible target node for each query node)
					if(domok){
						std:cout<<"domain ok\n";

#ifdef MDEBUG
	std::cout<<"initializing edge domain...\n";
#endif

						match_s=start_time();

						EdgeDomains edomains;
						std::cout<<"edomain init\n";

						s_tmp = start_time();
						init_edomains(*rrg, *query, domains, *edgeComparator, edomains);
						t_tmp = end_time(s_tmp);
						std::cout<<":time: init edomains "<<t_tmp<<"\n";

#ifdef MDEBUG
						print_domains(*query, *rrg, domains,edomains);
#endif

						s_tmp = start_time();

						#ifdef REDUCE_EDGES
						DomainReduction dr(*query, domains, edomains, rrg->nof_nodes);
						std::cout<<"edomain reduction\n";
						dr.reduce_by_paths(PATH_LENGTH);
						//dr.reduce_by_paths(query->nof_nodes+1);
						std::cout<<"edomain refinement\n";
						dr.final_refinement();
						std::cout<<"edomain done\n";
						#endif

						t_tmp = end_time(s_tmp);
						std::cout<<":time: reduce edomains "<<t_tmp<<"\n";

						match_t+=end_time(match_s);

#ifdef MDEBUG
						print_domains(*query, *rrg, domains,edomains);
#endif

#ifdef MDEBUG
	std::cout<<"building matching machine...\n";
#endif

						//just get the domain size for each query node
						int *domains_size = new int[query->nof_nodes];
						int dsize;
						for(int ii=0; ii<query->nof_nodes; ii++){
							dsize = 0;
							for(sbitset::iterator IT = domains[ii].first_ones(); IT!=domains[ii].end(); IT.next_ones()){
								dsize++;
							}
							domains_size[ii] = dsize;

							/*std::cout<<"dsize["<<ii<<"]("<<domains_size[ii]<<")\n";
							domains[ii].print_numbers();
							std::cout<<"\n";
							*/
						}

						for(int ii=0; ii<query->nof_nodes; ii++){
							std::cout<<ii<<" "<<query->out_adj_sizes[ii]<<" "<<domains_size[ii]<<"\n";
						}


						//build the static matching machine
						make_mama_s=start_time();
						
						//MatchingMachine* mama = new MaMaConstrFirstDs(*query, domains, domains_size);
						#ifdef MAMA_1
						MatchingMachine* mama = new MaMaConstrFirstDs(*query, domains, domains_size);
						#endif

						#ifdef MAMA_0
						MatchingMachine* mama = new MaMaConstrFirstEDs(*query, domains, domains_size, edomains);
						#endif

						#ifdef MAMA_FC
						MatchingMachine* mama = new MaMaFloodCore(*query, domains, domains_size, edomains, query->nof_nodes);
						#endif
						
						#ifdef MAMA_AC
						MatchingMachine* mama = new MaMaAngularCoefficient(*query, domains, domains_size, edomains);
						#endif

						#ifdef MAMA_NS
						MatchingMachine* mama = new MaMaConstrFirstNodeSets(*query, domains, domains_size);
						#endif

						#ifdef MAMA_NSL
						MatchingMachine* mama = new MaMaConstrFirstNodeSetsLeafs(*query, domains, domains_size);
						#endif

						#ifdef MAMA_CC
						MatchingMachine* mama = new MaMaConstrFirstNSCC(*query, domains, domains_size, *nodeComparator, *edgeComparator);
						#endif


						//std::cout<<"build mm\n";
						mama->build(*query);
						//std::cout<<"fix edis mm\n";
						mama->fix_eids(*query);
						make_mama_t+=end_time(make_mama_s);
						//std::cout<<"done\n";

						t_tmp = end_time(make_mama_s);
						std::cout<<":time: make mama "<<t_tmp<<"\n";

						std::cout<<"ordering: ";
						for(int ii=0; ii<mama->nof_sn; ii++){
							std::cout<<mama->map_state_to_node[ii]<<"("<<domains_size[mama->map_state_to_node[ii]]<<") ";
						}
						std::cout<<"\n";
						std::cout<<"domain sizes: ";
						for(int ii=0; ii<mama->nof_sn; ii++){
							std::cout<<domains_size[ii]<<" ";
						}
						std::cout<<"\n";

#ifdef MDEBUG
						mama->print();
						print_domains(*query, *rrg, domains,edomains);
						print_domains_extended(*query, *rrg, domains,edomains);
#endif

#ifdef MDEBUG
	mama->print();
#endif

						///match_s=start_time();

#ifdef MDEBUG
	std::cout<<"solving...\n";
#endif
						//prepare the matching phase
						Solver* solver;
						switch(matchtype){
						case MT_MONO:
							solver = new SubGISolver(*mama, *rrg, *query, *nodeComparator, *edgeComparator, *matchListener, domains, domains_size, edomains);
							break;
						case MT_ISO:  //a specialized solver for this will be better
						case MT_INDSUB:
							solver = new InducedSubGISolver(*mama, *rrg, *query, *nodeComparator, *edgeComparator, *matchListener, domains, domains_size, edomains);
							break;
						}

						//run the matching phase
						//std::cout<<"solving...\n";


						s_tmp = start_time();

						
						#ifdef SOLVER_0
						solver->solve();
						#endif

						#ifdef SOLVER_ED
						solver->solve_ed();
						#endif

						#ifdef SOLVER_DP
						solver->solve_rp();
						#endif


						t_tmp = end_time(s_tmp);
						std::cout<<":time: solve "<<t_tmp<<"\n";



						//std::cout<<"done\n";

						match_t+=end_time(match_s);

						steps += solver->steps;
						triedcouples += solver->triedcouples;
						matchedcouples += solver->matchedcouples;

						matchcount += solver->matchcount;

						delete solver;
						delete mama;
#ifdef MDEBUG
	std::cout<<"done\n";
#endif
					}
				}
//				delete rrg;
				//ReferenceGRaph destroyer is not yet developed...
			}
			i++;

			
		}while(rreaded);

#ifdef MDEBUG
	std::cout<<"all done\n";
#endif

		//if(matchListener != NULL)
		//matchcount += matchListener->matchcount;

		delete matchListener;

		fd->close();
	}
	else{
		std::cout<<"unable to open reference file\n";
		return -1;
	}

	total_t=end_time(total_s);

#ifdef CSV_FORMAT
	std::cout<<referencefile<<"\t"<<queryfile<<"\t";
	std:cout<<load_t_q<<"\t"<<make_mama_t<<"\t"<<load_t<<"\t"<<match_t<<"\t"<<total_t<<"\t"<<steps<<"\t"<<triedcouples<<"\t"<<matchedcouples<<"\t"<<matchcount;
#else
	std::cout<<"reference file: "<<referencefile<<"\n";
	std::cout<<"query file: "<<queryfile<<"\n";
	std::cout<<"reading time: "<<total_t<<"\n";
	std::cout<<"total time: "<<total_t<<"\n";
	std::cout<<"matching time: "<<match_t<<"\n";
	std::cout<<"number of found matches: "<<matchcount<<"\n";
	std::cout<<"search space size: "<<matchedcouples<<"\n";
#endif

	delete nodeComparator;
	delete edgeComparator;

	return 0;
};





