/*
 * MatchListener.h
 *
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

#ifndef MATCHLISTENER_H_
#define MATCHLISTENER_H_

// #include "Graph.h"

namespace rilib {

class MatchListener {
  public:
    long matchcount;
    MatchListener() { matchcount = 0; }
    virtual ~MatchListener(){};
    virtual void match(int n, int *qIDs, int *rIDs) = 0;
    virtual void match_multiple(int n, int *qIDs, int *rIDs, int si, std::set<int> *leaf_domains) = 0;
};

class EmptyMatchListener : public MatchListener {
  public:
    EmptyMatchListener() : MatchListener() {}
    virtual void match(int n, int *qIDs, int *rIDs) { matchcount++; };

    virtual void match_multiple(int n, int *qIDs, int *rIDs, int si, std::set<int> *leaf_domains){
        // TODO
    };
};

class ConsoleMatchListener : public MatchListener {
  public:
    ConsoleMatchListener() : MatchListener() {}
    virtual void match(int n, int *qIDs, int *rIDs) {
        matchcount++;
        std::cout << "M:{";
        for (int i = 0; i < n; i++) {
            std::cout << "(" << qIDs[i] << "," << rIDs[i] << ")";
        }
        std::cout << "}\n";
    }
    virtual void match_multiple(int n, int *qIDs, int *rIDs, int si, std::set<int> *leaf_domains) {
        matchcount++;
        std::cout << "M:{";
        for (int i = 0; i < si; i++) {
            std::cout << "(" << qIDs[i] << "," << rIDs[i] << ")";
        }
        for (int i = si; i < n; i++) {
            std::cout << "(" << qIDs[i] << ",[";
            int nof = leaf_domains[i - si].size();
            int ci = 0;
            for (auto it = leaf_domains[i - si].begin(); it != leaf_domains[i - si].end(); it++) {
                std::cout << (*it);
                if (ci < nof - 1)
                    std::cout << ",";
                ci++;
            }
            std::cout << "])";
        }
        std::cout << "}\n";
    };
};

} // namespace rilib

#endif /* MATCHLISTENER_H_ */
