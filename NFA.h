//
// Created by hldf on 2022/11/7.
//
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <fstream>
using namespace std;
#ifndef AUTOMIDTERM_NFA_H
#define AUTOMIDTERM_NFA_H


class NFA {
public:
    string name;//NFA name
    string re;//正则表达式
    string re_;//带连接符的正则表达式
    string pe;//正则表达式的后缀表达式
    int stateNum;//状态数
    pair<int,int> se;//起点和终点状态编号
    vector<vector<pair<int,char> > > graph;//NFA状态关系图
public:
    NFA(string name,string re);
    void insertContact();//插入连接符函数，为转后缀表达式做准备
    int priority(char c);//运算符优先级函数，用于中缀转后缀
    void rTop();//中缀转后缀
    int newState();//生成新状态的边集
    void pToNfa();//后缀转NFA
    void printNfa();//打印nfa函数
};


#endif //AUTOMIDTERM_NFA_H
