#include <iostream>
#include "clause.h"

using namespace std;

Clause :: Clause(){

	stack_status = false;
}

Clause :: Clause(vector<int> & cls){

	for(int i = 0; i < cls.size(); i++){
	
		literals.push_back(cls[i]);
		variables.push_back(abs(cls[i]));
	}
	stack_status = false;
}

Clause :: Clause(const Clause & clause){

	for(int i = 0; i < clause.literals.size(); i++){
	
		literals.push_back(clause.literals[i]);
		variables.push_back(clause.variables[i]);
	}
	stack_status = clause.stack_status;
}

// Clause :: ~Clause()
// {
// 	delete literals;
// }

void Clause :: setClause(vector<int> & cls){

	for(int i = 0; i < cls.size(); i++){
	
		literals.push_back(cls[i]);
		variables.push_back(abs(cls[i]));
	}
}

const vector<int16_t> & Clause :: getClause() const{

	return literals;
}

const vector<int16_t> & Clause :: getVariables() const{

	return variables;
}

int Clause :: getLength() const{

	return literals.size();
}

bool Clause :: inStack(){

	return stack_status;
}

void Clause :: enStack(){

	stack_status = true;
}

void Clause :: deStack(){

	stack_status = false;
}

bool Clause :: operator==(const Clause & clause) const{

	if(this -> literals.size() != clause.literals.size()){

		return false;
	}
	else{

		for(int i = 0; i < this -> literals.size(); i++){

			if(this -> literals[i] != clause.literals[i]){

				return false;
			}
		}
	}

	return true;
}