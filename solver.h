#ifndef SOLVER_H
#define SOLVER_H

#include "clause.h"
#include "vertex.h"

extern int8_t verbose;
extern int8_t interrupted;

class Solver{

private:
	int16_t n_var;
	int32_t n_cls;
	std :: vector<int8_t> assignment_table;
	std :: vector<int32_t> clause_stack;
	std :: vector<Clause> clauseList;
	std :: set<int16_t> var_list;
	int8_t status;
    std :: vector<Vertex> vertex_stack;
    int16_t highest_decision_level;
    std :: vector<std :: vector<int32_t>> adjacency_list;
    std :: pair<int32_t, int32_t> conflict_counter;
    float decay_c;
    std :: unordered_map<int32_t, std :: pair<int8_t, float>> literal_scores;
    // make this score array a hash map (faster to insert and lookup)
    std :: vector<std :: pair<float, int32_t>> sorted_lits_scores;
    // matintain a vector to be the copy of the map, sorted in descending order

    std :: unordered_map<std :: string, std :: pair<int, time_t>> func_clock;

public:
	Solver();
	Solver(int n_var, int n_cls, float decay_c, std :: vector<Clause> & cllist);
	bool pure();
	void singleton();
	void preprocessing();
	Clause unitPropogate(int times);
	void appendClause(int var, int times);
	std :: pair<Clause, int> pickClause(int times);
	bool moveWatch(Clause & c, int & w_moved, const int & w_fixed);
	int getStatus() const;
	bool allAssigned(int times);
	std :: pair<int, int> makeDecision();
    int findVertex(int var);
    void addVertex(int var, int val, Clause * cls = NULL);
    std :: pair<int, Clause> learn(const Clause & cls);
    void trackReason(std :: vector<int> & Q);
    bool UIP(const std :: vector<int> & Q) const;
    void backtrack(int dl);
    void addLearnedClause(const Clause & cls);
    void printAssignments();
    void printDecisions();
    void buildAdjlist();


    std :: unordered_map<std :: string, std :: pair<int, time_t>> getFuncClock();

    // void printClausestack();
};

#endif