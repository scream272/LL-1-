#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iomanip>
#include <map>
#include <cmath>
#include <stack>
using namespace std;

struct node
{
	char left;
	string right;
};

class Base
{
protected:
	int T;
	node analy_str[100]; //输入文法解析

	set<char> first_set[100];//first集
	set<char> follow_set[100];//follow集
	vector<char> ter_copy; //去$终结符
	vector<char> ter_colt;//终结符
	vector<char> non_colt;//非终结符

public:
	Base() :T(0){}
	bool isNotSymbol(char c);
	int get_index(char target);//获得在终结符集合中的下标
	int get_nindex(char target);//获得在非终结符集合中的下标
	void get_first(char target); //得到first集合
	void get_follow(char target);//得到follow集合
	void inputAndSolve(); //处理得到first和follow
	void display(); //显示

};

class TableStack :public Base
{
protected:
	vector<char> to_any; //分析栈
	vector<char> left_any;//剩余输入串
	int tableMap[100][100];//预测表
public:
	TableStack(){ memset(tableMap, -1, sizeof(tableMap)); }

	void get_table(); //得到预测表
	void analyExp(string s); //分析栈的处理
	void print_out();//输出
	void getAns(); //结合处理
};

bool Base::isNotSymbol(char c)
{
	if (c >= 'A' && c <= 'Z')
		return true;
	return false;
}
int Base::get_index(char target)
{
	for (int i = 0; i<non_colt.size(); i++)
	{
		if (target == non_colt[i])
			return i;
	}
	return -1;
}
int Base::get_nindex(char target)
{
	for (int i = 0; i<ter_copy.size(); i++)
	{
		if (target == ter_copy[i])
			return i;
	}
	return -1;
}
void Base::get_first(char target)
{
	int tag = 0;
	int flag = 0;
	for (int i = 0; i<T; i++)
	{
		if (analy_str[i].left == target)//匹配产生式左部
		{
			if (!isNotSymbol(analy_str[i].right[0]))//对于终结符，直接加入first
			{
				first_set[get_index(target)].insert(analy_str[i].right[0]);
			}
			else
			{
				for (int j = 0; j<analy_str[i].right.length(); j++)
				{
					if (!isNotSymbol(analy_str[i].right[j]))//终结符结束
					{
						first_set[get_index(target)].insert(analy_str[i].right[j]);
						break;
					}
					get_first(analy_str[i].right[j]);//递归
					//	cout<<"curr :"<<analy_str[i].right[j];
					set<char>::iterator it;
					for (it = first_set[get_index(analy_str[i].right[j])].begin(); it != first_set[get_index(analy_str[i].right[j])].end(); it++)
					{
						if (*it == '$')
							flag = 1;
						else
							first_set[get_index(target)].insert(*it);//将FIRST(Y)中的非$就加入FIRST(X)
					}
					if (flag == 0)
						break;
					else
					{
						tag += flag;
						flag = 0;
					}
				}
				if (tag == analy_str[i].right.length())//所有右部first(Y)都有$,将$加入FIRST(X)中
					first_set[get_index(target)].insert('$');
			}
		}
	}

}
void Base::get_follow(char target)
{
	for (int i = 0; i<T; i++)
	{
		int index = -1;
		int len = analy_str[i].right.length();
		for (int j = 0; j<len; j++)
		{
			if (analy_str[i].right[j] == target)
			{
				index = j;
				break;
			}
		}
		if (index != -1 && index < len - 1)
		{
			char nxt = analy_str[i].right[index + 1];
			if (!isNotSymbol(nxt))
			{
				follow_set[get_index(target)].insert(nxt);
			}
			else
			{
				int isExt = 0;
				set<char>::iterator it;
				for (it = first_set[get_index(nxt)].begin(); it != first_set[get_index(nxt)].end(); it++)
				{
					if (*it == '$')
						isExt = 1;
					else
						follow_set[get_index(target)].insert(*it);
				}

				if (isExt && analy_str[i].left != target)
				{
					get_follow(analy_str[i].left);
					set<char>::iterator it;
					char tmp = analy_str[i].left;
					for (it = follow_set[get_index(tmp)].begin(); it != follow_set[get_index(tmp)].end(); it++)
						follow_set[get_index(target)].insert(*it);
				}
			}
		}

		else if (index != -1 && index == len - 1 && target != analy_str[i].left)
		{
			get_follow(analy_str[i].left);
			set<char>::iterator it;
			char tmp = analy_str[i].left;
			for (it = follow_set[get_index(tmp)].begin(); it != follow_set[get_index(tmp)].end(); it++)
				follow_set[get_index(target)].insert(*it);
		}

	}
}
void Base::inputAndSolve()
{
	string s;
	cout << "输入的产生式的个数：" << endl;
	cin >> T;
	for (int index = 0; index<T; index++)
	{
		cin >> s;
		string temp = "";
		for (int i = 0; i<s.length(); i++)
		{
			if (s[i] != ' ')
				temp += s[i];
		}
		analy_str[index].left = temp[0];
		for (int i = 3; i<temp.length(); i++)
			analy_str[index].right += temp[i];


		for (int i = 0; i<temp.length(); i++)
		{
			if (temp[i] != '-' && temp[i] != '>')
			{
				if (isNotSymbol(temp[i]))
				{
					int flag = 0;
					for (int j = 0; j<non_colt.size(); j++)
					{
						if (non_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					if (!flag)
						non_colt.push_back(temp[i]);

				}
				else
				{
					int flag = 0;
					for (int j = 0; j<ter_colt.size(); j++)
					{
						if (ter_colt[j] == temp[i])
						{
							flag = 1;
							break;
						}
					}
					if (!flag)
						ter_colt.push_back(temp[i]);
				}
			}
		}

	}
	ter_colt.push_back('#');
	//first
	for (int i = 0; i<non_colt.size(); i++)
	{
		get_first(non_colt[i]);
	}

	for (int i = 0; i<non_colt.size(); i++)
	{
		if (i == 0)
			follow_set[0].insert('#');
		get_follow(non_colt[i]);
	}

	for (int i = 0; i<ter_colt.size(); i++)
	{
		if (ter_colt[i] != '$')
			ter_copy.push_back(ter_colt[i]);
	}
}

void Base::display()
{
	cout << "FIRST集合" << endl;
	for (int i = 0; i<non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = first_set[i].begin(); it != first_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}

	cout << "FOLLOW集合" << endl;
	for (int i = 0; i<non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		set<char>::iterator it;
		for (it = follow_set[i].begin(); it != follow_set[i].end(); it++)
			cout << *it << "  ";
		cout << endl;
	}
}

void TableStack::get_table()
{
	for (int i = 0; i<T; i++)
	{
		char tmp = analy_str[i].right[0];
		if (!isNotSymbol(tmp))
		{
			if (tmp != '$')
				tableMap[get_index(analy_str[i].left)][get_nindex(tmp)] = i;
			if (tmp == '$')
			{
				set<char>::iterator  it;
				for (it = follow_set[get_index(analy_str[i].left)].begin(); it != follow_set[get_index(analy_str[i].left)].end(); it++)
				{
					tableMap[get_index(analy_str[i].left)][get_nindex(*it)] = i;
				}
			}
		}
		else
		{
			set<char>::iterator ti;
			for (ti = first_set[get_index(tmp)].begin(); ti != first_set[get_index(tmp)].end(); ti++)
			{
				tableMap[get_index(analy_str[i].left)][get_nindex(*ti)] = i;
			}
			if (first_set[get_index(tmp)].count('$') != 0)
			{
				set<char>::iterator  it;
				for (it = follow_set[get_index(analy_str[i].left)].begin(); it != follow_set[get_index(analy_str[i].left)].end(); it++)
				{
					tableMap[get_index(analy_str[i].left)][get_nindex(*it)] = i;
				}
			}
		}
	}
}
void TableStack::analyExp(string s)
{
	for (int i = s.length() - 1; i >= 0; i--)
		left_any.push_back(s[i]);

	to_any.push_back('#');
	to_any.push_back(non_colt[0]);


	while (left_any.size()>0)
	{
		//cout<<"分析栈：";
		string outs = "";
		for (int i = 0; i<to_any.size(); i++)
			outs += to_any[i];
		cout << setw(15) << outs;

		//cout<<"剩余输入串：";
		outs = "";
		for (int i = left_any.size() - 1; i >= 0; i--)
			outs += left_any[i];
		cout << setw(15) << outs;

		char char1 = to_any[to_any.size() - 1];
		char char2 = left_any[left_any.size() - 1];
		if (char1 == char2 && char1 == '#')
		{
			cout << setw(15) << "Accepted!" << endl;
			return;
		}
		if (char1 == char2)
		{
			to_any.pop_back();
			left_any.pop_back();
			cout << setw(15) << char1 << "匹配" << endl;
		}
		else if (tableMap[get_index(char1)][get_nindex(char2)] != -1)
		{
			int tg = tableMap[get_index(char1)][get_nindex(char2)];
			to_any.pop_back();

			if (analy_str[tg].right != "$")
			{
				for (int i = analy_str[tg].right.length() - 1; i >= 0; i--)
					to_any.push_back(analy_str[tg].right[i]);
			}
			//cout<<"推导："<<analy_str[tg].right<<endl;
			cout << setw(15) << analy_str[tg].right << endl;
		}
		else
		{
			cout << setw(15) << "error!" << endl;
			return;
		}
	}
}

void TableStack::print_out()
{
	//table
	for (int i = 0; i<ter_copy.size(); i++)
	{
		cout << setw(10) << ter_copy[i];
	}
	cout << endl;
	for (int i = 0; i<non_colt.size(); i++)
	{
		cout << non_colt[i] << ": ";
		for (int j = 0; j<ter_copy.size(); j++)
		{
			if (tableMap[i][j] == -1)
				cout << setw(10) << "error";
			else
				cout << setw(10) << analy_str[tableMap[i][j]].right;

		}
		cout << endl;
	}
}

void TableStack::getAns()
{
	inputAndSolve();
	display();
	get_table();
	print_out();
	//栈匹配
	string ss;
	cout << "请输入符号串：" << endl;
	cin >> ss;
	cout << setw(15) << "分析栈" << setw(15) << "剩余输入串" << setw(15) << "推导式" << endl;
	analyExp(ss);

}

int main()
{
	TableStack res;
	res.getAns();
	return 0;
}
/*
1）求FIRST集的算法思想

如果产生式右部第一个字符为终结符，则将其计入左部first集
 如果产生式右部第一个字符为非终结符
 ->求该非终结符的first集
 ->将该非终结符的非$first集计入左部的first集
->若存在$，则将指向产生式的指针右移
->若不存在$，则停止遍历该产生式，进入下一个产生式
->若已经到达产生式的最右部的非终结符，则将$加入左部的first集
 处理数组中重复的first集中的终结符


（2）求FOLLOW集的算法思想

 对于文法G中每个非终结符A构造FOLLOW(A)的办法是,连续使用下面的规则,直到每个FOLLOW不在增大为止.
(1) 对于文法的开始符号S,置#于FOLLOW(S)中;
(2) 若A->aBb是一个产生式,则把FIRST(b)\{ε}加至FOLLOW(B)中;
(3) 若A->aB是一个产生式,或A->aBb是一个产生式而b=>ε(即ε∈FIRST(b))则把FOLLOW(A)加至FOLLOW(B)中


（3）生成预测分析表的算法思想

构造分析表M的算法是：
 (1) 对文法G的每个产生式A->a执行第二步和第三步;
 (2) 对每个终结符a∈FIRST(a),把A->a加至M[A,a]中;
 (3) 若ε∈FIRST(a),则把任何b∈FOLLOW(A)把A->a加至M[A,b]中;
 (4) 把所有无定义的M[A,a]标上出错标志.


（4）对符号串的分析过程

 预测分析程序的总控程序在任何时候都是按STACK栈顶符号X和当前的输入符号行事的,对于任何(X,a),总控程序
 每次都执行下述三种可能的动作之一;
(1) 若X=a=”#”,则宣布分析成功,停止分析过程.
(2) 若X=a≠”#”,则把X从STACK栈顶逐出,让a指向下一个输入符号.
(3) 若X是一个非终结符,则查看分析表M,若M[A,a]中存放着关于X的一个产生式,那么,首先把X逐出STACK栈顶,然后
把产生式的右部符号串按反序一一推进STACK栈(若右部符号为ε,则意味着不推什么东西进栈).在把产生式的右部
符号推进栈的同时应做这个产生式相应得语义动作,若M[A,a]中存放着”出错标志”,则调用出错诊察程序ERROR.


在我的程序中，Base类为基类，负责求出FIRST和FOLLOW集；TableStack继承Base类，求出预测分析表和显示符号串的分析过程。
*/

