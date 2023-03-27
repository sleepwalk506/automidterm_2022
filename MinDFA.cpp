//
// Created by hldf on 2022/11/9.
//

#include "MinDFA.h"
MinDFA::MinDFA(int sn, std::string name) {
    stateNum = sn;
    this->name = name;
    DMgraph.push_back(vector<pair<int,char> >());//我逐渐理解了一切
    DfaOrderGraph.push_back(vector<pair<int,char> >());//我逐渐理解了一切
    reOrderDMgraph.push_back(vector<pair<int,char> >());
}
void MinDFA::mergeDfa(DFA dfa) {//共有状态集，合并等价状态，去除无关状态
//    1
//    2
//    3
//      0 1 2
//stateNum = 4
    //bool mergeTmp[MAX_LIST_SIZE][MAX_LIST_SIZE];
    vector<int> end;
    vector<int> out_of_end;
    int endsize = 0;
    for(auto i:dfa.se.second){//获取终止状态集
        end.push_back(i-'1');
        endsize++;
    }
    int othersize = 0;
    for(int i = 0;i < dfa.stateNum;i++){//获取非终止状态集
        bool flag = false;
        for(auto k:dfa.se.second){
            if((k-'1') == i){
                flag = true;
                break;
            }
        }
        if (!flag) {
            out_of_end.push_back(i);
            othersize++;
        }
    }
    //上面两个状态集都是从0开始
    //现在已经有了两个状态集（分别是最终状态集和非最终状态集），开始分割状态集直到不能再分
    //现在我有两个group final 和 cur
    Group groupEnd(cnt++,end);//从第0组开始
    Group groupOutOfEnd(cnt++,out_of_end);
    finalGroup.push_back(groupEnd);
    finalGroup.push_back(groupOutOfEnd);//将最开始的两组全推到最终组中

//    curGroup = finalGroup;
//    finalGroup = splitDfa(dfa,'a',curGroup);
    //根据a分裂一次之后，再根据b分裂，是否可以再根据a分裂一次？此处需要加while可分裂
    while (ifStillSplitable()) {//这个地方出问题是因为0 8这组分不开，不知道为什么
        for (char letter: letters) {
            curGroup = finalGroup;
            finalGroup = splitDfa(dfa, letter, curGroup);
        }
    }
    curGroup = finalGroup;
    finalGroup = splitDfa(dfa, '0', curGroup);
    curGroup = finalGroup;
    finalGroup = splitDfa(dfa, '1', curGroup);
    curGroup = finalGroup;
    finalGroup = splitDfa(dfa, '0', curGroup);
    curGroup = finalGroup;
    finalGroup = splitDfa(dfa, '1', curGroup);

    for(int i = 0;i < finalGroup.size();i++){
        finalGroup[i].group_id = i;
    }

    trap = ifTrap(dfa,'0',finalGroup);//此处letter是废参数

    for(char letter:letters){
        curGroup = finalGroup;
        finalGroup = reMergeDfa(dfa,letter,curGroup);
    }
    for(int i = 0;i < finalGroup.size();i++){
        finalGroup[i].group_id = i;
    }

    trap = ifTrap(dfa,'0',finalGroup);//此处letter是废参数

    for(auto i:finalGroup){
        DMnodes.push_back(make_pair(i.group_id,i.state_set));
        for(auto j:i.state_set){
            if(j == dfa.se.first - '1'){
                se.first.push_back(i.group_id);
            }
            else{
                for(auto k:dfa.se.second){
                    if(j == k - '1'){
                        se.second.push_back(i.group_id);
                    }
                }
            }
        }
    }
//    for(auto i:DMnodes){
//        cout << ":";
//        for(auto j:i.second){
//            cout << j << " ";
//        }
//        cout << endl;
//    }

    for(int i = 0;i < DMnodes.size();i++){//遍历DMnodes，根据每个新状态代表的老状态和ordergraph找出mindfa的graph
        vector<pair<int,char> > tmp;
        for(auto k:DfaOrderGraph[DMnodes[i].second[0]]){
            //通过k.second到k.first,需要找到k.first对应的新状态
            for(int m = 0;m < DMnodes.size();m++){
                for(int l = 0;l < DMnodes[m].second.size();l++){
                    if(DMnodes[m].second[l] == k.first){
                        tmp.push_back(make_pair(m,k.second));
                    }
                }
            }
        }
        DMgraph.push_back(tmp);
    }

    //reOrderDfa(finalGroup);
}
void MinDFA::orderDfa(DFA dfa) {
    for(int i = 0;i < dfa.stateNum;i++){
        for(auto edge:dfa.Dgraph[i]){
            DfaOrderGraph.push_back(vector<pair<int,char> >());
            DfaOrderGraph[i].push_back(make_pair(edge.first-'1',edge.second));
        }
    }
}
bool MinDFA::ifInMap(Group a, vector<pair<Group, vector<int> > > Gmap) {
    for(auto i:Gmap){
        if(i.first.group_id == a.group_id){
            return true;//组号相等
        }
    }
    return false;
}

vector<Group> MinDFA::splitDfa(DFA dfa,char letter,vector<Group> canBeSplited) {//这部分是分割法
    vector<Group> finalGroupSet;
    vector<Group> curGroupSet = canBeSplited;
    queue<Group> groupQue;
    for(auto g:canBeSplited){
        groupQue.push(g);//在末尾加元素
    }
    while(!groupQue.empty()){
        Group oldGroup = groupQue.front();//返回第一个元素
        groupQue.pop();//删除第一个元素
        vector<pair<Group,vector<int> > > Gmap;//根据指向的组，对状态集进行分类
        bool flag = false;
        vector<int> trapStates;
        for(auto state:oldGroup.state_set){//这个group非空，但state_set是空的
            Group next_state_belong = belong(state,letter,curGroupSet);//此处若根本没有nextstate，应该直接跳过到下一个
            Group next_state_belong1 = belong(state,'0',curGroupSet);
            Group next_state_belong2 = belong(state,'1',curGroupSet);
            if(next_state_belong1.group_id == -1 && next_state_belong2.group_id == -1){//陷阱状态
//                vector<int> new_group_set;
//                new_group_set.push_back(state);
//                Group newGroup(cnt++,new_group_set);
//                curGroupSet.push_back(newGroup);
//                finalGroupSet.push_back(newGroup);
                flag = true;
                trapStates.push_back(state);
            }
            if(!ifInMap(next_state_belong,Gmap)){//当前组不在map中（nextstatebelong是个组group
                Gmap.push_back(make_pair(next_state_belong,vector<int>()));
            }
            for(int i = 0;i < Gmap.size();i++){//不要用auto的i推
                if(Gmap[i].first.group_id == next_state_belong.group_id){//这样造成了没有nextstate的状态单独成组
                    Gmap[i].second.push_back(state);
                }
            }
        }
        //这个地方 0 8 明明是映射到不同的组中，但是这里gmap的size是1
        if(Gmap.size() == 1){//如果这些状态映射到了同一个group中，则为最终状态集
            if(flag == true){
                for(auto itr= curGroupSet.begin();itr != curGroupSet.end();itr++){
                    if(itr->group_id == oldGroup.group_id){
                        itr = curGroupSet.erase(itr);
                        break;
                    }
                }
                vector<int> newStateSetWithoutTrap;
                for(auto j:Gmap[0].second){
                    bool flag2 = true;
                    for(auto k:trapStates){
                        if(k == j){
                            vector<int> new_group_set;
                            new_group_set.push_back(k);
                            Group newGroup(cnt++,new_group_set);
                            curGroupSet.push_back(newGroup);
                            finalGroupSet.push_back(newGroup);
                            flag2 = false;
                            break;
                        }
                    }
                    if(flag2){
                        newStateSetWithoutTrap.push_back(j);
                    }
                }
                Group newGroup(cnt++,newStateSetWithoutTrap);
                curGroupSet.push_back(newGroup);
                //finalGroupSet.push_back(newGroup);
                groupQue.push(newGroup);
            }
            else{
                finalGroupSet.push_back(oldGroup);
            }
        }
        else{
            for(auto itr= curGroupSet.begin();itr != curGroupSet.end();itr++){
                if(itr->group_id == oldGroup.group_id){//这里erease会导致越界
                    itr = curGroupSet.erase(itr);
                    break;
                }
            }
            for(auto i:Gmap){//这里新加进去的group是空的
                Group newGroup(cnt++,i.second);
                curGroupSet.push_back(newGroup);
                groupQue.push(newGroup);//在尾部push
            }
        }
        //groupQue.pop();//删除第一个元素
    }
    return finalGroupSet;
}
//注意在remerge的时候需要考虑陷阱状态
vector<Group> MinDFA::reMergeDfa(DFA dfa, char letter, vector<Group> ToBeMerged) {
    //重新归并dfa，找出可以被放到组中的孤立状态并放到相应组中
    vector<Group> finalGroupSet;
    vector<Group> curGroupSet = ToBeMerged;
    queue<Group> groupQue;
    for(auto g:curGroupSet){
        bool ifMergeAble = false;
        if (g.state_set.size() == 1) {
            if (belong(g.state_set[0], '0', curGroupSet).group_id == -1 &&
                    belong(g.state_set[0], '1', curGroupSet).group_id == -1) {
                ifMergeAble = true;
            }
            else{
                finalGroupSet.push_back(g);
            }
        }
        else{
            finalGroupSet.push_back(g);
        }
        if(ifMergeAble){
            groupQue.push(g);
        }
    }
    while (!groupQue.empty()){
        Group oldGroup = groupQue.front();
        groupQue.pop();
        //找到能接纳这个状态的组
        int state = oldGroup.state_set[0];
        bool ifCanBePushedIn = false;
        for(auto i:curGroupSet){//如果i组接纳了单个成组的状态并且由新i组能到达的所有状态都在新i组中，则可以接纳
            if(i.group_id != oldGroup.group_id){
                bool flag = true;
                bool flag2 = false;
                //推进到最后i是可以接纳的组 i中必须每个状态经过0和1都是state
                //这里i中的所有状态经过0和1能belong到的都有且只有一组
                Group next_state_belong = belong(i.state_set[0],'0',curGroupSet);
                Group next_state_belong1 = belong(i.state_set[0],'1',curGroupSet);
                next_state_belong.groupAppend(next_state_belong1);
                if(next_state_belong.group_id == -1 && next_state_belong1.group_id == -1){
                    flag2 = true;
                }
                for(auto k:next_state_belong.state_set){//如果state在i的生成集中，进行下一步判断
                    if(k == state){
                        flag2 = true;
                        break;
                    }
                }
                if(flag2){
                    if(next_state_belong.state_set.size() != 1){
                        if(next_state_belong.group_id == -1 && next_state_belong1.group_id == -1){
                            flag = true;
                        }
                        else{
                            flag = false;
                        }
                    }
                    if (flag) {
                        for (int f = 0; f < finalGroupSet.size(); f++) {
                            if (finalGroupSet[f].group_id == i.group_id) {
                                finalGroupSet[f].state_set.push_back(state);
                                ifCanBePushedIn = true;
                                break;
                            }
                        }
                        if(ifCanBePushedIn){
                            break;
                        }
                    }
                }

            }
        }
        if(!ifCanBePushedIn){
            finalGroupSet.push_back(oldGroup);
        }
    }
    return finalGroupSet;
}
int MinDFA::stateMove(int cur_state, char letter) {
    //这里的state都是从0开头的
    for(auto i:DfaOrderGraph[cur_state]){//这里可能越界了！！！！！
        if (i.second == letter){
            return i.first;//这个就是cur_state通过letter能达到的直接后继节点
        }
    }
    return -1;//若没有直接后继节点，返回-1
}

Group MinDFA::belong(int cur_state, char letter, vector<Group> groupSet) {//返回的是当前状态通过letter到达的状态所在的组
    int new_state = stateMove(cur_state,letter);//若当前的所有组中都不包含此状态，返回-1
    if(new_state == -1){
//        for(auto s:groupSet){//如果这组没有能到达的状态，这个状态应该归属于含有生成这个状态的组
//            if(count(s.state_set.begin(),s.state_set.end(),cur_state)){
//                return s;
//            }
//        }
        return Group(-1,vector<int> ());
    }
    for(auto s:groupSet){//如果新到达的状态在当前状态集中，返回状态集
        if(count(s.state_set.begin(),s.state_set.end(),new_state)){//count方法，若找不到则返回0
            return s;
        }
    }
    //return *tmp;//若没找到，返回一个空的对象
}
bool MinDFA::ifStillSplitable() {
    //如果当前所有组都只剩一个元素了，返回false
    //根据当前的finalGroup和状态转移函数确定是否可以继续分裂
    for(auto i:finalGroup){
        if (i.state_set.size() > 1) {
            vector<int> belong_states;
            for (int j = 0;j < i.state_set.size();j++) {
                if(stateMove(i.state_set[j], '0') == -1 && stateMove(i.state_set[j],'1') == -1){
                    return true;
                }
                for (char letter: letters) {
                    int next_state = stateMove(i.state_set[j], letter);
                    if(next_state == -1){
                        belong_states.push_back(i.state_set[j]);
                        continue;
                    }
                    bool flag = false;
                    for(auto k:belong_states){
                        if(k == next_state){
                            flag = true;
                        }
                    }
                    if(!flag){
                        if(j != 0){
                            return true;
                        }
                        belong_states.push_back(next_state);
                    }
                }
                std::sort(belong_states.begin(), belong_states.end());
            }
        }
    }
    return false;
}
vector<Group> MinDFA::reOrderDfa(vector<Group> ToBeReOrdered){
    int startState = se.first[0];
    int count = 0;
    reOrderDMnodes.push_back(make_pair(count++,finalGroup[startState].state_set));
    reOrderse.first.push_back(0);
    vector<bool> ifInreOrder;
    for(auto i:DMnodes){
        ifInreOrder.push_back(false);
    }
    ifInreOrder[startState] = true;
    queue<int> states;
    states.push(startState);
    while (!states.empty()){
        if(count == finalGroup.size()){
            break;
        }
        int preState = states.front();
        states.pop();
        vector<pair<int,char> > tmp;
        for(auto i:DMgraph[preState]){
            if(!ifInreOrder[i.first]){
                states.push(i.first);
                reOrderDMnodes.push_back(make_pair(count,finalGroup[i.first].state_set));
                tmp.push_back(make_pair(count,i.second));
                if(ifInend(i.first)){
                    reOrderse.second.push_back(count);
                }
                count++;
                ifInreOrder[i.first] = true;
            }
        }
        reOrderDMgraph.push_back(tmp);
    }
}
vector<int> MinDFA::ifTrap(DFA dfa, char letter,vector<Group> finalGroupSet) {//函数将groupid作为判断是否是陷阱状态的标准
    vector<int> tmpGroup;
    for(auto i:finalGroupSet){
        bool iftrap = true;
        for(auto j:i.state_set){
            if(belong(j,'0',finalGroupSet).group_id != i.group_id ||
            belong(j,'1',finalGroupSet).group_id != i.group_id ||
                    belong(j,'0',finalGroupSet).group_id != -1 &&
                            belong(j,'1',finalGroupSet).group_id != -1){
                iftrap = false;
                break;
            }
        }
        if(iftrap){
            tmpGroup.push_back(i.group_id);
        }
    }
    return tmpGroup;
}
void MinDFA::printOrderDfa(DFA dfa) {
    cout << endl;
    for(int i = 0;i < dfa.stateNum;i++){
        for(auto edge:DfaOrderGraph[i]){
            cout << i << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
}

void MinDFA::printFinalGroup() {
    cout << endl << "start print final groups" << endl;
    for(auto i:finalGroup){
        cout << i.group_id << " :";
        for(auto j:i.state_set){
            cout << j << " ";
        }
        cout << endl;
    }
}

void MinDFA::printMinDfaSe() {
    cout << endl;
    cout << "MinDfaStart:";
    for(auto i:se.first){
        cout << i << " ";
    }
    cout << endl << "MinDfaEnd:";
    for(auto i:se.second){
        cout << i << " ";
    }
    cout << endl;
}

void MinDFA::printMinDfaGraph() {
    cout << "start print MinDfaGraph:" << endl;
    for(int i = 1;i < DMgraph.size();i++){
        for(auto j:DMgraph[i]){
            cout << i-1 << "----" <<  j.second << "---->" << j.first << endl;
        }
    }
    cout << endl;
}

void MinDFA::RG() {
    cout << "start print RG:" << endl;
    for(int i = 1;i < DMgraph.size();i++){
        for(auto j:DMgraph[i]){
            cout << i-1 << "--->" << j.second << j.first << endl;
        }
    }
}
void MinDFA::lastTermPrint1() {
    cout << "      0 1" << endl;
    for(int i = 1;i < DMgraph.size();i++){
        if(ifInTrap(i-1)){
            //如果是陷阱状态 前面啥也不输出
        }
        else if(ifInStart(i-1)){
            cout << "(s)";
        }
        else if(ifInend(i-1)){
            cout << "(e)";
        }
        cout << "q" << i-1 << " ";
        for(auto j:DMgraph[i]){
            if(j.second == '0'){
                cout << "q" << j.first << " ";
            }
            else{
                cout << "q" << j.first;
            }
        }
        cout << endl;
    }
}
void MinDFA::lastTermPrint3() {
    cout << "      0 1" << endl;
    for(int i = 1;i < DMgraph.size();i++){
        if(ifInStart(i-1)){
            cout << "(s)";
            preStartState.push_back(i-1);
        }
        else if(ifInend(i-1)){
            cout << "(e)";
            preEndState.push_back(i-1);
        }
        cout << "q" << i-1 << " ";
        if(DMgraph[i].size() == 2){
            preGraph[i-1][0] = DMgraph[i][0].first;
            preGraph[i-1][1] = DMgraph[i][1].first;
            for(auto j:DMgraph[i]){
                if(j.second == '0'){
                    cout << "q" << j.first << " ";

                }
                else{
                    cout << "q" << j.first;

                }
            }
            cout << endl;
        }
        else{
            if(DMgraph[i].size() == 0){
                cout << "N" << " " << "N";
                cout << endl;
                preGraph[i-1][0] = -1;
                preGraph[i-1][1] = -1;
            }
            else if(DMgraph[i].size() == 1){
                if(DMgraph[i][0].second == '0'){
                    cout << "q" << DMgraph[i][0].first << " ";
                    cout << "N";
                    cout << endl;
                    preGraph[i-1][0] = DMgraph[i][0].first;
                    preGraph[i-1][1] = -1;
                }
                else if(DMgraph[i][0].second == '1'){
                    cout << "N" << " ";
                    cout << "q" << DMgraph[i][0].first;
                    cout << endl;
                    preGraph[i-1][1] = DMgraph[i][0].first;
                    preGraph[i-1][0] = -1;
                }
            }
        }
    }
}
void MinDFA::lastTermPrint2() {
    for(int i = 1;i < DMgraph.size();i++){
        bool flag = false;
        for(auto j:DMgraph[i]){
            if(ifInend(j.first)){
                flag = true;
            }
            cout << "q" << i-1 << "->" << j.second << "q" << j.first << endl;
        }
        if(flag){
            for(auto j:DMgraph[i]){
                if(ifInend(j.first)){
                    cout << "q" << i-1 << "->" << j.second << endl;
                }
            }
        }
    }
}
void MinDFA::lastTermPrint4() {
    for(int i = 1;i < DMgraph.size();i++){
        bool flag = false;
        for(auto j:DMgraph[i]){
            if(ifInend(j.first)){
                flag = true;
                bool flag2 = false;
                for(auto k:DMgraph[i]){
                    if(!ifInend(k.first)){
                        flag2 = true;
                        break;
                    }
                }
                if(!flag2){
                    cout << "q" << i-1 << "->" << j.second << "q" << j.first << endl;
                }
            }
            else{
                cout << "q" << i-1 << "->" << j.second << "q" << j.first << endl;
            }
        }
        if(flag){
            for(auto j:DMgraph[i]){
                if(ifInend(j.first)){
                    cout << "q" << i-1 << "->" << j.second << endl;
                }
            }
        }
    }
}
bool MinDFA::ifInend(int i) {
    for(auto j:se.second){
        if (j == i){
            return true;
        }
    }
    return false;
}
bool MinDFA::ifInTrap(int i){
    for(auto j:trap){//trap里装的是组号
        if(j == i){
            return true;
        }
    }
    return false;
}
bool MinDFA::ifInStart(int i) {
    for(auto j:se.first){
        if (j == i){
            return true;
        }
    }
    return false;
}
bool MinDFA::ifInReEnd(int i){
    for(auto j:reOrderse.second){
        if (j == i){
            return true;
        }
    }
    return false;
}
bool MinDFA::ifInReStart(int i){
    for(auto j:reOrderse.first){
        if (j == i){
            return true;
        }
    }
    return false;
}
int MinDFA::myfinalreorder(){
    int pre_start_state = preStartState[0];
    int count = 0;
    int count2 = 0;
    myfinalStartState.push_back(count);
    queue<int> myfinal_order;
    vector<int> already_ordered;
    vector<pair<int,int> > represent_state;
    represent_state.push_back(make_pair(count2,pre_start_state));
    already_ordered.push_back(pre_start_state);
    myfinal_order.push(pre_start_state);
    while(!myfinal_order.empty()){
        int pre_state = myfinal_order.front();
        myfinal_order.pop();
        for(auto k:preEndState){
            if(k == pre_state){
                myfinalEndState.push_back(count);
            }
        }
        int next_state0 = preGraph[pre_state][0];
        bool new_state_flag = false;
        if(preGraph[pre_state][0] == -1){
            myfinalGraph[count][0] = -1;
            new_state_flag = true;
        }
        else{
            for(auto l:represent_state){
                if(l.second == next_state0){
                    myfinalGraph[count][0] = l.first;
                    new_state_flag = true;
                    break;
                }
            }
        }
        if(!new_state_flag){
            myfinalGraph[count][0] = ++count2;
            represent_state.push_back(make_pair(count2, next_state0));
        }
        int next_state1 = preGraph[pre_state][1];
        bool new_state_flag2 = false;
        if(preGraph[pre_state][1] == -1){
            myfinalGraph[count][1] = -1;
            new_state_flag2 = true;
        }
        else{
            for(auto l:represent_state){
                if(l.second == next_state1){
                    myfinalGraph[count][1] = l.first;
                    new_state_flag2 = true;
                    break;
                }
            }
        }
        if(!new_state_flag2){
            myfinalGraph[count][1] = ++count2;
            represent_state.push_back(make_pair(count2, next_state1));
        }
        //myfinalGraph[count][0] = preGraph[pre_state][0];//这里的问题 这个赋过去的还是旧值
        //myfinalGraph[count][1] = preGraph[pre_state][1];
        bool flag2 = false;
        if(preGraph[pre_state][0] == -1 && preGraph[pre_state][1] == -1){
            if(myfinal_order.empty()){
                flag2 = true;
            }
        }
        if(!flag2){
            count++;
        }
        if(preGraph[pre_state][0] != -1){
            bool flag = false;
            for(auto i:already_ordered){
                if(i == preGraph[pre_state][0]){
                    flag = true;
                    break;
                }
            }
            if(!flag){
                myfinal_order.push(preGraph[pre_state][0]);
                already_ordered.push_back(preGraph[pre_state][0]);
                //count2++;
            }
        }
        if(preGraph[pre_state][1] != -1){
            bool flag = false;
            for(auto i:already_ordered){
                if(i == preGraph[pre_state][1]){
                    flag = true;
                    break;
                }
            }
            if(!flag){
                myfinal_order.push(preGraph[pre_state][1]);
                already_ordered.push_back(preGraph[pre_state][1]);
                //count2++;
            }
        }
    }
    return count2;
}
void MinDFA::lastTermPrint5() {
    cout << "      0 1" << endl;
    for(int i = 0;i <= myfinalcount;i++){
        for(auto j:myfinalStartState){
            if(j == i){
                cout << "(s)";
            }
        }
        for(auto j:myfinalEndState){
            if(j == i){
                cout << "(e)";
            }
        }
        cout << "q" << i << " ";
        if(myfinalGraph[i][0] == -1){
            cout << "N" << " ";
        }
        else{
            cout << "q" << myfinalGraph[i][0] << " ";
        }
        if(myfinalGraph[i][1] == -1){
            cout << "N";
        }
        else{
            cout << "q" << myfinalGraph[i][1];
        }
        cout << endl;
    }
    cout << endl;
    for(int i = 0;i <= myfinalcount;i++){
        bool flag1 = false;
        bool flag2 = false;
        bool flag3 = false;
        bool flag4 = false;
        if(myfinalGraph[myfinalGraph[i][0]][0] == -1 && myfinalGraph[myfinalGraph[i][0]][1] == -1){
            //cout << "q" << i << "->" << "0";
            flag3 = true;
            //cout << endl;
            flag1 = true;
        }
        else{
            if(myfinalGraph[i][0] != -1){
                cout << "q" << i << "->" << "0q" << myfinalGraph[i][0];
                cout << endl;
            }
        }
        if(myfinalGraph[myfinalGraph[i][1]][0] == -1 && myfinalGraph[myfinalGraph[i][1]][1] == -1){
            //cout << "q" << i << "->" << "1";
            flag4 = true;
            //cout << endl;
            flag2 = true;
        }
        else{
            if(myfinalGraph[i][1] != -1){
                cout << "q" << i << "->" << "1q" << myfinalGraph[i][1];
                cout << endl;
            }
        }
        if(!flag1){
            for(auto j:myfinalEndState){
                if(j == myfinalGraph[i][0]){
                    cout << "q" << i << "->" << "0";
                    cout << endl;
                }
            }
        }
        if(!flag2){
            for(auto j:myfinalEndState){
                if(j == myfinalGraph[i][1]){
                    cout << "q" << i << "->" << "1";
                    cout << endl;
                }
            }
        }
        if(flag3){
            cout << "q" << i << "->" << "0";
            cout << endl;
        }
        if(flag4){
            cout << "q" << i << "->" << "1";
            cout << endl;
        }
    }
}

bool MinDFA::testlinerule(string test) {
    int count = 0;
    for(auto i:test){
        if(i == '0'){
            if(myfinalGraph[count][0] != -1){
                count = myfinalGraph[count][0];
            }
            else{
                return false;
            }
        }
        else{
            if(myfinalGraph[count][1] != -1){
                count = myfinalGraph[count][1];
            }
            else{
                return false;
            }
        }
    }
    for(auto i:myfinalEndState){
        if(i == count){
            return true;
        }
    }
    return false;
}
