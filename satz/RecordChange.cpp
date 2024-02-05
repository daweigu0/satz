#include"RecordChange.h"
RecordChange::RecordChange(int flag, int idx)
{
	this->flag = flag;
	setPreMinimumClauseIdx(idx);
}

RecordChange::~RecordChange()
{
	this->assign_var_id.clear();
	this->delete_clauses.clear();
	this->delete_variables.clear();
	this->unit_clauses_remove.clear();
	this->unit_clauses_insert.clear();
}