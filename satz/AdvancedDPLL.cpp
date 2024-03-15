#include"AdvancedDPLL.h"
#include<stack>
#include<random>
#include"vtoc.h"
#include"RecordChange.h"
#include<algorithm>
#include <unordered_set>
#include"AdvancedFormula.h"
#include<map>
//extern Vtoc variableId_to_Clauses;
enum Result
{
	HCOMPLETE = -2,
	NORMAL = -1,
	UNSAT = 0,
	SAT = 1,
	COMPLETE = 2,	
	EXAMINE_NORMAL = 3,
};
AdvancedDPLL::AdvancedDPLL()
{

}

AdvancedDPLL::~AdvancedDPLL()
{
}

/**
 * @brief 检查var_id变元是否还存在于f中
 * @param var_id 传abs(var_id)、var_id都可
 * @return true为存在，false即为不存在
*/
bool isVariableNoDelete(AdvancedFormula& f, int var_id) {
	for (const auto& clause_id : f.getClausesByVariableId(var_id)) {
		if (!f.clauses[clause_id].flag) {
			for (const auto& var : f.clauses[clause_id].variables) {
				if (var.variable_id == var_id && !var.flag) return true;
			}
		}		
	}
	for (const auto& clause_id : f.getClausesByVariableId(-var_id)) {
		if (!f.clauses[clause_id].flag) {
			for (const auto& var : f.clauses[clause_id].variables) {
				if (var.variable_id == -var_id && !var.flag) return true;
			}
		}
	}
	return false;
}




/**
 * @brief 根据子句id删除子句
 * 1、将子句的flag置为true
 * 2、将当前子句数量减一
 * 3、如果单子句的集合中（unit_clauses）中也有该子句，要将其删除
 * @param f 
 * @param clause_idx 
 * @param rc 
*/
bool AdvancedDPLL::deleteClause(AdvancedFormula& f, int clause_idx, RecordChange& rc) {
	if (!f.clauses[clause_idx].flag) {
		rc.delete_clauses.push_back(clause_idx);
		f.clauses[clause_idx].flag = true;
		f.current_clauses_cnt--;
		for (int i = 0; i < f.unit_clauses.size(); i++) {
			if (f.unit_clauses[i] == clause_idx) {
				rc.unit_clauses_remove.push_back(clause_idx);
				f.unit_clauses[i] = f.unit_clauses.back();
				f.unit_clauses.pop_back();
			}
		}
		f.updateMinimunClauseIdx(clause_idx, 0);
		return true;
	}
	return false;
}

bool AdvancedDPLL::deleteVariableByClauseId(AdvancedFormula& f, int clause_idx, int variable_id, RecordChange& rc) {
	bool flag = false;//为true代表删除variable_id变元导致该子句成为了空子句。
	if (!f.clauses[clause_idx].flag) {
		for (auto& item : f.clauses[clause_idx].variables) {
			if (!item.flag && item.variable_id == variable_id) {
				item.flag = true;
				rc.delete_variables.push_back(make_pair(clause_idx, item.variable_id));//可优化
				break;
			}
		}
		int res = --f.clauses[clause_idx].length;
		if (res == 0) {
			f.clauses[clause_idx].flag = true;
			f.current_clauses_cnt--;
			rc.delete_clauses.push_back(clause_idx);
			flag = true;
			for (int i = 0; i < f.unit_clauses.size(); i++) {
				if (f.unit_clauses[i] == clause_idx) {
					rc.unit_clauses_remove.push_back(clause_idx);
					f.unit_clauses[i] = f.unit_clauses.back();
					f.unit_clauses.pop_back();
				}
			}
		}
		if (res == 1) {
			f.unit_clauses.push_back(clause_idx);
			rc.unit_clauses_insert.push_back(clause_idx);
		}
		f.updateMinimunClauseIdx(clause_idx, res);
	}
	return flag;
}

/**
 * @brief 判断算例中是否为空
 * @param f 
 * @return true为算例为空，false则不为空。
*/
inline bool AdvancedDPLL::isClausesEmpty(AdvancedFormula& f) {
	/*if (f.current_clauses_cnt > 0) return false;
	return true;*/
	if (f.current_clauses_cnt == 0) return true;
	return false;
}

int AdvancedDPLL::findNoDeleteVariableByClauseId(AdvancedFormula& f, int clause_id) {
	for (auto& var : f.clauses[clause_id].variables) {
		if (!var.flag) return var.variable_id;
	}
	return 0;
}

/**
 * @brief 对变元variable_id进行赋值
 * @param f 
 * @param variable_id 要赋值的变元，传进来必须为abs(variable_id)
 * @param value 要赋值的值，true或者false，也即1或0
 * @param rc 记录变化的RecordChange对象
 * @return 
*/
int AdvancedDPLL::applyVariableAssign(AdvancedFormula& f, int variable_id, bool value,RecordChange& rc) {
	AdvancedFormula::flip_flag_ptr[variable_id]++;
	if (AdvancedFormula::flip_flag_ptr[variable_id] > 1) 
		cout << "apply flip error" << endl;
	AdvancedFormula::variables_assign_ptr[variable_id] = value;
	rc.assign_var_id.push_back(variable_id);
	//f.current_variables_cnt--;
	int var_id = value ? variable_id : -variable_id;
	if (f.variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : f.getClausesByVariableId(var_id)) {
			AdvancedDPLL::deleteClause(f, clause_id, rc);
			if (AdvancedDPLL::isClausesEmpty(f)) return SAT;
		}
	}
	var_id = -var_id;
	if (f.variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : f.getClausesByVariableId(var_id)) {
			bool flag = AdvancedDPLL::deleteVariableByClauseId(f, clause_id, var_id, rc);
			if (flag) return UNSAT;
		}
	}
	return NORMAL;
}

/**
 * @brief 验证结果赋值函数
 * @param f 
 * @param variable_id 
 * @param value 
 * @return 
*/
int AdvancedDPLL::applyVariableAssign(AdvancedFormula& verify_f, int variable_id, bool value) {
	RecordChange rc(RecordChange::moms_flag,verify_f.minimum_clause_idx);
	int var_id = value ? variable_id : -variable_id;
	if (verify_f.variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : verify_f.getClausesByVariableId(var_id)) {
			AdvancedDPLL::deleteClause(verify_f, clause_id, rc);
			if (AdvancedDPLL::isClausesEmpty(verify_f)) return SAT;
		}
	}
	var_id = -var_id;
	if (verify_f.variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : verify_f.getClausesByVariableId(var_id)) {
			bool flag = AdvancedDPLL::deleteVariableByClauseId(verify_f, clause_id, var_id, rc);
			if (flag) return UNSAT;
		}
	}
	return NORMAL;
}

void AdvancedDPLL::pureLiteralSimplify(AdvancedFormula& f) {
	cout << "#######################纯文字化简#######################" << endl;
	RecordChange* rc = new RecordChange(RecordChange::moms_flag,f.minimum_clause_idx);
	for (int i = 1; i < f.variableId_to_Clauses.size(); i += 2) {
		if (!f.variableId_to_Clauses.at(i).empty() && f.variableId_to_Clauses.at(i + 1).empty()) {
			int var_id = Vtoc::idxToVariableId(i);
			for (auto& clause_id : f.variableId_to_Clauses.at(i)) {
				bool value = var_id > 0 ? true : false;
				AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, *rc);
				if (AdvancedDPLL::isClausesEmpty(f)) {
					delete rc;
					return;
				}
			}
		}
		if (f.variableId_to_Clauses.at(i).empty() && !f.variableId_to_Clauses.at(i + 1).empty()) {
			int var_id = Vtoc::idxToVariableId(i + 1);
			for (auto& clause_id : f.variableId_to_Clauses.at(i + 1)) {
				bool value = var_id > 0 ? true : false;
				AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, *rc);
				if (AdvancedDPLL::isClausesEmpty(f)) {
					delete rc;
					return;
				} 
			}
		}
	}
	delete rc;
	cout << "######################################" << endl;
}
/**
 * @brief 
 * @param f 
 * @param rc 
 * @param flag true为H中的UP探测
 * @return 
*/
int AdvancedDPLL::up(AdvancedFormula& f,RecordChange& rc,int flag) {
	bool unit_clause_find = false;
	vector<int>& uc = f.unit_clauses;
	do
	{
		unit_clause_find = false;
		if (!uc.empty()) {
			unit_clause_find = true;
			int clause_id = uc.back();
			int var_id = AdvancedDPLL::findNoDeleteVariableByClauseId(f, clause_id);
			uc.pop_back();
			rc.unit_clauses_remove.push_back(clause_id);			
			if(var_id==0) cout << "up error" << endl;
			bool value = var_id > 0 ? true : false;
			int result = AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, rc);
			switch (flag)
			{
			case 1:
				f.NB_UP_DETECT++;
				break;
			case 0:
				f.NB_UP++;
				break;
			}
			if (result == SAT || result == UNSAT) return result;
		}
	} while (unit_clause_find);
	return NORMAL;
}

int AdvancedDPLL::momsSelectVariable(AdvancedFormula& f) {
	int min_clause_length = INT_MAX;
	for (auto it = (++f.clauses.begin()); it != f.clauses.end(); it++) {
		clause c = *it;
		if (!c.flag) {
			min_clause_length = min(min_clause_length, c.length);
		}
	}
	vector<int> min_clauses;
	for (int i = 1; i < f.clauses.size(); i++) {
		if (!f.clauses[i].flag && f.clauses[i].length == min_clause_length) {
			min_clauses.push_back(i);
		}
	}
	int n = f.variables_cnt+1;
	int* bucket_count = new int[n];
	fill(bucket_count, bucket_count + n, 0);
	int max_emerge_cnt = INT_MIN;
	for (auto& clause_id : min_clauses) {
		for (auto& var : f.clauses[clause_id].variables) {
			if (!var.flag) {
				max_emerge_cnt = max(++bucket_count[abs(var.variable_id)], max_emerge_cnt);
			}
		}
	}
	vector<int> vars;
	for (int i = 1; i < n; i++) {
		if (bucket_count[i] == max_emerge_cnt) {
			vars.push_back(i);
		}
	}
	delete[] bucket_count;
	if (vars.size() == 1) return *vars.begin();
	return AdvancedDPLL::randomSelectVariableIdInVector(vars);
}
int AdvancedDPLL::getMinimumClauseLength(AdvancedFormula& f) {
	int min_clause_length = INT_MAX;
	for (auto it = (++f.clauses.begin()); it != f.clauses.end(); it++) {
		clause c = *it;
		if (!c.flag) {
			min_clause_length = min(min_clause_length, c.length);
		}
	}
	return min_clause_length;
}

/**
 * @brief 注意返回的vector的第一个元素是最短子句的长度
 * @param f 
 * @return vector<int>*
*/
vector<int>* AdvancedDPLL::minimumSentenceSet(AdvancedFormula& f) {
	vector<int>* min_clauses = new vector<int>();
	int min_clause_length = f.getMinimumClauseLength();
	min_clauses->push_back(min_clause_length);
	for (int i = 1; i < f.clauses.size(); i++) {
		if (!f.clauses[i].flag && f.clauses[i].length == min_clause_length) {
			(*min_clauses).push_back(i);
		}
	}
	return min_clauses;
}

/**
 * @brief advancedH函数挑选变元时碰到f+=x或f+=-x都不满足时，将f恢复到原来的状态
 * @param f 
 * @param st 
*/
void AdvancedDPLL::recoveryForAdvancedH(AdvancedFormula& f, stack<RecordChange*>& st) {
	while (st.top()->flag == RecordChange::add_unit_clause_flag) {
		AdvancedDPLL::deleteUnitClause(f, *st.top());
		delete st.top();
		st.pop();
	}
}



struct cmp_map 
{
	bool operator()(const long long& n1, const long long& n2) const{
		return n1 > n2;
	}
};

/**
 * @brief 将v中的元素重新散列到s中
 * @param s 
 * @param v 
*/
void reHashSet(unordered_set<int>*& s,vector<int>& v,int& source_f_min_clause_len,RecordChange& rc) {
	if(source_f_min_clause_len != v[0]) {
		source_f_min_clause_len = v[0];
		delete s;
		s = new unordered_set<int>();
		for (int i = 1; i < v.size();i++) {
			s->insert(v[i]);
		}
	}
	else {
		for (int clause_id : rc.delete_clauses) {
			s->erase(clause_id);
		}
	}
}
void shuffleVector(std::vector<int>& vec, std::vector<int>::iterator leftIter, std::vector<int>::iterator rightIter) {
	if(leftIter >= rightIter) return;
	// 使用随机数引擎作为随机数生成器
	std::random_device rd;
	std::mt19937 rng(rd());

	// 使用 std::shuffle 打乱 vector 中的元素
	std::shuffle(leftIter, rightIter, rng);
}

int AdvancedDPLL::randomSelectVariableIdInVector(vector<int>& v) {
	// 创建随机数引擎
	random_device rd;
	mt19937 gen(rd());

	// 创建均匀分布函数
	uniform_int_distribution<> dis(0, v.size() - 1);

	// 随机选择一项
	int randomIndex = dis(gen);
	int var_id = v[randomIndex];
	return var_id;
}

int AdvancedDPLL::selectVariableInPriorityQueue(priority_queue<pair<long long, int>, vector<pair<long long, int>>, cmp>& q) {
	long long pre_H_x;
	if (!q.empty())
		pre_H_x = q.top().first;
	else
		return NORMAL;
	vector<int> vars;
	while (!q.empty()) {
		int var_id = q.top().second;
		long long hx = q.top().first;
		//&& isVariableNoDelete(f, var_id)
		if (AdvancedFormula::variables_assign_ptr[var_id] == -1 && AdvancedFormula::flip_flag_ptr[var_id] == -1) {
			if (pre_H_x == hx) {
				vars.push_back(var_id);
			}
			else {
				if (!vars.empty()) {
					if (vars.size() == 1) return vars[0];
					else {
						return AdvancedDPLL::randomSelectVariableIdInVector(vars);
					}
				}
				else {
					pre_H_x = hx;
					vars.push_back(var_id);
				}
			}
		}
		q.pop();
	}
	return NORMAL;
}

/**
 * @brief 根据satz的策略返回H(x)值最大的变元
 * @param f 
 * @return 返回H(x)值最大的变元id
*/
int AdvancedDPLL::H(AdvancedFormula& f, stack<RecordChange*>& st) {
	//cout << "======================advancedH函数======================" << endl;
	//f.printFormula();
	enum PROP{P41,P31,P} flag;
	flag = P;
	const int T = 10;
	vector<int> free_variable;
	for (int i = 1; i <= f.variables_cnt; i++) {
		//&& isVariableNoDelete(f, i)
		if (AdvancedFormula::variables_assign_ptr[i] == -1 && AdvancedFormula::flip_flag_ptr[i] == -1) {
			free_variable.push_back(i);
		}
	}
	vector<int> PROP41;
	vector<int> PROP31;
	for (auto& var : free_variable) {
		int cnt = 0;//包含X或-X的二元子句的数量
		int p_cnt = 0;//子句含有变元X的数量
		int n_cnt = 0;//子句含有变元-X的数量
		for (auto& clause_id : f.getClausesByVariableId(var)) {
			if (!f.clauses[clause_id].flag) {
				if (f.clauses[clause_id].length == 2) cnt++;
				p_cnt++;
			}
		}
		for (auto& clause_id : f.getClausesByVariableId(-var)) {
			if (!f.clauses[clause_id].flag) {
				if (f.clauses[clause_id].length == 2) cnt++;
				n_cnt++;
			}
		}
		if (cnt >= 4 && p_cnt >= 1 && n_cnt >= 1) PROP41.push_back(var);
		if (cnt >= 3 && p_cnt >= 1 && n_cnt >= 1) PROP31.push_back(var);
	}
	if (PROP41.size() >= T)
	{
		flag = P41;
	}	
	else
	{
		if (PROP31.size() >= T) 
			flag = P31;
	}
	vector<int>* variables = NULL;
	switch (flag)
	{
	case P41:
		variables = &PROP41;
		break;
	case P31:
		variables = &PROP31;
		break;
	case P:
		variables = &free_variable;
		break;
	}
	unordered_set<int>* s_ptr = new unordered_set<int>();//找出f中最的短子句
	int source_f_min_clause_len = f.getMinimumClauseLength();
	for (int i = 1; i < f.clauses.size(); i++) {
		if (!f.clauses[i].flag && f.clauses[i].length == source_f_min_clause_len) {
			s_ptr->insert(i);
		}
	}
	//pair<long long,int> first为H(x)的值，second为变元x的id
	priority_queue<pair<long long,int>,vector<pair<long long,int>>,cmp> q;

	/*long long max_H_x = LLONG_MIN;
	int select_var = NORMAL;	*/

	int seselect_var_cnt = 0;
	int up_cnt = 0;
	if(f.NB_UP_DETECT%2==0)
		shuffleVector(*variables,variables->begin(),variables->end());
	for (auto& var : (*variables)) {
		if (AdvancedFormula::getVariableAssignByVariableId(var) != -1) continue;
		
		RecordChange* rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag,f.minimum_clause_idx);		
		AdvancedDPLL::addUnitClause(f, var);//f + { x }		
		Result res_of_f1 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { x }

		if (res_of_f1 == SAT) {
			return HCOMPLETE;
		}

		vector<int>* Q1 = AdvancedDPLL::minimumSentenceSet(f);//找出f + { x }中的最短子句
		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		/*if (st.size() == 70) {
			f.printFormula();
		}*/
		delete rc;

		rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag,f.minimum_clause_idx);
		AdvancedDPLL::addUnitClause(f, -var);//f + { -x }		
		Result res_of_f2 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { -x }

		if (res_of_f2 == SAT) {
			return HCOMPLETE;
		}

		if (res_of_f1 == UNSAT && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
			delete rc;
			delete Q1;
			delete s_ptr;
			//AdvancedDPLL::recoveryForAdvancedH(f, st);
			return NORMAL;
		}

		vector<int>* Q2 = AdvancedDPLL::minimumSentenceSet(f);//找出f + { -x }中的最短子句

		if ( res_of_f1 == UNSAT && res_of_f2 == NORMAL) {
			reHashSet(s_ptr, *Q2, source_f_min_clause_len, *rc);
			st.push(rc);
			delete Q1,Q2;
			f.NB_SINGLE++;
			continue;
		}
		if (res_of_f1 == NORMAL && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);
			delete rc;
			rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
			AdvancedDPLL::addUnitClause(f, var);			
			AdvancedDPLL::up(f, *rc, -1);
			reHashSet(s_ptr, *Q1,source_f_min_clause_len,*rc);
			st.push(rc);
			delete Q1,Q2;
			f.NB_SINGLE++;
			continue;
		}

		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		delete rc;
		long long W_x, W_nx;
		up_cnt++;
		if ((*Q1)[0] == source_f_min_clause_len) {//该分支计算最短子句在f1而不在f中的总数目。			
			int cnt = 0;
			for (int i = 1; i < (Q1->size()); i++)
			{
				if(s_ptr->find((*Q1)[i])== s_ptr->end()) cnt++;
			}
			W_x = cnt;
		}
		else {
			W_x = Q1->size() - 1;
		}
		if ((*Q2)[0] == source_f_min_clause_len) {
			int cnt = 0;
			for (int i = 1; i < (Q2->size()); i++)
			{
				if (s_ptr->find((*Q2)[i]) == s_ptr->end()) cnt++;
			}
			W_nx = cnt;
		}
		else {
			W_nx = Q2->size() - 1;
		}
		long long H_x = ((W_x * W_nx) << 10) + W_x + W_nx;
		/*if (H_x > max_H_x) {
			max_H_x = H_x;
			select_var = var;
			seselect_var_cnt++;
			if (seselect_var_cnt == T) {
				delete Q1;
				delete Q2;
				break;
			}
		}*/
		q.push(make_pair(H_x, var));
		delete Q1;
		delete Q2;
		/*if (up_cnt >= (*variables).size()*3/4) {
			while (!q.empty()) {
				int var_id = q.top().second;
				if (AdvancedFormula::variables_assign_ptr[var_id] == -1 && AdvancedFormula::flip_flag_ptr[var_id] == -1) {
					delete s_ptr;
					return var_id;
				}
				q.pop();
			}
		}*/
	}
	delete s_ptr;
	while (!q.empty()) {
		int var_id = q.top().second;
		//&& isVariableNoDelete(f, var_id)
		if (AdvancedFormula::variables_assign_ptr[var_id] == -1  && AdvancedFormula::flip_flag_ptr[var_id] == -1 ) {
			return var_id;
		}
		q.pop();
	}
	return NORMAL;
}

long long AdvancedDPLL::calculateWx(AdvancedFormula& f,const RecordChange& rc) {	
	/*long long wx = 0;
	for (auto& pair_it : rc.delete_variables) {
		int clause_id = pair_it.first;
		if (!f.isClauseDelete(clause_id) && AdvancedFormula::getCluaseLengthByClauseId(f,clause_id) == f.getMinimumClauseLength())
			wx++;
	}*
	return wx;*/
	return rc.delete_clauses.size() + rc.delete_variables.size();
}
int AdvancedDPLL::examine(AdvancedFormula& f, stack<RecordChange*>& st,int test_var,vector<pair<int,long long>>& tested_vars) {
	//for (auto it = test_vars.begin(); it != test_vars.end(); it++) {
		int& var =  test_var;
		if (!f.checkVariableIsInitialState(test_var)) return EXAMINE_NORMAL;
		long long W_x, W_nx;

		RecordChange* rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
		AdvancedDPLL::addUnitClause(f, var);//f + { x }		
		Result res_of_f1 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { x }

		W_x = AdvancedDPLL::calculateWx(f, *rc);

		if (res_of_f1 == SAT) {
			return HCOMPLETE;
		}

		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复

		delete rc;

		rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
		AdvancedDPLL::addUnitClause(f, -var);//f + { -x }		
		Result res_of_f2 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { -x }

		W_nx = AdvancedDPLL::calculateWx(f, *rc);

		if (res_of_f2 == SAT) {
			return HCOMPLETE;
		}

		if (res_of_f1 == UNSAT && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
			delete rc;
			return NORMAL;
		}

		if (res_of_f1 == UNSAT && res_of_f2 == NORMAL) {
			st.push(rc);
			f.NB_SINGLE++;
			return EXAMINE_NORMAL;
		}
		if (res_of_f1 == NORMAL && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);
			delete rc;
			rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
			AdvancedDPLL::addUnitClause(f, var);
			AdvancedDPLL::up(f, *rc, -1);
			st.push(rc);
			f.NB_SINGLE++;
			return EXAMINE_NORMAL;
		}

		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		delete rc;

		long long H_x = ((W_x * W_nx) << 10) + W_x + W_nx;

		//q.push(make_pair(H_x, var));
		tested_vars.push_back(make_pair(var, H_x));
	//}
	return EXAMINE_NORMAL;
}
int AdvancedDPLL::examine1(AdvancedFormula& f, stack<RecordChange*>& st, int test_var, vector<pair<int, long long>>& tested_vars) {
	int& var = test_var;
	if (!f.checkVariableIsInitialState(test_var)) return EXAMINE_NORMAL;
	long long W_x, W_nx;

	RecordChange* rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
	AdvancedDPLL::addUnitClause(f, var);//f + { x }		
	Result res_of_f1 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { x }

	W_x = AdvancedDPLL::calculateWx(f, *rc);

	if (res_of_f1 == SAT) {
		return HCOMPLETE;
	}

	AdvancedDPLL::deleteUnitClause(f, *rc);//恢复

	delete rc;

	rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
	AdvancedDPLL::addUnitClause(f, -var);//f + { -x }		
	Result res_of_f2 = (Result)AdvancedDPLL::up(f, *rc, true);//f + { -x }

	W_nx = AdvancedDPLL::calculateWx(f, *rc);

	if (res_of_f2 == SAT) {
		return HCOMPLETE;
	}

	if (res_of_f1 == UNSAT && res_of_f2 == UNSAT) {
		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		delete rc;
		return NORMAL;
	}

	if (res_of_f1 == UNSAT && res_of_f2 == NORMAL) {
		st.push(rc);
		f.NB_SINGLE++;
		return EXAMINE_NORMAL;
	}
	if (res_of_f1 == NORMAL && res_of_f2 == UNSAT) {
		AdvancedDPLL::deleteUnitClause(f, *rc);
		delete rc;
		rc = new RecordChange(RecordChange::add_unit_clause_in_H_flag, f.minimum_clause_idx);
		AdvancedDPLL::addUnitClause(f, var);
		AdvancedDPLL::up(f, *rc, -1);
		st.push(rc);
		f.NB_SINGLE++;
		return EXAMINE_NORMAL;
	}

	AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
	delete rc;

	long long H_x = ((W_x * W_nx) << 10) + W_x + W_nx;

	//q.push(make_pair(H_x, var));
	tested_vars.push_back(make_pair(var, H_x));
	//}
	return EXAMINE_NORMAL;
}
template<typename T>
int AdvancedDPLL::getRandomIdx(T& t) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, t.size() - 1);
	return dis(gen);
}

bool AdvancedDPLL::getRandomBoolean() {
	// 使用随机设备生成器获取随机种子
	std::random_device rd;

	// 使用 Mersenne Twister 引擎生成随机数
	std::mt19937 gen(rd());

	// 使用均匀分布来生成布尔值
	std::uniform_int_distribution<> dis(0, 1);

	// 生成随机布尔值
	return dis(gen) == 1;
}
pair<int, pair<int,int>> AdvancedDPLL::countForProp(AdvancedFormula& f,int var) {
	var = abs(var);
	int cnt = 0;//包含X或-X的二元子句的数量
	int p_cnt = 0;//子句含有变元X的数量
	int n_cnt = 0;//子句含有变元-X的数量
	for (auto& clause_id : f.getClausesByVariableId(var)) {
		if (!f.clauses[clause_id].flag) {
			if (f.clauses[clause_id].length == 2) cnt++;
			p_cnt++;
		}
	}
	for (auto& clause_id : f.getClausesByVariableId(-var)) {
		if (!f.clauses[clause_id].flag) {
			if (f.clauses[clause_id].length == 2) cnt++;
			n_cnt++;
		}
	}
	return make_pair(cnt,make_pair(p_cnt,n_cnt));
}
/**
 * @brief 根据satz的策略返回H(x)值最大的变元
 * @param f
 * @return 返回H(x)值最大的变元id
*/
int AdvancedDPLL::improveH(AdvancedFormula& f, stack<RecordChange*>& st) {
	enum PROP { P41, P31, P } flag;
	flag = P;
	const int T = 10;	
	vector<pair<int,long long>> tested_vars;
	vector<int> PROP41;
	vector<int> PROP31;
	vector<int> PROP;
	int result_prop;
	for (int i = 1; i <= f.variables_cnt; i++) {
		if (f.checkVariableIsInitialState(i)) {
			const auto& res = AdvancedDPLL::countForProp(f, i);
			if (res.first >= 4 && res.second.first >= 1 && res.second.second >= 1) {
				PROP41.push_back(i);
			}
			else {
				if (res.first >= 3 && res.second.first >= 1 && res.second.second >= 1) {
					PROP31.push_back(i);
				}
				else {
					PROP.push_back(i);
				}
			}
		}
	}

	for (const int& var : PROP41) {
		if (f.checkVariableIsInitialState(var)) {
				result_prop = examine(f, st, var, tested_vars);
				if (result_prop == NORMAL) return NORMAL;
				if (result_prop == HCOMPLETE) return HCOMPLETE;
		}
	}
	if (tested_vars.size() < T) {
		for (const int& var : PROP31) {
			if (f.checkVariableIsInitialState(var)) {
				result_prop = examine(f, st, var, tested_vars);
				if (result_prop == NORMAL) return NORMAL;
				if (result_prop == HCOMPLETE) return HCOMPLETE;
			}
		}
	}
	if (tested_vars.size() < T) {
		tested_vars.clear();
		for (const int& var : PROP) {
			if (f.checkVariableIsInitialState(var)) {
				result_prop = examine(f, st, var, tested_vars);
				if (result_prop == NORMAL) return NORMAL;
				if (result_prop == HCOMPLETE) return HCOMPLETE;
			}
		}
	}
	if (tested_vars.size() == 0) return NORMAL;
	int select_var = NORMAL;
	long long max_H = LLONG_MIN;
	for (const auto& item : tested_vars) {
		if (f.checkVariableIsInitialState(item.first)) {
			if (item.second > max_H) {
				max_H = item.second;
				select_var = item.first;
			}
			if (item.second == max_H && f.NB_UP_DETECT%7==0 && AdvancedDPLL::getRandomBoolean()) {
				f.NB_SHUFFLE_ING++;
				select_var = item.first;
			}
		}
	}
	return select_var;
}

/**
 * @brief 在算例的末尾添加一个包含var_id变元的单子句
 * @param f 算例
 * @param var_id 要添加为单子句的变元，x传x，-x传-x
*/
void AdvancedDPLL::addUnitClause(AdvancedFormula& f, int var_id) {
	f.current_clauses_cnt++;
	variable var;
	var.variable_id = var_id;
	clause c;
	c.length = 1;
	c.variables.push_back(var);
	f.clauses.push_back(c);
	f.variableId_to_Clauses[var_id].push_back(f.clauses.size() - 1);
	f.unit_clauses.push_back(f.clauses.size() - 1);
	f.minimum_clause_idx = (int)f.clauses.size() - 1;
}

/**
 * @brief 删除添加的单子句
 * @param f 算例
 * @param rc RecordChange对象
*/
void AdvancedDPLL::deleteUnitClause(AdvancedFormula& f, RecordChange& rc) {
	AdvancedDPLL::upBackTrackingHelpFun(f, rc);
	f.current_clauses_cnt--;
	int var_id = f.clauses.rbegin()->variables.begin()->variable_id;
	if (abs(var_id) != rc.assign_var_id[0]) cout << "delete unit clause error" << endl;
	int clause_id = f.clauses.size() - 1;
	if (*(f.variableId_to_Clauses[var_id].rbegin()) != clause_id) cout << "not equal error" << endl;
	f.clauses.pop_back();
	f.variableId_to_Clauses[var_id].pop_back();
	for (int i = f.unit_clauses.size() - 1; i >= 0; i--) {
		if (f.unit_clauses[i] == clause_id) {
			f.unit_clauses[i] = f.unit_clauses.back();
			f.unit_clauses.pop_back();
		}
	}
	AdvancedFormula::flip_flag_ptr[abs(var_id)] = -1;
	AdvancedFormula::variables_assign_ptr[abs(var_id)] = -1;
	//f.printFormula();
	//f.setMinimunClauseIdx(f.findMinimunClauseIdx());
}

void AdvancedDPLL::upBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc) {
	//恢复被删除的子句
	for (const auto& clause_id : rc.delete_clauses) {
		f.clauses[clause_id].flag = false;
		f.current_clauses_cnt++;
	}
	for (const auto& pair_item : rc.delete_variables) {
		for (auto& var : f.clauses[pair_item.first].variables) {
			if (var.variable_id == pair_item.second) {
				var.flag = false;
				f.clauses[pair_item.first].length++;
				break;
			}
		}
	}
	for (auto& clause_id : rc.unit_clauses_remove) {
		f.unit_clauses.push_back(clause_id);
	}
	for (auto& clause_id : rc.unit_clauses_insert) {
		for (int i = f.unit_clauses.size() - 1; i >= 0; i--) {
			if (f.unit_clauses[i] == clause_id) {
				f.unit_clauses[i] = f.unit_clauses.back();
				f.unit_clauses.pop_back();
			}
		
		}
	}
	for (auto& var_id : rc.assign_var_id) {
		if (AdvancedFormula::flip_flag_ptr[var_id] != 0) {
			cout << "upback flip_flag_ptr error" << endl;
			cout << "var_id:" << var_id << " flip_value:" << AdvancedFormula::flip_flag_ptr[var_id] << endl;
		}
		AdvancedFormula::variables_assign_ptr[var_id] = -1;
		AdvancedFormula::flip_flag_ptr[var_id] = -1;
		//f.current_variables_cnt++;
	}
	f.minimum_clause_idx = rc.pre_minimum_clause_idx;
}

int AdvancedDPLL::momsBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc) {	
	if (rc.assign_var_id.size() != 1) {
		cout << "assign size not one" << endl;
	}
	f.minimum_clause_idx = rc.pre_minimum_clause_idx;
	if (rc.assign_var_id.empty()) return -1;	
	for (const auto& clause_id : rc.delete_clauses) {
		f.clauses[clause_id].flag = false;
		f.current_clauses_cnt++;
	}
	for (const auto& pair_item : rc.delete_variables) {
		for (auto& var : f.clauses[pair_item.first].variables) {
			if (var.variable_id == pair_item.second) {
				var.flag = false;
				f.clauses[pair_item.first].length++;
				break;
			}
		}
	}
	for (auto& clause_id : rc.unit_clauses_remove) {
		f.unit_clauses.push_back(clause_id);
	}
	for (auto& clause_id : rc.unit_clauses_insert) {
		for (int i = f.unit_clauses.size() - 1; i >= 0; i--) {
			if (f.unit_clauses[i] == clause_id) {
				f.unit_clauses[i] = f.unit_clauses.back();
				f.unit_clauses.pop_back();
			}
		}
	}	
	int var_id = *rc.assign_var_id.begin();
	int flag = AdvancedFormula::flip_flag_ptr[var_id];
	int flip_var_id = -1;
	switch (flag)
	{
	case 0: {
		flip_var_id = var_id;
		break;
		}		
	case 1: {
		AdvancedFormula::flip_flag_ptr[var_id] = -1;
		AdvancedFormula::variables_assign_ptr[var_id] = -1;
		//f.current_variables_cnt++;
		break;
		}
	}
	return flip_var_id;
}

int AdvancedDPLL::upFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s) {
	while (!s.empty()) {
		RecordChange*& rc = s.top();
		s.pop();
		switch (rc->flag)
		{
			case RecordChange::up_flag: {
				AdvancedDPLL::upBackTrackingHelpFun(f, *rc);
				break;
			}
			case RecordChange::moms_flag: {
				int var_id = AdvancedDPLL::momsBackTrackingHelpFun(f, *rc);
				if (var_id != -1) return var_id;
				break;
			}
			case RecordChange::add_unit_clause_flag: {
				AdvancedDPLL::deleteUnitClause(f, *rc);
				break;
			}
			case RecordChange::add_unit_clause_in_H_flag: {
				f.NB_SINGLE--;
				AdvancedDPLL::deleteUnitClause(f, *rc);
				break;
			}
		}
		delete rc;
	}	
	return NORMAL;
}
int AdvancedDPLL::momsFailBackTracking(AdvancedFormula& f, stack<RecordChange*>& s) {
	while (!s.empty()) {
		RecordChange*& rc = s.top();
		s.pop();
		switch (rc->flag)
		{
			case RecordChange::moms_flag: {
				int var_id = AdvancedDPLL::momsBackTrackingHelpFun(f, *rc);
				if (var_id != -1) return var_id;
				break;
			}
			case RecordChange::up_flag: {
				AdvancedDPLL::upBackTrackingHelpFun(f, *rc);
				break;
			}
			case RecordChange::add_unit_clause_flag: {
				AdvancedDPLL::deleteUnitClause(f, *rc);
				break;
			}
			case RecordChange::add_unit_clause_in_H_flag: {
				f.NB_SINGLE--;
				AdvancedDPLL::deleteUnitClause(f, *rc);
				break;
			}
		}
		delete rc;
	}	
	return NORMAL;
}

int AdvancedDPLL::backTracking(AdvancedFormula& f, stack<RecordChange*>& s, int flag) {
	f.NB_BACK++;
	switch (flag)
	{
		case RecordChange::up_flag:
			return upFailBackTracking(f, s);
		case RecordChange::moms_flag:
			return momsFailBackTracking(f, s);
	}
}

int AdvancedDPLL::solverByIncrementalUpdate(AdvancedFormula& f) {
	if (AdvancedDPLL::isClausesEmpty(f)) return COMPLETE;
	AdvancedDPLL::pureLiteralSimplify(f);
	if (AdvancedDPLL::isClausesEmpty(f)) return COMPLETE;
	stack<RecordChange*> stack_rc;
	bool add_unit_clause = false; 
	enum from{one,two,three};
	from w;
	do{
		RecordChange* rc_up;
		if (add_unit_clause) {
			rc_up = new RecordChange(RecordChange::add_unit_clause_flag,f.minimum_clause_idx);
			add_unit_clause = false;
		}
		else rc_up = new RecordChange(RecordChange::up_flag,f.minimum_clause_idx);
		int result_up = AdvancedDPLL::up(f, *rc_up, false);
		stack_rc.push(rc_up);
		int var_id = -1;
		bool value = true;
		if (result_up == SAT) {
			return COMPLETE;
		}
		if (result_up == UNSAT) {
			int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::up_flag);
			if (f.getMinimumClauseLength() != AdvancedFormula::getCluaseLengthByClauseId(f,f.findMinimunClauseIdx())) {
				cout << "MinimumClauseLength error" << endl;
			}
			if (result_back == NORMAL) {
				break;
			}
			var_id = result_back;
			value = !AdvancedFormula::variables_assign_ptr[var_id];
			w = one;
		}
		if (result_up == NORMAL) {
			bool flag = true;
			//AdvancedDPLL::H(f, stack_rc);
			var_id = AdvancedDPLL::improveH(f, stack_rc);
			w = two;
			if (var_id == HCOMPLETE) {
				return COMPLETE;
			}
			else if(var_id == NORMAL){
				flag = false;
				int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::up_flag);
				if (f.getMinimumClauseLength() != AdvancedFormula::getCluaseLengthByClauseId(f, f.findMinimunClauseIdx())) {
					cout << "MinimumClauseLength error" << endl;
				}
				if (result_back == NORMAL) {
					break;
				}
				var_id = result_back;
				value = !AdvancedFormula::variables_assign_ptr[var_id];
				w = three;
			}
			if (flag) {
				value = var_id > 0 ? true : false;
				f.NB_BRANCHE++;
			}				
		}		
		RecordChange* rc_moms = new RecordChange(RecordChange::moms_flag,f.minimum_clause_idx);
		int result_apply_variable = AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, *rc_moms);
		stack_rc.push(rc_moms);
		if (result_apply_variable == SAT) {
			return COMPLETE;
		}
		if (result_apply_variable == UNSAT) {
			int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::moms_flag);
			if (f.getMinimumClauseLength() != AdvancedFormula::getCluaseLengthByClauseId(f, f.findMinimunClauseIdx())) {
				cout << "MinimumClauseLength error" << endl;
			}
			if (result_back == NORMAL) {
				break;
			}
			int variable_id = AdvancedFormula::variables_assign_ptr[result_back] == 1 ? 0 - result_back : result_back;
			AdvancedDPLL::addUnitClause(f, variable_id);
			add_unit_clause = true;
		}
	} while (!stack_rc.empty());
	return NORMAL;
}

string AdvancedDPLL::verifyResult(AdvancedFormula& verify_f) {
	for (int i = 1; i <= verify_f.variables_cnt; i++) {
		int v = AdvancedFormula::variables_assign_ptr[i];
		//cout << v << " ";
		if (v==0 || v==1) {
			int result = AdvancedDPLL::applyVariableAssign(verify_f, i, bool(v));
			switch (result)
			{
			case SAT:
				cout << "\nverify result is SAT!" << endl;
				return "SAT";
			case UNSAT:
				cout << "\nverify result is ERROR!" << endl;
				return "ERROR";
			}
		}
		else {
			if (v != -1) {
				cout << "\n变元赋值出现错误！" << endl;
				return "变元赋值出现错误";
			}
		}
	}
	cout << "\nverify result is NORMAL!" << endl;
	return "NORMAL";
}


string AdvancedDPLL::showResult(int result) {
	if (result == COMPLETE) {
		cout << "SAT\n";
		return "SAT";
	}
	if (result == NORMAL) {
		cout << "UNSAT\n";
		return "UNSAT";
	}
}