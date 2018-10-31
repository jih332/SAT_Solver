#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <utility>

class Vertex{

private:
	int16_t depth;
	int16_t decision_level;
	// std :: vector<Vertex *> parents;
	// cherish your life and do not use pointer.
	// vector.push_back() will re-allocate memory sometimes. It changes the address of a vertex from 0x2e6393c to 0x2e64c4c. So, just overload operator== and do not compare objects with their addresses.
	std :: vector<int16_t> parents_index;
	int16_t var;
	int8_t val;

public:
	Vertex();
	Vertex(int dp, int dl, std :: vector<int> pa_index, int var, int val);
	Vertex(const Vertex & vtx);
	int getDepth() const;
	int getDecisionLevel() const;
	const std :: vector<int16_t> & getParents() const;
	std :: pair<int, int> getVarVal() const;
	bool operator==(const Vertex & vtx) const;
};




#endif