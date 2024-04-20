#include<iostream>
#include"input_class.cpp"
#include"vtoc.h"
#include <chrono>
#include"AdvancedDPLL.h"
#include"AdvancedFormula.h"
#include "json.h"
using namespace std;
//extern Vtoc variableId_to_Clauses;

void testAdvancedDPLL1() {
	//"D:/SAT/DPLLÅàÑµ/DPLLÅàÑµ/SAT²âÊÔ±¸Ñ¡ËãÀý/²»Âú×ãËãÀý/u-problem10-100.cnf"
	//D:/SAT/DPLLÅàÑµ/DPLLÅàÑµ/SAT²âÊÔ±¸Ñ¡ËãÀý/Beijing/4blocks.cnf ewddr2-10-by-5-8.cnf
	string filename = "D:/SAT/DPLLÅàÑµ/DPLLÅàÑµ/SAT²âÊÔ±¸Ñ¡ËãÀý/Beijing/e0ddr2-10-by-5-1.cnf";
	Json::Value root;
	//.substr(filename.rfind('/')+1)
	root["instance"] = filename;
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
	root["result"] = AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	root["time"] = duration.count() / 1000.0;
	root["others"]["NB_SINGLE"] = f_ptr->NB_SINGLE;
	root["others"]["NB_BACK"] = f_ptr->NB_BACK;
	root["others"]["NB_BRANCHE"] = f_ptr->NB_BRANCHE;
	root["others"]["NB_UP"] = f_ptr->NB_UP;
	root["others"]["NB_UP_DETECT"] = f_ptr->NB_UP_DETECT;
	root["others"]["NB_SHUFFLE_ING"] = f_ptr->NB_SHUFFLE_ING;
	root["verify_result"] = AdvancedDPLL::verifyResult(verify_f);
	cout << root.toStyledString() << endl;
}
void testAdvancedDPLLByConsole(string filename) {
	Json::Value root;
	int idx1 = filename.rfind('/');
	int idx2 = filename.rfind('\\');
	int idx = idx2 > idx1 ? idx2+1 : idx1+1;
	root["instance"] = filename.substr(idx < 0 ? 0 : idx);
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
	root["result"] = AdvancedDPLL::showResult(result);
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	root["time"] = duration.count() / 1000.0;
	root["others"]["NB_SINGLE"] = f_ptr->NB_SINGLE;
	root["others"]["NB_BACK"] = f_ptr->NB_BACK;
	root["others"]["NB_BRANCHE"] = f_ptr->NB_BRANCHE;
	root["others"]["NB_UP"] = f_ptr->NB_UP;
	root["others"]["NB_UP_DETECT"] = f_ptr->NB_UP_DETECT;
	root["others"]["NB_SHUFFLE_ING"] = f_ptr->NB_SHUFFLE_ING;
	root["verify_result"] = AdvancedDPLL::verifyResult(verify_f);
	cout << "over" << endl;
	cout << root.toStyledString() << endl;
}

int main(int argc,char* argv[]) {
	testAdvancedDPLL1();
	//testAdvancedDPLLByConsole(argv[1]);
	return 0;
}