#include<iostream>
#include"input_class.cpp"
#include"vtoc.h"
#include <chrono>
#include"AdvancedDPLL.h"
#include"AdvancedFormula.h"
using namespace std;
extern Vtoc variableId_to_Clauses;

void testAdvancedDPLL1() {
	string filename = "C:/Users/l1768/Desktop/sat/DPLL≈‡—µ/DPLL≈‡—µ/SAT≤‚ ‘±∏—°À„¿˝/¬˙◊„À„¿˝/S/problem2-50.cnf";
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
	//test2(argv[1]);
	testAdvancedDPLL1();
	return 0;
}