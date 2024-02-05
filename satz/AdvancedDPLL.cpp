#include"AdvancedDPLL.h"
#include<stack>
#include<random>
#include"vtoc.h"
#include"RecordChange.h"
#include<algorithm>
#include <unordered_set>
#include<queue>
extern Vtoc variableId_to_Clauses;
enum Result
{
	HCOMPLETE = -2,
	NORMAL = -1,
	UNSAT = 0,
	SAT = 1,
	COMPLETE = 2,	
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
bool isVariableNoDelete(const AdvancedFormula& f, int var_id) {
	for (const auto& clause_id : variableId_to_Clauses[var_id]) {
		if (!f.clauses[clause_id].flag) {
			for (const auto& var : f.clauses[clause_id].variables) {
				if (var.variable_id == var_id && !var.flag) return true;
			}
		}		
	}
	for (const auto& clause_id : variableId_to_Clauses[-var_id]) {
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
		f.updateMinimunClauseIdx(clause_idx, res);
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
	if (AdvancedFormula::flip_flag_ptr[variable_id] > 1) cout << "apply flip error" << endl;
	AdvancedFormula::variables_assign_ptr[variable_id] = value;
	rc.assign_var_id.push_back(variable_id);
	int var_id = value ? variable_id : -variable_id;
	if (variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : variableId_to_Clauses[var_id]) {
			if(AdvancedDPLL::deleteClause(f, clause_id, rc))
				f.updateMinimunClauseIdx(clause_id, 0);
			if (AdvancedDPLL::isClausesEmpty(f)) return SAT;
		}
	}
	var_id = -var_id;
	if (variableId_to_Clauses.find(var_id) != 0) {
		for (auto& clause_id : variableId_to_Clauses[var_id]) {
			bool flag = AdvancedDPLL::deleteVariableByClauseId(f, clause_id, var_id, rc);
			if (flag) return UNSAT;
		}
	}
	return NORMAL;
}

void AdvancedDPLL::pureLiteralSimplify(AdvancedFormula& f) {
	RecordChange* rc = new RecordChange(RecordChange::moms_flag,f.minimum_clause_idx);
	for (int i = 1; i < variableId_to_Clauses.size(); i += 2) {
		if (!variableId_to_Clauses.at(i).empty() && variableId_to_Clauses.at(i + 1).empty()) {
			int var_id = Vtoc::idxToVariableId(i);
			for (auto& clause_id : variableId_to_Clauses.at(i)) {
				bool value = var_id > 0 ? true : false;
				AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, *rc);
				if (AdvancedDPLL::isClausesEmpty(f)) {
					delete rc;
					return;
				}
			}
		}
		if (variableId_to_Clauses.at(i).empty() && !variableId_to_Clauses.at(i + 1).empty()) {
			int var_id = Vtoc::idxToVariableId(i + 1);
			for (auto& clause_id : variableId_to_Clauses.at(i + 1)) {
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
}

int AdvancedDPLL::up(AdvancedFormula& f,RecordChange& rc) {
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
			if (result == SAT || result == UNSAT) return result;
		}
	} while (unit_clause_find);
	return NORMAL;
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
vector<int>* AdvancedDPLL::minimumSentenceSet(AdvancedFormula& f) {
	vector<int>* min_clauses = new vector<int>();
	int min_clause_length = f.clauses[f.minimum_clause_idx].length;
	min_clauses->push_back(min_clause_length);
	for (int i = 1; i < f.clauses.size(); i++) {
		if (!f.clauses[i].flag && f.clauses[i].length == min_clause_length) {
			(*min_clauses).push_back(i);
		}
	}
	return min_clauses;
}
//int AdvancedDPLL::H(AdvancedFormula& f) {
//	vector<int> free_variable;
//	for (int i = 1; i <= f.variables_cnt; i++) {
//		if (AdvancedFormula::variables_assign_ptr[i] == -1) {
//			free_variable.push_back(i);
//		}
//	}
//	vector<int>* Q = AdvancedDPLL::minimumSentenceSet(f);
//	sort((*Q).begin(), (*Q).end());
//	int max_H_x = INT_MIN;
//	int select_var = 0;
//	for (auto& var : free_variable) {
//		AdvancedDPLL::addUnitClause(f,var);
//		RecordChange* rc = new RecordChange(RecordChange::up_flag,f.minimum_clause_idx);
//		AdvancedDPLL::up(f, *rc);
//		vector<int>* Q1 = AdvancedDPLL::minimumSentenceSet(f);
//		AdvancedDPLL::deleteUnitClause(f,*rc);
//		delete rc;
//		AdvancedDPLL::addUnitClause(f, -var);
//		rc = new RecordChange(RecordChange::up_flag,f.minimum_clause_idx);
//		AdvancedDPLL::up(f, *rc);
//		vector<int>* Q2 = AdvancedDPLL::minimumSentenceSet(f);
//		AdvancedDPLL::deleteUnitClause(f, *rc);
//		delete rc;		
//		vector<int> ans;
//		int W_x;
//		int W_nx;
//		if ((*Q1)[0] == (*Q)[0]) {
//			sort(++(Q1->begin()), Q1->end());
//			set_difference(++(Q1->begin()), Q1->end(), ++(Q->begin()), Q->end(), back_inserter(ans));
//			W_x = ans.size();
//			ans.clear();
//		}
//		else {
//			W_x = Q1->size() - 1;
//		}
//		if ((*Q2)[0] == (*Q)[0]) {
//			sort(Q2->begin(), Q2->end());
//			set_difference(++(Q2->begin()), Q2->end(), ++(Q->begin()), Q->end(), back_inserter(ans));
//			W_nx = ans.size();
//			ans.clear();
//		}
//		else {
//			W_nx = Q2->size() - 1;
//		}
//		int H_x = W_x * W_nx * 1024 + W_x + W_nx;
//		if (H_x > max_H_x) {
//			max_H_x = H_x;
//			select_var = var;
//		}
//		delete Q1;
//		delete Q2;
//	}
//	delete Q;
//	return select_var;
//}

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

struct cmp
{
	bool operator()(pair<long long,int>& p1,pair<long long,int>& p2) {
		return p1.first > p2.first;
	}
};

/**
 * @brief 根据satz的策略返回H(x)值最大的变元
 * @param f 
 * @return 返回H(x)值最大的变元id
*/
int AdvancedDPLL::advancedH(AdvancedFormula& f, stack<RecordChange*>& st) {
	//cout << "======================advancedH函数======================" << endl;
	enum PROP{P41,P31,P} flag;
	flag = P;
	const int T = 10;
	vector<int> free_variable;
	for (int i = 1; i <= f.variables_cnt; i++) {
		if (AdvancedFormula::variables_assign_ptr[i] == -1 && AdvancedFormula::flip_flag_ptr[i] == -1 && isVariableNoDelete(f, i)) {
			free_variable.push_back(i);
		}
	}
	vector<int> PROP41;
	vector<int> PROP31;
	for (auto& var : free_variable) {
		int cnt = 0;//包含X或-X的二元子句的数量
		int p_cnt = 0;//子句含有变元X的数量
		int n_cnt = 0;//子句含有变元-X的数量
		for (auto& clause_id : variableId_to_Clauses[var]) {
			if (!f.clauses[clause_id].flag) {
				if (f.clauses[clause_id].length == 2) cnt++;
				p_cnt++;
			}
		}
		for (auto& clause_id : variableId_to_Clauses[-var]) {
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
	unordered_set<int> s;//找出f中最的短子句
	int source_f_min_clause_len = f.clauses[f.minimum_clause_idx].length;
	for (int i = 1; i < f.clauses.size(); i++) {
		if (!f.clauses[i].flag && f.clauses[i].length == source_f_min_clause_len) {
			s.insert(i);
		}
	}
	//pair<long long,int> first为H(x)的值，second为变元x的id
	priority_queue<pair<long long,int>,vector<pair<long long,int>>,cmp> q;

	/*long long max_H_x = LLONG_MIN;
	int select_var = NORMAL;	*/

	int seselect_var_cnt = 0;

	for (auto& var : (*variables)) {
		if (AdvancedFormula::variables_assign_ptr[var] != -1) continue;
		//if (AdvancedFormula::variables_assign_ptr[var] != -1 || !isVariableNoDelete(f, var) || AdvancedFormula::flip_flag_ptr[var] != -1) continue;
		RecordChange* rc = new RecordChange(RecordChange::add_unit_clause_flag,f.minimum_clause_idx);		

		AdvancedDPLL::addUnitClause(f, var);//f + { x }		
		Result res_of_f1 = (Result)AdvancedDPLL::up(f, *rc);//f + { x }

		if (res_of_f1 == SAT) {
			return HCOMPLETE;
		}
		vector<int>* Q1 = AdvancedDPLL::minimumSentenceSet(f);//找出f + { x }中的最短子句
		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		delete rc;

		rc = new RecordChange(RecordChange::add_unit_clause_flag,f.minimum_clause_idx);

		AdvancedDPLL::addUnitClause(f, -var);//f + { -x }		
		Result res_of_f2 = (Result)AdvancedDPLL::up(f, *rc);//f + { -x }

		if (res_of_f2 == SAT) {
			return HCOMPLETE;
		}
		if (res_of_f1 == UNSAT && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
			delete rc;
			delete Q1;
			//AdvancedDPLL::recoveryForAdvancedH(f, st);
			return NORMAL;
		}
		if ( res_of_f1 == UNSAT && res_of_f2 == NORMAL) {
			st.push(rc);
			delete Q1;
			continue;
		}
		if (res_of_f1 == NORMAL && res_of_f2 == UNSAT) {
			AdvancedDPLL::deleteUnitClause(f, *rc);
			delete rc;
			AdvancedDPLL::addUnitClause(f, var);
			rc = new RecordChange(RecordChange::add_unit_clause_flag,f.minimum_clause_idx);
			AdvancedDPLL::up(f, *rc);
			st.push(rc);
			delete Q1;
			continue;
		}
		vector<int>* Q2 = AdvancedDPLL::minimumSentenceSet(f);//找出f + { -x }中的最短子句
		AdvancedDPLL::deleteUnitClause(f, *rc);//恢复
		delete rc;	

		long long W_x, W_nx;
		if ((*Q1)[0] == source_f_min_clause_len) {//该分支计算最短子句在f1而不在f中的总数目。			
			int cnt = 0;
			for (int i = 1; i < (Q1->size()); i++)
			{
				if(s.find((*Q1)[i])==s.end()) cnt++;
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
				if (s.find((*Q2)[i]) == s.end()) cnt++; 
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
		if (q.size() == T) break;
	}
	while (!q.empty()) {
		int var_id = q.top().second;
		//&& isVariableNoDelete(f, var_id)
		if (AdvancedFormula::variables_assign_ptr[var_id] == -1  && AdvancedFormula::flip_flag_ptr[var_id] == -1) {
			return var_id;
		}
		q.pop();
	}
	return NORMAL;
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
	variableId_to_Clauses[var_id].push_back(f.clauses.size() - 1);
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
	f.clauses.pop_back();
	variableId_to_Clauses[var_id].pop_back();
	for (int i = f.unit_clauses.size() - 1; i >= 0; i--) {
		if (f.unit_clauses[i] == clause_id) {
			f.unit_clauses[i] = f.unit_clauses.back();
			f.unit_clauses.pop_back();
		}
	}
	AdvancedFormula::flip_flag_ptr[abs(var_id)] = -1;
	AdvancedFormula::variables_assign_ptr[abs(var_id)] = -1;
	//f.minimum_clause_idx = f.findMinimunClauseIdx();
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
	}
	f.minimum_clause_idx = rc.pre_minimum_clause_idx;
}
int AdvancedDPLL::momsBackTrackingHelpFun(AdvancedFormula& f, RecordChange& rc) {	
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
	if (rc.assign_var_id.size() != 1) {
		cout << "assign size not one" << endl;
	}
	int var_id = *rc.assign_var_id.begin();
	int flag = AdvancedFormula::flip_flag_ptr[var_id];
	if (flag == 0) {
		return var_id;
	}
	if (flag == 1) {
		AdvancedFormula::flip_flag_ptr[var_id] = -1;
		AdvancedFormula::variables_assign_ptr[var_id] = -1;
	}
	return -1;
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
		}
		delete rc;
	}
	return -1;
}

int AdvancedDPLL::backTracking(AdvancedFormula& f, stack<RecordChange*>& s, int flag) {
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
		int result_up = AdvancedDPLL::up(f, *rc_up);
		stack_rc.push(rc_up);
		int var_id = -1;
		bool value = true;
		if (result_up == SAT) {
			return COMPLETE;
		}
		if (result_up == UNSAT) {
			int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::up_flag);
			if (result_back == NORMAL) {
				return NORMAL;
			}
			var_id = result_back;
			value = !AdvancedFormula::variables_assign_ptr[var_id];
			w = one;
		}
		if (result_up == NORMAL) {
			bool flag = true;
			var_id = AdvancedDPLL::advancedH(f, stack_rc);
			w = two;
			if (var_id == HCOMPLETE) {
				return COMPLETE;
			}
			else if(var_id == NORMAL){
				flag = false;
				int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::up_flag);
				if (result_back == NORMAL) {
					return NORMAL;
				}
				var_id = result_back;
				value = !AdvancedFormula::variables_assign_ptr[var_id];
				w = three;
			}
			if(flag)
				value = var_id > 0 ? true : false;
		}		
		RecordChange* rc_moms = new RecordChange(RecordChange::moms_flag,f.minimum_clause_idx);
		int result_apply_variable = AdvancedDPLL::applyVariableAssign(f, abs(var_id), value, *rc_moms);
		stack_rc.push(rc_moms);
		if (result_apply_variable == SAT) {
			return COMPLETE;
		}
		if (result_apply_variable == UNSAT) {
			int result_back = AdvancedDPLL::backTracking(f, stack_rc, RecordChange::moms_flag);
			if (result_back == NORMAL) {
				return NORMAL;
			}
			int variable_id = AdvancedFormula::variables_assign_ptr[result_back] == 1 ? 0 - result_back : result_back;
			AdvancedDPLL::addUnitClause(f, variable_id);
			add_unit_clause = true;
		}
	} while (!stack_rc.empty());
	return NORMAL;
}

void AdvancedDPLL::showResult(int result) {
	if (result == COMPLETE) cout << "SAT\n";
	if (result == NORMAL) cout << "UNSAT\n";
}