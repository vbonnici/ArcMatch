# ArcMatch
***Version: 1.0***
> ArcMatch is a general-purpose algorithm for one-to-one exact subgraph isomorphism problems by maintaining topological constraints. 

<hr />

## Description
***ArcMatch*** is a general-purpose algorithm for one-to-one exact subgraph isomorphism problems by maintaining topological constraints.
It is based on the concept of vertex and edge domains ( feasible candidate elements) by introducing a new path-based technique for filtering such domains 
and by exploiting the information coming from them at all levels of the search process.
Domains allow ***ArcMatch*** to efficiently and effectively exploit the information coming from vertex and edge labels on matching vertices of a query graph to vertices of a target graph.
***ArcMatch*** is a C++ stand-alone tool with a command-line interface for managing directed or undirected graphs having labels on vertices and/or edges.
It is mainly developed for finding monomorphisms, but it can also search for induced subgraphs and test for graph isomorphism.

Please read the scientific paper for further details about the methodology of ***ArcMatch***.

<hr />

## License
ArcMatch is distributed under the MIT license. This means that it is free for both academic and commercial use. Note however that some third-party components in ArcMatch require that you reference certain works in scientific publications.
You are free to link or use ArcMatch inside the source code of your own program. If do so, please reference (cite) ArcMatch and this website. We appreciate bug fixes and would be happy to collaborate for improvements. 

<hr />

## Requirements

***ArcMatch*** is developed in C++. Thus, an operating system with g++ compiler must be available to compile and use the software, gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1).
No dependencies out of the standard GNU C++ library are required.

<hr />

## Usage

Before using the ***ArcMatch*** command line interface of all the distributions, please compile them by typing  `bash compile.sh`.
The `compile.sh` script generates a series of executables that were used for assessing the performance of ***ArcMatch***,
plus alternative versions.

Executables named `arcmatch-1` to `arcmatch-16` are the alternative configurations of all the techniques embedded in ***ArcMatch***. Please, read the appendix of the scientific paper for a better understanding of such techniques.

***ArcMatch*** comes with two main versions, `arcmatch` and `arcmatch-lt`. 
The first version is intended for very general purposes. It runs an expensive domain reduction technique that can be useful for many of the subgraph isomorphism instances.
On the contrary, `arcmatch-lt` avoids expensive reduction techniques that may be unfeasible in specific cases, such as searching a limited number of matches.

By default, the software just returns a count of the number of matches between the query and the target graphs.
Print of the found matches is enabled for the executables suffixed by `-print`.
Please, consider that printing is an expensive task which does not directly correlate with methodology performance, thus it should be avoided in your goal if to compare ***ArcMatch*** with other tools.
Details regarding the output forms of the matches are given in what follows. 

Lastly, specific versions of `arcmatch` and `arcmatch-lt` are provided for stopping at the first found match or after 100k matches.

### Command line interface

All versions of ***ArcMatch*** take as input the same parameters:
```
 ./arcmatch* ISO_TYPE INPUT_FORMAT target_graph pattern_graph    
```
|ISO_TYPE|specify isomorphism|
|---------|-------------------|
|iso|bijective graphs isomorphism.|
|ind|induced subgraph isomorphism|
|mono|monomorphism, the classical subgraph matching|

|INPUT_FORMAT	| specify input file format|
|----------------|-------------------------|
|gfd	|directed graphs with attributes only on nodes.|
|gfu	|undirected graphs with attributes only on nodes.|
|ged	|directed graphs with attributes both on nodes and edges.|
|geu	|undirected graphs with attributes both on nodes and edges.|

### Input graph file format

***ArcMatch*** is able to read two graph file formats, gfu and gfd, for undirected and directed graphs, respectively, with attributes on nodes and/or edges.

Reference graphs are stored in text files containing one or more graphs.
query graphs are stored in text files containing one graph per file.

The current input format allows the description of graphs with labels on nodes.
> #[graph_name] <br>
[number of nodes] <br>
[label_of_first_node] <br>
[label_of_second_node] <br>
... <br>
[number of edges] <br>
[node id] [node id] <br>
[node id] [node id] <br>
... <br>

Node ids are assigned following the order in which they are written in the input file, starting from 0.
***[graph_name] and labels can not contain blank characters (spaces).
Labels are case sensitive.***

An example of an input file is the following:

> #my_graph <br>
4 <br>
A <br>
B <br>
C <br>
Br <br>
5 <br>
0 1 <br>
2 1 <br>
2 3 <br>
0 3 <br>
0 2 <br>


Indeed, an example of an input file in geu format (undirected graph with labels both on nodes and attributes) is:
> #my_graph <br>
4 <br>
A <br>
B <br>
C <br>
Br <br>
5 <br>
0 1 a <br>
2 1 n<br>
2 3 m<br>
0 3 k<br>
0 2 a<br>

### Output match format
When the print of matches is enabled, a basic behaviour of the tool is to print one match per line.
The line is prefixed by `M:`.
The match is embedded between curly brackets.
For each query vertex, the corresponding matched target vertex is reported by a pair in the form `(query_vertex, target_veterx)`.

An example with a 3-vertices query in which the query vertex 0 is mapped to the target vertex 120 is the following one:
> M:{(0,120),(2,130),(1,140)}

The order between the query vertices is not ensured.

However, many of the executables use a particular technique that is able to find multiple matches at once.
For this reason, a query vertex can be mapped to a list of target vertices on the same line.
the list of target vertices is reported by using squared brackets.

An example in which, the query vertex 1 is mapped to three target vertices is the following one:
An example with a 3-vertices query in which the query vertex 0 is mapped to the target vertex 120 is the following one:
> M:{(0,120),(2,130),(1,[140,144,142])}

The order within the list of target vertices is not ensured.


<hr />


## Datasets

The data sets used for evaluating the performance of ArcMatch are available at the following [repository](https://github.com/vbonnici/ArcMatch-datasets).

Among these, a PPI data set that was built on top of a biochemical data set originally used to evaluate the performance of RI and RI-DS, which is available at this [repository](https://github.com/GiugnoLab/RI-Datasets).

A further synthetic dataset, used for assessing the scalability of ArcMatch, is available at the following [repository](https://github.com/GiugnoLab/RI-synthds).

<hr />


## Running from Docker

Here we give instructions on how to run the software on a Docker container with the same specifications of the requirements.

**WE HARDLY DISCOURAGE TO RUN PERFORMANCE TESTS IN THIS MODALITY**, because within a container an executable is slower than running it in a non-dockerized environment.


To build the container, locate in this directory (where the dockerfile file is located) and digit :
```
docker build -t arcmatch .   
```
The build will compile all the versions that this repository makes available.
<br>
To run arcmatch on the examples files digit:
```
 docker run arcmatch /arcmatch/arcmatch mono gfd  /arcmatch/file_format_examples/reference.gfd /arcmatch/file_format_examples/query.gfd
```
Remember that to run the executable on local files you must copy or mount them into the counter first.
For example, if your files are located in a local directory `/myhome/mydirectory`, you can use the option `-v` to link the directory into the container.
```
 docker run -v /myhome/mydirectory:/myfiles/ arcmatch /arcmatch/arcmatch mono gfd  /myfiles/reference.gfd /myfiles/query.gfd
```

<hr />


## Citation
Please, cite the ***ArcMatch*** paper 
 
     Bonnici, V., Giugno, R., Pulvirenti, A., Shasha, D., & Ferro, A. (2013).
     A subgraph isomorphism algorithm and its application to biochemical data. 
     BMC bioinformatics, 14(7), S13.

     Bonnici V, Giugno R. 
     On the variable ordering in subgraph isomorphism algorithms. 
     IEEE/ACM transactions on computational biology and bioinformatics. 2016 Jan 7;14(1):193-203.
<hr />
