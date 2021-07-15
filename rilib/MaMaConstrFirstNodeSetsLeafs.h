/*
 * MaMaConstrFirst.h
 *
 *  Created on: Aug 5, 2012
 *      Author: vbonnici
 */
/*
Copyright (c) 2014 by Rosalba Giugno

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


#ifndef MAMACONSTRFIRSTNODESETSLEAFS_H_
#define MAMACONSTRFIRSTNODESETSLEAFS_H_

//#define MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG 1

#include "Graph.h"
#include "sbitset.h"
#include "MatchingMachine.h"
#include <set>

namespace rilib{

class MaMaConstrFirstNodeSetsLeafs : public MatchingMachine{
	enum NodeFlag {NS_CORE, NS_CNEIGH, NS_UNV};

	sbitset *domains;
	int *domains_size;

public:

	MaMaConstrFirstNodeSetsLeafs(Graph& query, sbitset *_domains, int *_domains_size)
		: MatchingMachine(query), domains(_domains), domains_size(_domains_size){

	}

	virtual void build(Graph& ssg){

#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
		ssg.print();
#endif

		NodeFlag* node_flags = new NodeFlag[nof_sn]; 						//indexed by node_id
		for(int i=0; i<nof_sn; i++){
			node_flags[i] = NS_UNV;
		}

		int si = 0;
		for(int i=0; i<nof_sn; i++){
			if(domains_size[i] == 1){
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
				std::cout<<"ssi["<<si<<"] = "<<i<<"\n";
#endif
				push_node_to_core(i, si, node_flags, ssg, map_state_to_node, map_node_to_state);
				si++;
			}
		}



		bool *good_leafs = new bool[nof_sn];
		bool *bad_leafs = new bool[nof_sn];
		for(int i=0; i<nof_sn; i++){
			good_leafs[i] = false;
			bad_leafs[i] = false;
			if(node_flags[i] != NS_CORE){
				std::set<int> neighs;
				for(int j=0; j< ssg.out_adj_sizes[i]; j++){
					neighs.insert(ssg.out_adj_list[i][j]);
				}
				for(int j=0; j< ssg.in_adj_sizes[i]; j++){
					neighs.insert(ssg.in_adj_list[i][j]);
				}
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
				std::cout<<i<<" nof neighs "<<neighs.size()<<"\n";
#endif
				if(neighs.size() == 1){
					good_leafs[i] = true;
				}
			}
		}

#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
		std::cout<<"preliminary leafs: ";
		for(int i=0; i<nof_sn; i++){
			if(good_leafs[i]){
				std::cout<<i<<" ";
			}
		}
		std::cout<<"\n";
#endif
		for(int i=0; i<nof_sn; i++){
			if(good_leafs[i] && (!bad_leafs[i])){
				for(int j=0; j<nof_sn; j++){
					if(i!=j){
						if(good_leafs[j] && (!bad_leafs[j])){
							if( ! domains[i].emptyAND(domains[j]) ){

								if(domains_size[j] >= domains_size[i]){
									//bad_leafs[i] = true;
									bad_leafs[j] = true;
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
								std::cout<<"remove leaf "<<j<<" by "<<i<<" \n";
#endif
								}
								else{
									bad_leafs[i] = true;
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
								std::cout<<"remove leaf "<<i<<" by "<<j<<" \n";
#endif	
									break;
								}
								
							}
						}
					}
				}
			}
		}


		int leafi = nof_sn-1;
		nof_leafs = 0;

		for(int i=0; i<nof_sn; i++){
			if(good_leafs[i] && (!bad_leafs[i])){
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
				std::cout<<"leaf vertex "<<i<<" at "<<leafi<<"\n";
#endif				
				map_state_to_node[leafi] = i;
				map_node_to_state[i] = leafi;
				leafi--;
				nof_leafs++;
			}
			else{
				good_leafs[i] = false;
			}
		}
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
				std::cout<<"nof leafs = "<<nof_leafs<<" \n";
#endif	
		//nof_leafs = nof_sn - leafi + 1;




#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
		for(int i=0; i<nof_sn; i++){
			std::cout<<i<<"["<<node_flags[i]<<"] ";
		}
		std::cout<<"\n";
#endif

		for( ; si<leafi + 1; si++){

#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			std::cout<<"SI["<<si<<"]\n";
#endif
			int best_nid = -1;
			int best_nid_score[] = {0,0,0,0,0};
			int current_nid_score[] = {0,0,0,0,0};


#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			for(int nid=0; nid<nof_sn; nid++){
				std::cout<<nid<<"("<<node_flags[nid]<<") ";
				get_scores(nid, current_nid_score, node_flags, ssg);
				std::cout<<"[";
				for(int i=0; i<5; i++){
					std::cout<<current_nid_score[i]<<" ";
				}
				std::cout<<"]\n";
			}
#endif

			for(int nid=0; nid<nof_sn; nid++){
				if( (node_flags[nid] == NS_CNEIGH) && (!good_leafs[nid]) ){
					if(best_nid == -1){
						best_nid = nid;
						get_scores(nid, best_nid_score, node_flags, ssg);
					}
					else{
						get_scores(nid, current_nid_score, node_flags, ssg);
						if(compare_scores(current_nid_score, best_nid_score, nid, best_nid) > 0){
							best_nid = nid;
							for(int i=0; i<5; i++){
								best_nid_score[i] = current_nid_score[i];
							}
						}
					}
				}
			}

			if(best_nid == -1){//firs node without singletons or disconnected query
				for(int nid=0; nid<nof_sn; nid++){
					if( (node_flags[nid] == NS_UNV)  && (!good_leafs[nid])  ){
						if(best_nid == -1){
							best_nid = nid;
							get_scores(nid, best_nid_score, node_flags, ssg);
						}
						else{
							get_scores(nid, current_nid_score, node_flags, ssg);
							if(compare_scores(current_nid_score, best_nid_score, nid, best_nid) > 0){
								best_nid = nid;
								for(int i=0; i<5; i++){
									best_nid_score[i] = current_nid_score[i];
								}
							}
						}
					}
				}
			}
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			std::cout<<"si["<<si<<"] = "<<best_nid<<"\n";
#endif
			push_node_to_core(best_nid, si, node_flags, ssg, map_state_to_node, map_node_to_state);

#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			for(int i=0; i<nof_sn; i++){
				std::cout<<i<<"["<<node_flags[i]<<"] ";
			}
			std::cout<<"\n";
#endif
		}


#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			std::cout<<"node to state: ";
			for(int i=0; i<nof_sn; i++){
				std::cout<<i<<"["<< map_node_to_state[i]<<"] ";
			}
			std::cout<<"\n";
			std::cout<<"state to node: ";
			for(int i=0; i<nof_sn; i++){
				std::cout<<i<<"["<< map_state_to_node[i]<<"] ";
			}
			std::cout<<"\n";
#endif

		int e_count,o_e_count,i_e_count,n,nn;
		for(int si = 0; si<nof_sn; si++){

			n = map_state_to_node[si];
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


#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			std::cout<<"si["<<si<<"] n["<<n<<"] e_cout["<<e_count<<"] o_count["<<o_e_count<<"] i_count["<<i_e_count<<"]\n";
#endif

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

	}





private:
	void push_node_to_core(int nid, int si, NodeFlag* node_flags, Graph &qg, int *map_state_to_node, int *map_node_to_state){
		node_flags[nid] = NS_CORE;

		for(int i=0; i< qg.out_adj_sizes[nid]; i++){
			if(node_flags[ qg.out_adj_list[nid][i] ] == NS_UNV ){
				node_flags[ qg.out_adj_list[nid][i] ] = NS_CNEIGH;
			}
		}
		for(int i=0; i< qg.in_adj_sizes[nid]; i++){
			if(node_flags[ qg.in_adj_list[nid][i] ] == NS_UNV ){
				node_flags[ qg.in_adj_list[nid][i] ] = NS_CNEIGH;
			}
		}
		map_state_to_node[si] = nid;
		map_node_to_state[nid] = si;
	}

	void get_scores(int nid, int *scores,  NodeFlag* node_flags, Graph &qg){
		//for(int i=0; i<4; i++){
		//	scores[i] = 0;
		//}

		std::set<int> all;

		std::set<int> cores;
		std::set<int> neighs;
		std::set<int> unvs;

		for(int i=0; i<qg.out_adj_sizes[nid]; i++){
			if(node_flags[ qg.out_adj_list[nid][i] ] == NS_CORE){
				//scores[0]++;
				cores.insert(qg.out_adj_list[nid][i]);
			}
			else if(node_flags[ qg.out_adj_list[nid][i] ] == NS_CNEIGH){
				//scores[1]++;
				neighs.insert(qg.out_adj_list[nid][i]);
			}
			else{
				//scores[2]++;
				unvs.insert(qg.out_adj_list[nid][i]);
			}
			all.insert(qg.out_adj_list[nid][i]);
		}
		for(int i=0; i<qg.in_adj_sizes[nid]; i++){
			if(node_flags[ qg.in_adj_list[nid][i] ] == NS_CORE){
				//scores[0]++;
				cores.insert(qg.in_adj_list[nid][i]);
			}
			else if(node_flags[ qg.in_adj_list[nid][i] ] == NS_CNEIGH){
				//scores[1]++;
				neighs.insert(qg.in_adj_list[nid][i]);
			}
			else{
				//scores[2]++;
				unvs.insert(qg.in_adj_list[nid][i]);
			}
			all.insert(qg.in_adj_list[nid][i]);
		}

		scores[0] = cores.size();
		scores[1] = neighs.size();
		scores[2] = unvs.size();
		scores[3] = all.size();
		scores[4] = domains_size[nid];
		//scores[4] = qg.out_adj_sizes[nid] + qg.in_adj_sizes[nid];
	}


	int compare_scores(int *s1, int *s2, int n1, int n2){
		for(int i=0; i<4; i++){
			if(s1[i] != s2[i]){
				return s1[i]-s2[i];
			}
		}

		if(s1[4] != s2[4]){
#ifdef MAMACONSTRFIRSTNODESETSLEAFS_H_MDEBUG
			std::cout<<"dcomp "<<n1<<"-"<<n2<<" "<<s1[4]<<"-"<<s2[4]<<" \n";
#endif
			return s2[4]- s1[4];
		}
		/*
		if(s1[3] != s2[3]){
			return s2[3]- s1[3];
		}
		if(s1[4] != s2[4]){
			return s1[4]- s2[4];
		}*/
		return n2-n1;
	}
};

}


#endif /* MAMACONSTRFIRSTNODESETSLEAFS_H_ */

