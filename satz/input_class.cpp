#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include"AdvancedFormula.h"
#include"vtoc.h"
using namespace std;
extern Vtoc variableId_to_Clauses;
class Input
{
private:
	ifstream ifs;
	string filename;//�ļ�����·����
	long record;//��¼��һ�ж����˶��ٸ��ֽڣ����ã���ǰ�����ļ�ָ��
	string comments;//CNF��ע��˵��
	int bool_variable_cnt;//������Ԫ������
	int clause_cnt;//�Ӿ������
	streampos currentPosition;
	void readComments() {
		//cout << "readComments����" << endl;
		string result = "";
		string buffer;
		while (getline(this->ifs, buffer)) {
			//cout << buffer << endl;
			if (buffer[0] != 'c') {
				this->readOverallInfoOfCNF(buffer);
				break;
			}
			this->currentPosition = ifs.tellg();
			result += buffer + "\n";
		}
		this->comments = result;
	}

	void readOverallInfoOfCNF(string result) {
		this->parseVarAndClauseCnt(result);
	}

	void parseVarAndClauseCnt(string s) {
//		cout << s << endl;
		if(s.size()<6) throw "parseVarAndClauseCnt�����������ַ�������sΪ��" + s;
		s = s.substr(6, s.size());
//		cout << s;
		int space_idx = 0;
		while (s[space_idx] != ' ') space_idx++;
		this->bool_variable_cnt = stoi(s.substr(0, space_idx));
		this->clause_cnt = stoi(s.substr(space_idx + 1, s.size()));
	}

public:
	Input(string filename) {
		this->record = 0;
		this->filename = filename;
		this->ifs.open(this->filename, ios::in);
		if (!ifs.is_open()) {
			cout << "����ȷ�����ļ��Ĵ洢·��" << endl;
			throw "����ȷ�����ļ��Ĵ洢·��";
		}
		this->readComments();
	}
	string getComments() {
		return this->comments;
	}
	int getBoolVarCnt() {
		return this->bool_variable_cnt;
	}
	int getClauseCnt() {
		return this->clause_cnt;
	}
	template<typename T>
	void readClauses(T& f) {
		//cout << "readClauses����" << endl;
		Vtoc& m = variableId_to_Clauses;
		vector<clause>& clauses = f.clauses;
		string buffer;
		clauses.push_back(clause{});
		int idx = 0;
		while (getline(this->ifs, buffer)) {
//			cout << "buffer:" <<buffer << endl;
			stringstream ss(buffer);
			clause c;
			int temp_num;
			idx++;
			while (ss >> temp_num) {
				if(temp_num!=0){
					variable var;
					var.variable_id = temp_num;
					c.variables.push_back(var);
					m.insert(temp_num, idx);
				}
			}
			if(c.variables.empty()) continue;
			c.length = c.variables.size();
			if (c.length == 1) {
				f.unit_clauses.push_back(idx);
			}
			clauses.push_back(c);
		}
	}
};
