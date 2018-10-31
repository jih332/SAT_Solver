//========================================================//
//  CSE 291E SAT Solver                                   //
//                                                        //
//  author: jih332                                        //
//========================================================//

// non-log version
// int_t version


#define __USE_MINGW_ANSI_STDIO 0
// to disable vsnprintf

#include <fstream>
#include <iostream>
#include <math.h>
#include <unordered_set>
#include <csignal>
#include <cstring>
#include "solver.h"
#include <io.h>

using namespace std;


void usage(){

	cout << "Usage: solver --name <filename (without .cnf suffix)> [<options>]" << endl;
	cout << " Options:" << endl 
		<< " 	--time t     Specify the time limit for SAT Solver" << endl
  		<< " 	--verbose    Print additional informoation on stdout" << endl;
  	cout << "when running, you can use Ctrl-C to abort" << endl << endl;
}


void handle_signal(int signum){

	if(signum == 2){

		interrupted = 1;
	}
}



int main(int argc, char *argv[]){

	usage();

	signal(SIGINT, handle_signal); 

	ifstream infile;
	string filename;
	int time_limit = 30;

	verbose = 0;
	interrupted = 0;

	if(argc == 1){

		return 0;
	}


	for(int i = 1; i < argc; ++i){


		if(!strcmp(argv[i], "--name")){

	    	filename = argv[++i];
	    }
	    else if(!strcmp(argv[i], "--time")){

	    	time_limit = stoi(argv[++i]);
	    }
		else if(!strcmp(argv[i], "--verbose")){

	    	verbose = 1;
		}
		else {

			cout << "Unrecognized argument: " << argv[i] << endl;
		    return 1;
		}
	}

	char initial;
	string format;
	int n_var = 0, n_cls = 0;


	infile.open(filename + ".cnf");

	if(!infile.is_open()){
	
		cout << "Error occurs when open file." << endl;
		return 1;
	}
	

	while(infile.peek() != EOF){

		int i = 0;

		if(infile.peek() == 'c'){
		
			string comment;
			infile.get(initial);
			getline(infile, comment);
		}
		else if(infile.peek() == 'p'){
		
			infile >> initial >> format >> n_var >> n_cls;
		}
		else{

			break;
		}
	}


	vector<Clause> cllist(n_cls);

	int clause_count = 0;

	if(format == "cnf" && n_var * n_cls != 0){
	
		while(infile.peek() != EOF){
		
			vector<int> cls;
			int temp;

			for(int j = 0; j < n_var; j++){
			
				infile >> temp;
				if(temp != 0){
				
					cls.push_back(temp);
				}
				else{
				
					infile.get();
					break;
				}
			}

			cllist[clause_count].setClause(cls);

			clause_count += 1;
		}
	}
	else{

		cout << "reading error. please check the format of input file.";
		return 1;
	}

	cout << "start processing..." << endl;

	infile.close();

	float decay_c = 0.9;

	Solver sol(n_var, n_cls, decay_c, cllist);

	time_t t_begin = clock();

	sol.preprocessing();

	int loop_count = 0;

	Clause last_c;

	while(clock() - t_begin < time_limit * 1000){

		loop_count++;

		if(interrupted){

			cout << "UNKNOWN due to keyboard interrupt (Ctrl-C)" << endl;
			if(verbose){

				cout << "decisions:" << endl;
				sol.printDecisions();
				cout << "time used: " << clock() - t_begin << "ms" << endl;
				cout << "func_clock:" << endl;
				unordered_map<string, pair<int, time_t>> fc = sol.getFuncClock();
				for(unordered_map<string, pair<int, time_t>> :: iterator it = fc.begin(); it != fc.end(); it++){

					cout << it -> first << ": " << it -> second.first << "times in " << it -> second.second << "ms" << endl;
				}
				return 0;
			}
		}
	
		Clause c =  sol.unitPropogate(loop_count);

		if(sol.getStatus() != c_CONFLICT){

			if(sol.allAssigned(loop_count)){

				cout << "SAT" << endl;
				if(verbose){

					cout << "assignments:" << endl;
					sol.printAssignments();
					cout << "time used: " << clock() - t_begin << "ms" << endl;
					cout << "func_clock:" << endl;
					unordered_map<string, pair<int, time_t>> fc = sol.getFuncClock();
					for(unordered_map<string, pair<int, time_t>> :: iterator it = fc.begin(); it != fc.end(); it++){

						cout << it -> first << ": " << it -> second.first << "times in " << it -> second.second << "ms" << endl;
					}
				}
				
				return 0;
			}
			else{

				pair<int, int> var_val = sol.makeDecision();
				sol.addVertex(var_val.first, var_val.second);
				sol.appendClause(((var_val.second == 0) - (var_val.second == 1)) * var_val.first, loop_count);
			}
		}
		else{

			pair<int, Clause> dl_c = sol.learn(c);

			if(dl_c.second.getLength() != 0){

				last_c = dl_c.second;
			}

			if(dl_c.first == -1){

				cout << "UNSAT" << endl;
				if(verbose){

					cout << "last learned clause:" << endl;
					for(int i = 0; i < last_c.getLength(); i++){

						cout << last_c.getClause()[i] << " ";
					}
					cout << endl;
					cout << "time used: " << clock() - t_begin << "ms" << endl;
					cout << "func_clock:" << endl;
					unordered_map<string, pair<int, time_t>> fc = sol.getFuncClock();
					for(unordered_map<string, pair<int, time_t>> :: iterator it = fc.begin(); it != fc.end(); it++){

						cout << it -> first << ": " << it -> second.first << "times in " << it -> second.second << "ms" << endl;
					}
				}

				return 0;
			}
			else{

				sol.backtrack(dl_c.first);
				sol.addLearnedClause(dl_c.second);
			}
		}
		
	}


	cout << "UNKNOWN due to time_limit" << endl;

	if(verbose){

		cout << "decisions:" << endl;
		sol.printDecisions();
		cout << "time used: " << clock() - t_begin << "ms" << endl;
		cout << "func_clock:" << endl;
		unordered_map<string, pair<int, time_t>> fc = sol.getFuncClock();
		for(unordered_map<string, pair<int, time_t>> :: iterator it = fc.begin(); it != fc.end(); it++){

			cout << it -> first << ": " << it -> second.first << "times in " << it -> second.second << "ms" << endl;
		}
	}

	return 0;

}


