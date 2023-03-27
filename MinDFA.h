//
// Created by hldf on 2022/11/9.
//
#include "DFA.h"
#include <queue>
#ifndef AUTOMIDTERM_MINDFA_H
#define AUTOMIDTERM_MINDFA_H
#define MAX_LIST_SIZE 100

class Group{
public:
    int group_id;//组号
    vector<int> state_set;//这组代表的状态集
public:
    Group(int group_id,vector<int> state_set) {
        this->group_id = group_id;
        this->state_set = state_set;
    }
    void groupAppend(Group anotherGroup){
        for(auto i:anotherGroup.state_set){
            bool flag = false;
            for(auto j:this->state_set){
                if(j == i){
                    flag = true;
                    break;
                }
            }
            if(!flag){
                this->state_set.push_back(i);
            }

        }
    }
};

class MinDFA {
    //极小化dfa
public:
    string name;
    char letters[2] = {'0','1'};
    int stateNum;//极小化之后的状态总数string name;
    pair<vector<int>,vector<int>> se;//始末节点
    vector<int> trap;//陷阱状态节点集
    vector<pair<int,vector<int> > > DMnodes;//Mdfa中每个节点对应的dfa状态集合
    vector<vector<pair<int,char> > > DMgraph;//dfa中的这个是吃second到first
    vector<vector<pair<int,char> > > DfaOrderGraph;//dfa的graph是char的，这个是状态吃second到第int个状态
    //vector<pair<int,vector<bool> > > mergeList;
    //bool mergeList[MAX_LIST_SIZE][MAX_LIST_SIZE];//归并状态表，表示几个状态能否归并
public:
    int cnt = 0;//共有cnt组状态集
    vector<Group> finalGroup;
    vector<Group> curGroup;

    vector<Group> reOrderGroup;
    vector<pair<int,vector<int> > > reOrderDMnodes;
    vector<vector<pair<int,char> > > reOrderDMgraph;
    pair<vector<int>,vector<int>> reOrderse;

    int preGraph[100][2];
    vector<int> preStartState;
    vector<int> preEndState;

    int myfinalGraph[100][2];
    int myfinalcount;
    vector<int> myfinalStartState;
    vector<int> myfinalEndState;
public:
    MinDFA(int sn,string name);
    void mergeDfa(DFA dfa);//造出归并状态表
    void orderDfa(DFA dfa);//将dfa中的状态数字化
    void printOrderDfa(DFA dfa);
    vector<Group> splitDfa(DFA dfa,char letter,vector<Group> canBeSplited);//分割函数，当前集合可被分割的情况下递归执行
    vector<Group> reMergeDfa(DFA dfa,char letter,vector<Group> ToBeMerged);
    vector<Group> reOrderDfa(vector<Group> ToBeReOrdered);
    int myfinalreorder();
    vector<int> ifTrap(DFA dfa,char letter,vector<Group> finalGroupSet);
    bool ifStillSplitable();
    int stateMove(int cur_state,char letter);//当前状态通过letter能到达的唯一后继状态
    bool ifInMap(Group a,vector<pair<Group,vector<int> > > Gmap);
    vector<int> reMergeTrapGroup();
    //int findGroupInMap(Group a,vector<pair<Group,vector<int> > > Gmap);
    Group belong(int cur_state,char letter,vector<Group> groupSet);//某状态经过一次letter到达当前分组group
    void printFinalGroup();
    //void deleteState();//删除无关状态的函数，去除死状态和不可到达的状态
    void printMinDfaSe();//打印mindfa的始末状态
    void printMinDfaGraph();//打印mindfa的graph
    void RG();//将dfa按照rg的形式输出
    void lastTermPrint1();
    void lastTermPrint2();
    void lastTermPrint3();
    void lastTermPrint4();
    void lastTermPrint5();
    void lastTermPrint6();
    bool ifInStart(int i);
    bool ifInend(int i);
    bool ifInTrap(int i);
    bool ifInReEnd(int i);
    bool ifInReStart(int i);

    bool testlinerule(string test);
};

#endif //AUTOMIDTERM_MINDFA_H
