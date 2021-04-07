import sys

itarget = sys.argv[1]
iquery = sys.argv[2]
imatch = sys.argv[3]
idirected = bool(sys.argv[4])


def read_gf(ifile, directed):
    nodes = list()
    edges = set()

    nof_e = 0
    ce = 0
    state = 0
    for line in open(ifile, 'r'):
        if state == 0:
            state = 1
        elif state == 1:
            nof_e = int(line)
            ce = 0
            state = 2
        elif state == 2:
            if ce == nof_e:
                state = 3
            else:
                nodes.append(line.strip())
                ce += 1
        else:
            cc = line.strip().split(' ')
            edges.add( (int(cc[0]),int(cc[1]),cc[2]) )
    return nodes,edges


t_nodes, t_edges = read_gf(itarget, idirected)
print(len(t_nodes), len(t_edges))

q_nodes, q_edges = read_gf(iquery, idirected)
print(len(t_nodes), len(t_edges))
print(q_nodes)
print(q_edges)


qmap = dict()
imatch = imatch[2:-2].split(')(')
for c in imatch:
    cc = c.split(',')
    qmap[ int(cc[0]) ] = int(cc[1])
print(qmap)

mapped = set(qmap.values())
print('matchable edges:')
for t in t_edges:
    if (t[0] in mapped) and (t[1] in mapped):
        print(t)

print('checkng node labels...')
for q,t in sorted(qmap.items()):
    if q_nodes[q] != t_nodes[t]:
        print('mismatching node label', q,t, q_nodes[q],t_nodes[t])
print('checing edges...')
for q in q_edges:
    t = ( qmap[q[0]], qmap[q[1]], q[2])
    if t not in t_edges:
        print('unmatched edge', q, t)