/*
 * Domains.h
 */
/*
Copyright (c) 2022

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

#ifndef DOMAINS_H_
#define DOMAINS_H_


#include "sbitset.h"
#include "Graph.h"
#include "AttributeComparator.h"

#include <unordered_set>
#include <tuple>
#include <set>

namespace rilib{




bool init_domains(
		Graph&			target,
		Graph& 			pattern,
		AttributeComparator& 	nodeComparator,
		AttributeComparator& 	edgeComparator,
		sbitset *domains,
		bool iso
	){



	if(iso){
		for(int q=0; q<pattern.nof_nodes; q++){
			for(int r=target.nof_nodes-1; r>=0; r--){
				if(		target.out_adj_sizes[r] == pattern.out_adj_sizes[q]
					&&	target.in_adj_sizes[r] == pattern.in_adj_sizes[q]
					&& 	nodeComparator.compare(target.nodes_attrs[r], pattern.nodes_attrs[q])){
					domains[q].set(r, true);
				}
			}
			if(domains[q].is_empty()){
				return false;
			}
		}
	}
	else{
		for(int q=0; q<pattern.nof_nodes; q++){
			for(int r=target.nof_nodes-1; r>=0; r--){
				if(		target.out_adj_sizes[r] >= pattern.out_adj_sizes[q]
					&&	target.in_adj_sizes[r] >= pattern.in_adj_sizes[q]
					&& 	nodeComparator.compare(target.nodes_attrs[r], pattern.nodes_attrs[q])){
					domains[q].set(r, true);
				}
			}
			if(domains[q].is_empty()){
				return false;
			}
		}
	}






	int ra, qb, rb;
	bool notfound;

	//1°-level neighborhood and edges labels
	for(int qa=0; qa<pattern.nof_nodes; qa++){

		for(sbitset::iterator qaIT=domains[qa].first_ones(); qaIT!=domains[qa].end(); qaIT.next_ones()){
			ra = qaIT.first;
			//for each edge qa->qb  check if exists ra->rb
			for(int i_qb=0; i_qb<pattern.out_adj_sizes[qa]; i_qb++){
				qb = pattern.out_adj_list[qa][i_qb];
				notfound = true;

				for(int i_rb=0; i_rb<target.out_adj_sizes[ra]; i_rb++){
					rb = target.out_adj_list[ra][i_rb];
					if(domains[qb].get(rb) && edgeComparator.compare(pattern.out_adj_attrs[qa][i_qb], target.out_adj_attrs[ra][i_rb])){
						notfound = false;
						break;
					}
				}

				if(notfound){
					domains[qa].set(ra, false);
					break;
				}
			}
		}

		if(domains[qa].is_empty())
			return false;
	}


#ifdef NODE_D_CONV
	bool changes = true;
	while(changes){
		changes = false;
		for(int qa=0; qa<pattern.nof_nodes; qa++){
			for(sbitset::iterator qaIT=domains[qa].first_ones(); qaIT!=domains[qa].end(); qaIT.next_ones()){
				ra = qaIT.first;
				//fore each edge qa->qb  check if exists ra->rb
				for(int i_qb=0; i_qb<pattern.out_adj_sizes[qa]; i_qb++){
					qb = pattern.out_adj_list[qa][i_qb];
					notfound = true;
					for(int i_rb=0; i_rb<target.out_adj_sizes[ra]; i_rb++){
						rb = target.out_adj_list[ra][i_rb];
						if(domains[qb].get(rb) && edgeComparator.compare(pattern.out_adj_attrs[qa][i_qb], target.out_adj_attrs[ra][i_rb])){
							notfound = false;
							break;
						}
					}

					if(notfound){
						domains[qa].set(ra, false);
						changes = true;
					}
				}
			}
			if(domains[qa].is_empty())
				return false;
		}
	}
#endif

	return true;

};


struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return v.first*31+v.second;
    }
};

typedef std::unordered_set< std::pair<int,int>, pair_hash> unordered_edge_set;


class EdgeDomains{
public:
	int** pattern_out_adj_eids;
	int* inv_pattern_edge_ids;
	int* inv_target_edge_ids;
	int nof_pattern_edges;
	int nof_target_edges;

	int** pattern_in_adj_eids;

	unordered_edge_set* domains;

	EdgeDomains(){};
};



bool init_edomains(
		Graph&			target,
		Graph& 			pattern,
		sbitset* node_domains,
		AttributeComparator& 	edgeComparator,
		EdgeDomains& edomains
){
	int nof_pedges = 0;
	for(int i=0; i<pattern.nof_nodes; i++){
		nof_pedges += pattern.out_adj_sizes[i];
	}

	int nof_tedges = 0;
	for(int i=0; i<target.nof_nodes; i++){
		nof_tedges += target.out_adj_sizes[i];
	}

	edomains.nof_pattern_edges = nof_pedges;
	edomains.nof_target_edges = nof_tedges;

	edomains.pattern_out_adj_eids = new int*[pattern.nof_nodes]; 
	//edomains.target_out_adj_eids = new int*[target.nof_nodes]; 

	edomains.inv_pattern_edge_ids = new int[nof_pedges * 2];


	edomains.inv_target_edge_ids = new int[nof_tedges * 2];


	int n = 0;
	for(int i=0; i<pattern.nof_nodes; i++){
		edomains.pattern_out_adj_eids[i] = new int[pattern.out_adj_sizes[i]];
		for(int j=0; j<pattern.out_adj_sizes[i]; j++){
			edomains.inv_pattern_edge_ids[n*2] = i;
			edomains.inv_pattern_edge_ids[(n*2)+1] = j;
			edomains.pattern_out_adj_eids[i][j] = n;
			n++;
		}
	}

	edomains.pattern_in_adj_eids = new int*[pattern.nof_nodes]; 
	for(int i=0; i<pattern.nof_nodes; i++){
		edomains.pattern_in_adj_eids[i] = new int[pattern.in_adj_sizes[i]];

		for(int j=0; j<pattern.in_adj_sizes[i]; j++){
			int nn = pattern.in_adj_list[i][j];

			for(int ni =0; ni<pattern.out_adj_sizes[nn]; ni++){
				if(pattern.out_adj_list[nn][ni] == i){
					edomains.pattern_in_adj_eids[i][j] = edomains.pattern_out_adj_eids[nn][ni];
					break;
				}
			}
		}
	}

	n = 0;
	for(int i=0; i<target.nof_nodes; i++){
		//edomains.target_out_adj_eids[i] = new int[target.out_adj_sizes[i]];
		for(int j=0; j<target.out_adj_sizes[i]; j++){
			edomains.inv_target_edge_ids[n*2] = i;
			edomains.inv_target_edge_ids[(n*2)+1] = j;
			//edomains.target_out_adj_eids[i][j] = n;
			n++;
		}
	}

	//edomains.domains = new sbitset[nof_pedges];
	edomains.domains = new unordered_edge_set[nof_pedges];

	//std::cout<<"nof edges: "<<nof_pedges<<"\t"<<nof_tedges<<"\n";

	/*given a pattern edge, we search for compatible target edges.
	Given a pattern node p_s and a negihborn of it p_t, we look at the aleady computed domain of it.
	Given a target node t_s in the domain of p_s,
	we search for a neighborn t_t of t_s in the domain of p_t.
	The search is made by checking fo edge label compatibility.
	*/

	for(int ps = 0; ps<pattern.nof_nodes; ps++){
		for(int ps_n = 0; ps_n<pattern.out_adj_sizes[ps]; ps_n++){

			int pt = pattern.out_adj_list[ps][ps_n];

			for(sbitset::iterator psIT=node_domains[ps].first_ones(); 
				psIT != node_domains[ps].end(); 
				psIT.next_ones()){
				
				int ts = psIT.first;

				for(sbitset::iterator ptIT=node_domains[pt].first_ones(); 
				ptIT != node_domains[pt].end(); 
				ptIT.next_ones()){
				
					int tt = ptIT.first;

					for(int ts_n = 0; ts_n<target.out_adj_sizes[ts]; ts_n++){
						if (tt == target.out_adj_list[ts][ts_n]){
							if(edgeComparator.compare(
								pattern.out_adj_attrs[ps][ps_n], 
								target.out_adj_attrs[ts][ts_n])
								){


								//std::cout<<edomains.pattern_out_adj_eids[ps][ps_n]<<"\t"<<edomains.target_out_adj_eids[ts][ts_n]<<"\n";
								
								//std::cout<<edomains.pattern_out_adj_eids[ps][ps_n]<<"("<<ps<<","<<pt<<")"<<*((std::string*)pattern.out_adj_attrs[ps][ps_n])
								//	<<"] -> ("<<ts<<","<<tt<<")"<<*((std::string*)target.out_adj_attrs[ts][ts_n])<<"]"<<"\n";



								edomains.domains[  edomains.pattern_out_adj_eids[ps][ps_n]  ]
								.insert(  std::pair<int,int>(ts,tt) );
								//.set( edomains.target_out_adj_eids[ts][ts_n] , true);
								//.insert(  std::pair<int,int>(ts,tt) );
							}
						}
					}
				}
			}
		}
	}

	return true;

};


class DomainReduction {
public:
	Graph& query;
	sbitset* node_domains;
	EdgeDomains& edge_domains;
	int nof_target_nodes;

	DomainReduction(Graph& _query, sbitset* ndomains, EdgeDomains& edomains, int noftargetnodes) 
				: query(_query), node_domains(ndomains), edge_domains(edomains), nof_target_nodes(noftargetnodes)
	{};


	bool refine_domains(int altered_q_node){
		//std::cout<<"refine on altered node "<<altered_q_node<<"\n";
			
		bool erased = false;

		int n;
		for(int ni=0; ni < query.out_adj_sizes[altered_q_node]; ni++ ){
			n = query.out_adj_list[ altered_q_node][ni];
			unordered_edge_set* eset = 
			&(edge_domains.domains[  edge_domains.pattern_out_adj_eids[altered_q_node][ni]  ]);

			for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); ){
				if( node_domains[altered_q_node].get(eit->first) == false ){
					eset->erase(eit++);
					erased = true;
				}
				else{
					++eit;
				}
			}
		}

		for(int ni=0; ni < query.in_adj_sizes[altered_q_node]; ni++ ){
			n = query.in_adj_list[ altered_q_node][ni];
			unordered_edge_set* eset = 
			&(edge_domains.domains[  edge_domains.pattern_in_adj_eids[altered_q_node][ni]  ]);

			for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); ){
				if( node_domains[altered_q_node].get(eit->second) == false ){
					eset->erase(eit++);
					erased = true;
				}
				else{
					++eit;
				}
			}
		}



#ifdef EDGE_D_CONV
		if(erased){
			while(erased){
				erased = false;

				
				for(int s=0; s<query.nof_nodes; s++){
					for(int ni=0; ni < query.out_adj_sizes[s]; ni++ ){
						n = query.out_adj_list[s][ni];
						unordered_edge_set* eset = 
						&(edge_domains.domains[  edge_domains.pattern_out_adj_eids[s][ni]  ]);
						for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); ){
							if( (node_domains[s].get(eit->first) == false)  || (node_domains[n].get(eit->second) == false) ) {
								eset->erase(eit++);
							}
							else{
								++eit;
							}
						}
					}
				}

				for(int ni=0; ni < query.in_adj_sizes[altered_q_node]; ni++ ){
					n = query.in_adj_list[ altered_q_node][ni];
					unordered_edge_set* eset = 
					&(edge_domains.domains[  edge_domains.pattern_in_adj_eids[altered_q_node][ni]  ]);

					for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); ){
						if( node_domains[altered_q_node].get(eit->second) == false ){
							eset->erase(eit++);
							erased = true;
						}
						else{
							++eit;
						}
					}
				}
				

				for(int s=0; s<query.nof_nodes; s++){

					for(int ni=0; ni < query.out_adj_sizes[s]; ni++ ){
						n = query.out_adj_list[s][ni];

						std::set<int> source_set;
						unordered_edge_set* eset = 
						&(edge_domains.domains[  edge_domains.pattern_out_adj_eids[s][ni]  ]);
						for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); eit++){
							source_set.insert(eit->first);
						}


						for(sbitset::iterator dit = node_domains[s].first_ones();
							dit != node_domains[s].end();
							dit.next_ones()
							){

							if(source_set.find(dit.first) == source_set.end() ){
								node_domains[s].set(dit.first,false);
								erased = true;
							}
						}
					}


					
					for(int ni=0; ni < query.in_adj_sizes[s]; ni++ ){
						n = query.in_adj_list[s][ni];

						std::set<int> source_set;
						unordered_edge_set* eset = 
						&(edge_domains.domains[  edge_domains.pattern_in_adj_eids[s][ni]  ]);
						for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end(); eit++){
							source_set.insert(eit->second);
						}


						for(sbitset::iterator dit = node_domains[s].first_ones();
							dit != node_domains[s].end();
							dit.next_ones()
							){

							if(source_set.find(dit.first) == source_set.end() ){
								node_domains[s].set(dit.first,false);
								erased = true;
							}
						}
					}
					


				}
			}

		}
#endif
		return true;
	};

	bool verify_path_dfs(int *q_dfs, int* q_dfs_adji, int q_level, int *c_dfs, bool *c_visited, int c_level, bool circle){
		if(c_level == q_level - 1){
			if(circle){
				unordered_edge_set eset = 
				edge_domains.domains[  edge_domains.pattern_out_adj_eids[q_dfs[c_level]][q_dfs_adji[c_level+1]]  ];
				for(unordered_edge_set::iterator eit = eset.begin(); eit != eset.end(); eit++){
					if( (eit->first == c_dfs[c_level]) && (eit->second == c_dfs[0])){
						return true;
					}
				}
				return false;
			}
			else{
				unordered_edge_set eset = 
				edge_domains.domains[  edge_domains.pattern_out_adj_eids[q_dfs[c_level]][q_dfs_adji[c_level+1]]  ];
				for(unordered_edge_set::iterator eit = eset.begin(); eit != eset.end(); eit++){
					//std::cout<<"("<<eit->first<<","<<eit->second<<")\n";
					if( (eit->first == c_dfs[c_level]) && (!c_visited[eit->second])){
						return true;
					}
				}
				return false;
			}
		}
		else{
			bool found = false;
			unordered_edge_set eset = 
			edge_domains.domains[  edge_domains.pattern_out_adj_eids[q_dfs[c_level]][q_dfs_adji[c_level+1]]  ];
			for(unordered_edge_set::iterator eit = eset.begin(); eit != eset.end(); eit++){
				//std::cout<<"("<<eit->first<<","<<eit->second<<")\n";
				if( (eit->first == c_dfs[c_level]) && (!c_visited[eit->second])){
					c_dfs[c_level+1] = eit->second;
					c_visited[eit->second] = true;
					found |= verify_path_dfs(q_dfs, q_dfs_adji, q_level, c_dfs, c_visited, c_level+1, circle);
					c_visited[eit->second] = false;
					if(found){
						break;
					}
				}
			}
			return found;
		}
		return false;
		
	};

	void verify_path(int* q_dfs, int* q_dfs_adji, int q_level, bool circle){
		if(q_level > 1){
			//std::cout<<":>verify_path\n";
			int* c_dfs = new int[query.nof_nodes];
			bool* c_visited = new bool[nof_target_nodes];
			for(int i=0; i<nof_target_nodes; i++){
				c_visited[i] = false;
			}

			bool removed = false;
			int cnode;
			for(sbitset::iterator dit = node_domains[q_dfs[0]].first_ones();
				dit != node_domains[q_dfs[0]].end();
				dit.next_ones()
				){
				cnode = dit.first;
				c_dfs[0] = cnode;
				c_visited[cnode] = true;

				if(! verify_path_dfs(q_dfs, q_dfs_adji, q_level, c_dfs, c_visited, 0, circle) ){
					//std::cout<<"cadidate "<<cnode<<" of "<<q_dfs[0]<<" is going to be removed\n";
					node_domains[q_dfs[0]].set(cnode, false);
					removed = true;
				}

				c_visited[cnode] = false;
			}
			if(removed){
				//std::cout<<"updating node domains after removal ...\n";
				refine_domains(q_dfs[0]);
			}

			delete[] c_visited;
			delete[] c_dfs;
		}
	};


	/*
	This is a temporary version which works for undirected graphs 
	because only out edges are visited during the DFS visit.
	*/
	void reduce_by_paths_dfs(int* dfs, int* dfs_adji, bool* visited, int level, int max_lp){
		//std::cout<<">dfs("<<level<<","<<dfs[level]<<")\n";
		int n;
		int nof_p = 0;
		for(int ni=0; ni < query.out_adj_sizes[dfs[level]]; ni++ ){
			n = query.out_adj_list[ dfs[level] ][ni];
			//std::cout<<"neigh "<<n<<"\n";
			if( (!visited[n]) ){
				if(level == query.nof_nodes-2){
					/*std::cout<<"max path: ";
					for(int i=0; i<level+1; i++){
						std::cout<<dfs[i]<<" ";
					}
					std::cout<<n<<"\n";*/
					nof_p++;

					dfs[level+1] = n;
					dfs_adji[level+1] = ni;
					verify_path(dfs, dfs_adji, level+1, false);
				}
				else{
					nof_p++;
					dfs[level+1] = n;
					dfs_adji[level+1] = ni;

					if(level ==max_lp){
						verify_path(dfs, dfs_adji, level+1, false);
					}
					else{
						visited[n] = true;
						reduce_by_paths_dfs(dfs, dfs_adji, visited, level+1, max_lp);
						visited[n] = false;
					}
				}
			}
			else if ((level>0) && (n!=dfs[level-1]) && (n==dfs[0])){
				/*std::cout<<"circle: ";
				for(int i=0; i<level+1; i++){
					std::cout<<dfs[i]<<" ";
				}
				std::cout<<n<<"\n";*/

				nof_p++;
				dfs[level+1] = n;
				dfs_adji[level+1] = ni;
				
				verify_path(dfs, dfs_adji, level+1, true);
			}
		}
		if( (level>0) && (nof_p == 0)){
			/*std::cout<<"path: ";
			for(int i=0; i<level+1; i++){
				std::cout<<dfs[i]<<" ";
			}
			std::cout<<"\n";*/
			
			verify_path(dfs, dfs_adji, level-1, false);
		}
		//std::cout<<"<<dfs\n";
	};

	void reduce_by_paths(int starting_node, int max_lp){
		int* dfs = new int[query.nof_nodes];
		int* dfs_adji = new int[query.nof_nodes];
		bool* visited = new bool[query.nof_nodes];
		for(int i=0; i<query.nof_nodes;i++){
			visited[i] = false;
		}
		dfs[0] = starting_node;
		dfs_adji[0] = -1;
		visited[starting_node] = true;

		reduce_by_paths_dfs(dfs, dfs_adji, visited, 0, max_lp);

		delete[] visited;
		delete[] dfs;
		delete[] dfs_adji;
	};

	void reduce_by_paths(int max_lp){
		//std::cout<<"reducing by paths...\n";
		for(int i=0; i<query.nof_nodes; i++){
			//std::cout<<"reducing by paths "<<i<<"\n";
			reduce_by_paths(i, max_lp);

		}
		//std::cout<<"<<reducing by paths\n";
	};




	void final_refinement(){

		for(int n=0; n<query.nof_nodes; n++){


			for(	sbitset::iterator nnIT=node_domains[n].first_ones(); 
					nnIT!=node_domains[n].end(); 
					nnIT.next_ones()){
				int dn = nnIT.first;

				bool found;

				for(int ni=0; ni<query.out_adj_sizes[n]; ni++){
					found = false;

					unordered_edge_set eset = 
					edge_domains.domains[  edge_domains.pattern_out_adj_eids[n][ni]  ];
					for(unordered_edge_set::iterator eit = eset.begin(); eit != eset.end(); eit++){
						if(dn == eit->first){
							found = true;
							break;
						}
					}

					if(!found){
						node_domains[n].set(dn,false);
						break;
					}
				}

				for(int ni=0; ni<query.in_adj_sizes[n]; ni++){
					found = false;

					unordered_edge_set eset = 
					edge_domains.domains[  edge_domains.pattern_in_adj_eids[n][ni]  ];
					for(unordered_edge_set::iterator eit = eset.begin(); eit != eset.end(); eit++){
						if(dn == eit->second){
							found = true;
							break;
						}
					}

					if(!found){
						node_domains[n].set(dn,false);
						break;
					}
				}

			}

		}
	};

};


void print_domains(Graph& query, Graph& target, sbitset* node_domains,	EdgeDomains& edge_domains){
	std::cout<<"nof query nodes "<< query.nof_nodes<<"\n";
	for(int i=0; i<query.nof_nodes; i++){
		//std::cout<<"node domain "<<i<<":"<<node_domains[i].count_ones()<<"\n";
		//std::cout<<"node domain "<<i<<": ";
		std::cout<<"node domain "<<i<<":"<<node_domains[i].count_ones()<<": ";
		for(sbitset::iterator it = node_domains[i].first_ones(); it!=node_domains[i].end(); it.next_ones()){
			std:cout<<it.first<<" ";
		}
		std::cout<<"\n";
	}
	for(int n=0; n<query.nof_nodes; n++){
		for(int ni=0; ni<query.out_adj_sizes[n]; ni++){
			int eid = edge_domains.pattern_out_adj_eids[n][ni];
			std::cout<<"edge domain: "<<n<<"-"<<query.out_adj_list[n][ni]<<":eid "<<eid<<":"<<edge_domains.domains[eid].size()<<"\n";
		}
	}

};


void print_domains_extended(Graph& query, Graph& target, sbitset* node_domains,	EdgeDomains& edge_domains){
	std::cout<<"nof query nodes "<< query.nof_nodes<<"\n";
	for(int i=0; i<query.nof_nodes; i++){
		//std::cout<<"node domain "<<i<<":"<<node_domains[i].count_ones()<<"\n";
		//std::cout<<"node domain "<<i<<": ";
		std::cout<<"node domain "<<i<<":"<<node_domains[i].count_ones()<<": ";
		for(sbitset::iterator it = node_domains[i].first_ones(); it!=node_domains[i].end(); it.next_ones()){
			std:cout<<it.first<<" ";
		}
		std::cout<<"\n";
	}
	for(int n=0; n<query.nof_nodes; n++){
		for(int ni=0; ni<query.out_adj_sizes[n]; ni++){
			int eid = edge_domains.pattern_out_adj_eids[n][ni];
			std::cout<<"edge domain: "<<n<<"-"<<query.out_adj_list[n][ni]<<":eid "<<eid<<":"<<edge_domains.domains[eid].size()<<"\n";

			unordered_edge_set* eset = 
			&(edge_domains.domains[ eid  ]);

			for(unordered_edge_set::iterator eit = eset->begin(); eit != eset->end();eit++ ){
				std::cout<<"("<<(*eit).first<<","<<(*eit).second<<")";
			}
			std::cout<<"\n";
		}
	}

};





}//namespace rilib
#endif /* DOMAINS_H_ */
