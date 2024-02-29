#pragma once
#include<vector>
using namespace std;
class Vtoc
{
public:
	Vtoc() {};
	Vtoc(int size);
	Vtoc(const Vtoc& other);
	void setVtocSize(int size);
	int find(int variable_id);
	void insert(int variable_id, int clause_id);
	vector<int>& operator[](int variable_id);
	int size();
	static int idxToVariableId(int idx);
	static int variableIdToIdx(int variable_id);
	void pirntVtoc();
	vector<int>& at(int idx);
private:
	vector<vector<int>> v;
};

