//
// Created by hldf on 2022/11/7.
//

#include "NFA.h"
NFA::NFA(std::string name, std::string re) {
    this->name = name;
    this->re = re;
    re_ = pe = "";
    stateNum = 0;
    graph.push_back(vector<pair<int,char> >());//向vector的最后添加空的intchar组合
}
void NFA::insertContact() {
    //cout << "start insert contact\n";
    for(int i = 0;i < re.size()-1;i++){
        re_ += re[i];
        if(re[i] != '(' && re[i+1] != ')' && re[i] != '+' && re[i+1] != '+'
        && re[i+1] != '*'){
            re_ += '.';//只有非符号接非符号才加连接符
            //*符号后可接是因为前面一整块相当于非符号
        }
    }
    re_ += re.back();//前面防止判定re[i+1]时访问溢出，少访问了最后一个元素
    //在这里加回去,补全完整的后缀正则表达式
}
int NFA::priority(char c) {
    if(c == '*'){return 3;}//相当于幂
    else if(c == '.'){return 2;}//相当于乘
    else if(c == '+'){return 1;}//相当于加
    else return 0;
}
void NFA::rTop() {
    //cout << "start change re to pe\n";
    stack<char> op;
    for(auto c:re_)
    {
        if(c == '('){op.push(c);}
        else if(c == ')'){//当前为右括号时，一顿弹栈，直到遇到左括号
            while(op.top() != '('){
                pe += op.top();//将弹出来的东西加到pe后面
                op.pop();
            }
            op.pop();//弹出左括号
        }
        else if(c == '*' || c == '.' || c == '+'){
            while(op.size()){//当栈非空，执行此操作
                if(op.top() != '(' && priority(c) <= priority(op.top())){
                    pe += op.top();//当前字符优先级小于栈顶优先级时，弹栈
                    op.pop();
                }
                else break;
            }
            op.push(c);//直到遇见优先级小于自己的符号，入栈
        }
        else{
            pe += c;//非符号直接进入后缀表达式
        }
    }
    while (op.size()){//将栈中剩余的符号全部弹出
        pe += op.top();
        op.pop();
    }
}
int NFA::newState() {
    graph.push_back(vector<pair<int,char> >());//生成新状态的边集
    return ++stateNum;
}
void NFA::pToNfa() {
    //cout << "start change pe to nfa\n";
    stack<pair<int,int> > states;//状态栈
    int s,e;//状态起点和终点状态编号
    for(auto c:pe){
        if(c != '*' && c != '.' && c != '+'){//非符号直接入栈
            s = newState();//表示第几个状态
            e = newState();
            states.push(make_pair(s,e));//起始为s终止为e的边
            graph[s].push_back(make_pair(e,c));//s通过c到e
            continue;
        }
        switch (c) {
            case '*':{//通过空转移到当前状态
                pair<int,int> origin = states.top();//对栈顶的边进行操作
                states.pop();
                s = newState();
                e = newState();
                states.push(make_pair(s,e));
                graph[s].push_back(make_pair(origin.first,' '));
                graph[s].push_back(make_pair(e,' '));
                graph[origin.second].push_back(make_pair(e,' '));
                graph[origin.second].push_back(make_pair(origin.first,' '));
                break;
            }
            case '.': {//这里其实应该将两个状态合并成同一个状态
                pair<int, int> right = states.top(); states.pop();
                pair<int, int> left = states.top(); states.pop();
                states.push(make_pair(left.first, right.second));
                //graph[left.second].push_back(make_pair(right.first, ' '));
                char ch[10];
                int states_next[10];
                int next_state_num = 0;
                for(auto i:graph[right.first]){
                    if(i.second == '0' || i.second == '1' || i.second == ' '){
                        ch[next_state_num] = i.second;
                        states_next[next_state_num] = i.first;
                        next_state_num++;
                    }
                }
                for(int i = 0;i < next_state_num;i++){
                    graph[left.second].push_back(make_pair(states_next[i], ch[i]));
                }
                graph[right.first] = graph[left.second];
                //stateNum--;
                break;
            }
            case '+': {
                pair<int, int> down = states.top(); states.pop();
                pair<int, int> up = states.top(); states.pop();
                s = newState();
                e = newState();
                states.push(make_pair(s, e));
                graph[s].push_back(make_pair(up.first, ' '));
                graph[s].push_back(make_pair(down.first, ' '));
                graph[up.second].push_back(make_pair(e, ' '));
                graph[down.second].push_back(make_pair(e, ' '));
                break;
            }
            default:
                break;
        }
        se = make_pair(states.top().first, states.top().second);
    }
}
void NFA::printNfa() {
    cout << "start print nfa\n";
    cout << "name: " << name << "\n"
         << "re: " << re << "\n"
         << "pe: " << pe << "\n"
         << "stateNum: " << stateNum << "\n"
         << "start: " << se.first << "\n"
         << "end: " << se.second << endl;
    for(int i = 1; i <= stateNum; i++) {
        for(auto edge : graph[i]) {
            cout << i << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
    cout << endl;
    //cout << endl;
}