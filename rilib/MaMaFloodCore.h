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

#ifndef MAMAFLOODCORE_H_
#define MAMAFLOODCORE_H_

#include <string.h>

#include <algorithm>
#include <vector>

#include "Domains.h"
#include "Graph.h"
#include "MatchingMachine.h"
#include "sbitset.h"

#include <bits/stdc++.h>
#include <tuple>

namespace rilib {

class MaMaFloodCore : public MatchingMachine {

    sbitset *node_domains;
    int *node_domains_size;
    EdgeDomains &edge_domains;
    int max_depth;

  public:
    MaMaFloodCore(Graph &query, sbitset *_node_domains, int *_node_domains_size, EdgeDomains &_edomains, int _max_depth) : MatchingMachine(query), node_domains(_node_domains), node_domains_size(_node_domains_size), edge_domains(_edomains), max_depth(_max_depth) {}

    enum NodeFlag { NS_CORE, NS_CNEIGH, NS_UNV };

    void flood_centrality(Graph &query, int nfs, int inode, double *centrality, double *ccentrality, int *depth, double **o_query_e_weights, double **i_query_e_weights, NodeFlag *node_flags, int max_depth) {

        int *queue = new int[nfs];
        int cn, n;
        int q, ql = 0, qr = 0, nqr = 0;
        node_flags[inode] = NS_CORE;

        for (int cn = 0; cn < nfs; cn++) {
            centrality[cn] = 0.0;
            ccentrality[cn] = 0.0;
            depth[cn] = nfs;
        }

        cn = inode;
        depth[cn] = 0;
        for (int i = 0; i < query.out_adj_sizes[cn]; i++) {
            n = query.out_adj_list[cn][i];
            if (node_flags[n] != NS_CORE) {
                if (depth[n] == nfs) {
                    depth[n] = 1;
                    queue[qr] = n;
                    qr++;
                }
            } else {
                depth[n] = 0;
                centrality[cn] += 1.0;
            }
        }
        for (int i = 0; i < query.in_adj_sizes[cn]; i++) {
            n = query.in_adj_list[cn][i];
            if (node_flags[n] != NS_CORE) {
                if (depth[n] == nfs) {
                    depth[n] = 1;
                    queue[qr] = n;
                    qr++;
                }
            } else {
                depth[n] = 0;
                centrality[cn] += 1.0;
            }
        }

        for (cn = 0; cn < nfs; cn++) {
            if (node_flags[cn] == NS_CORE) {

                for (int i = 0; i < query.out_adj_sizes[cn]; i++) {
                    n = query.out_adj_list[cn][i];
                    if (depth[n] == 1) {
                        centrality[n] += o_query_e_weights[cn][i];
                        ccentrality[n] += centrality[inode] * o_query_e_weights[cn][i];
                    }
                }

                for (int i = 0; i < query.in_adj_sizes[cn]; i++) {
                    n = query.in_adj_list[cn][i];
                    if (depth[n] == 1) {
                        centrality[n] += i_query_e_weights[cn][i];
                        ccentrality[n] += centrality[inode] * i_query_e_weights[cn][i];
                    }
                }
            }
        }

        int cdepth = 1;
        while ((ql < qr) && (cdepth <= max_depth)) {
            nqr = qr;
            for (q = ql; q < nqr; q++) {
                cn = queue[q];

                for (int i = 0; i < query.out_adj_sizes[cn]; i++) {
                    n = query.out_adj_list[cn][i];
                    if (depth[n] == cdepth) {
                        centrality[n] += centrality[cn] * o_query_e_weights[cn][i];
                    } else if (depth[n] > cdepth) {
                        centrality[n] += centrality[cn] * o_query_e_weights[cn][i];
                        depth[n] = cdepth + 1;
                        if (depth[n] == nfs) {
                            queue[qr] = n;
                            qr++;
                        }
                    } else if (node_flags[n] == NS_CORE) {
#ifdef MDEBUG
                        std::cout << "@" << depth[cn] << " " << centrality[cn] << " " << o_query_e_weights[cn][i] << "\n";
#endif
                        ccentrality[cn] += centrality[cn] * o_query_e_weights[cn][i];
                    }
                }

                for (int i = 0; i < query.in_adj_sizes[cn]; i++) {
                    n = query.in_adj_list[cn][i];
                    if (depth[n] == cdepth) {
                        centrality[n] += centrality[cn] * i_query_e_weights[cn][i];
                    } else if (depth[n] > cdepth) {
                        centrality[n] += centrality[cn] * i_query_e_weights[cn][i];
                        depth[n] = cdepth + 1;
                        if (depth[n] == nfs) {
                            queue[qr] = n;
                            qr++;
                        }
                    } else if (node_flags[n] == NS_CORE) {
#ifdef MDEBUG
                        std::cout << "@" << depth[cn] << " " << centrality[cn] << " " << i_query_e_weights[cn][i] << "\n";
#endif
                        ccentrality[cn] += centrality[cn] * i_query_e_weights[cn][i];
                    }
                }
            }

            cdepth++;
            ql += qr;
        }

        node_flags[inode] = NS_CNEIGH;
    }

    void update_score(int nfs, int inode, Graph &query, double **scores, double *centrality, double *ccentrality, int *depth) {
        for (int i = 0; i < 5; i++) {
            scores[inode][i] = 0.0;
        }

        for (int i = 0; i < nfs; i++) {
            if (i != inode) {
                if (depth[i] == 1) {
                    scores[inode][1] += ccentrality[i];
                    scores[inode][3] += centrality[i];
                } else if (depth[i] == 2) {
                    scores[inode][2] += ccentrality[i];
                    scores[inode][4] += centrality[i];
                }
            } else {
                scores[inode][0] += centrality[i];
            }
        }
    }

    double wcompare(double **scores, int ni, int nj) {
        double ret = 0.0;
        for (int i = 0; i < 5; i++) {
            if (scores[ni][i] != scores[nj][i]) {
                return (scores[ni][i] - scores[nj][i]);
            }
        }
        return ret;
    }

    virtual void build(Graph &query) {
        const int nfs = query.nof_nodes;

        double **o_query_e_weights = new double *[nfs];
        for (int i = 0; i < query.nof_nodes; i++) {
            o_query_e_weights[i] = new double[query.out_adj_sizes[i]];
            for (int j = 0; j < query.out_adj_sizes[i]; j++) {

                o_query_e_weights[i][j] = 1.0 - (((double)edge_domains.domains[edge_domains.pattern_out_adj_eids[i][j]].size()) / ((double)edge_domains.nof_target_edges));
            }
        }

        double **i_query_e_weights = new double *[nfs];
        for (int i = 0; i < query.nof_nodes; i++) {
            i_query_e_weights[i] = new double[query.in_adj_sizes[i]];
            for (int j = 0; j < query.in_adj_sizes[i]; j++) {

                i_query_e_weights[i][j] = 1.0 - (((double)edge_domains.domains[edge_domains.pattern_in_adj_eids[i][j]].size()) / ((double)edge_domains.nof_target_edges));
            }
        }

        int *ordering = new int[nfs];

        NodeFlag *node_flags = new NodeFlag[nfs];
        for (int i = 0; i < nfs; i++) {
            node_flags[i] = NS_UNV;
        }

        double **scores = new double *[nfs];
        for (int i = 0; i < nfs; i++) {
            scores[i] = new double[5];
            for (int j = 0; j < 5; j++) {
                scores[i][j] = 0.0;
            }
        }

        double *centrality = new double[nfs];
        double *ccentrality = new double[nfs];
        int *depth = new int[nfs];

        int si = 0, n, nn;
        for (int i = 0; i < nfs; i++) {
            if (node_domains_size[i] == 1) {
#ifdef MDEBUG
                std::cout << "SINGLETON " << i << " " << si << "\n";
#endif
                ordering[si] = i;
                node_flags[i] = NS_CORE;
                flood_centrality(query, nfs, i, centrality, ccentrality, depth, o_query_e_weights, i_query_e_weights, node_flags, 3);
                update_score(nfs, i, query, scores, centrality, ccentrality, depth);

                node_flags[i] = NS_CORE;

                for (int j = 0; j < query.out_adj_sizes[i]; j++) {
                    n = query.out_adj_list[i][j];
                    if (node_flags[n] == NS_UNV) {
                        node_flags[n] = NS_CNEIGH;
                    }
                }
                for (int j = 0; j < query.in_adj_sizes[i]; j++) {
                    n = query.in_adj_list[i][j];
                    if (node_flags[n] == NS_UNV) {
                        node_flags[n] = NS_CNEIGH;
                    }
                }
                si++;
            }
        }

        if (si < nfs) {
#ifdef MDEBUG
            std::cout << "si " << si << "\n";
#endif

            NodeFlag oflag;
            for (int i = 0; i < nfs; i++) {
                oflag = node_flags[i];
                flood_centrality(query, nfs, i, centrality, ccentrality, depth, o_query_e_weights, i_query_e_weights, node_flags, 3);
                node_flags[i] = oflag;
                update_score(nfs, i, query, scores, centrality, ccentrality, depth);
            }

#ifdef MDEBUG
            for (int cn = 0; cn < nfs; cn++) {
                std::cout << "FIRST " << cn << " ";
                for (int k = 0; k < 5; k++) {
                    std::cout << scores[cn][k] << " ";
                }
                std::cout << "\n";
            }
#endif

            int max_score_node = -1;
            for (int cn = 0; cn < nfs; cn++) {
                if (node_flags[cn] != NS_CORE) {
                    if (max_score_node == -1) {
                        max_score_node = cn;
                    } else {
                        if (wcompare(scores, cn, max_score_node) > 0.0) {
                            max_score_node = cn;
                        }
                    }
                }
            }

#ifdef MDEBUG
            std::cout << "CHOOSEN " << max_score_node << "\n";
#endif
            ordering[si] = max_score_node;
            node_flags[max_score_node] = NS_CORE;
            for (int i = 0; i < query.out_adj_sizes[max_score_node]; i++) {
                n = query.out_adj_list[max_score_node][i];
                if (node_flags[n] == NS_UNV) {
                    node_flags[n] = NS_CNEIGH;
                }
            }
            for (int i = 0; i < query.in_adj_sizes[max_score_node]; i++) {
                n = query.in_adj_list[max_score_node][i];
                if (node_flags[n] == NS_UNV) {
                    node_flags[n] = NS_CNEIGH;
                }
            }
            si++;

            while (si < nfs) {
#ifdef MDEBUG
                std::cout << "\n========================================\n";
                std::cout << "si " << si << "\n";
                std::cout << "CORES: ";
                for (int cn = 0; cn < nfs; cn++) {
                    if (node_flags[cn] == NS_CORE) {
                        std::cout << cn << " ";
                    }
                }
                std::cout << "\n";
#endif

                for (int cn = 0; cn < nfs; cn++) {
                    if (node_flags[cn] == NS_CNEIGH) {
                        flood_centrality(query, nfs, cn, centrality, ccentrality, depth, o_query_e_weights, i_query_e_weights, node_flags, 3);
                        update_score(nfs, cn, query, scores, centrality, ccentrality, depth);

#ifdef MDEBUG
                        std::cout << "CS_NEIGH " << cn << " ";
                        for (int k = 0; k < 5; k++) {
                            std::cout << scores[cn][k] << " ";
                        }
                        std::cout << "\n";
#endif
                    }
                }

                max_score_node = -1;
                for (int cn = 0; cn < nfs; cn++) {
                    if (node_flags[cn] != NS_CORE) {
                        if (max_score_node == -1) {
                            max_score_node = cn;
                        } else if (wcompare(scores, cn, max_score_node) > 0.0) {
                            max_score_node = cn;
                        }
                    }
                }

#ifdef MDEBUG
                std::cout << "CHOOSEN " << max_score_node << "\n";
#endif

                ordering[si] = max_score_node;
                node_flags[max_score_node] = NS_CORE;

                for (int i = 0; i < query.out_adj_sizes[max_score_node]; i++) {
                    n = query.out_adj_list[max_score_node][i];
                    if (node_flags[n] == NS_UNV) {
                        node_flags[n] = NS_CNEIGH;
                    }
                }
                for (int i = 0; i < query.in_adj_sizes[max_score_node]; i++) {
                    n = query.in_adj_list[max_score_node][i];
                    if (node_flags[n] == NS_UNV) {
                        node_flags[n] = NS_CNEIGH;
                    }
                }

                si++;
            }
        }

        for (int i = 0; i < nfs; i++) {
            map_node_to_state[ordering[i]] = i;
            map_state_to_node[i] = ordering[i];
            parent_type[i] = PARENTTYPE_NULL;
            parent_state[i] = -1;
        }

        parent_type[0] = PARENTTYPE_NULL;
        edges_sizes[0] = o_edges_sizes[0] = i_edges_sizes[0] = 0;
        edges[0] = new MaMaEdge[0];

#ifdef MDEBUG
        std::cout << "ORDERING\n";
        for (int i = 0; i < nfs; i++) {
            std::cout << i << " " << ordering[i] << " " << map_state_to_node[i] << " " << map_node_to_state[ordering[i]] << "\n";
        }
#endif

        int nof_sn = nfs;
        int e_count, o_e_count, i_e_count;
        for (int si = 1; si < nof_sn; si++) {

            n = map_state_to_node[si];

            for (int j = 0; j < query.out_adj_sizes[n]; j++) {
                nn = query.out_adj_list[n][j];
                if (map_node_to_state[nn] < si) {
                    if (parent_state[si] == -1) {
#ifdef MDEBUG
                        std::cout << "IN:" << n << " <- " << nn << "\n";
#endif
                        parent_state[si] = map_node_to_state[nn];
                        parent_type[si] = PARENTTYPE_IN;
                        break;
                    }
                }
            }
            if (parent_state[si] == -1) {
                for (int j = 0; j < query.in_adj_sizes[n]; j++) {
                    nn = query.in_adj_list[n][j];
                    if (map_node_to_state[nn] < si) {
                        if (parent_state[si] == -1) {
#ifdef MDEBUG
                            std::cout << "OUT:" << n << " <- " << nn << "\n";
#endif
                            parent_state[si] = map_node_to_state[nn];
                            parent_type[si] = PARENTTYPE_OUT;
                            break;
                        }
                    }
                }
            }
#ifdef MDEBUG
            if (parent_type[si] != PARENTTYPE_NULL) {
                std::cout << "P:" << si << " " << n << " " << parent_state[si] << " " << map_state_to_node[parent_state[si]];
                if (parent_type[si] == PARENTTYPE_OUT)
                    std::cout << " out\n";
                else
                    std::cout << " in\n";
            } else {
                std::cout << "P:" << si << " " << n << " " << parent_state[si] << " NULL\n";
                ;
            }
#endif
            e_count = 0;
            o_e_count = 0;
            std::vector<std::tuple<double, int, int>> crdeges;
            for (int i = 0; i < query.out_adj_sizes[n]; i++) {
                if (map_node_to_state[query.out_adj_list[n][i]] < si) {
                    e_count++;
                    o_e_count++;
                    crdeges.push_back(std::tuple<double, int, int>(o_query_e_weights[n][i], query.out_adj_list[n][i], 0));
                }
            }
            i_e_count = 0;

            for (int i = 0; i < query.in_adj_sizes[n]; i++) {
                if (map_node_to_state[query.in_adj_list[n][i]] < si) {
                    e_count++;
                    i_e_count++;

                    crdeges.push_back(std::tuple<double, int, int>(i_query_e_weights[n][i], query.in_adj_list[n][i], 1));
                }
            }

            edges_sizes[si] = e_count;
            o_edges_sizes[si] = o_e_count;
            i_edges_sizes[si] = i_e_count;

            edges[si] = new MaMaEdge[e_count];

            if (e_count > 0) {
                std::sort(crdeges.begin(), crdeges.end());
                std::reverse(crdeges.begin(), crdeges.end());

                e_count = 0;

                for (std::vector<std::tuple<double, int, int>>::iterator it = crdeges.begin(); it != crdeges.end(); it++) {
                    if (std::get<2>(*it) == 0) {
                        edges[si][e_count].source = map_node_to_state[n];
                        edges[si][e_count].target = map_node_to_state[std::get<1>(*it)];

#ifdef MDEBUG
                        std::cout << n << " " << std::get<1>(*it) << "\n";
                        std::cout << edges[si][e_count].source << " " << edges[si][e_count].target << "-----++++++<\n";
#endif
                        e_count++;
                    } else {
                        edges[si][e_count].target = map_node_to_state[n];
                        edges[si][e_count].source = map_node_to_state[std::get<1>(*it)];

#ifdef MDEBUG
                        std::cout << n << " " << std::get<1>(*it) << "\n";
                        std::cout << edges[si][e_count].source << " " << edges[si][e_count].target << "-----++++++<\n";
#endif
                        e_count++;
                    }
                }
            }
        }
    }
};

} // namespace rilib
#endif // MAMAFLOODCORE_H_
