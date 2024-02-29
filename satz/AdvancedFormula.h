#pragma once
#include<vector>
#include"vtoc.h"
using namespace std;
struct variable {
	bool flag = false;//ɾ����ǣ�True����ñ�Ԫ�ѱ�ɾ��
	int variable_id;
};
struct clause {
	bool flag = false;//ɾ����ǣ�True������Ӿ��ѱ�ɾ��
	vector<struct variable> variables;
	int length;
};

class AdvancedFormula{
	
public:	
	static int* variables_assign_ptr;
	static int* flip_flag_ptr;
	Vtoc variableId_to_Clauses;
	vector<struct clause> clauses;
	int current_clauses_cnt;//��δ��ɾ�����Ӿ������
	//int current_variables_cnt;//��δ��ֵ�ı�Ԫ������
	int variables_cnt;
	int minimum_clause_idx = 0;
	long long NB_SINGLE = 0;
	long long NB_BACK = 0;
	long long NB_BRANCHE = 0;
	long long NB_UP = 0;
	long long NB_UP_DETECT = 0;
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
};