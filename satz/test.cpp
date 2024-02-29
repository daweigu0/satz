#include<iostream>
#include"input_class.cpp"
#include"vtoc.h"
#include <chrono>
#include"AdvancedDPLL.h"
#include"AdvancedFormula.h"
using namespace std;
//extern Vtoc variableId_to_Clauses;

void testAdvancedDPLL1() {
	//C:/Users/l1768/Desktop/sat/DPLL≈‡—µ/DPLL≈‡—µ/SAT≤‚ ‘±∏—°À„¿˝/≤ª¬˙◊„À„¿˝/u-problem10-100.cnf
	string filename = "C:/Users/l1768/Desktop/sat/DPLL≈‡—µ/DPLL≈‡—µ/SAT≤‚ ‘±∏—°À„¿˝/Beijing/4blocks.cnf";
	cout << filename << endl;  
	Input input(filename); 
	AdvancedFormula* f_ptr = new AdvancedFormula();
	f_ptr->setCurrentClausesCnt(input.getClauseCnt());
	f_ptr->setVariablesCnt(input.getBoolVarCnt()); 
	//f_ptr->variableId_to_Clauses.setVtocSize(input.getBoolVarCnt());
	f_ptr->setVtocSize(input.getBoolVarCnt());
	input.readClauses<AdvancedFormula>(*f_ptr);
	f_ptr->initVariablesAssignAndFlipFlag(input.getBoolVarCnt());

	AdvancedFormula verify_f(*f_ptr);

	auto start = chrono::high_resolution_clock::now();
	int result = AdvancedDPLL::solverByIncrementalUpdate(*f_ptr);
	//cout << f_ptr->current_variables_cnt << endl;
	auto stop = chrono::high_resolution_clock::now();
	AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "TIME:" << duration.count() / 1000.0 << "s" << " ";
	cout << "NB_SINGLE:" << f_ptr->NB_SINGLE << " " << 
		"NB_BACK:" << f_ptr->NB_BACK << " " 
		<< "NB_BRANCHE:" << f_ptr->NB_BRANCHE << " " 
		<< "NB_UP:" << f_ptr->NB_UP << " " 
		<< "NB_UP_DETECT:" << f_ptr->NB_UP_DETECT << " "
		<< "UP_DETECT_PERCENT:" << f_ptr->NB_UP_DETECT*1.0 / (f_ptr->NB_UP + f_ptr->NB_UP_DETECT) * 1.0 << endl;
	cout << endl;
	AdvancedDPLL::verifyResult(verify_f);
}
void testAdvancedDPLLByConsole(string filename) {
	cout << filename << endl;
	Input input(filename);
	AdvancedFormula* f_ptr = new AdvancedFormula();
	f_ptr->setCurrentClausesCnt(input.getClauseCnt());
	f_ptr->setVariablesCnt(input.getBoolVarCnt());
	//f_ptr->variableId_to_Clauses.setVtocSize(input.getBoolVarCnt());
	f_ptr->setVtocSize(input.getBoolVarCnt());
	input.readClauses<AdvancedFormula>(*f_ptr);
	f_ptr->initVariablesAssignAndFlipFlag(input.getBoolVarCnt());

	AdvancedFormula verify_f(*f_ptr);

	auto start = chrono::high_resolution_clock::now();
	int result = AdvancedDPLL::solverByIncrementalUpdate(*f_ptr);
	//cout << f_ptr->current_variables_cnt << endl;
	auto stop = chrono::high_resolution_clock::now();
	AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "TIME:" << duration.count() / 1000.0 << "s" << " ";
	cout << "NB_SINGLE:" << f_ptr->NB_SINGLE << " " <<
		"NB_BACK:" << f_ptr->NB_BACK << " "
		<< "NB_BRANCHE:" << f_ptr->NB_BRANCHE << " "
		<< "NB_UP:" << f_ptr->NB_UP << " "
		<< "NB_UP_DETECT:" << f_ptr->NB_UP_DETECT << " "
		<< "UP_DETECT_PERCENT:" << f_ptr->NB_UP_DETECT * 1.0 / (f_ptr->NB_UP + f_ptr->NB_UP_DETECT) * 1.0 << endl;
	cout << endl;
	AdvancedDPLL::verifyResult(verify_f);
}

int main(int argc,char* argv[]) {
	testAdvancedDPLL1();
	//testAdvancedDPLLByConsole(argv[1]);
	return 0;
}