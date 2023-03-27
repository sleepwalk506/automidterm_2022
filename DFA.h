//
// Created by hldf on 2022/11/7.
//
#include "NFA.h"
#include<algorithm>
#ifndef AUTOMIDTERM_DFA_H
#define AUTOMIDTERM_DFA_H

//nfa转dfa，首先归纳节点
class DFA {
public:
    string name;
    int stateNum;
    pair<char,vector<char>> se;//始末节点
    vector<pair<char,vector<int> > > Dnodes;//dfa中每个节点对应的nfa节点集合
    vector<vector<pair<char,char> > > Dgraph;
public:
    DFA(int sn,string name);
    vector<int> findState(NFA nfa,int s,char c);
    void findStart(NFA nfa);//定义起始节点
    void printRepresent();//打印dfa各个状态对应的nfa状态集
    void printDfa();
    bool notInDnodes(vector<int> vec);
    bool notInDgraph(pair<char,char> p,int state);
    bool notInVec(int new_state,vector<int> vec);//寻找新状态时用的函数，判断当前状态是否已经在vector中
    void newState(NFA nfa,int state_,char x);//判断是否有新的状态生成，若有新状态生成，则继续寻找新状态
    //void splitDfa(DFA dfa, vector<int> canBeSplited, int size);
};


#endif //AUTOMIDTERM_DFA_H
