/*根据产生式求first集和follow集，将对不同的非终极符号求这两个集合写成两个方法
然后根据输入的文法G，针对每一个产生式进行上面的计算，然后求得的两个集合放到我们建立的预测分析表中，最后输出这张预测分析表

这个部分的程序的逻辑应该是遍历文法，对每个产生式进行处理，根据产生式的不同有的求FIRST，有的求FOLLOW
当产生式处理完时，整个填表的过程结束，其他的空都是ERROR
预测分析表【LL(1)分析表】的构造


算法：

对于G中的每一个产生式， A -> α ,执行以下2步：

1.for  ∀ a ∈ FIRST(α)， 将 A -> α 填入 M [A, a ];

1. if（ε ∈ FIRST(α)）

         ∀ a ∈ FOLLOW (A) ， 将 A -> ε 填入 M [A, a ];

PS: 这里的 a 包括 #
例子：
文法：
E -> TE'
E' -> +TE' | ε
T -> FT'
T' -> *FT' | ε
F -> (E) | id

解析：
1.E -> TE'
FIRST(TE')={(,id}     所以 E -> TE'   填入  M[E, id] 和 M[ E, ( ]
步骤2不满足

2.E' -> +TE'
FIRST(+TE')={+}     所以 E' -> +TE'  填入  M[E', +]
步骤2不满足

3.E' -> ε
FOLLOW(E')={ ), # }   所以 E' -> ε 填入 M[E', )]   和 M[E', #]
步骤1不满足

4.T -> FT'
FIRST(FT')={(,id}    所以T -> FT' 填入 M[T, (]    和 M[T, id]
步骤2不满足

5.T' -> *FT'
FIRST(*FT')={*}     所以 T' -> *FT' 填入 M[T' , * ]
步骤2不满足

6. T' -> ε
FOLLOW(T')={ +, ), # }   所以 T' -> ε  填入M[T', +]、M[T', ）] 和M[T', #]
步骤1不满足

7.F -> (E)
FIRST((E))={(}   所以F -> (E) 填入  M[F, (]
步骤2不满足

8.F ->  id
FIRST(id)={id}   所以 F ->  id 填入 M[F,   id]
步骤2不满足




问题在于对每个非终结符我们是不是要保存一个数据结构：
typedef struct Nonter{
        elementType data;
        set                  first;
        set                  follow;
}
每个非终结符的first集和follow集，重点在于每次求这个需要遍历与该非终极符相关的所有的产生式，但是我们构造表时是一个一个的产生式来处理的
时间复杂度貌似为N^N,是不是应该有更好的实现方法
想起老师上课讲的这一部分了，貌似没有什么更好的办法，只能从上到下一遍遍的扫描，直到follow集不再增大为止
http://blog.csdn.net/nk_test/article/details/51476663

First
如A->aB | CD
这里面包含了组成First（A）的两种情况：
           以终结符开头，    当然要把这个终结符放到A的First里
           以非终结符开头， 先把C的First放到A的First里
                                        再看如果C的First中有空的话就把D的First放到A的First里，如果D也有空的话往后依次类推
技巧：First一般从下往上找。
           如果要找A的First，我们要找A的定义式，即A在左边的式子，看着他的右边来找。

Follow
如S->(L) | aL | LC
找Follow的三种情况：先在候选式（右边）中找到该非终结符，如L（注意例中只有一个定义，但找Follow要看到所有右边出现该非终结符的）
           如果L的右边是终结符，    那么这个终结符加入L的Follow
           如果L的右边是非终结符， 那么把这个非终结符的First除去空加到L的Follow中
           如果L处在末尾，               那么，'->'左边符号的Follow成为L的Follow
另外要注意的是：
           开始符号的Follow中要加上‘#’
技巧：Follow一般从上往下找。
           如果要找L的Follow，要从式子的右边找到L，然后来找L的Follow，这与First是不同的。

*/

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <cctype>
#include <map>
#include <set>
#define MAX 507

using namespace std;

class WF
{
    public:
        string left;
        set<string> right;
        WF ( const string& temp )
        {
            left = temp;
            right.clear();
        }
        void print ( )
        {
            printf ( "%s::=" , left.c_str() );
            set<string>::iterator it = right.begin();
            printf ( "%s" , it->c_str());
            it++;
            for ( ; it!= right.end() ; it++ )
                printf ( "|%s" , it->c_str() );
            puts("");
        }
        void insert ( const string& temp )
        {
            right.insert(temp);
        }
};

map<string,int> VN_dic;
vector<WF> VN_set;
string start;
bool used[MAX];

//消除间接左递归
void remove1 ( )
{
    for ( int i = 0 ; i < VN_set.size() ; i++ )
        for ( int j = 0 ; j < i ; j++ )
        {
            vector<string> cont;
            set<string>& right1 = VN_set[i].right;
            set<string>& right2 = VN_set[j].right;
            char ch = VN_set[j].left[0];
            set<string>::iterator it1 = right1.begin();
            set<string>::iterator it2;
            for ( ; it1 != right1.end() ; it1++ )
                if ( it1->at(0) == ch )
                    for ( it2 = right2.begin() ; it2 != right2.end() ; it2++ )
                        cont.push_back ( *it2 + it1->substr(1) );
            int nn = right1.size();
            while ( nn-- )
            {
                if ( right1.begin()->at(0) == ch )
                    right1.erase ( right1.begin() );
                else
                {
                    cont.push_back ( *right1.begin() );
                    right1.erase ( right1.begin() );
                }
            }
            for ( int i = 0 ; i < cont.size() ; i++ )
                right1.insert ( cont[i] );
        }
#define DEBUG
#ifdef DEBUG
    for ( int i = 0 ; i < VN_set.size() ; i++ )
        VN_set[i].print();
#endif
}

//消除直接左递归
void remove2 ( )
{
    for ( int i = 0 ; i < VN_set.size() ; i++ )
    {
        char ch = VN_set[i].left[0];
        set<string>& temp = VN_set[i].right;
        set<string>::iterator it = temp.begin();
        string tt = VN_set[i].left.substr(0,1)+"\'";
        bool flag = true;
        for ( ; it != temp.end() ; it++ )
            if ( it->at(0) == ch )
            {
                VN_set.push_back ( WF(tt));
                VN_dic[tt] = VN_set.size();
                flag = false;
                break;
            }
        int x = VN_dic[tt]-1;
        if ( flag ) continue;
        vector<string> cont;
        set<string>& ss = VN_set[x].right;
        ss.insert ( "~" );
        while (!temp.empty())
        {
            if ( temp.begin()->at(0) == ch )
                ss.insert(temp.begin()->substr(1)+tt);
            else
            {
                //cout << "YES : " << temp.begin()->substr(1)+tt;
                cont.push_back (temp.begin()->substr(0)+tt);
            }
            temp.erase(temp.begin());
        }
        puts ("");
        for ( int i = 0 ; i < cont.size() ; i++ )
        {
            //cout << cont[i] << endl;
            temp.insert ( cont[i] );
        }
    }
#define DEBUG
#ifdef DEBUG
    for ( int i = 0 ; i < VN_set.size() ; i++ )
        VN_set[i].print();
#endif
}

void dfs ( int x )
{
    if ( used[x] ) return;
    used[x] = 1;
    set<string>::iterator it = VN_set[x].right.begin();
    for ( ; it != VN_set[x].right.end(); it++ )
        for ( int i = 0 ; i < it->length() ; i++ )
            if ( isupper(it->at(i)) )
            {
                if ( it->length() > i+1 && it->at(i+1) == '\'' )
                    dfs ( VN_dic[it->substr(i,2)]-1 );
                else
                    dfs ( VN_dic[it->substr(i,1)]-1 );
            }
}

//化简
void simplify ( )
{
    memset ( used , 0 , sizeof ( used ) );
    int x = VN_dic[start]-1;
    dfs ( x );
    puts ( "finish" );
    vector<WF> res;
    for ( int i = 0 ; i < VN_set.size() ; i++ )
        if ( used[i] )
            res.push_back ( VN_set[i] );
    VN_set.clear();
    VN_set = res;
}

void  print ()
{
    puts("**********消除左递归后的结果********");
    for ( int i = 0 ; i < VN_set.size() ; i++ )
        VN_set[i].print();
    puts("");
}

int main ( )
{
    char buf[MAX];
    int n;
    VN_dic.clear();
    VN_set.clear();
    start="S";
    puts ("请输入文法G[S]的产生式数量");
    while ( ~scanf ("%d" , &n ) )
    {
        scanf ( "%d" , &n );
        while ( n-- )
        {
            scanf ( "%s" , buf );
            int len = strlen ( buf ),i;
            for ( i = 0 ; i < len ; i++ )
                if ( buf[i] == ':' )
                {
                    buf[i] = 0;
                    break;
                }
            string temp = buf;
            if ( !VN_dic[temp] )
            {
                VN_set.push_back ( WF(temp));
                VN_dic[temp] = VN_set.size();
            }
            int x = VN_dic[temp]-1;
            temp = buf+i+3;
            //cout <<"the right :  " << temp << endl;
            VN_set[x].insert(temp);
        }
        remove1();
        remove2();
        simplify();
        print();
        //puts ("请输入文法G[S]的产生式数量");
    }
}
