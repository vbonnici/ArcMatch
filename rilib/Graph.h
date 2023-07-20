/*
 * Graph.h
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

#ifndef GRAPH_H_
#define GRAPH_H_


namespace rilib{
class Graph{
public:
	int id;
	int nof_nodes;

	void** nodes_attrs;

	int* out_adj_sizes;
	int* in_adj_sizes;

	int** out_adj_list;
	int** in_adj_list;
	void*** out_adj_attrs;

	Graph(){
		id = -1;
		nof_nodes = 0;
		nodes_attrs = NULL;
		out_adj_sizes = NULL;
		in_adj_sizes = NULL;
		out_adj_list = NULL;
		in_adj_list = NULL;
		out_adj_attrs = NULL;
	}


	void print(){
		std::cout<<"| ReferenceGraph["<<id<<"] nof nodes["<<nof_nodes<<"]\n";
		for(int i=0; i<nof_nodes; i++){
			std::cout<<"| node["<<i<<"]\n";
			std::cout<<"| \tattribute_pointer["<<nodes_attrs[i]<<"]\n";
			std::cout<<"| \tattribute["<<*((std::string*)(nodes_attrs[i]))<<"]\n";
			std::cout<<"| \tout_adjs["<<out_adj_sizes[i]<<"][";
			for(int j=0; j<out_adj_sizes[i]; j++){
				std::cout<<out_adj_list[i][j];
				if(j!=out_adj_sizes[i]-1)
					std::cout<<", ";
			}
			std::cout<<"]\n";
			std::cout<<"| \tin_adjs["<<in_adj_sizes[i]<<"][";
			for(int j=0; j<in_adj_sizes[i]; j++){
				std::cout<<in_adj_list[i][j];
				if(j!=in_adj_sizes[i]-1)
					std::cout<<", ";
			}
			std::cout<<"]\n";
		}
	}
};
}


#endif /* REFERENCEGRAPH_H_ */
