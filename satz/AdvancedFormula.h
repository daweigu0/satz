#pragma once
#include<vector>
#include"vtoc.h"
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
	Vtoc variableId_to_Clauses;
	vector<struct clause> clauses;
	int current_clauses_cnt;//还未被删除的子句的数量
	//int current_variables_cnt;//还未赋值的变元的数量
	int variables_cnt;
	int minimum_clause_idx = 0;
	long long NB_SINGLE = 0;
	long long NB_BACK = 0;
	long long NB_BRANCHE = 0;
	long long NB_UP = 0;
	long long NB_UP_DETECT = 0;
	long long NB_SHUFFLE_FRONT = 0;
	long long NB_SHUFFLE_ING = 0;
	
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
	int getMinimumClauseLength();
	void printFormula();
	bool isClauseDelete(int clause_id);
	void setVtocSize(int size);
	vector<int>& getClausesByVariableId(int variable_id);
	static int getCluaseLengthByClauseId(AdvancedFormula& f,int clause_id);
	static int getVariableAssignByVariableId(const int var_id);
	inline bool checkVariableIsInitialState(int var);
};

/**
 * @brief 检查一个变元是否是初始状态
 * @param var
 * @return 是初始状态返回true，否则返回false
*/
inline bool AdvancedFormula::checkVariableIsInitialState(int var) {
	var = abs(var);
	if (var < 1 || var > this->variables_cnt) {
		cout << "checkVariableIsNoAssign error" << endl;
		cout << "变元id越界" << endl;
	}
	return AdvancedFormula::variables_assign_ptr[var] == -1 && AdvancedFormula::flip_flag_ptr[var] == -1;
}