#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <algorithm> // sort函数引入
#include <cstdlib>   // system函数引入
#include <fstream>   // 文件读写
#define epsilon "ε"
using namespace std;

class Graph
{
private:
    vector<vector<int>> matrix;
    unordered_map<string, int> e;  // 除epsilon边
    vector<pair<int, int>> epTran; // epsilon边集合
    int start;                     // 初态
    int end;                       // 终态
    int VerNum;
    int EdgeNum;  // 不计epsilon的边数(重复)
    int TokenNum; // 不计epsilon的边数(不重复)
public:
    Graph()
    {
        EdgeNum = 0;
        TokenNum = -1;
        VerNum = -1;
        start = -1;
    }
    ~Graph()
    {
        e.clear();
        epTran.clear();
        matrix.clear();
        start = -1;
        end = -1;
        VerNum = -1;
        EdgeNum = 0;
        TokenNum = -1;
    }
    // in
    int addState() // 返回状态编号
    {
        vector<int> tmp;
        for (int i = 0; i <= EdgeNum; i++) // 多一个顶点多添加一行
            tmp.push_back(-1);
        matrix.push_back(tmp);
        return ++VerNum;
    }
    void addTrans(int strt, int end, string sta)
    {
        if (strt <= VerNum && end <= VerNum) // 顶点存在
        {
            if (sta != epsilon)
            {
                if (e.find(sta) == e.end()) // 如果是新边
                {
                    TokenNum++;
                    e.insert(make_pair(sta, TokenNum));
                }
                EdgeNum++;
                int tran = e[sta];                // 边的编号
                for (int i = 0; i <= VerNum; i++) // 多一条边多一列
                    matrix[i].push_back(-1);
                matrix[strt][tran] = end;
            }
            else
            {
                epTran.push_back(make_pair(strt, end));
            }
        }
        else
        {
            cout << "顶点不存在" << endl;
        }
    }
    int getVerNum()
    {
        return VerNum;
    }
    int getTokenNum() // 返回边的数量(不重复)
    {
        return TokenNum;
    }
    void setStrt(int n) // 设置第n个状态为初态
    {
        start = n;
    }
    void setEnd(int n) // 设置第n个状态为终态
    {
        end = n;
    }
    // out
    int getStrt()
    {
        return start;
    }
    int getEnd()
    {
        return end;
    }
    vector<string> getTokens() // 返回除epsilon外的边
    {
        vector<string> Token;
        unordered_map<string, int>::iterator it = e.begin();
        while (it != e.end())
        {
            Token.push_back(it->first);
            it++;
        }
        return Token;
    }
    /* 优化：将其if-else语句写成两个函数 */
    vector<int> getState(int strt, string sta) // 输入起点和边返回存在的下一状态集合
    {
        // NFA有多重转换,需要返回一个集合
        vector<int> res;
        if (sta == epsilon)
        {
            for (int i = 0; i < epTran.size(); i++)
                if (epTran[i].first == strt)
                    res.push_back(epTran[i].second);
        }
        else if (strt <= VerNum && e.find(sta) != e.end()) // 顶点存在且边也存在
        {
            int tran = e[sta];
            if (matrix[strt][tran] != -1) // 下一状态存在
                res.push_back(matrix[strt][tran]);
        }
        return res;
    }
    vector<int> getNextOfStateByEpsilon(int n) // 返回顶点编号n通过epsilon到达下一状态集合
    {
        vector<int> res;
        for (int i = 0; i < epTran.size(); i++)
        {
            if (epTran[i].first == n)
            {
                res.push_back(epTran[i].second);
            }
        }
        return res;
    }
};
struct node // 存储DFA状态传递关系
{
    // 存储dfa的起点，边，终点
    // 起点终点为子集构造集合中子集的编号
    int strt;
    string sta;
    int end;
    node(int strt, string sta, int end) : strt(strt), sta(sta), end(end) {}
    bool operator==(const node &a)
    {
        if (sta == a.sta && end == a.end)
            return true;
        return false;
    }
};
class RetoDFA
{
private:
    string re;
    vector<string> MySta; // 自定义正则表达式字符
    Graph NFA;
    vector<node> DFA;                       // 存储DFA的传递关系
    vector<pair<int, vector<int>>> subsets; // 存储子集构造集合(first字段存储是否为终态: 1为终态，-1为初态)
private:
    // input
    vector<string> Format() // 格式化处理输入返回中缀表达式
    {
        vector<pair<int, string>> infix; // first字段记录该字符是否为操作符
        vector<string> res;
        string tmp = "";
        int lenOfMySta = 0, isMySta = 0;
        int i = 0;
        // 检查【】
        // 替换自定义
        while (i < re.size())
        {
            tmp = "";
            if (re[i] == '[') // 范围自定义【】之间的字符拿走
            {
                while (re[i] != ']' && i < re.size())
                    tmp += re[i++];
                if (i >= re.size()) // 没有】
                {
                    cout << "Regex grammar【】 ERROR!" << endl;
                    exit(1);
                }
                tmp += re[i];
                infix.push_back(make_pair(0, tmp));
                i++;
            }
            else if (re[i] == '\\') // 是转义把下一个字符拿走
            {
                tmp += re[i++];
                if (i < re.size())
                    tmp += re[i];
                infix.push_back(make_pair(0, tmp));
                i++;
            }
            // 操作符
            else if (re[i] == '(' || re[i] == ')' || re[i] == '|' || re[i] == '*' || re[i] == '+' || re[i] == '?')
            {
                tmp += re[i];
                infix.push_back(make_pair(1, tmp));
                i++;
            }
            else
            {
                for (int j = 0; j < MySta.size(); j++) // 查找是否为自定义
                {
                    lenOfMySta = MySta[j].length();
                    if (lenOfMySta + i <= re.length() && re.substr(i, lenOfMySta) == MySta[j])
                    {
                        infix.push_back(make_pair(0, MySta[j]));
                        isMySta = 1;
                        break;
                    }
                }
                if (isMySta) // 是自定义字符调整指针i的位置
                {
                    i += lenOfMySta;
                    isMySta = 0;
                }
                else // 不是自定义字符直接加入infix
                {
                    tmp += re[i];
                    infix.push_back(make_pair(0, tmp));
                    i++;
                }
            }
        }
        // 添加&
        for (int i = 0; i < infix.size() - 1; i++)
        {
            res.push_back(infix[i].second);
            if (infix[i].first == 0 && (infix[i + 1].first == 0 || infix[i + 1].second == "("))
                res.push_back("&"); // 两个字符之间或字符和左括号之间
            else if (infix[i].first == 1 && infix[i].second != "|" && infix[i].second != "(" && (infix[i + 1].first == 0 || infix[i + 1].second == "("))
                res.push_back("&"); // 操作符与字符之间或操作符与左括号之间(操作符不为'|'和'(')
        }
        res.push_back(infix[infix.size() - 1].second);
        return res;
    }
    // re->nfa
    int parioty(const char ch) // 返回运算符优先级
    {
        int p;
        switch (ch)
        {
        case '*':
        case '+':
        case '?':
            p = 2;
            break;
        case '|':
            p = 0;
            break;
        case '&':
            p = 1;
            break;
        default:
            p = -1;
            break;
        }
        return p;
    }
    vector<string> toPost(const vector<string> &infix) // 正则表达式转后缀
    {
        vector<string> post;
        string tmp;
        stack<string> s;
        for (string ch : infix)
        {
            if (ch == "(")
                s.push(ch);
            else if (ch == ")")
            {
                while (s.top() != "(")
                {
                    tmp = s.top();
                    post.push_back(tmp);
                    s.pop();
                }
                s.pop();
            }
            else if (ch == "*" || ch == "&" || ch == "+" || ch == "?" || ch == "|")
            {
                while (!s.empty() && parioty(s.top()[0]) >= parioty(ch[0]))
                {
                    tmp = s.top();
                    s.pop();
                    post.push_back(tmp);
                }
                s.push(ch);
            }
            else
            {
                tmp = ch;
                post.push_back(tmp);
            }
        }
        if (!s.empty())
        {
            while (!s.empty())
            {
                tmp = s.top();
                s.pop();
                post.push_back(tmp);
            }
        }
        return post;
    }
    /* 优化：将其if-else语句写成两个函数 */
    void deal(const vector<string> &postfix) // 正则后缀表达式处理
    {
        cout << "Postfix: ";
        for (auto x : postfix)
            cout << x;
        cout << endl;
        stack<pair<int, int>> s;
        int i = 0;
        string tmp = "";
        while (i < postfix.size())
        {
            tmp = postfix[i++];
            // 运算符nfa生成
            if (tmp == "|")
            {
                pair<int, int> state1 = s.top();
                s.pop();
                pair<int, int> state2 = s.top();
                s.pop();
                int epStrt = NFA.addState(); // 头
                int epEnd = NFA.addState();  // 尾
                /**
                 *     O--->O
                 *   *        *
                 * O            O
                 *   *        *
                 *     O--->O
                 */
                NFA.addTrans(epStrt, state1.first, epsilon);
                NFA.addTrans(epStrt, state2.first, epsilon);
                NFA.addTrans(state1.second, epEnd, epsilon);
                NFA.addTrans(state2.second, epEnd, epsilon);
                NFA.setStrt(epStrt);
                NFA.setEnd(epEnd);
                s.push(make_pair(epStrt, epEnd));
            }
            else if (tmp == "&") // 连接运算符有个先后的顺序关系需要维持
            {
                // O--->O***O--->O
                pair<int, int> state1 = s.top(); // 后
                s.pop();
                pair<int, int> state2 = s.top(); // 前
                s.pop();
                NFA.addTrans(state2.second, state1.first, epsilon);
                NFA.setStrt(state2.first);
                NFA.setEnd(state1.second);
                s.push(make_pair(state2.first, state1.second));
            }
            else if (tmp == "*")
            {
                pair<int, int> state = s.top();
                s.pop();
                int epStrt = NFA.addState(); // 头
                int epEnd = NFA.addState();  // 尾
                /**
                 *      **
                 *    *    *
                 * O**O--->O**O
                 *  *        *
                 *     ****
                 */
                NFA.addTrans(epStrt, state.first, epsilon);
                NFA.addTrans(state.second, epEnd, epsilon);
                NFA.addTrans(epStrt, epEnd, epsilon);
                NFA.addTrans(state.second, state.first, epsilon);
                NFA.setStrt(epStrt);
                NFA.setEnd(epEnd);
                s.push(make_pair(epStrt, epEnd));
            }
            else if (tmp == "+")
            {
                pair<int, int> state = s.top();
                s.pop();
                int epStrt = NFA.addState(); // 头
                int epEnd = NFA.addState();  // 尾
                /**
                 *      **
                 *    *    *
                 * O**O--->O**O
                 */
                NFA.addTrans(epStrt, state.first, epsilon);
                NFA.addTrans(state.second, epEnd, epsilon);
                NFA.addTrans(state.second, state.first, epsilon);
                NFA.setStrt(epStrt);
                NFA.setEnd(epEnd);
                s.push(make_pair(epStrt, epEnd));
            }
            else if (tmp == "?")
            {
                pair<int, int> state = s.top();
                s.pop();
                int epStrt = NFA.addState(); // 头
                int epEnd = NFA.addState();  // 尾
                /**
                 * O**O--->O**O
                 *   *      *
                 *     ***
                 */
                NFA.addTrans(epStrt, state.first, epsilon);
                NFA.addTrans(state.second, epEnd, epsilon);
                NFA.addTrans(epStrt, epEnd, epsilon);
                NFA.setStrt(epStrt);
                NFA.setEnd(epEnd);
                s.push(make_pair(epStrt, epEnd));
            }
            else // 字符nfa生成
            {
                int strt = NFA.addState();
                int end = NFA.addState();
                NFA.addTrans(strt, end, tmp);
                s.push(make_pair(strt, end));
            }
        }
    }
    // nfa->dfa
    vector<int> epClosure(int n) // epsilon闭包(输入状态编号，返回状态集合)
    {
        vector<int> res; // 使用集合存储状态编号
        // 初始化辅助访问数组,避免重复访问顶点
        int *vis = new int[NFA.getVerNum() + 1];
        for (int i = 0; i <= NFA.getVerNum(); i++)
            vis[i] = 0;
        stack<int> s;
        s.push(n);
        while (!s.empty())
        {
            int curr = s.top();
            s.pop();
            if (!vis[curr]) // 访问该状态
            {
                vis[curr] = 1;
                res.push_back(curr);
            }
            vector<int> next = NFA.getNextOfStateByEpsilon(curr);
            for (int i = 0; i < next.size(); i++)
                s.push(next[i]);
        }
        sort(res.begin(), res.end());
        return res;
    }
    int isSame(const vector<int> &a, const vector<int> &b) // 判断两个状态集合是否相等
    {
        // 输入状态集合有序且元素唯一
        if (a.size() != b.size()) // 长度不等
            return 0;
        for (int i = 0; i < a.size(); i++)
            if (a[i] != b[i]) // 元素不同
                return 0;
        return 1;
    }
    int isInit(const vector<pair<int, vector<int>>> &a, const vector<int> &b) // 集合a有元素b则返回b的编号
    {
        // 输入b有序且元素唯一
        for (int it = 0; it < a.size(); it++)
            if (isSame(b, a[it].second))
                return it;
        return -1;
    }
    int BinaryFind(const vector<int> &a, int b) // 二分查找
    {
        int l = 0;
        int r = a.size() - 1;
        while (l <= r)
        {
            int mid = l + (r - l) / 2;
            if (a[mid] == b)
                return mid;
            else if (a[mid] > b)
                r = mid - 1;
            else if (a[mid] < b)
                l = mid + 1;
        }
        return -1;
    }
    void setUnion(vector<int> &a, const vector<int> &b) // 将集合b并入集合a
    {
        // 输入a有序
        for (int i = 0; i < b.size(); i++)
            if (BinaryFind(a, b[i]) == -1)
                a.push_back(b[i]);
    }
    int isEndSet(const vector<int> s) // 终态返回1，初态返回-1，其余0
    {
        // 输入s有序
        int end = NFA.getEnd();
        if (BinaryFind(s, end) != -1) // 找到end
            return 1;
        int strt = NFA.getStrt();
        if (BinaryFind(s, strt) != -1) // 找到strt
            return -1;
        return 0;
    }
    void Construct() // subset Construction
    {
        vector<int> epc = epClosure(NFA.getStrt()); // 从初态开始
        subsets.push_back(make_pair(-1, epc));      // 第一个nfa状态集合肯定是初态
        vector<int> tmp;
        vector<string> Tokens = NFA.getTokens();
        int x = 0, se = 0;
        while (x < subsets.size())
        {
            for (int j = 0; j < Tokens.size(); j++)
            {
                tmp.clear();
                for (int i = 0; i < subsets[x].second.size(); i++)
                {
                    vector<int> t = NFA.getState(subsets[x].second[i], Tokens[j]);
                    for (int k = 0; k < t.size(); k++)
                        setUnion(tmp, epClosure(t[k])); // 闭包已经有序,产生的tmp也是有序的
                }
                int it = isInit(subsets, tmp);
                if (!tmp.size()) // 集合为空不作存储
                    DFA.push_back(node(x, Tokens[j], -1));
                else if (it != -1) // 已存在集合
                    DFA.push_back(node(x, Tokens[j], it));
                else if (tmp.size()) // 若是新出现的集合且不为空
                {
                    se = isEndSet(tmp);
                    subsets.push_back(make_pair(se, tmp));
                    DFA.push_back(node(x, Tokens[j], subsets.size() - 1));
                }
            }
            x++;
        }
    }
    // mindfa
    void helperDrive()
    {
        int NumofTokens = NFA.getTokenNum() + 1; // 边数(状态转移数目)
        int NumofDFA = DFA.size();
        int deletedDone = 0;
        vector<int> a, b;
        for (int i = 0; i < subsets.size() - 1; i++)
        {
            for (int j = i + 1; j < subsets.size(); j++)
            {
                if (DFA[i * NumofTokens].end == -2 || DFA[j * NumofTokens].end == -2) // 如果有被删集
                    break;
                if (subsets[i].first != subsets[j].first) // 如果不同态
                    break;
                a.clear();                            // 清空a
                b.clear();                            // 清空b
                for (int e = 0; e < NumofTokens; e++) // 使用a收集dfa状态编号i对应的所有下一状态编号
                    a.push_back(DFA[e + i * NumofTokens].end);
                for (int e = 0; e < NumofTokens; e++) // 使用b收集dfa状态编号j对应的所有下一状态编号
                    b.push_back(DFA[e + j * NumofTokens].end);
                if (isSame(a, b)) // 同态且等价
                {
                    for (int e = 0; e < NumofTokens; e++) // 删除(将其视为被删)
                        DFA[e + j * NumofTokens].end = -2;
                    for (int e = 0; e < NumofDFA; e++) // 修改
                        if (DFA[e].end == j)
                            DFA[e].end = i;
                    deletedDone = 1;
                    break;
                }
            }
            if (deletedDone) // 若修改过就要重新判重
            {
                i = -1;
                deletedDone = 0;
            }
        }
    }
    string generProgram(int xx)
    {
        string res = "#include<iostream>\r\nusing namespace std;\r\n"; // 头文件

        // 字符集Token——状态转移string Tokens[]={"","",...};
        vector<string> Tokens = NFA.getTokens();
        res += "string Tokens[]={ \"" + Tokens[0] + "\"";
        for (int i = 1; i < Tokens.size(); i++)
            res += " , \"" + Tokens[i] + "\"";
        res += " };\r\n";

        // DFA——int DFA[NumofDFA][TokenNum]={{},{},...};
        int TokenNum = Tokens.size();
        int NumofDFA = 0;
        for (int i = 0; i < DFA.size(); i++)
            if (DFA[i * TokenNum].end != -2)
                NumofDFA++;
        res += "int DFA[" + to_string(NumofDFA) + "][" + to_string(TokenNum) + "]={";
        for (int i = 0; i < subsets.size(); i++)
        {
            if (DFA[i * TokenNum].end == -2) // 跳过被删
                continue;
            res += "{";
            for (int j = 0; j < TokenNum; j++)
                res += to_string(DFA[j + i * TokenNum].end) + ",";
            res += "},";
        }
        res += "};\r\n";
        // 输入和检查是否正确
        // int main()
        // {
        //     int i = 0, strt = 0, isMatch = 0; // 初态肯定为状态集合中的第一个
        //     string sta;
        //     getline(cin, sta);
        //     while (sta[i] != '\0')
        //     {
        //         for (int j = 0; j < Tokens.size(); j++)
        //         {
        //             int lenOfToken = Tokens[j].length();
        //             if (lenOfToken + i <= sta.lenth() && sta.substr(i, lenOfToken) == Tokens[i])
        //             {
        //                 if (strt != -1)
        //                 {
        //                     strt = DFA[strt][j];
        //                     isMatch = 1;
        //                 }
        //             }
        //         }
        //         if (isMatch) // 如果不匹配
        //         {
        //             cout << "NO" << endl;
        //             return -1;
        //         }
        //         i++;
        //     }
        //     cout << "YES" << endl;
        //     return 0;
        // }
        res += "int main()\r\n{\r\n\tint i = 0, strt = 0, isMatch = 0;\r\n\t";
        res += "string sta;\r\n\tgetline(cin, sta);\r\n\twhile (sta[i] != \'\\0\')\r\n\t{\r\n";
        res += "\t\tfor (int j = 0; j < Token.size(); j++)\r\n\t\t{\r\n";
        res += "\t\t\tint lenOfToken = Tokens[j].length();\r\n\t\t\tif (lenOfToken + i <= sta.length() && sta.substr(i, lenOfToken) == Tokens[i])\r\n\t\t\t{\r\n";
        res += "\t\t\t\tif (strt != -1)\r\n\t\t\t\t{\r\n\t\t\t\t\tstrt = DFA[strt][j];\r\n\t\t\t\t\tisMatch = 1;\r\n\t\t\t\t}\r\n\t\t\t}\r\n\t\t}\r\n";
        res += "\t\tif (isMatch)\r\n\t\t{\r\n\t\t\tcout << \"NO\" << endl;\r\n\t\t\treturn -1;\r\n\t\t}\r\n\t\ti++;\r\n\t}\r\n";
        res += "\tcout << \"YES\" << endl;\r\n\treturn 0;\r\n}";
        return res;
    }

public:
    RetoDFA(string regex, vector<string> mysta)
    {
        re = regex;
        for (int i = 0; i < mysta.size(); i++)
            MySta.push_back(mysta[i]);
        Format();
    }
    ~RetoDFA()
    {
        DFA.clear();
        subsets.clear();
        re.clear();
    }
    void RetoNFA()
    {
        deal(toPost(Format())); // 生成NFA
    }
    void generNFApng()
    {
        ofstream dotfile("C:/Users/xu/Desktop/experiment/code/byyllab2/nfa.dot"); // 创建输出文件
        if (!dotfile.is_open())
            cerr << "ERROR: unable create DOT file." << endl;
        vector<string> Tokens = NFA.getTokens();
        vector<int> nextState;
        // 写入NFA的DOT语言描述
        dotfile << "digraph NFA {" << endl;
        dotfile << "    rankdir=LR;" << endl;
        dotfile << "    node [shape=circle];" << std::endl;
        dotfile << "    " + to_string(NFA.getEnd()) << " [shape=doublecircle];" << std::endl;
        for (int i = 0; i <= NFA.getVerNum(); i++) // 索引nfa结点编号
        {
            nextState.clear();
            for (int j = 0; j < Tokens.size(); j++) // 索引nfa边
            {
                nextState = NFA.getState(i, Tokens[j]);
                if (nextState.size()) // 如果下一状态存在
                    for (int e = 0; e < nextState.size(); e++)
                        dotfile << "    " + to_string(i) + " -> " + to_string(nextState[e]) + "[label=\"" + Tokens[j] + "\"];" << endl;
            }
            nextState.clear();
            nextState = NFA.getNextOfStateByEpsilon(i);
            for (int j = 0; j < nextState.size(); j++)
                dotfile << "    " + to_string(i) + " -> " + to_string(nextState[j]) + "[label=\"" + epsilon + "\"];" << endl;
        }

        // dotfile << "    S2 -> S0 [label=\"a\"];" << std::endl;
        dotfile << "}" << std::endl;
        dotfile.close();
        system("dot -Tpng C:/Users/xu/Desktop/experiment/code/byyllab2/nfa.dot -o C:/Users/xu/Desktop/experiment/code/byyllab2/nfa.png");
    }
    void generDFApng()
    {
        ofstream dotfile("C:/Users/xu/Desktop/experiment/code/byyllab2/dfa.dot"); // 创建输出文件
        if (!dotfile.is_open())
            cerr << "ERROR: unable create DOT file." << endl;
        vector<string> Tokens = NFA.getTokens();
        int nextState = -1;
        int NumOfEdges = Tokens.size();
        // 写入DFA的DOT语言描述
        dotfile << "digraph DFA {" << endl;
        dotfile << "    rankdir=LR;" << endl;
        dotfile << "    node [shape=circle];" << std::endl;
        vector<int> curr;
        vector<string> DFAStates;
        string tmp = "";
        // 将DFA结点转成字符串
        for (int i = 0; i < subsets.size(); i++)
        {
            tmp = "\"" + to_string(i) + ": {";
            curr = subsets[i].second;
            for (int j = 0; j < curr.size(); j++)
                tmp += to_string(curr[j]) + ",";
            tmp.pop_back();
            tmp += "}\"";
            if (subsets[i].first)
                dotfile << "    " + tmp + " [shape=doublecircle];" << endl;
            DFAStates.push_back(tmp);
        }
        for (int i = 0; i < DFAStates.size(); i++) // 索引DFA结点编号
        {
            for (int j = 0; j < NumOfEdges; j++) // 索引nfa边
            {
                nextState = DFA[i * NumOfEdges + j].end;
                if (nextState != -1 && nextState != -2) // 如果下一状态存在
                    dotfile << "    " + DFAStates[i] + " -> " + DFAStates[nextState] + "[label=\"" + Tokens[j] + "\"];" << endl;
            }
        } // dotfile << "    S2 -> S0 [label=\"a\"];" << std::endl;
        dotfile << "}" << std::endl;
        dotfile.close();
        system("dot -Tpng C:\\Users\\xu\\Desktop\\experiment\\code\\byyllab2\\dfa.dot -o C:/Users/xu/Desktop/experiment/code/byyllab2/dfa.png");
    }
    void generminDFApng()
    {
        ofstream dotfile("C:/Users/xu/Desktop/experiment/code/byyllab2/mindfa.dot"); // 创建输出文件
        if (!dotfile.is_open())
            cerr << "ERROR: unable create DOT file." << endl;
        vector<string> Tokens = NFA.getTokens();
        int nextState = -1;
        int NumOfEdges = Tokens.size();
        // 写入DFA的DOT语言描述
        dotfile << "digraph minDFA {" << endl;
        dotfile << "    rankdir=LR;" << endl;
        dotfile << "    node [shape=circle];" << std::endl;
        vector<int> curr;
        for (int i = 0; i < subsets.size(); i++) // 索引DFA结点编号
        {
            for (int j = 0; j < NumOfEdges; j++) // 索引nfa边
            {
                nextState = DFA[i * NumOfEdges + j].end;
                if (nextState != -1 && nextState != -2) // 如果下一状态存在
                {
                    if (subsets[i].first)
                        dotfile << "    " + to_string(i) + " [shape=doublecircle];" << endl;
                    if (subsets[j].first)
                        dotfile << "    " + to_string(j) + " [shape=doublecircle];" << endl;
                    dotfile << "    " + to_string(i) + " -> " + to_string(nextState) + "[label=\"" + Tokens[j] + "\"];" << endl;
                }
            }
        }
        // dotfile << "    S2 -> S0 [label=\"a\"];" << std::endl;
        dotfile << "}" << std::endl;
        dotfile.close();
        system("dot -Tpng C:/Users/xu/Desktop/experiment/code/byyllab2/mindfa.dot -o C:/Users/xu/Desktop/experiment/code/byyllab2/mindfa.png");
    }
    void NFAtoDFA()
    {
        Construct();
    }
    void minDFA()
    {
        helperDrive();
    }
    void showDFA()
    {
        vector<string> Tokens = NFA.getTokens();
        int NumofTokens = Tokens.size();
        for (int i = 0; i < subsets.size(); i++)
        {
            if (subsets[i].first) // 终态
                cout << '*';
            cout << '(';
            if (DFA[i * NumofTokens].end != -2) // 如果没被删,则输出
                for (auto x : subsets[i].second)
                    cout << x << ',';
            cout << ")\t";
            for (int j = 0; j <= NFA.getTokenNum(); j++)
            {
                cout << Tokens[j] << ": ";
                cout << '(';
                int nextState = DFA[j + i * NumofTokens].end;
                if (nextState != -1 && nextState != -2)
                    for (auto y : subsets[nextState].second)
                        cout << y << ',';
                cout << ")\t";
            }
            cout << endl;
        }
    }
    void showNFA()
    {
        cout << " \t";
        vector<string> Tokens = NFA.getTokens();
        for (int i = 0; i <= NFA.getTokenNum(); i++)
            cout << Tokens[i] << '\t';
        cout << epsilon << endl;
        for (int i = 0; i <= NFA.getVerNum(); i++)
        {
            cout << i << '\t';
            vector<int> t;
            for (int e = 0; e <= NFA.getTokenNum(); e++)
            {
                t = NFA.getState(i, Tokens[e]);
                cout << '(';
                for (int j = 0; j < t.size(); j++)
                    cout << t[j] << ',';
                cout << ')' << '\t';
            }
            t = NFA.getState(i, epsilon);
            cout << '(';
            for (int j = 0; j < t.size(); j++)
                cout << t[j] << ',';
            cout << ')' << endl;
        }
        cout << "STRT: " << NFA.getStrt() << endl;
        cout << "END: " << NFA.getEnd() << endl;
        cout << "epsilon*: (";
        for (int x : epClosure(NFA.getStrt()))
            cout << x;
        cout << ')' << endl;
    }
    void generProgram()
    {
        if (subsets.empty())
        {
            cout << "未生成DFA" << endl;
            return;
        }
        ofstream outfile("C:/Users/xu/Desktop/experiment/code/byyllab2/program.txt");
        if (!outfile.is_open())
        {
            cerr << "ERROR: unable create DOT file." << endl;
            return;
        }
        outfile << generProgram(1);
        outfile.close();
        return;
    }
};

int main()
{
    vector<string> mysta;
    mysta.push_back("letter");
    mysta.push_back("digit");
    RetoDFA f = RetoDFA("digit|letter", mysta);
    f.RetoNFA();
    f.showNFA();
    f.generNFApng();

    f.NFAtoDFA();
    f.generDFApng();

    // f.minDFA();
    // f.generminDFApng();
    f.showDFA();
    f.generProgram();
    return 0;
}
