#pragma once
#include<iostream>
#include"AdvancedFormula.h"
#include"RecordChange.h"
#include<stack>
class AdvancedDPLL
{
public:
	AdvancedDPLL();
	~AdvancedDPLL();
	static void deleteClause(AdvancedFormula& f, int clause_idx, RecordChange& rc);
	static bool deleteVariableByClauseId(AdvancedFormula& f, int clause_idx, int variable_id, RecordChange& rc);
	static bool isClausesEmpty(AdvancedFormula& f);
	static int findNoDeleteVariableByClauseId(AdvancedFormula& f, int clause_id);
	static int applyVariableAssign(AdvancedFormula& f, int variable_id, bool value);
	static int applyVariableAssign(AdvancedFormula& f, int variable_id, bool value, RecordChange& rc);
	static void pureLiteralSimplify(AdvancedFormula& f);
	static int up(AdvancedFormula& f,RecordChange& rc);
	static int randomSelectVariableIdInVector(vector<int>& v);
	static int momsSelectVariable(AdvancedFormula& f);
	static int getMinimumClauseLength(AdvancedFormula& f);
	static void recoveryForAdvancedH(AdvancedFormula& f, stack<RecordChange*>& st);
	static vector<int>* minimumSentenceSet(AdvancedFormula& f);
	static int H(AdvancedFormula& f);
	static int advancedH(AdvancedFormula& f);
	static void addUnitClause(AdvancedFormula& f, int var_id);
	static void deleteUnitClause(AdvancedFormula& f, RecordChange& rc);
	static void upBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc);
	static int momsBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc);
	static int upFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s);
	static int momsFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s);
	static int backTracking(AdvancedFormula& f, stack<RecordChange*>& s, bool up_fail_or_moms_fail_flag);
	static int solverByIncrementalUpdate(AdvancedFormula& f);
	static void showResult(int result);
};