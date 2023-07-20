/*
 * MaMaConstrFirst.h
 */
/*
Copyright (c) 2023

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

ArcMatch is provided under the terms of The MIT License (MIT):

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


#ifndef MAMAANGULARCOEFFICIENT_H_
#define MAMAANGULARCOEFFICIENT_H_

#include "Graph.h"
#include "sbitset.h"
#include "MatchingMachine.h"
#include "Domains.h"
#include <math.h>

namespace rilib{

class MaMaAngularCoefficient : public MatchingMachine{
	sbitset *domains;
	int *domains_size;
	EdgeDomains &edge_domains;

public:

	MaMaAngularCoefficient(Graph& query, sbitset *_domains, int *_domains_size, EdgeDomains &_edomains)
		: MatchingMachine(query), domains(_domains), domains_size(_domains_size), edge_domains(_edomains){

	}

	virtual void build(Graph& ssg){

#ifdef MDEBUG
	std::cout<<"init mama...\n";
#endif

		enum NodeFlag {NS_CORE, NS_CNEIGH, NS_UNV};
		NodeFlag* node_flags = new NodeFlag[nof_sn]; 						//indexed by node_id

		for(int n=0; n<nof_sn; n++){
			node_flags[n] = NS_UNV;
			map_node_to_state[n] = nof_sn;
			map_state_to_node[n] = nof_sn;
		}

		int nn;

		int si = 0;
		for(int n=0; n<nof_sn; n++){
			if(domains_size[n] == 1){
				map_node_to_state[n] = si;
				map_state_to_node[si] = n;
				si++;

				node_flags[n] = NS_CORE;
				for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
					nn = ssg.out_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}
				for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
					nn = ssg.in_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}
			}
		}

		double max_weight, max_out;

		double cw, tc, tn;
		double n_core, n_neigh, n_out;
		double w_core, w_neigh, w_out;
		int max_node;

		for(; si<nof_sn; si++){
			max_node = -1;

			for(int n=0; n<nof_sn; n++){
				if(node_flags[n] == NS_CNEIGH){

					n_core = 0.0;
					n_neigh = 0.0;
					n_out = 0.0;
					w_core = 0.0;
					w_neigh = 0.0;
					w_out = 0.0;

					for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
						nn = ssg.out_adj_list[n][ni];
						if(node_flags[nn] == NS_CORE){
							n_core++;
							w_core += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_out_adj_eids[n][ni]].size());
						}
						else if(node_flags[nn] == NS_CNEIGH){
							n_neigh++;
							w_neigh += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_out_adj_eids[n][ni]].size());
						}
						else{
							n_out++;
							w_out += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_out_adj_eids[n][ni]].size());
						}
					}
					for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
						nn = ssg.in_adj_list[n][ni];
						if(node_flags[nn] == NS_CORE){
							n_core++;
							w_core += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_in_adj_eids[n][ni]].size());
						}
						else if(node_flags[nn] == NS_CNEIGH){
							n_neigh++;
							w_neigh += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_in_adj_eids[n][ni]].size());
						}
						else{
							n_out++;
							w_out += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_in_adj_eids[n][ni]].size());
						}
					}

					tc = (n_core*n_core)/(w_core);
					tn = (n_neigh*n_neigh)/(w_neigh);
					cw = sqrt((tc*tc)+(tn*tn)) / ((double)domains_size[n]);

					if((max_node == -1)){
						max_node = n;
						max_weight = cw;
						max_out = (n_out*n_out) / w_out;
					}
					else{
						if(max_weight < cw){
							max_node = n;
							max_weight = cw;
							max_out = (n_out*n_out) / w_out;
						}
						else if(max_weight == cw){
							if(max_out < (n_out*n_out) / w_out){
								max_node = n;
								max_weight = cw;
								max_out = (n_out*n_out) / w_out;
							}
						}
					}
					
				}
			}

			if(max_node != -1){
				int n = max_node;
				map_node_to_state[n] = si;
				map_state_to_node[si] = n;
				node_flags[n] = NS_CORE;
				for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
					nn = ssg.out_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}
				for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
					nn = ssg.in_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}
			}
			else{
				//first node or new connected component

				for(int n=0; n<nof_sn; n++){
					if(node_flags[n] == NS_UNV){

						n_out = 0.0;
						w_out = 0.0;

						for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
							nn = ssg.out_adj_list[n][ni];
							n_out++;
							w_out += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_out_adj_eids[n][ni]].size());
						}
						for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
							nn = ssg.in_adj_list[n][ni];
							n_out++;
							w_out += 1.0 / ((double)edge_domains.domains[edge_domains.pattern_in_adj_eids[n][ni]].size());
						}

						tc = (n_out*n_out)/(w_out);

						if((max_node == -1)){
							max_node = n;
							max_weight = tc;
						}
						else{
							if(max_weight < tc){
								max_node = n;
								max_weight = tc;
							}
						}
						
					}
				}

				int n = max_node;
				map_node_to_state[n] = si;
				map_state_to_node[si] = n;
				node_flags[n] = NS_CORE;
				for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
					nn = ssg.out_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}
				for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
					nn = ssg.in_adj_list[n][ni];
					if(node_flags[nn] != NS_CORE){
						node_flags[nn] = NS_CNEIGH;
					}
				}


			}

		}



		for(int si=0; si<nof_sn; si++){
			parent_state[si] = -1;
			parent_type[si] = PARENTTYPE_NULL;
		}


		int max_parent, max_w=0, n, w;
		MAMA_PARENTTYPE max_t;


		for(int si=1; si<nof_sn; si++){
			max_parent = -1;
			max_t = PARENTTYPE_NULL;

			n = map_state_to_node[si];
#ifdef MDEBUG	
std::cout<<"PARENTING SI "<<si<<" N "<<n<<"\n";
#endif

			for(int ni=0; ni<ssg.out_adj_sizes[n]; ni++){
				nn = ssg.out_adj_list[n][ni];
				if(map_node_to_state[nn] < si){
					w = edge_domains.domains[edge_domains.pattern_out_adj_eids[n][ni]].size();
					if(max_parent == -1){
						max_parent = nn;
						max_w = w;
						max_t = PARENTTYPE_IN;
					}
					else if(max_w > w){
						max_parent = nn;
						max_w = w;
						max_t = PARENTTYPE_IN;
					}
				}
			}
			for(int ni=0; ni<ssg.in_adj_sizes[n]; ni++){
				nn = ssg.in_adj_list[n][ni];
				if(map_node_to_state[nn] < si){
					w = edge_domains.domains[edge_domains.pattern_in_adj_eids[n][ni]].size();
					if(max_parent == -1){
						max_parent = nn;
						max_w = w;
						max_t = PARENTTYPE_OUT;
					}
					else if(max_w > w){
						max_parent = nn;
						max_w = w;
						max_t = PARENTTYPE_OUT;
					}
				}
			}
			if(max_parent != -1){
				parent_state[si] = map_node_to_state[max_parent];
				parent_type[si] = max_t;

#ifdef MDEBUG	
std::cout<<"parent N "<<max_parent<<"\n" ;
std::cout<<"parent SI "<<map_node_to_state[max_parent]<<"\n" ;
if(max_t == PARENTTYPE_OUT){std::cout<<"OUT\n";}else{std::cout<<"IN\n";}
#endif
			}
		}




	int e_count,o_e_count,i_e_count;
	for(int si = 0; si<nof_sn; si++){

		n = map_state_to_node[si];

#ifdef MDEBUG
		if(parent_type[si] != PARENTTYPE_NULL){
			std::cout<<"P:"<<si<<" "<<n<<" "<<parent_state[si]<<" "<<map_state_to_node[parent_state[si]];
			if(parent_type[si] == PARENTTYPE_OUT)std::cout<<" out\n"; else std::cout<<" in\n";
		}
		else{
			std::cout<<"P:"<<si<<" "<<n<<" "<<parent_state[si]<<" NULL\n";;
		}
#endif
		e_count = 0;
		o_e_count = 0;
		for(int i=0; i<ssg.out_adj_sizes[n]; i++){
			if(map_node_to_state[ssg.out_adj_list[n][i]] < si){
				e_count++;
				o_e_count++;
			}
		}
		i_e_count = 0;
		for(int i=0; i<ssg.in_adj_sizes[n]; i++){
			if(map_node_to_state[ssg.in_adj_list[n][i]] < si){
				e_count++;
				i_e_count++;
			}
		}


		edges_sizes[si] = e_count;
		o_edges_sizes[si] = o_e_count;
		i_edges_sizes[si] = i_e_count;

		edges[si] = new MaMaEdge[e_count];

		if(e_count > 0){

			e_count = 0;
			
			
			for(int i=0; i<ssg.out_adj_sizes[n];i++){
				if(map_node_to_state[ssg.out_adj_list[n][i]] < si){
					edges[si][e_count].source = map_node_to_state[n];
					edges[si][e_count].target = map_node_to_state[ssg.out_adj_list[n][i]];
					e_count++;
				}
			}
			for(int i=0; i<ssg.in_adj_sizes[n];i++){
				if(map_node_to_state[ssg.in_adj_list[n][i]] < si){
					edges[si][e_count].target = map_node_to_state[n];
					edges[si][e_count].source = map_node_to_state[ssg.in_adj_list[n][i]];
					e_count++;
				}
			}
			
		}
	}


	delete [] node_flags;

	}
};

}


#endif /* MAMAANGULARCOEFFICIENT_H_ */
