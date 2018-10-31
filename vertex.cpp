#include <iostream>
#include "vertex.h"

using namespace std;

Vertex :: Vertex(){

	depth = -1;
	decision_level = -1;
	var = -1;
	val = -1;
}

Vertex :: Vertex(int dp, int dl, std :: vector<int> pa_index, int var, int val){

	// clog << "valid vertex created: var:" << var << " val:" << val << " dp:" << dp << " dl:" << dl << endl;
	// clog << "parents:" << endl;
	depth = dp;
	decision_level = dl;
	for(int i = 0; i < pa_index.size(); i++){

		parents_index.push_back(pa_index[i]);
	}
	this -> var = var;
	this -> val = val;

	// for(int i = 0; i < parents.size(); i++){

	// 	clog << "var:" << parents[i] -> getVarVal().first << " val:" << parents[i] -> getVarVal().second << " dp:" << parents[i] -> getDepth() << " dl:" << parents[i] -> getDecisionLevel() << endl;
	// }
}

Vertex :: Vertex(const Vertex & vtx){

	depth = vtx.depth;
	decision_level = vtx.decision_level;
	for(int i = 0; i < vtx.parents_index.size(); i++){

		parents_index.push_back(vtx.parents_index[i]);
	}
	var = vtx.var;
	val = vtx.val;
}

int Vertex :: getDepth() const{

	return depth;
}

int Vertex :: getDecisionLevel() const{

	return decision_level;
}

const vector<int16_t> & Vertex :: getParents() const{

	return parents_index;
}

pair<int, int> Vertex :: getVarVal() const{

	return make_pair(var, val);
}

bool Vertex :: operator==(const Vertex & vtx) const{

	if(var == vtx.var){

		return true;
	}
	else{

		return false;
	}
}