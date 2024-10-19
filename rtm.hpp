#ifndef __RTM_HPP__
#define __RTM_HPP__

#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

struct Transition {
    int nextState;
    char writeSymbol;
    char direction;
};

struct Quadruple {
    int state;
    char symbol;
    int nextState;
    char direction;
};

struct Tape {
    vector<char> content;
    int headPosition;
};

class RTM {
private:
    string input;
    vector<int> states;
    vector<char> inputAlphabet, tapeAlphabet;
    map<pair<int, char>, Transition> transitions;
    Tape inputTape, historyTape, outputTape;
    int currentState;
    int finalState;

    pair<Quadruple, Quadruple> breaksQuintupleApart(pair<int, char> current, Transition t);
    pair<Quadruple, Quadruple> revertsQuadruple(pair<Quadruple, Quadruple> quads);
    void printQuadruple(pair<Quadruple, Quadruple> q);

    pair<int, char> getActualKey();
    Transition getTransition(pair<int, char> key);

    bool applyTransition(pair<Quadruple, Quadruple> transition);
    bool applyReversedTransition(pair<Quadruple, Quadruple> transition);

public:
    RTM(const vector<int>& stateList, const vector<char>& inputAlpha, const vector<char>& tapeAlpha, int startState, int acceptState);
    void defineInput(const string& inputStr);
    void addTransition(int state, char readSymbol, int nextState, char writeSymbol, char direction);
    void stage1();
    void stage2();
    void stage3();
    void execute();
    void printTape();
};

#endif
