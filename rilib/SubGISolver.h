/*
 * SubGISolver.h
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

#ifndef SUBGISOLVER_H_
#define SUBGISOLVER_H_

#include "Solver.h"

namespace rilib{

class SubGISolver : public Solver{
public:

	SubGISolver(
				MatchingMachine& _mama,
				Graph& _rgraph,
				Graph& _qgraph,
				AttributeComparator& _nodeComparator,
				AttributeComparator& _edgeComparator,
				MatchListener& _matchListener,
				sbitset *_domains,
				int *_domains_size,
				EdgeDomains& _edomains
				) : Solver(_mama, _rgraph, _qgraph, _nodeComparator, _edgeComparator, _matchListener, _domains, _domains_size, _edomains){

	}



	virtual bool edgesCheck(int si, int ci, int* solution, bool* matched){
		return true;
	}
};

}


#endif /* SUBGISOLVER_H_ */
