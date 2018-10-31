#ifndef CLAUSE_H
#define CLAUSE_H

#define c_CONFLICT -1
#define c_NULL 0
#define c_NORMAL 1
#define UNASSIGNED -1
#define UNAVAILABLE 0
#define ASSIGNED 1

#include <vector>
#include <utility>
#include <array>
#include <algorithm>
#include <set>
#include <cmath>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <iterator>
#include <unordered_map>

class Clause{
private:
	std :: vector<int16_t> literals; 
	std :: vector<int16_t> variables;
	bool stack_status;
public:
	Clause();
	Clause(std :: vector<int> & cls);
	Clause(const Clause & clause);  // deep copy function
	// ~Clause();
	void setClause(std :: vector<int> & cls);
	const std :: vector<int16_t> & getClause() const;
	const std :: vector<int16_t> & getVariables() const;
	int getLength() const;
	bool inStack();
	void enStack();
	void deStack();

	bool operator==(const Clause & clause) const;
};

// a non-const reference cannot be returned by a const member function. a const member function indicates that this function cannot modify the object itself. However, returning a non-const reference grants the receiver the access to modify the original object. While returning a object (not a reference) from a const member function is ok, since it produces duplicate when returning.

#endif