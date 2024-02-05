#pragma once
#include<vector>
using namespace std;
class RecordChange
{
public:
	enum{ 
		up_flag = 0,
		moms_flag = 1,
		add_unit_clause_flag = 2,
	};
	//static const bool up_flag = false;
	//static const bool moms_flag = true;
	int flag;
	int pre_minimum_clause_idx;
	vector<int> assign_var_id;
	vector<pair<int, int>> delete_variables;
	vector<int> delete_clauses;
	vector<int> unit_clauses_insert;
	vector<int> unit_clauses_remove;
	RecordChange(int flag, int idx);
	~RecordChange();

	void setPreMinimumClauseIdx(int idx) {
		this->pre_minimum_clause_idx = idx;
	}
};
