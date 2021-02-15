#!/usr/bin/python3

import sys
import networkx as nx
import matplotlib.pyplot as plt

ifile = sys.argv[1]


g = nx.DiGraph()
node_labels = dict()


state = 0
nofe = 0
ce = 0
nid = 0
for line in open(ifile,'r'):
	if state == 0:
		state = 1
	elif state == 1:
		nofe = int(line)
		ce = 0
		state = 2
	elif state == 2:
		ce += 1
		if ce > nofe:
			state = 3 
			nofe = int(line)
			ce = 0
		else:
			node_labels[nid] = str(nid)+'-'+line.strip()
			nid += 1
	else:
		print(line)
		cc = line.strip().split(' ')
		g.add_edge(int(cc[0]),int(cc[1]))
		

g = nx.relabel_nodes(g,node_labels)
nx.draw(g, with_labels = True)
plt.show()



