#include"RecordChange.h"
RecordChange::RecordChange(bool flag)
{
	this->up_or_moms_flag = flag;
}

RecordChange::~RecordChange()
{
	this->assign_var_id.clear();
	this->delete_clauses.clear();
	this->delete_variables.clear();
	this->unit_clauses_remove.clear();
	this->unit_clauses_insert.clear();
}