#include "MinDFA.h"
int main() {
    string name,re,testline;
    //cout << "input the name of re:";
    //cin >> name;
    name = "hualidefeng";
    //cout << "input the re:";
    cin >> re;
    cin >> testline;
    NFA nfa(name,re);
    nfa.insertContact();
    nfa.rTop();
    nfa.pToNfa();
    nfa.printNfa();

    DFA dfa(0,name);
    dfa.findStart(nfa);
    dfa.newState(nfa,0,'0');
    dfa.newState(nfa,0,'1');
//    dfa.newState(nfa,4,'0');
//    dfa.newState(nfa,4,'1');
    dfa.printRepresent();
    dfa.printDfa();

    MinDFA mindfa(0,name);
    mindfa.orderDfa(dfa);
    mindfa.printOrderDfa(dfa);
    mindfa.mergeDfa(dfa);
    mindfa.printFinalGroup();
    mindfa.printMinDfaSe();
    mindfa.printMinDfaGraph();
    mindfa.RG();
    cout << endl;
    mindfa.lastTermPrint3();
    cout << endl;
    mindfa.lastTermPrint4();

    mindfa.myfinalcount = mindfa.myfinalreorder();
    cout << endl;
    mindfa.lastTermPrint5();

    bool aline = mindfa.testlinerule(testline);
    if(aline){
        cout << "true";
    }else{
        cout << "false";
    }

    return 0;
}
