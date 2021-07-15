#ifndef C_FR_TEXTDB_DRIVER_H_
#define C_FR_TEXTDB_DRIVER_H_


#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FileReader.h"

#include "Graph.h"

#include "timer.h"

using namespace std;
using namespace rilib;

enum GRAPH_FILE_TYPE {GFT_GFU, GFT_GFD, GFT_EGFU, GFT_EGFD, GFT_LAD};

#define STR_READ_LENGTH 256


int read_gfu(const char* fileName, FileReader* fd, Graph* graph);
int read_gfd(const char* fileName, FileReader* fd, Graph* graph);
//int read_lad(const char* fileName, FileReader* fd, Graph* graph);
int read_egfu(const char* fileName, FileReader* fd, Graph* graph);
int read_egfd(const char* fileName, FileReader* fd, Graph* graph);


FileReader* open_file(const char* filename, enum GRAPH_FILE_TYPE type){
	FileReader* fd = new FileReader(filename);
	return fd;
};


int read_dbgraph(const char* filename, FileReader* fd, Graph* g, enum GRAPH_FILE_TYPE type){
	int ret = 0;
	switch (type){
	case GFT_GFU:
		ret = read_gfu(filename, fd, g);
		break;
	case GFT_GFD:
		ret = read_gfd(filename, fd, g);
		break;
	case GFT_EGFU:
		ret = read_egfu(filename, fd, g);
		break;
	case GFT_EGFD:
		ret = read_egfd(filename, fd, g);
		break;
	//case GFT_LAD:
		//ret = read_lad(filename, fd, g);
	//	break;
	}

	return ret;
};

int read_graph(const char* filename, Graph* g, enum GRAPH_FILE_TYPE type){
	FileReader* fd = new FileReader(filename);
	if (fd==NULL){
		printf("ERROR: Cannot open input file %s\n", filename);
		exit(1);
	}
	int ret = 0;
	switch (type){
	case GFT_GFU:
		ret = read_gfu(filename, fd, g);
		break;
	case GFT_GFD:
		ret = read_gfd(filename, fd, g);
		break;
	case GFT_EGFU:
		ret = read_egfu(filename, fd, g);
		break;
	case GFT_EGFD:
		ret = read_egfd(filename, fd, g);
		break;
	//case GFT_LAD:
		//ret = read_lad(filename, fd, g);
	//	break;
	}

	fd->close();
    delete fd;
	return ret;
};



struct gr_neighs_t{
public:
	int nid;
	gr_neighs_t *next;
};





int read_gfu(const char* fileName, FileReader* fd, Graph* graph){
	TIMEHANDLE time_s;
	double time_e;


	time_s = start_time();
	char str[STR_READ_LENGTH];
	int i,j;

    fd->next_string();
	// if (!fd->is_valid()){	//#graphname
	// 	return -1;
	// }

    graph->nof_nodes = fd->next_int();
	// if(!fd->is_valid()){//nof nodes
	// 	return -1;
	// }

	//node labels
	graph->nodes_attrs = (void**)malloc(graph->nof_nodes * sizeof(void*));
	const char *label;
	for(i=0; i<graph->nof_nodes; i++){
        label = fd->next_string();
		// if(!fd->is_valid()){
		// 	return -1;
		// }
		graph->nodes_attrs[i] = new std::string(label);
	}

	//edges
	graph->out_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));
	graph->in_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));

	gr_neighs_t **ns_o = (gr_neighs_t**)malloc(graph->nof_nodes * sizeof(gr_neighs_t));
	gr_neighs_t **ns_i = (gr_neighs_t**)malloc(graph->nof_nodes * sizeof(gr_neighs_t));
	for(i=0; i<graph->nof_nodes; i++){
		ns_o[i] = NULL;
		ns_i[i] = NULL;
	}
	int temp = fd->next_int();
	// if (fscanf(fd,"%d",&temp) != 1){//number of edges
	// 	return -1;
	// }

	int es = 0, et = 0;
	for(i=0; i<temp; i++){
        es = fd->next_int();
        et = fd->next_int();
		// if (fscanf(fd,"%d",&es) != 1){//source node
		// 	return -1;
		// }
		// if (fscanf(fd,"%d",&et) != 1){//target node
		// 	return -1;
		// }

		graph->out_adj_sizes[es]++;
		graph->in_adj_sizes[et]++;

		if(ns_o[es] == NULL){
			ns_o[es] = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			ns_o[es]->nid = et;
			ns_o[es]->next = NULL;
		}
		else{
			gr_neighs_t* n = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			n->nid = et;
			n->next = (struct gr_neighs_t*)ns_o[es];
			ns_o[es] = n;
		}

		graph->out_adj_sizes[et]++;
		graph->in_adj_sizes[es]++;

		if(ns_o[et] == NULL){
			ns_o[et] = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			ns_o[et]->nid = es;
			ns_o[et]->next = NULL;
		}
		else{
			gr_neighs_t* n = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			n->nid = es;
			n->next = (struct gr_neighs_t*)ns_o[et];
			ns_o[et] = n;
		}

	}
	time_e = end_time(time_s);
	std::cout<<":rtime: from file "<<time_e<<"\n";
	time_s = start_time();


	graph->out_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->in_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->out_adj_attrs = (void***)malloc(graph->nof_nodes * sizeof(void**));

	int* ink = (int*)calloc(graph->nof_nodes, sizeof(int));
	for (i=0; i<graph->nof_nodes; i++){
		graph->in_adj_list[i] = (int*)calloc(graph->in_adj_sizes[i], sizeof(int));

	}
	for (i=0; i<graph->nof_nodes; i++){
		// reading degree and successors of vertex i
		graph->out_adj_list[i] = (int*)calloc(graph->out_adj_sizes[i], sizeof(int));
		graph->out_adj_attrs[i] = (void**)malloc(graph->out_adj_sizes[i] * sizeof(void*));

		gr_neighs_t *n = ns_o[i];
		for (j=0; j<graph->out_adj_sizes[i]; j++){
			graph->out_adj_list[i][j] = n->nid;
			graph->out_adj_attrs[i][j] = NULL;

			graph->in_adj_list[n->nid][ink[n->nid]] = i;

			ink[n->nid]++;

			n = n->next;
		}
	}

	time_e = end_time(time_s);
	std::cout<<":rtime: data structures "<<time_e<<"\n";
	time_s = start_time();


//	graph->sort_edges();

	for(int i=0; i<graph->nof_nodes; i++){
		if(ns_o[i] != NULL){
			gr_neighs_t *p = NULL;
			gr_neighs_t *n = ns_o[i];
			for (j=0; j<graph->out_adj_sizes[i]; j++){
				if(p!=NULL)
					free(p);
				p = n;
				n = n->next;
			}
			if(p!=NULL)
			free(p);
		}

		if(ns_i[i] != NULL){
			gr_neighs_t *p = NULL;
			gr_neighs_t *n = ns_i[i];
			for (j=0; j<graph->out_adj_sizes[i]; j++){
				if(p!=NULL)
					free(p);
				p = n;
				n = n->next;
			}
			if(p!=NULL)
			free(p);
		}


//			free(ns_o);
//			free(ns_i);
	}

	time_e = end_time(time_s);
	std::cout<<":rtime: free "<<time_e<<"\n";

	return 0;
};




int read_gfd(const char* fileName, FileReader* fd, Graph* graph){
	char str[STR_READ_LENGTH];
	int i,j;

    fd->next_string();
	// if (fscanf(fd,"%s",str) != 1){	//#graphname
	// 	return -1;
	// }
    graph->nof_nodes = fd->next_int();
	// if (fscanf(fd,"%d",&(graph->nof_nodes)) != 1){//nof nodes
	// 	return -1;
	// }
	//node labels
	graph->nodes_attrs = (void**)malloc(graph->nof_nodes * sizeof(void*));
	const char *label;// = new char[STR_READ_LENGTH];
	for(i=0; i<graph->nof_nodes; i++){
        label = fd->next_string();
		// if (fscanf(fd,"%s",label) != 1){
		// 	return -1;
		// }
		graph->nodes_attrs[i] = new std::string(label);
	}

	//edges
	graph->out_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));
	graph->in_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));

	gr_neighs_t **ns_o = (gr_neighs_t**)malloc(graph->nof_nodes * sizeof(gr_neighs_t));
	gr_neighs_t **ns_i = (gr_neighs_t**)malloc(graph->nof_nodes * sizeof(gr_neighs_t));
	for(i=0; i<graph->nof_nodes; i++){
		ns_o[i] = NULL;
		ns_i[i] = NULL;
	}
	int temp = fd->next_int();;
	// if (fscanf(fd,"%d",&temp) != 1){//number of edges
	// 	return -1;
	// }
	int es = 0, et = 0;
	for(i=0; i<temp; i++){
        es = fd->next_int();
        et = fd->next_int();
		// if (fscanf(fd,"%d",&es) != 1){//source node
		// 	return -1;
		// }
		// if (fscanf(fd,"%d",&et) != 1){//target node
		// 	return -1;
		// }
		graph->out_adj_sizes[es]++;
		graph->in_adj_sizes[et]++;

		if(ns_o[es] == NULL){
			ns_o[es] = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			ns_o[es]->nid = et;
			ns_o[es]->next = NULL;
		}
		else{
			gr_neighs_t* n = (gr_neighs_t*)malloc(sizeof(gr_neighs_t));
			n->nid = et;
			n->next = (struct gr_neighs_t*)ns_o[es];
			ns_o[es] = n;
		}
	}


	graph->out_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->in_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->out_adj_attrs = (void***)malloc(graph->nof_nodes * sizeof(void**));
	int* ink = (int*)calloc(graph->nof_nodes, sizeof(int));
	for (i=0; i<graph->nof_nodes; i++)
		graph->in_adj_list[i] = (int*)calloc(graph->in_adj_sizes[i], sizeof(int));
	for (i=0; i<graph->nof_nodes; i++){
		// reading degree and successors of vertex i
		graph->out_adj_list[i] = (int*)calloc(graph->out_adj_sizes[i], sizeof(int));
		graph->out_adj_attrs[i] = (void**)malloc(graph->out_adj_sizes[i] * sizeof(void*));

		gr_neighs_t *n = ns_o[i];
		for (j=0; j<graph->out_adj_sizes[i]; j++){
			graph->out_adj_list[i][j] = n->nid;
			graph->out_adj_attrs[i][j] = NULL;

			graph->in_adj_list[n->nid][ink[n->nid]] = i;
			ink[n->nid]++;

			n = n->next;
		}
	}

//	graph->sort_edges();

	return 0;
};






struct egr_neighs_t{
public:
	int nid;
	egr_neighs_t *next;
	std::string* label;
};





int read_egfu(const char* fileName, FileReader* fd, Graph* graph){
	char str[STR_READ_LENGTH];
	int i,j;

    fd->next_string();
    graph->nof_nodes = fd->next_int();
	// if (fscanf(fd,"%s",str) != 1){	//#graphname
	// 	return -1;
	// }
	// if (fscanf(fd,"%d",&(graph->nof_nodes)) != 1){//nof nodes
	// 	return -1;
	// }

	//node labels
	graph->nodes_attrs = (void**)malloc(graph->nof_nodes * sizeof(void*));
	//char *label = new char[STR_READ_LENGTH];
    const char * label;
	for(i=0; i<graph->nof_nodes; i++){
        label = fd->next_string();
		// if (fscanf(fd,"%s",label) != 1){
		// 	return -1;
		// }
		graph->nodes_attrs[i] = new std::string(label);
	}

	//edges
	graph->out_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));
	graph->in_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));

	egr_neighs_t **ns_o = (egr_neighs_t**)malloc(graph->nof_nodes * sizeof(egr_neighs_t));
	egr_neighs_t **ns_i = (egr_neighs_t**)malloc(graph->nof_nodes * sizeof(egr_neighs_t));
	for(i=0; i<graph->nof_nodes; i++){
		ns_o[i] = NULL;
		ns_i[i] = NULL;
	}
	// int temp = 0;
	// if (fscanf(fd,"%d",&temp) != 1){//number of edges
	// 	return -1;
	// }
    int temp = fd->next_int();

	int es = 0, et = 0;
	for(i=0; i<temp; i++){
        es = fd->next_int();
        et = fd->next_int();
        label = fd->next_string();
		// if (fscanf(fd,"%d",&es) != 1){//source node
		// 	return -1;
		// }
		// if (fscanf(fd,"%d",&et) != 1){//target node
		// 	return -1;
		// }
		// if (fscanf(fd,"%s",label) != 1){
		// 	return -1;
		// }

		graph->out_adj_sizes[es]++;
		graph->in_adj_sizes[et]++;

		if(ns_o[es] == NULL){
			ns_o[es] = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			ns_o[es]->nid = et;
			ns_o[es]->next = NULL;
			ns_o[es]->label = new std::string(label);
		}
		else{
			egr_neighs_t* n = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			n->nid = et;
			n->next = (struct egr_neighs_t*)ns_o[es];
			n->label = new std::string(label);
			ns_o[es] = n;
		}

		graph->out_adj_sizes[et]++;
		graph->in_adj_sizes[es]++;

		if(ns_o[et] == NULL){
			ns_o[et] = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			ns_o[et]->nid = es;
			ns_o[et]->next = NULL;
			ns_o[et]->label = new std::string(label);
		}
		else{
			egr_neighs_t* n = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			n->nid = es;
			n->next = (struct egr_neighs_t*)ns_o[et];
			n->label = new std::string(label);
			ns_o[et] = n;
		}

	}


	graph->out_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->in_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->out_adj_attrs = (void***)malloc(graph->nof_nodes * sizeof(void**));

	int* ink = (int*)calloc(graph->nof_nodes, sizeof(int));
	for (i=0; i<graph->nof_nodes; i++){
		graph->in_adj_list[i] = (int*)calloc(graph->in_adj_sizes[i], sizeof(int));

	}
	for (i=0; i<graph->nof_nodes; i++){
		// reading degree and successors of vertex i
		graph->out_adj_list[i] = (int*)calloc(graph->out_adj_sizes[i], sizeof(int));
		graph->out_adj_attrs[i] = (void**)malloc(graph->out_adj_sizes[i] * sizeof(void*));

		egr_neighs_t *n = ns_o[i];
		for (j=0; j<graph->out_adj_sizes[i]; j++){
			graph->out_adj_list[i][j] = n->nid;
			graph->out_adj_attrs[i][j] = n->label;

			graph->in_adj_list[n->nid][ink[n->nid]] = i;

			ink[n->nid]++;

			n = n->next;
		}
	}

//	graph->sort_edges();



	for(int i=0; i<graph->nof_nodes; i++){
		if(ns_o[i] != NULL){
			egr_neighs_t *p = NULL;
			egr_neighs_t *n = ns_o[i];
			for (j=0; j<graph->out_adj_sizes[i]; j++){
				if(p!=NULL)
					free(p);
				p = n;
				n = n->next;
			}
			if(p!=NULL)
			free(p);
		}

		if(ns_i[i] != NULL){
			egr_neighs_t *p = NULL;
			egr_neighs_t *n = ns_i[i];
			for (j=0; j<graph->out_adj_sizes[i]; j++){
				if(p!=NULL)
					free(p);
				p = n;
				n = n->next;
			}
			if(p!=NULL)
			free(p);
		}


//			free(ns_o);
//			free(ns_i);
	}

	return 0;
};


int read_egfd(const char* fileName, FileReader* fd, Graph* graph){
	std::cout<<"reading............\n";

	char str[STR_READ_LENGTH];
	int i,j;

    fd->next_string();
    graph->nof_nodes = fd->next_int();
	// if (fscanf(fd,"%s",str) != 1){	//#graphname
	// 	return -1;
	// }
	// if (fscanf(fd,"%d",&(graph->nof_nodes)) != 1){//nof nodes
	// 	return -1;
	// }
	//node labels
	graph->nodes_attrs = (void**)malloc(graph->nof_nodes * sizeof(void*));
	//char *label = new char[STR_READ_LENGTH];
    const char * label;
	for(i=0; i<graph->nof_nodes; i++){
        label = fd->next_string();
		// if (fscanf(fd,"%s",label) != 1){
		// 	return -1;
		// }
		graph->nodes_attrs[i] = new std::string(label);
	}

	//edges
	graph->out_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));
	graph->in_adj_sizes = (int*)calloc(graph->nof_nodes, sizeof(int));

	egr_neighs_t **ns_o = (egr_neighs_t**)malloc(graph->nof_nodes * sizeof(egr_neighs_t));
	egr_neighs_t **ns_i = (egr_neighs_t**)malloc(graph->nof_nodes * sizeof(egr_neighs_t));
	for(i=0; i<graph->nof_nodes; i++){
		ns_o[i] = NULL;
		ns_i[i] = NULL;
	}
	// int temp = 0;
	// if (fscanf(fd,"%d",&temp) != 1){//number of edges
	// 	return -1;
	// }
    int temp = fd->next_int();
	int es = 0, et = 0;
	for(i=0; i<temp; i++){
        es = fd->next_int();
        et = fd->next_int();
        label = fd->next_string();
		// if (fscanf(fd,"%d",&es) != 1){//source node
		// 	return -1;
		// }
		// if (fscanf(fd,"%d",&et) != 1){//target node
		// 	return -1;
		// }
		// if (fscanf(fd,"%s",label) != 1){
		// 	return -1;
		// }

		//std::cout<<es<<" "<<et<<" "<<label<<"\n";

		graph->out_adj_sizes[es]++;
		graph->in_adj_sizes[et]++;

		if(ns_o[es] == NULL){
			ns_o[es] = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			ns_o[es]->nid = et;
			ns_o[es]->next = NULL;
			ns_o[es]->label = new std::string(label);
		}
		else{
			egr_neighs_t* n = (egr_neighs_t*)malloc(sizeof(egr_neighs_t));
			n->nid = et;
			n->next = (struct egr_neighs_t*)ns_o[es];
			n->label = new std::string(label);
			ns_o[es] = n;
		}

	}

	graph->out_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->in_adj_list = (int**)malloc(graph->nof_nodes * sizeof(int*));
	graph->out_adj_attrs = (void***)malloc(graph->nof_nodes * sizeof(void**));

	int* ink = (int*)calloc(graph->nof_nodes, sizeof(int));
	for (i=0; i<graph->nof_nodes; i++){
		graph->in_adj_list[i] = (int*)calloc(graph->in_adj_sizes[i], sizeof(int));

	}
	for (i=0; i<graph->nof_nodes; i++){
		// reading degree and successors of vertex i
		graph->out_adj_list[i] = (int*)calloc(graph->out_adj_sizes[i], sizeof(int));
		graph->out_adj_attrs[i] = (void**)malloc(graph->out_adj_sizes[i] * sizeof(void*));

		egr_neighs_t *n = ns_o[i];
		for (j=0; j<graph->out_adj_sizes[i]; j++){
			graph->out_adj_list[i][j] = n->nid;
			graph->out_adj_attrs[i][j] = n->label;

			graph->in_adj_list[n->nid][ink[n->nid]] = i;
			ink[n->nid]++;

			n = n->next;
		}
	}

//	graph->sort_edges();

	return 0;
};


#endif //C_FR_TEXTDB_DRIVER_H_