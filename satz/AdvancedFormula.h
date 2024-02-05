#pragma once
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
	int minimum_clause_idx = 0;
	vector<int> unit_clauses;
	AdvancedFormula() {};
	AdvancedFormula(const AdvancedFormula& other);
	~AdvancedFormula();
	void setCurrentClausesCnt(int cnt);
	void setVariablesCnt(int cnt);
	void setMinimunClauseIdx(int idx);
	void initVariablesAssignAndFlipFlag(int variables_cnt);
	int findMinimunClauseIdx();
	void updateMinimunClauseIdx(int change_length_clause_id, int current_length);
	void printFormula();
};