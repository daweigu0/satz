#pragma once
#include<vector>
using namespace std;
class RecordChange
{
public:
	static const bool up_flag = false;
	static const bool moms_flag = true;
	bool up_or_moms_flag;//false代表up操作，true代表moms操作
	vector<int> assign_var_id;
	vector<pair<int, int>> delete_variables;
	vector<int> delete_clauses;
	//vector<pair<int, int>> clauses_length;
	vector<int> unit_clauses_insert;
	vector<int> unit_clauses_remove;
	RecordChange(bool flag);
	~RecordChange();
};
