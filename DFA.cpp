//
// Created by hldf on 2022/11/7.
//

#include "DFA.h"
DFA::DFA(int sn,string name){//clion发疯发出来的b东西，全是废话但别删
    this->stateNum = sn;
    this->name = name;
    Dgraph.push_back(vector<pair<char,char> >());//我逐渐理解了一切
}
template <typename T>
vector<T> &operator +(vector<T> &v1,vector<T> &v2)
{
    v1.insert(v1.end(),v2.begin(),v2.end());
    return v1;
}
vector<int> DFA::findState(NFA nfa,int s,char c) {
    vector<int> state_;
    for(auto i:nfa.graph[s]){//s---char--->i.first
        if(i.second == c){
            if (notInVec(i.first,state_)) {
                state_.push_back(i.first);
                //vector<int> tmp = findState(nfa,i.first,c);
                //state_ = state_ + tmp;
                vector<int> tmp = findState(nfa,i.first,' ');
                for(auto j:tmp){
                    bool flag = false;
                    for(auto k:state_){
                        if(k == j){
                            flag = true;
                            break;
                        }
                    }
                    if(!flag){
                        state_.push_back(j);
                    }
                }
            }
        }
    }
    return state_;
}
void DFA::findStart(NFA nfa) {
    //定位nfa的起始状态x，然后将x能通过空转移到达的状态全部加入dfa的起始状态中
    char startNode = '1';//将起始状态设置为0
    vector<int> start_;
    start_.push_back(nfa.se.first);//初始化为nfa的起始状态，后面根据能飘到哪个状态进行添加
    vector<int> tmp = findState(nfa,nfa.se.first,' ');
    start_ = start_ + tmp;
//    for(auto e:nfa.graph[nfa.se.first]){
//        if(e.second == ' '){
//            start_.push_back(e.first);
//            for(auto e1:nfa.graph[e.first]){
//                //if new node not in vector add it in
//                if (e1.second == ' ') {
//                    bool if_in_vector = false;
//                    for (auto e2: start_) {
//                        if (e1.first == e2) {
//                            if_in_vector = true;
//                            break;
//                        }
//                    }
//                    if (!if_in_vector) {
//                        start_.push_back(e1.first);
//                    }
//                }
//            }
//        }
//    }
    sort(start_.begin(),start_.end());
    Dnodes.push_back(make_pair(startNode,start_));
    se.first = startNode;
    stateNum += 1;
}
void DFA::printRepresent() {
    for(int i = 0;i < stateNum;i++){
        cout << Dnodes[i].first << ':';
        for(auto j:Dnodes[i].second){
            cout << j << ' ';
        }
        cout << endl;
    }
}
bool DFA::notInVec(int new_state,vector<int> vec) {
    for(auto i:vec){
        if(new_state == i){
            return false;
        }
    }
    return true;
}
bool DFA::notInDnodes(vector<int> vec) {
    for(int i = 0;i < stateNum;i++){
        if(vec == Dnodes[i].second){
            return false;
        }
//        bool flag1 = true;
//        for(int j = 0;j < vec.size();j++){
//            if(vec[j] != Dnodes[i].second[j]){
//                flag1 = false;
//                break;
//            }
//        }
//        if(flag1){
//            return false;
//        }
    }
    return true;
}
bool DFA::notInDgraph(pair<char,char> p,int state){
    for(auto i:Dgraph[state]){
        if(p.first == i.first && p.second == i.second){
            return false;
        }
    }
    return true;
}
void DFA::newState(NFA nfa,int state_,char x) {
    //根据state_通过转移x能否产生全新的状态，若能产生新的状态，递归
    vector<int> new_state;
    bool if_there_is_a_real_state = false;
    for(auto i:Dnodes[state_].second){
        vector<int> tmp = findState(nfa,i,x);
        for(auto j:tmp){
            if(notInVec(j,new_state)){
                new_state.push_back(j);
                if_there_is_a_real_state = true;
            }
        }
    }
//    vector<int> tmp = findState(nfa,nfa.se.first,x);
//    new_state = new_state + tmp;
    sort(new_state.begin(),new_state.end());
//    if(!notInDnodes(new_state)){
//        Dgraph[state_].push_back(make_pair(Dnodes[state_].first,x));//通过x到new_state_node
//    }
    if(notInDnodes(new_state) && if_there_is_a_real_state){
        Dgraph.push_back(vector<pair<char,char> >());
        char new_state_node = '1' + stateNum;//后面要添加状态转移边state_ to new_state
        for(auto i:new_state){
            if(i == nfa.se.second){
                se.second.push_back(new_state_node);
            }
        }
        Dnodes.push_back(make_pair(new_state_node,new_state));
        Dgraph[state_].push_back(make_pair(new_state_node,x));//通过x到new_state_node
        stateNum += 1;
        //这里我忘记判空了，导致第二个状态集其实是空的
        newState(nfa,state_+1,'0');
        newState(nfa,state_+1,'1');
    }
    else if(if_there_is_a_real_state){//这里缺少判断是否已经存在一个表达式，导致输出其实会重复
        //int out_state;
        //find the fking node in dnodes
        for(auto i:Dnodes){
            if(new_state == i.second && notInDgraph(make_pair(i.first,x),state_)){
                Dgraph[state_].push_back(make_pair(i.first,x));
            }
        }
//        newState(nfa,state_,'0');
//        newState(nfa,state_,'1');
//        for(int i = 0;i < stateNum;i++){
//            if(new_state == Dnodes[i].second && notInDgraph(make_pair(Dnodes[i].first,x),state_)){
//                Dgraph[state_].push_back(make_pair(Dnodes[i].first,x));//通过x到new_state_node
//            }
//        }
    }
    if(state_ < Dnodes.size()-1){
        newState(nfa,state_+1,'0');
        newState(nfa,state_+1,'1');
    }
}
void DFA::printDfa() {
    cout << "start print dfa\n";
    cout << "start:" << se.first << endl;
    cout << "end:";
    for(auto m:se.second){
        cout << m << " ";
    }
    cout << endl;
    for(int i = 0;i < stateNum;i++){
        for(auto edge:Dgraph[i]){
            char tmp = '1' + i;
            cout << tmp << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
}