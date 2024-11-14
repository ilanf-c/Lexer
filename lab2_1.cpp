#include <iostream>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

struct State
{
    unordered_map<string, int> transitions;
    unordered_set<int> epsilonTransitions;
    int acpt;
    State() : acpt(0) {}
};

class Graph
{

private:
    vector<State> states;
    int start;

public:
    Graph()
    {
        start = addState();
    }
    // 返回初态
    int getStrt()
    {
        return start;
    }
    // 创建新状态并返回编号
    int addState()
    {
        states.push_back(State());
        return states.size() - 1;
    }
    // 添加转移
    void addTransition(int strt, int end, string con)
    {
        states[strt].transitions[con] = end;
    }
    // 添加epsilon转移
    void addEpsilon(int strt, int end)
    {
        states[strt].epsilonTransitions.insert(end);
    }
    // 输出nfa表
    void printNFA()
    {
        for (int i = 0; i < ; i++)
        {
        }
    }
};

class ReToDFA
{
private:
    vector<string> re;
    int isOperator(string s)
    {
        if (s[0] == '|' || s[0] == '*' || s[0] == '?' || s[0] == '+')
            return 1;
        return 0;
    }

public:
    Graph ReToNFA()
    {
        Graph nfa;
        stack<pair<int, int>> s;
        string tmp = "";
        for (int i = 0; i < re.size(); i++)
        {
            tmp = re[i];
            if (tmp[0] == '|') // 选择
            {
                pair<int, int> tmp1 = s.top();
                s.pop();
                pair<int, int> tmp2 = s.top();
                s.pop();
                /**
                 *     O--->O
                 *   *        *
                 * O            O
                 *   *        *
                 *     O--->O
                 */
                int strt = nfa.addState();
                int end = nfa.addState();

                nfa.addEpsilon(strt, tmp1.first);
                nfa.addEpsilon(strt, tmp2.first);
                nfa.addEpsilon(tmp1.second, end);
                nfa.addEpsilon(tmp2.second, end);
                s.push(make_pair(strt, end));
            }
            else if (tmp[0] == '*') // 闭包
            {
                pair<int, int> t = s.top();
                s.pop();
                int strt = nfa.addState();
                int end = nfa.addState();
                /**
                 *      **
                 *    *    *
                 * O**O--->O**O
                 *  *        *
                 *     ****
                 */
                nfa.addEpsilon(strt, t.first);
                nfa.addEpsilon(t.second, end);
                nfa.addEpsilon(strt, end);
                nfa.addEpsilon(t.second, t.first);
                s.push(make_pair(strt, end));
            }
            else if (tmp[0] == '+') // 正闭包
            {
                pair<int, int> t = s.top();
                s.pop();
                int strt = nfa.addState();
                int end = nfa.addState();
                /**
                 *      **
                 *    *    *
                 * O**O--->O**O
                 */
                nfa.addEpsilon(strt, t.first);
                nfa.addEpsilon(t.second, end);
                nfa.addEpsilon(t.second, t.first);
                s.push(make_pair(strt, end));
            }
            else if (tmp[0] == '?')
            {
                pair<int, int> t = s.top();
                s.pop();
                int strt = nfa.addState();
                int end = nfa.addState();
                /**
                 * O**O--->O**O
                 *   *      *
                 *     ***
                 */
                nfa.addEpsilon(strt, t.first);
                nfa.addEpsilon(t.second, end);
                nfa.addEpsilon(strt, end);
                s.push(make_pair(strt, end));
            }
            if (tmp[0] == '.') // 并置
            {
                pair<int, int> tmp1 = s.top();
                s.pop();
                pair<int, int> tmp2 = s.top();
                s.pop();
                // O--->O***O--->O
                nfa.addEpsilon(tmp1.second, tmp2.first);
                s.push(make_pair(tmp1.first, tmp2.second));
            }
            else // 字符
            {

                int strt = nfa.addState();
                int end = nfa.addState();
                nfa.addTransition(strt, end, tmp);
                s.push(make_pair(strt, end));
            }
        }
        return nfa;
    }
    ReToDFA(/* args */);
    ~ReToDFA();
};

class graph
{
private:
    vector<vector<string>> matrix;
    int verNum; // 顶点数
    int start;  // 初态
    int accept; // 接受状态
public:
    graph()
    {
        // 将0行0列空出并置为0
        // 初态接受态初始为0
        vector<string> tmp;
        tmp.push_back(0);
        matrix.push_back(tmp);
        verNum = 0;
        start = 0;
        accept = 0;
    }
    ~graph(){};
    // 拷贝函数
    graph(const graph &g)
    {
        verNum = g.verNum;
        start = g.start;
        accept = g.accept;
        for (int i = 0; i <= verNum; i++)
            addVertex();
        for (int i = 1; i <= g.matrix.size(); i++)
            for (int j = 1; j <= g.matrix[i].size(); j++)
                matrix[i][j] = g.matrix[i][j];
    }
    // 返回顶点数
    int getVerNum()
    {
        return verNum;
    }
    // 返回初态
    int getStrt()
    {
        return start;
    }
    // 返回接受状态
    int getAccpt()
    {
        return accept;
    }
    // 返回边
    string getEdge(int s, int e)
    {
        return matrix[s][e];
    }
    // 添加顶点(返回当前顶点数)
    int addVertex()
    {
        vector<string> tmp;
        // 增加一行
        for (int i = 0; i <= verNum; i++)
            tmp.push_back(0);
        matrix.push_back(tmp);
        // 增加一列
        for (int i = 0; i <= verNum + 1; i++)
            matrix[i].push_back(0);
        verNum = matrix.size();
        return verNum;
    }
    // 添加边
    void addEdge(int strt, int end, string sta)
    {
        if (verNum < strt || verNum < end)
            cout << "顶点不存在" << endl;
        else
            matrix[strt][end] = sta;
    }
    // 设置初态
    void setStrt(int n)
    {
        start = n;
    }
    // 设置接受状态
    void setAccpt(int m)
    {
        accept = m;
    }
};
class NFA_helper
{
public:
    static graph generNFA(string sta) // 生成NFA
    {
        graph nfa = graph();
        // O--->O
        int t1 = nfa.addVertex(); // 获得状态编号
        int t2 = nfa.addVertex(); // 获得状态编号
        nfa.addEdge(t1, t2, sta);
        nfa.setStrt(t1);
        nfa.setAccpt(t2);
        return nfa;
    }
    static graph Union(graph nfa1, graph nfa2) // 连接并置
    {
        // O--->O***O--->O
        pair<int, int> tmp = addGra(nfa1, nfa2);
        if (tmp.first < tmp.second)
        {
            nfa1.addEdge(nfa1.getAccpt(), tmp.first, epsilon);
            nfa1.setAccpt(tmp.second);
        }
        return nfa1;
    }
    static graph Select(graph nfa1, graph nfa2) // 选择|
    {
        pair<int, int> t = addGra(nfa1, nfa2);
        if (t.first < t.second)
        {
            int t1 = nfa1.addVertex();
            int t2 = nfa1.addVertex();
            /**
             *     O--->O
             *   *        *
             * O            O
             *   *        *
             *     O--->O
             */
            nfa1.addEdge(t1, nfa1.getStrt(), epsilon);  // 头与nfa1
            nfa1.addEdge(t1, t.first, epsilon);         // 头与nfa2
            nfa1.addEdge(t.second, t2, epsilon);        // nfa2与尾
            nfa1.addEdge(nfa1.getAccpt(), t2, epsilon); // nfa1与尾
            nfa1.setAccpt(t2);
            nfa1.setStrt(t1);
        }
        return nfa1;
    }
    static graph Repetition(graph nfa, int closure = 0) // 闭包和正闭包(默认闭包)*/+
    {
        int t1 = nfa.addVertex();
        int t2 = nfa.addVertex();
        /**
         *      **
         *    *    *
         * O**O--->O**O
         *  *        *
         *     ****
         */
        nfa.addEdge(t1, nfa.getStrt(), epsilon);
        nfa.addEdge(nfa.getAccpt(), t2, epsilon);
        nfa.addEdge(nfa.getAccpt(), nfa.getStrt(), epsilon);
        if (!closure) // 闭包
            nfa.addEdge(t1, t2, epsilon);
        nfa.setStrt(t1);
        nfa.setAccpt(t2);
        return nfa;
    }
    static graph possibilize(graph nfa) // 可选?
    {
        int t1 = nfa.addVertex();
        int t2 = nfa.addVertex();
        /**
         * O**O--->O**O
         *   *      *
         *     ***
         */
        nfa.addEdge(t1, nfa.getStrt(), epsilon);  // 头
        nfa.addEdge(nfa.getAccpt(), t2, epsilon); // 尾
        nfa.addEdge(t1, t2, epsilon);             // 头到尾
        nfa.setAccpt(t2);
        nfa.setStrt(t1);
        return nfa;
    }
    static pair<int, int> addGra(graph &g1, graph &g2) // 向g1添加图g2(返回g2在g1中的头和尾)；若g2为空，返回的头大于尾
    {
        pair<int, int> seOfg2;
        int offset = g1.getVerNum();
        seOfg2.first = g1.getVerNum() + 1;
        for (int i = 1; i <= g2.getVerNum(); i++)
            g1.addVertex();
        seOfg2.second = g1.getVerNum();

        for (int i = 1; i <= g2.getVerNum(); i++)
            for (int j = 1; j <= g2.getVerNum(); j++)
                if (g2.getEdge(i, j) != "")
                    g1.addEdge(i + offset, j + offset, g2.getEdge(i, j));
        return seOfg2;
    }
    static graph Deal(vector<string> postfix) // 传入后缀表达式，返回nfa
    {
        stack<graph> s;
        int i = 0;
        string tmp = "";
        while (i < postfix.size())
        {
            tmp = postfix[i++];
            if (tmp == "&")
            {
                graph t1 = s.top();
                s.pop();
                graph t2 = s.top();
                s.pop();
                s.push(NFA_helper::Union(t1, t2));
            }
            else if (tmp == "|")
            {
                graph t1 = s.top();
                s.pop();
                graph t2 = s.top();
                s.pop();
                s.push(NFA_helper::Select(t1, t2));
            }
            else if (tmp == "*")
            {
                graph t1 = s.top();
                s.pop();
                s.push(NFA_helper::Repetition(t1, 0));
            }
            else if (tmp == "+")
            {
                graph t1 = s.top();
                s.pop();
                s.push(NFA_helper::Repetition(t1, 1));
            }
            else if (tmp == "?")
            {
                graph t1 = s.top();
                s.pop();
                s.push(NFA_helper::possibilize(t1));
            }
            else
            {
                s.push(NFA_helper::generNFA(tmp));
            }
        }
        return s.top();
    }
};
