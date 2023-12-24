#pragma once
#include<unordered_map>
#include<vector>
using namespace std;
struct variable {
	bool flag = false;//删除标记，True代表该变元已被删除
	int variable_id;
};
struct clause {
	bool flag = false;//删除标记，True代表该子句已被删除
	vector<struct variable> variables;
	int length;
};

class AdvancedFormula{
	
public:	
	static int* variables_assign_ptr;
	static int* flip_flag_ptr;
	vector<struct clause> clauses;
	int current_clauses_cnt;
	int variables_cnt;
	vector<int> unit_clauses;
	AdvancedFormula() {};
	AdvancedFormula(const AdvancedFormula& other);
	~AdvancedFormula();
	void setCurrentClausesCnt(int cnt);
	void setVariablesCnt(int cnt);
	void initVariablesAssignAndFlipFlag(int variables_cnt);
	void printFormula();
};