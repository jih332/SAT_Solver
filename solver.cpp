#include <iostream>
#include "solver.h"
#include <typeinfo>

using namespace std;

int8_t verbose;
int8_t interrupted;


Solver :: Solver(){

	status = c_CONFLICT;
}



Solver :: Solver(int n_var, int n_cls, float decay_c, std :: vector<Clause> & cllist){

	status = c_NORMAL;
	this -> n_var = n_var;
	this -> n_cls = n_cls;
	this -> decay_c = decay_c;

	for(int i = 0; i < cllist.size(); i++){

		clauseList.push_back(cllist[i]);
	}
	highest_decision_level = 0;
	conflict_counter = {0, 0};

	// cout << "create: length of literal_scores:" << literal_scores.size() << endl;
	// cout << "first, second:" << literal_scores[0].first << " " << literal_scores[0].second << endl;
}



bool Solver :: pure(){


	vector<std :: array<int, 2>> rec(n_var, {0, 0});
	vector<int> pureLiterals;

	for(int i = 0; i < n_cls; i++){

		for(int j = 0; j < clauseList[i].getLength(); j++){

			if(clauseList[i].getClause()[j] < 0){

				rec[clauseList[i].getVariables()[j] - 1][1] = 1;
			}
			else{

				rec[clauseList[i].getVariables()[j] - 1][0] = 1;
			}
		}
	}

	for(int i = 0; i < n_var; i++){
	
		if(rec[i][0] == 1 && rec[i][1] == 0){

			pureLiterals.push_back(i + 1);
			continue;
		}
		else if(rec[i][0] == 0 && rec[i][1] == 1){

			pureLiterals.push_back(-1 * (i + 1));
			continue;
		}
	}

	if(pureLiterals.empty() || n_cls <= 0){

		return false;
	}

	for(int i = n_cls - 1; i >= 0; i--){

		for(int j = 0; j < pureLiterals.size(); j++){

			if(find(clauseList[i].getClause().begin(), clauseList[i].getClause().end(), pureLiterals[j]) != clauseList[i].getClause().end()){

				clauseList.erase(clauseList.begin() + i);
				n_cls -= 1;
				break;
			}
		}
	}

	return pure();
}



void Solver :: singleton(){


	for(int i = 0; i < n_cls; i++){

		if(clauseList[i].getLength() == 1){

				clause_stack.push_back(i);
				clauseList[i].enStack();
		}
	}
}



void Solver :: preprocessing(){

	pure();

	singleton();
}


Clause Solver :: unitPropogate(int times){

	// cout << "unitPropogate:" << endl;

	// cout << "clause_stack:" << endl;
	// printClausestack();

	if(times == 2){

		buildAdjlist();
	}

	while(true){

		pair<Clause, int> c_ind = pickClause(times);

		if(status == c_NULL){

			return c_ind.first;
		}
		else if(status == c_CONFLICT){

			return c_ind.first;
		}
		else{

			addVertex(c_ind.first.getVariables()[c_ind.second], !signbit(c_ind.first.getClause()[c_ind.second]), &c_ind.first);
			appendClause(-1 * c_ind.first.getClause()[c_ind.second], times);
		}
	}
}



void Solver :: appendClause(int lit, int times){

	time_t t_begin = clock();

	if(times == 1){

		// times == 1, the adjacency list has not been established.

		// func_clock[__func__].first = 0;
		// func_clock[__func__].second = 0;

		for(int i = 0; i < n_cls; i++){

			if(find(clauseList[i].getClause().begin(), clauseList[i].getClause().end(), lit) != clauseList[i].getClause().end()){

				if(!clauseList[i].inStack()){

					clause_stack.push_back(i);
					clauseList[i].enStack();
				}
			}
		}
	}
	else{

		// TODO: append clauses using adjacency list.
		// Done.
		int index = lit + n_var - (lit > 0);

		for(int i = 0; i < adjacency_list[index].size(); ++i){

			// TODO: determine whether the appending clause already exist in clause stack.
			// can we use vec of int as our clause stack?
			// Done.
			if(!clauseList[adjacency_list[index][i]].inStack()){

				clause_stack.push_back(adjacency_list[index][i]);
				clauseList[adjacency_list[index][i]].enStack();
			}
		}
	}

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;
}



pair<Clause, int> Solver :: pickClause(int times){

	int w1, w2;
	int w1_val, w2_val;


	assignment_table.assign(n_var, -1);

	for(int i = 0; i < vertex_stack.size(); i++){

		assignment_table[vertex_stack[i].getVarVal().first - 1] = vertex_stack[i].getVarVal().second;
	}

	Clause c;

	// int dispo = 1;
	int w_reset = 1;

	while(!clause_stack.empty()){

		if(w_reset){

			w1 = 0;
			w2 = 1;
			w1_val = UNASSIGNED;
			w2_val = UNASSIGNED;
		}
		else{

			w_reset = 1;
		}

		int c_index = clause_stack.back();
		c = clauseList[c_index];
		clause_stack.pop_back();
		clauseList[c_index].deStack();

		// cout << "in pickClause:" << endl;
		// cout << "clause picked:";
		// for(int i = 0; i < c.getLength(); i++){

		// 	cout << c.getClause()[i] << " ";
		// }
		// cout << endl;


		// if the clause c is a singleton clause, we can return c directly, and append the relavent clause in UP.
		if(c.getLength() == 1){

			if(assignment_table[c.getVariables()[0] - 1] == UNASSIGNED){

				status = c_NORMAL;
				return make_pair(c, 0);
			}
			else{

				int single_val = (c.getClause()[0] < 0) + ((c.getClause()[0] > 0) - (c.getClause()[0] < 0)) * assignment_table[c.getVariables()[0] - 1];
				if(single_val == 0){

					status = c_CONFLICT;
					return make_pair(c, -1);
				}
				else if(single_val == 1){

					continue;
				}
			}
			
		}
		//but what if this singleton clause has already handled in another clause? just like [1], [-1, 2], [-2]
		// before return the singleton clause, we have to examine the value of its only variable. if it's 0, we return conflict; if it's 1, we treat it as [1, 0, ..., 0], i.e., just pop it without do anything; if it's unassigned, we return it to UP to apply the unitRule.

		// do we avoid repeatedly appending ? taking care of might can increase the speed?

		if(assignment_table[c.getVariables()[w1] - 1] != UNASSIGNED){

			w1_val = (c.getClause()[w1] < 0) + ((c.getClause()[w1] > 0) - (c.getClause()[w1] < 0)) * assignment_table[c.getVariables()[w1] - 1];
			// cout << "w1_val:" << w1_val << endl;
		}
		else{

			// cout << "w1 set to UNASSIGNED" << endl;
			w1_val = UNASSIGNED;
		}

		if(assignment_table[c.getVariables()[w2] - 1] != UNASSIGNED){

			w2_val = (c.getClause()[w2] < 0) + ((c.getClause()[w2] > 0) - (c.getClause()[w2] < 0)) * assignment_table[c.getVariables()[w2] - 1];
			// cout << "w2_val:" << w2_val << endl;
		}
		else{

			// cout << "w2 set to UNASSIGNED" << endl;
			w2_val = UNASSIGNED;
		}

		if(w1_val == 1 || w2_val == 1){

			if(times == 1){

				clauseList.erase(find(clauseList.begin(), clauseList.end(), c));
				n_cls -= 1;
			}
			continue;
		}
		else if(w1_val == UNASSIGNED){


			if (w2_val == UNASSIGNED){

				continue;
			}
			else{
				if(moveWatch(c, w2, w1)){

					continue;
				}
				else{

					status = c_NORMAL;
					return make_pair(c, w1);
				}
			}
		}
		else{


			if(moveWatch(c, w1, w2)){

				clause_stack.push_back(c_index);
				clauseList[c_index].enStack();
				w_reset = 0;
				continue;
			}
			else{

				if(w2_val == UNASSIGNED){

					status = c_NORMAL;
					return make_pair(c, w2);
				}
				else{

					for(int i = 0; i < clause_stack.size(); ++i){

						clauseList[clause_stack[i]].deStack();
					}
					clause_stack.clear();
					status = c_CONFLICT;
					return make_pair(c, -1);
				}
			}
		}
	}

	status = c_NULL;
	return make_pair(c, -1);
}



bool Solver :: moveWatch(Clause & c, int & w_moved, const int & w_fixed){


	for(int i = 0; i < c.getLength(); i++){

		if(i == w_moved || i == w_fixed){

			continue;
		}


		if(assignment_table[c.getVariables()[i] - 1] == UNASSIGNED){

			w_moved = i;
			return true;
		}
		else{

			int w_val = (c.getClause()[i] < 0) + ((c.getClause()[i] > 0) - (c.getClause()[i] < 0)) * assignment_table[c.getVariables()[i] - 1];
			if(w_val == 1){

				w_moved = i;
				return true;
			}
		}
	}
	return false;
}



int Solver :: getStatus() const{

	return status;
}



bool Solver :: allAssigned(int times){

	time_t t_begin = clock();

	if(times == 1){

		for(int i = 0; i < n_cls; i++){

			for(int j = 0; j < clauseList[i].getLength(); j++){

				var_list.insert(clauseList[i].getVariables()[j]);
			}
		}

		for(set<int16_t> :: iterator it = var_list.begin(); it != var_list.end(); ++it){

			// wait for test
			literal_scores[*it].first = -(literal_scores[*it].first == UNAVAILABLE);
			// literal_scores[-(*it)].first = -(literal_scores[-(*it)].first == UNAVAILABLE);
		}
	}

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;


	if(var_list.size() == vertex_stack.size()){

		return true;
	}
	else{

		return false;
	}
}



pair<int, int> Solver :: makeDecision(){

	time_t t_begin = clock();

	int var;
	// int val = 1;

	if(conflict_counter.first == 0){
		// no conflict encountered by far, all the scores are 0. can pick any unassigned var to make decision
		// cout << "no conflict encountered by far" << endl;
		for(unordered_map<int32_t, pair<int8_t, float>> :: iterator it = literal_scores.begin(); it != literal_scores.end(); ++it){

			if(it -> second.first == UNASSIGNED){

				var = it -> first;
				break;
			}
		}
	}
	else if(conflict_counter.first > conflict_counter.second){
		// cout << "at least one learn() happened, reconstruct heap" << endl;
		// at least one learn() happened, scores are changed. need to resort the score array using heap
		sorted_lits_scores.clear();
		for(unordered_map<int32_t, pair<int8_t, float>> :: iterator it = literal_scores.begin(); it != literal_scores.end(); ++it){

			if(it -> second.first == UNASSIGNED){

				sorted_lits_scores.push_back({it -> second.second, it -> first});
				// cout << "push_back: asign: " << int(it -> second.first) << " score: " << it -> second.second << " var: " << it -> first << endl;
			}
		}
		make_heap(sorted_lits_scores.begin(), sorted_lits_scores.end());
		var = sorted_lits_scores.front().second;
		// cout << "lit:" << lit << endl;
		pop_heap(sorted_lits_scores.begin(), sorted_lits_scores.end());
		sorted_lits_scores.pop_back();
	}
	else{
		// cout << "no learn() between these two decisison" << endl;
		// no learn() between these two decisison, use old sorted array.
		while(true){

			var = sorted_lits_scores.front().second;
			pop_heap(sorted_lits_scores.begin(), sorted_lits_scores.end());
			sorted_lits_scores.pop_back();
			if(literal_scores[var].first == UNASSIGNED){

				break;
			}
		}	
	}

	// float maxscore = -1.0;
	// int maxindex = 0;

	// cout << "literal_scores size:" << literal_scores.size() << endl;

	// for(int i = 0; i < literal_scores.size(); ++i){

	// 	if(literal_scores[i].first == UNASSIGNED){

	// 		if(literal_scores[i].second > maxscore){

	// 			maxscore = literal_scores[i].second;
	// 			maxindex = i + 1;
	// 		}
	// 	}
	// }

	// cout << "decision:" << maxindex << " " << 1 << " with score:" << maxscore << endl;

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;

	conflict_counter.second = conflict_counter.first;

	// cout << "return " << abs(lit) << " with value " << !signbit(lit) << " as decision." << endl;

	// cout << "now the decisions are:" << endl;
	// printDecisions();

	return make_pair(var, 0);


	// srand(clock());

	// int val = rand() % 2;

	// int var_d;

	// for (set<int16_t> :: iterator it = var_list.begin(); it != var_list.end(); ++it){

	// 	int flag = 0;
		
	// 	for(int i = 0; i < vertex_stack.size(); i++){

	// 		if(*it == vertex_stack[i].getVarVal().first){

	// 			flag = 1;
	// 			break;
	// 		}
	// 	}
	// 	if(flag == 0){

	// 		var_d = *it;
	// 		break;
	// 	}
	// }

	// return make_pair(var_d, val);
}



int Solver :: findVertex(int var){
	// vtx_p is a pointer of a pointer, which is the address of a pointer. use *vtx_p will issue the value pointed by this address, i.e., the inner pointer (the address of a vertex).

	// TODO: change this function to return the index of found vertex. vtx_p is int now.
	// Done.


	for(int i = 0; i < vertex_stack.size(); i++){

		if(vertex_stack[i].getVarVal().first == var){

			return i;
		}
	}
	return -1;
}



void Solver :: addVertex(int var, int val, Clause * cls){

	// cout << "add vertex:" << var << endl;

	time_t t_begin = clock();


	literal_scores[var].first = ASSIGNED;


	vector<int> pa_index;
	int lgst_dp = 0;
	int dp = 1;

	if(cls == NULL){

		highest_decision_level += 1;
	}
	else{

		for(int i = 0; i < cls -> getLength(); i++){

			if(cls -> getVariables()[i] != var){

				int vtx_p;
				vtx_p = findVertex(cls -> getVariables()[i]);
				pa_index.push_back(vtx_p);
				if(lgst_dp < vertex_stack[vtx_p].getDepth()){

					lgst_dp = vertex_stack[vtx_p].getDepth();
					dp = lgst_dp + 1;
				}
			}
		}
	}

	vertex_stack.push_back(Vertex(dp, highest_decision_level, pa_index, var, val));

	// cout << "now the assignments are:" << endl;
	// printAssignments();

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;
}



pair<int, Clause> Solver :: learn(const Clause & cls){

	time_t t_begin = clock();

	set<int> path;

	conflict_counter.first += 1;

	for(unordered_map<int32_t, std :: pair<int8_t, float>> :: iterator it = literal_scores.begin(); it != literal_scores.end(); ++it){

		it -> second.second *= decay_c;
	}

	// cout << "the conflict clause is:" << endl;
	// for(int i = 0; i < cls.getLength(); ++i){

	// 	cout << cls.getClause()[i] << " ";
	// }
	// cout << endl;


	int hdl_2 = -1;
	Clause c;
	vector<int> lits;

	if(highest_decision_level == 0){

		func_clock[__func__].first += 1;
		func_clock[__func__].second += clock() - t_begin;

		return make_pair(hdl_2, c);
	}

	vector<int> Q;
	for(int i = 0; i < cls.getLength(); i++){

		int var = cls.getVariables()[i];
		path.insert(var);
		int vtx_p = findVertex(var);
		Q.push_back(vtx_p);
	}

	// cout << "length of path:" << path.size() << endl;


	while(!UIP(Q)){

		trackReason(Q);

		for(int i = 0; i < Q.size(); ++i){

			int var = vertex_stack[Q[i]].getVarVal().first;

			path.insert(var);
		}
	}
	// cout << "length of path:" << path.size() << endl;

	// cout << "path:" << endl;

	for(set<int> :: iterator it = path.begin(); it != path.end(); ++it){

		literal_scores[*it].second += 1.0;
		// cout << *it << " ";
	}
	// cout << endl;

	// cout << "literal_scores:" << endl;
	// for(unordered_map<int32_t, pair<int8_t, float>> :: iterator it = literal_scores.begin(); it != literal_scores.end(); ++it){

	// 	cout << "literal: " << it -> first << " value: " << int(it -> second.first) << " score: " << it -> second.second << endl;
	// }

	for(int i = 0; i < Q.size(); i++){

		if(vertex_stack[Q[i]].getDecisionLevel() < highest_decision_level && vertex_stack[Q[i]].getDecisionLevel() > hdl_2){

			hdl_2 = vertex_stack[Q[i]].getDecisionLevel();
		}
	}

	for(int i = 0; i < Q.size(); i++){

		pair<int, int> var_val = vertex_stack[Q[i]].getVarVal();
		int lit = var_val.first * ((var_val.second == 0) - (var_val.second == 1));
		lits.push_back(lit);
	}
	c.setClause(lits);

	if(hdl_2 == -1 && highest_decision_level != 0){

		hdl_2 = 0;
	}

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;

	// cout << "the learned clause is:" << endl;
	// for(int i = 0; i < c.getLength(); i++){

	// 	cout << c.getClause()[i] << " ";
	// }
	// cout << endl;

	return make_pair(hdl_2, c);
}



void Solver :: trackReason(vector<int> & Q){

	int dpst_vtx;
	vector<int16_t> pa_index;


	int dpst_ind = 0;
	int lgst_dp = 0;

	for(int i = 0; i < Q.size(); i++){

		if(vertex_stack[Q[i]].getDecisionLevel() == highest_decision_level){

			if(lgst_dp <= vertex_stack[Q[i]].getDepth()){

				dpst_ind = i;
				lgst_dp = vertex_stack[Q[i]].getDepth();
			}
		}
	}
	dpst_vtx = Q[dpst_ind];
	Q.erase(Q.begin() + dpst_ind);

	pa_index = vertex_stack[dpst_vtx].getParents();

	for(int i = 0; i < pa_index.size(); i++){

		int flag = 0;
		for(int j = 0; j < Q.size(); j++){

			if(pa_index[i] == Q[j]){

				flag = 1;
				break;
			}
		}

		if(!flag){

			Q.push_back(pa_index[i]);
		}

	}


}



bool Solver :: UIP(const vector<int> & Q) const{

	// TODO: modify UIP to new input type
	// Done.

	int hdl_count = 0;
	for(int i = Q.size() - 1; i >= 0; i--){

		if(highest_decision_level == vertex_stack[Q[i]].getDecisionLevel()){

			hdl_count += 1;
		}
	}

	if(hdl_count >= 2){

		return false;
	}
	else{

		return true;
	}
}



void Solver :: backtrack(int dl){

	// TODO: in time 0 or dl = 0, if we encounter a conflict, that means UNSAT.
	// backtrack to decision level dl in this function. backtrack assignment stack at the same time.
	// Done.

	time_t t_begin = clock();

	for(int i = vertex_stack.size() - 1; i >= 0; i--){

		if(vertex_stack[i].getDecisionLevel() > dl){

			int var = vertex_stack[i].getVarVal().first;
			literal_scores[var].first = UNASSIGNED;

			// cout << "backtrack: " << var << endl;

			vertex_stack.erase(vertex_stack.begin() + i);
		}
	}

	// cout << "now the assignments are:" << endl;
	// printAssignments();
	// cout << "now the decisions are:" << endl;
	// printDecisions();
	highest_decision_level = dl;

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;
}



void Solver :: addLearnedClause(const Clause & cls){

	// append the learned clause into current clause stack and clauselist.
	// do we need another list to store those learned clause?
	clauseList.push_back(cls);
	clause_stack.push_back(clauseList.size() - 1);
	clauseList[clauseList.size() - 1].enStack();
	n_cls += 1;

	// cout << "add learned clause:" << endl;
	// for(int i = 0; i < cls.getLength(); i++){

	// 	cout << cls.getClause()[i] << " ";
	// }
	// cout << endl;

	for(int i = 0; i < cls.getLength(); ++i){

		int lit = cls.getClause()[i];
		int index = lit + n_var - (lit > 0);

		adjacency_list[index].push_back(clauseList.size() - 1);
	}
}


void Solver :: printAssignments(){

	for(int i = 0; i < vertex_stack.size(); i++){

		pair<int, int> var_val = vertex_stack[i].getVarVal();
		cout << var_val.first * ((var_val.second == 1) - (var_val.second == 0)) << " ";
	}
	cout << endl;
}



void Solver :: printDecisions(){

	for(int i = 0; i < vertex_stack.size(); i++){

		if(vertex_stack[i].getDepth() == 1 && vertex_stack[i].getDecisionLevel() != 0){

			pair<int, int> var_val = vertex_stack[i].getVarVal();
			cout << var_val.first * ((var_val.second == 1) - (var_val.second == 0)) << " ";
		}
	}
	cout << endl;
}



void Solver :: buildAdjlist(){

	// func_clock[__func__].first = 0;
	// func_clock[__func__].second = 0;

	time_t t_begin = clock();

	adjacency_list.resize(2 * n_var);

	for(int i = 0; i < clauseList.size(); ++i){

		for(int j = 0; j < clauseList[i].getLength(); ++j){

			int lit = clauseList[i].getClause()[j];

			int index = lit + n_var - (lit > 0);

			adjacency_list[index].push_back(i);
		}
	}

	func_clock[__func__].first += 1;
	func_clock[__func__].second += clock() - t_begin;
}



unordered_map<string, pair<int, time_t>> Solver :: getFuncClock(){

	return func_clock;
}



// void Solver :: printClausestack(){

// 	for(int i = 0; i < clause_stack.size(); i++){

// 		int index = clause_stack[i];
// 		Clause c = clauseList[index];
// 		cout << "clause " << index << ": ";
// 		for(int j = 0; j < c.getLength(); j++){

// 			cout << c.getClause()[j] << " ";
// 		}
// 		cout << endl;
// 	}
// }