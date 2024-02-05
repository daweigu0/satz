#include<iostream>
#include"input_class.cpp"
#include"vtoc.h"
#include <chrono>
#include"AdvancedDPLL.h"
#include"AdvancedFormula.h"
using namespace std;
extern Vtoc variableId_to_Clauses;

void testAdvancedDPLL1() {
	//C:/Users/l1768/Desktop/sat/DPLL≈‡—µ/DPLL≈‡—µ/SAT≤‚ ‘±∏—°À„¿˝/≤ª¬˙◊„À„¿˝/u-problem10-100.cnf
	string filename = "C:/Users/l1768/Desktop/sat/DPLL≈‡—µ/DPLL≈‡—µ/SAT≤‚ ‘±∏—°À„¿˝/Beijing/2bitadd_11.cnf";
	cout << filename << endl;  
	Input input(filename); 
	AdvancedFormula* f_ptr = new AdvancedFormula();
	f_ptr->setCurrentClausesCnt(input.getClauseCnt());
	f_ptr->setVariablesCnt(input.getBoolVarCnt()); 
	variableId_to_Clauses.setVtocSize(input.getBoolVarCnt());  
	input.readClauses<AdvancedFormula>(*f_ptr);
	f_ptr->initVariablesAssignAndFlipFlag(input.getBoolVarCnt());
	auto start = chrono::high_resolution_clock::now();
	int result = AdvancedDPLL::solverByIncrementalUpdate(*f_ptr);
	auto stop = chrono::high_resolution_clock::now();
	AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "∫ƒ ±£∫" << duration.count() / 1000.0 << "s" << endl;
}
void testAdvancedDPLLByConsole(string path) {
	string filename = path;
	//cout << filename << endl;
	Input input(filename);
	AdvancedFormula* f_ptr = new AdvancedFormula();
	f_ptr->setCurrentClausesCnt(input.getClauseCnt());
	f_ptr->setVariablesCnt(input.getBoolVarCnt());
	variableId_to_Clauses.setVtocSize(input.getBoolVarCnt());
	input.readClauses<AdvancedFormula>(*f_ptr);
	f_ptr->initVariablesAssignAndFlipFlag(input.getBoolVarCnt());
	auto start = chrono::high_resolution_clock::now();
	int result = AdvancedDPLL::solverByIncrementalUpdate(*f_ptr);
	auto stop = chrono::high_resolution_clock::now();
	AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << duration.count() / 1000.0 << endl;
}
void testAdvancedDPLL2(string filename) {
	//string filename = "../instances/unsat-php-15-10.cnf";
	cout << filename << endl;
	Input input(filename);
	AdvancedFormula* f_ptr = new AdvancedFormula();
	f_ptr->setCurrentClausesCnt(input.getClauseCnt());
	f_ptr->setVariablesCnt(input.getBoolVarCnt());
	variableId_to_Clauses.setVtocSize(input.getBoolVarCnt());
	input.readClauses<AdvancedFormula>(*f_ptr);
	f_ptr->initVariablesAssignAndFlipFlag(input.getBoolVarCnt());
	auto start = chrono::high_resolution_clock::now();
	int result = AdvancedDPLL::solverByIncrementalUpdate(*f_ptr);
	auto stop = chrono::high_resolution_clock::now();
	AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << duration.count() / 1000.0 << endl;
}
int main(int argc,char* argv[]) {
	//testAdvancedDPLL1();
	//test2(argv[1]);
	testAdvancedDPLLByConsole(argv[1]);
	return 0;
}