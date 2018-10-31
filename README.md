# sat-solver-jih332

This is an instruction of how to run my program, also a update log recording problems encountered.  
You should be able to run my program by simply typing  
`make`  
in command line (make sure you're at the right directory). Then you will get *solver.exe*. To run it, type  
`solver`  
and you'll see a brief instruction.  

----

12/19/2017  
A minor change to previous version. Add a bool variable to indicate whether a specific clause is already in clause stack.  
Now it's much faster than before. The result of running on *graph-ordering-8* is listed below:  
~~~~
UNSAT
last learned clause:
3 53 -52 -54 -55 -51 -56
time used: 3687ms
func_clock:
backtrack: 1154times in 37ms
learn: 1155times in 205ms
buildAdjlist: 1times in 1ms
makeDecision: 1375times in 12ms
allAssigned: 1375times in 2ms
addVertex: 24054times in 363ms
appendClause: 24054times in 263ms
~~~~
As a comparision, the previous version takes 9817ms.  

----

12/09/2017  
A VSIDS version using heap sort.  

----

12/07/2017  
Add VSIDS heuristics for CDCL (based on previous version). However, it even becomes slower on some benchmarks, compared to the previous version.   
The results from *engine_4.cnf* (contains 6944 vars and 66654 cls):  
~~~~
UNKNOWN due to time_limit  
decisions:  
1 2 3 -4 -10 13 14 16 17 19 20 23 25 26 -27 30 31 -32 -35 36 -37 40 41 -42 60 -66 68 -70 -82 84  
func_clock:   
addVertex: 13502times in 7413ms  
allAssigned: 272times in 195ms  
appendClause: 13502times in 1484ms  
backtrack: 51times in 0ms  
buildAdjlist: 1times in 47ms  
learn: 51times in 402ms  
makeDecision: 272times in 12919ms`  
~~~~
shows that when there're many variables, makeDecision() could be expensive, and the time of calling it is much more than conflicts encountered. Hence, we may use heap sort to maintain a copy of literal_scores in descending order, then update it only a conflict has happened.     

----

12/06/2017  
Add adjacency list associated with each literals to speed up appending clause, now it doesn't need to traverse all the clauses to find those to be appended. It saves a lot of time on some benchmarks. But it also increases the memory comsumed.  

----

12/05/2017  
Specifys the int type for member variable, i.e., int8_t, int16_t or int32_t are used, instead of int.  
The program spends most running time in function: appendClause(), specifically, 20417ms out of 31807ms when runs on graph-ordering-8. Seems like building a adjacent list would help a lot, however, this may exceed the memory requirement.  

----

11/14/2017  
CDCL first version.  
Please use the command:  
`g++ -o 1 main.cpp clause.cpp solver.cpp vertex.cpp -std=c++11`  
to complie my program, the output file is 1.exe  
It gives the correct answers as DPLL, however, it's slower than DPLL sometimes. There might be some bugs?  
Cherish your life and don't use pointer.  
What's more, only enable log writting when running on some small .cnf files. The log for large CNF is pretty long. I'll make it more concise.  

----

11/06/2017  
Apply some minor changes to improve the performance a little bit.  
Now graph-ordering-14.cnf can be finished within 30 secs. The result is:  
UNSAT; time used: 25295ms  
To compare with it, the original version gives:  
UNSAT; time used: 32566ms  
Furthermore, please disable the log writting when running on some big cnfs.  

----

10/28/2017  
DPLL algorithm.  
use the same command to compile.  
The program will ask for filename as input(without .cnf as a suffix).  
Provided an option of whether to write log during running the program (recommended). The log will be written to filename.log, at current folder. The log will tell you the detail information about the whole procedure.  

The results of all the benchmarks in *sat-benchmarks-master\petite* are recorded in *Results.txt*.  

----

10/18/2017  
replaced all the code with a new version using STL container.

----

Please use the command:  
`g++ -o 1 main.cpp clause.cpp solver.cpp -std=c++11`  
to complie my program, the output file is *1.exe*  

I disabled the default input .cnf file in my code, so the user has to enter the name of input file by him/herself.  
The .cnf used for test is *Schur_160_5_d20.cnf*.  
The final output for this .cnf file is:   
UNKNOWN  
1877 assignments have been explored. The ratio is 6.23025e-229 of all the possible assignments.











