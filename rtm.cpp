#include "RTM.hpp"

using namespace std;

RTM::RTM(const vector<int>& stateList, const vector<char>& inputAlpha, const vector<char>& tapeAlpha, int startState, int acceptState) {
    states = stateList;
    inputAlphabet = inputAlpha;
    tapeAlphabet = tapeAlpha;
    currentState = startState;
    finalState = acceptState;

    inputTape = {vector<char>(1000, 'B'), 0};
    historyTape = {vector<char>(1000, 'B'), 0};
    outputTape = {vector<char>(1000, 'B'), 0};
}

void RTM::defineInput(const string& inputStr) {
    input = inputStr;
    for (int i = 0; i < (int) input.size(); i++) {
        inputTape.content[i] = input[i];
    }
}

void RTM::addTransition(int state, char readSymbol, int nextState, char writeSymbol, char direction) {
    transitions[{state, readSymbol}] = {nextState, writeSymbol, direction};
}

pair<int, char> RTM::getActualKey() {
    char readSymbol = inputTape.content[inputTape.headPosition];
    return make_pair(currentState, readSymbol);
}

Transition RTM::getTransition(pair<int, char> key) {
    if (transitions.find(key) != transitions.end()) {
        return transitions[key];
    }
    return {0, 'N', 'N'};
}

pair<Quadruple, Quadruple> RTM::breaksQuintupleApart(pair<int, char> current, Transition t) {
    Quadruple firstQuadruple = {current.first, current.second, t.nextState, t.writeSymbol};
    Quadruple secondQuadruple = {t.nextState, t.writeSymbol, t.nextState, t.direction};
    return make_pair(firstQuadruple, secondQuadruple);
}

pair<Quadruple, Quadruple> RTM::revertsQuadruple(pair<Quadruple, Quadruple> quads) {
    Quadruple firstQuadruple = quads.first;
    Quadruple secondQuadruple = quads.second;

    Quadruple reversedFirst = {secondQuadruple.nextState, secondQuadruple.symbol, secondQuadruple.state, secondQuadruple.direction};
    Quadruple reversedSecond = {firstQuadruple.nextState, firstQuadruple.symbol, firstQuadruple.state, firstQuadruple.direction};

    if (reversedFirst.direction == 'L') reversedFirst.direction = 'R';
    else if (reversedFirst.direction == 'R') reversedFirst.direction = 'L';

    return make_pair(reversedFirst, reversedSecond);
}

void RTM::printQuadruple(pair<Quadruple, Quadruple> q) {
    cout << "first Quadruple: (" 
         << q.first.state << ", " 
         << q.first.symbol << ", " 
         << q.first.nextState << ", " 
         << q.first.direction << ")" << endl;

    cout << "second Quadruple: (" 
         << q.second.state << ", " 
         << q.second.symbol << ", " 
         << q.second.nextState << ", " 
         << q.second.direction << ")" << endl << endl;
}

bool RTM::applyTransition(pair<Quadruple, Quadruple> transition) {
    if (inputTape.headPosition < 0 || inputTape.headPosition >= (int) inputTape.content.size()) {
        cout << "Erro: Cabecote fora dos limites da fita!" << endl;
        return false;
    }
    
    if (transition.first.direction == 'N') {
        cout << "Erro: transição não existente." << endl;
        return false;
    }

    pair<int, char> keyMove = getActualKey();
    historyTape.content[historyTape.headPosition++] = '0' + keyMove.first;
    historyTape.content[historyTape.headPosition++] = keyMove.second;

    Quadruple first = transition.first;
    Quadruple second = transition.second;

    inputTape.content[inputTape.headPosition] = first.direction;

    if (second.direction == 'L') {
        inputTape.headPosition--;
    } else if (second.direction == 'R') {
        inputTape.headPosition++;
    }

    currentState = second.nextState;

    cout << "Simbolo escrito: " << inputTape.content[inputTape.headPosition] << ", Proximo Estado: " << currentState   << endl;

    return true;
}

bool RTM::applyReversedTransition(pair<Quadruple, Quadruple> transition) {
    Quadruple first = transition.first;
    Quadruple second = transition.second;

    if (first.direction == 'L') {
        inputTape.headPosition--;
    } else if (first.direction == 'R') {
        inputTape.headPosition++;
    }

    inputTape.content[inputTape.headPosition] = second.symbol;

    return true;
}

void RTM::stage1() {
    while (currentState != finalState) {
        cout << endl << "Estado atual: " << currentState << ", Simbolo lido: " << inputTape.content[inputTape.headPosition] << endl;
        if (!applyTransition(breaksQuintupleApart(getActualKey(), getTransition(getActualKey())))) {
            cout << "Erro durante a aplicação da transição." << endl;
            break;
        }
    }
    cout << endl << "Final do estagio 1. Estado final: " << currentState << endl;
    printTape();
}

void RTM::stage2() {
    for (int i = 0; i < (int)inputTape.content.size(); i++) {
        outputTape.content[i] = inputTape.content[i];
    }
    cout << "Final do estagio 2. Saida copiada para fita 3." << endl;
    printTape();
}

void RTM::stage3() {
    cout << "Estagio 3: Inversao das transicoes." << endl;
    for (int i = historyTape.headPosition - 2; i >= 0; i -= 2) {
        int keyState = historyTape.content[i] - '0';
        char keySymbol = historyTape.content[i + 1];
        pair<int, char> key = make_pair(keyState, keySymbol);
        
        pair<Quadruple, Quadruple> reversedTransition = revertsQuadruple(breaksQuintupleApart(key, getTransition(key)));
        applyReversedTransition(reversedTransition);
        historyTape.content[i] = 'B';
        historyTape.content[i + 1] = 'B';
    }
    printTape();
}

void RTM::execute() {
    stage1();
    stage2();
    stage3();
}

void RTM::printTape() {
    cout << endl << "Input: ";
    for (char c : inputTape.content) {
        if (c != 'B') cout << c;
    }
    cout << endl;

    cout << "History: ";
    for (char c : historyTape.content) {
        if (c != 'B') cout << c;
    }
    cout << endl;

    cout << "Output: ";
    for (char c : outputTape.content) {
        if (c != 'B') cout << c;
    }
    cout << endl << endl;
}
