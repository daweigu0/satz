#include<iostream>
#include<vector>
#include<unordered_map>
#include"AdvancedFormula.h"
#include"vtoc.h"
using namespace std;
Vtoc variableId_to_Clauses;
int* AdvancedFormula::variables_assign_ptr = nullptr;
int* AdvancedFormula::flip_flag_ptr = nullptr;
AdvancedFormula::AdvancedFormula(const AdvancedFormula& other) {
	this->clauses = other.clauses;
	this->current_clauses_cnt = other.current_clauses_cnt;
	this->unit_clauses = other.unit_clauses;
	this->variables_cnt = other.variables_cnt;
}
AdvancedFormula::~AdvancedFormula() {
	this->clauses.clear();
	this->unit_clauses.clear();
}
void AdvancedFormula::setCurrentClausesCnt(int cnt)
{
	this->current_clauses_cnt = cnt;
}
void AdvancedFormula::printFormula() {
	for (int i = 1; i < this->clauses.size(); i++) {
		if (!this->clauses[i].flag) {
			cout << "clause_id: " << i << " length:" << this->clauses[i].length << " variables: ";
			for (int j = 0; j < this->clauses[i].variables.size(); j++) {
				if (!this->clauses[i].variables[j].flag)
					cout << this->clauses[i].variables[j].variable_id << " ";
			}
			cout << endl;
		}
	}
	cout << "unit_clauses: ";
	for (auto& item : this->unit_clauses) {
		cout << item << " ";
	}
	cout << endl;
	//variableId_to_Clauses.pirntVtoc();
	for (int i = 1; i < variableId_to_Clauses.size(); i++) {
		//cout << "i: " << i << endl;
		//!variableId_to_Clauses[i].empty()
		if (!variableId_to_Clauses.at(i).empty()) {
			int var_id = Vtoc::idxToVariableId(i);
			cout << "variable_id: " << var_id << " clause_id: ";
			for (auto& clause_id : variableId_to_Clauses.at(i)) {
				if (!this->clauses[clause_id].flag)
					cout << clause_id << " L:" << this->clauses[clause_id].length << " ";
			}
			cout << endl;
		}
	}
	cout << "variables_assign: " << endl;
	for (int i = 1; i < this->variables_cnt + 1; i++) {
		cout << "var_id: " << i << " value: " << (int)this->variables_assign_ptr[i] << endl;
	}
	cout << "flip_flag: " << endl;
	for (int i = 1; i < this->variables_cnt + 1; i++) {
		cout << "var_id: " << i << " flip_flag: " << (int)this->flip_flag_ptr[i] << endl;
	}
	cout << "============printFormulaº¯Êý============" << endl;
}

void AdvancedFormula::setVariablesCnt(int cnt) {
	this->variables_cnt = cnt;
}

void AdvancedFormula::initVariablesAssignAndFlipFlag(int variables_cnt) {
	int n = variables_cnt + 1;
	this->variables_assign_ptr = new int[n];
	this->flip_flag_ptr = new int[n];
	fill(this->variables_assign_ptr, this->variables_assign_ptr + n, -1);
	fill(this->flip_flag_ptr, this->flip_flag_ptr + n, -1);	
}