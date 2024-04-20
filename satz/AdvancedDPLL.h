#pragma once
#include<iostream>
#include"AdvancedFormula.h"
#include"RecordChange.h"
#include<stack>
#include<queue>
struct cmp
{
	bool operator()(pair<long long, int>& p1, pair<long long, int>& p2) {
		return p1.first < p2.first;
	}
};
class AdvancedDPLL
{
public:
	AdvancedDPLL();
	~AdvancedDPLL();
	static bool deleteClause(AdvancedFormula& f, int clause_idx, RecordChange& rc);
	static bool deleteVariableByClauseId(AdvancedFormula& f, int clause_idx, int variable_id, RecordChange& rc);
	static inline bool isClausesEmpty(AdvancedFormula& f);
	static int findNoDeleteVariableByClauseId(AdvancedFormula& f, int clause_id);
	static int applyVariableAssign(AdvancedFormula& f, int variable_id, bool value, RecordChange& rc);
	static int applyVariableAssign(AdvancedFormula& verify_f, int variable_id, bool value);
	static void pureLiteralSimplify(AdvancedFormula& f);
	static int up(AdvancedFormula& f, RecordChange& rc, int flag);
	static int randomSelectVariableIdInVector(vector<int>& v);
	template<typename T>
	static int getRandomIdx(T& t);
	static int selectVariableInPriorityQueue(priority_queue<pair<long long, int>, vector<pair<long long, int>>, cmp>& q);
	static int momsSelectVariable(AdvancedFormula& f);
	static int getMinimumClauseLength(AdvancedFormula& f);
	static void recoveryForAdvancedH(AdvancedFormula& f, stack<RecordChange*>& st);
	static vector<int>* minimumSentenceSet(AdvancedFormula& f);
	static int H(AdvancedFormula& f, stack<RecordChange*>& st);
	static long long get_resolvant_nb(AdvancedFormula& f,RecordChange& rc, vector<pair<pair<int, int>, pair<int, int>>>& count);
	static int examine1(AdvancedFormula& f, stack<RecordChange*>& st, int test_var, vector<pair<int, long long>>& tested_vars, vector<pair<pair<int, int>, pair<int, int>>>& count);
	static int examine(AdvancedFormula& f, stack<RecordChange*>& st, int test_var, vector<pair<int, long long>>& tested_vars);
	static bool getRandomBoolean();
	static void countForProp(AdvancedFormula& f, int var, pair<pair<int, int>, pair<int, int>>& p);
	static int getSelectVariable(AdvancedFormula& f, vector<pair<int, long long>>& tested_vars);
	static int improveH(AdvancedFormula& f, stack<RecordChange*>& st);
	static long long calculateWx(AdvancedFormula& f, const RecordChange& rc);
	static void addUnitClause(AdvancedFormula& f, int var_id);
	static void deleteUnitClause(AdvancedFormula& f, RecordChange& rc);
	static void upBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc);
	static int momsBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc);
	static int upFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s);
	static int momsFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s);
	static int backTracking(AdvancedFormula& f, stack<RecordChange*>& s, int up_fail_or_moms_fail_flag);
	static int solverByIncrementalUpdate(AdvancedFormula& f);
	static string verifyResult(AdvancedFormula& verify_f);
	static string showResult(int result);
};