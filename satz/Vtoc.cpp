#include"vtoc.h"
#include<iostream>
using namespace std;
Vtoc::Vtoc(int size) {
	this->v.resize(size * 2 + 1);
}
void Vtoc::setVtocSize(int size) {
	this->v.resize(size * 2 + 1);
}
int Vtoc::find(int variable_id) {
	variable_id = Vtoc::variableIdToIdx(variable_id);
	if (this->v[variable_id].size() != 0) return variable_id;
	return 0;
}
void Vtoc::insert(int variable_id, int clause_id) {
	variable_id = Vtoc::variableIdToIdx(variable_id);
	this->v[variable_id].push_back(clause_id);
}
vector<int>& Vtoc::operator[](int variable_id) {
	int idx = Vtoc::variableIdToIdx(variable_id);
	if (idx >= 1 && idx < this->v.size()) {
		return this->v[idx];
	}
	throw "索引下标有误！\n";
}
int Vtoc::size() {
	return this->v.size();
}
int Vtoc::idxToVariableId(int idx) {
	if (idx % 2 == 0) return -(idx/2);
	return idx/2+1;
}
int Vtoc::variableIdToIdx(int variable_id) {
	return variable_id > 0 ? 2 * variable_id - 1 : -variable_id * 2;
}

void Vtoc::pirntVtoc() {
	for (auto& row : v) {
		for (auto& col : row) {
			cout << col << " ";
		}
		cout << endl;
	}
}
vector<int>& Vtoc::at(int idx) {
	return this->v[idx];
}