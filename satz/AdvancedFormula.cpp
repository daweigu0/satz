#include<iostream>
#include<vector>
#include<unordered_map>
#include"AdvancedFormula.h"
#include"vtoc.h"
//Vtoc variableId_to_Clauses;
int* AdvancedFormula::variables_assign_ptr = nullptr;
int* AdvancedFormula::flip_flag_ptr = nullptr;

AdvancedFormula::AdvancedFormula(const AdvancedFormula& other) {
	this->clauses = other.clauses;
	this->current_clauses_cnt = other.current_clauses_cnt;
	this->unit_clauses = other.unit_clauses;
	this->variables_cnt = other.variables_cnt;
	this->minimum_clause_idx = other.minimum_clause_idx;
	this->variableId_to_Clauses = other.variableId_to_Clauses;
	//this->current_variables_cnt = other.current_variables_cnt;
}
AdvancedFormula::~AdvancedFormula() {
	this->clauses.clear();
	this->unit_clauses.clear();
}
void AdvancedFormula::setCurrentClausesCnt(int cnt)
{
	this->current_clauses_cnt = cnt;
}
void AdvancedFormula::printFormula() {
	using namespace std;
	for (int i = 1; i < this->clauses.size(); i++) {
		if (!this->clauses[i].flag) {
			cout << "clause_id: " << i << " length:" << this->clauses[i].length << " variables: ";
			for (int j = 0; j < this->clauses[i].variables.size(); j++) {
				if (!this->clauses[i].variables[j].flag)
					cout << this->clauses[i].variables[j].variable_id << " ";
			}
			cout << endl;
		}
	}
	cout << "unit_clauses: ";
	for (auto& item : this->unit_clauses) {
		cout << item << " ";
	}
	cout << endl;
	//variableId_to_Clauses.pirntVtoc();
	for (int i = 1; i < this->variableId_to_Clauses.size(); i++) {
		//cout << "i: " << i << endl;
		//!variableId_to_Clauses[i].empty()
		if (!this->variableId_to_Clauses.at(i).empty()) {
			int var_id = Vtoc::idxToVariableId(i);
			if (AdvancedFormula::variables_assign_ptr[abs(var_id)] == -1) {
				cout << "variable_id: " << var_id << " clause_id:";
				for (auto& clause_id : this->variableId_to_Clauses.at(i)) {
					if (!this->clauses[clause_id].flag)
						cout << clause_id << " L:" << this->clauses[clause_id].length << " ";
				}
				cout << endl;
			}			
		}
	}
	cout << "variables_assign: " << endl;
	for (int i = 1; i < this->variables_cnt + 1; i++) {
		cout << "var_id: " << i << " value: " << (int)this->variables_assign_ptr[i] << endl;
	}
	cout << "flip_flag: " << endl;
	for (int i = 1; i < this->variables_cnt + 1; i++) {
		cout << "var_id: " << i << " flip_flag: " << (int)this->flip_flag_ptr[i] << endl;
	}
	cout << "����Ӿ��±꣺" << this->minimum_clause_idx << " " << "���ȣ�" << this->clauses[this->minimum_clause_idx].length << endl;
	int idx = this->findMinimunClauseIdx();
	cout << "����Ӿ��±꣺" << idx << " " << "���ȣ�" << this->clauses[idx].length << endl;
	cout << "============printFormula����============" << endl;
}

void AdvancedFormula::setVariablesCnt(int cnt) {
	this->variables_cnt = cnt;
	//this->current_variables_cnt = cnt;
}

void AdvancedFormula::initVariablesAssignAndFlipFlag(int variables_cnt) {
	int n = variables_cnt + 1;
	this->variables_assign_ptr = new int[n];
	this->flip_flag_ptr = new int[n];
	fill(this->variables_assign_ptr, this->variables_assign_ptr + n, -1);
	fill(this->flip_flag_ptr, this->flip_flag_ptr + n, -1);	
}

/**
 * @brief ����minimum_clause_idxΪ����Ӿ���±�
 * @param idx ����Ӿ���±�
*/
void AdvancedFormula::setMinimunClauseIdx(int idx) {
	this->minimum_clause_idx = idx;
}

/**
 * @brief �ҵ�������δ��ɾ��������Ӿ���±�
 * @return ����Ӿ���±�
*/
int AdvancedFormula::findMinimunClauseIdx() {
	int idx = 0;
	for (int i = 1; i < this->clauses.size(); i++) {
		if (!isClauseDelete(i)) {
			if(this->clauses[i].length<this->clauses[idx].length)
				idx = i;
		}
	}
	if (idx == 0) {
		cout << "findMinimunClauseIdx error" << endl;
		cout << "�������е��Ӿ��Ѿ���ɾ����" << endl;
	}
	return idx;
}

/**
 * @brief �������е��Ӿ䷢���䶯ʱ��Ҫ����minimum_clause_idx��ֵ������������Ӿ���±�
 * @param change_length_clause_id �����ı���Ӿ���±�
 * @param current_length �ı�֮����Ӿ䳤��
*/
void AdvancedFormula::updateMinimunClauseIdx(int change_length_clause_id, int current_length) {
	if (change_length_clause_id == this->minimum_clause_idx) {
		if (current_length == 0)
			this->minimum_clause_idx = this->findMinimunClauseIdx();
	}
	else {
		if (current_length != 0&&current_length<this->clauses[this->minimum_clause_idx].length) {
			this->minimum_clause_idx = change_length_clause_id;
		}
	}
}

/**
 * @brief ��������������Ӿ�ĳ���
 * @return ����Ӿ�ĳ���
*/
int AdvancedFormula::getMinimumClauseLength() {
	if (isClauseDelete(this->minimum_clause_idx)) {
		cout << "minimum_clause_idx����Ӧ���Ӿ��Ѿ���ɾ��\n";
	}
	return this->clauses[this->minimum_clause_idx].length;
}

/**
 * @brief �ж�clause_id�Ӿ��Ƿ��Ѿ���ɾ��
 * @param clause_id 
 * @return trueΪ�Ѿ���ɾ��
*/
bool AdvancedFormula::isClauseDelete(int clause_id) {
	return this->clauses[clause_id].flag;
}
/**
 * @brief ����cluase_id�����Ӿ�ĳ���
 * @param f 
 * @param cluase_id 
 * @return cluase_id�Ӿ�ĳ���
*/
int AdvancedFormula::getCluaseLengthByClauseId(AdvancedFormula& f, int clause_id) {
	if (f.isClauseDelete(clause_id)) cout << "cluase_id�Ӿ��Ѿ���ɾ��\n";
	return f.clauses[clause_id].length;
}


int AdvancedFormula::getVariableAssignByVariableId(const int var_id){
	return AdvancedFormula::variables_assign_ptr[var_id];
}

/**
 * @brief ����variableId_to_Clauses�Ĵ�С
 * @param size �����б�Ԫ�ĸ���
*/
void AdvancedFormula::setVtocSize(int size) {
	this->variableId_to_Clauses.setVtocSize(size);
}

/**
 * @brief ����variable_id����������ȫ���Ӿ���±�
 * @param variable_id ��Ԫid
 * @return vector<int>& ȫ���Ӿ���±�
 */
vector<int>& AdvancedFormula::getClausesByVariableId(int variable_id) {
	return this->variableId_to_Clauses[variable_id];
}