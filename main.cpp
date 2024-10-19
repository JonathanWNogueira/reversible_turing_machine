#include <iostream>
#include "RTM.hpp"

int main() {
    int numStates, inputAlphabetSize, tapeAlphabetSize, numTransitions;

    cin >> numStates >> inputAlphabetSize >> tapeAlphabetSize >> numTransitions;

    vector<int> states(numStates);
    vector<char> inputAlphabet(inputAlphabetSize);
    vector<char> tapeAlphabet(tapeAlphabetSize);

    for (int i = 0; i < numStates; i++) {
        cin >> states[i];
    }

    for (int i = 0; i < inputAlphabetSize; i++) {
        cin >> inputAlphabet[i];
    }

    for (int i = 0; i < tapeAlphabetSize; i++) {
        cin >> tapeAlphabet[i];
    }

    RTM tm(states, inputAlphabet, tapeAlphabet, states[0], states[numStates - 1]);

    string line;
    getline(cin, line);

    for (int i = 0; i < numTransitions; i++) {
        getline(cin, line);
        size_t equalPos = line.find('=');
        if (equalPos == string::npos) continue;

        string leftPart = line.substr(0, equalPos);
        string rightPart = line.substr(equalPos + 1);

        int currentState;
        char readSymbol;
        sscanf(leftPart.c_str(), "(%d,%c)", &currentState, &readSymbol);

        int nextState;
        char writeSymbol, direction;
        sscanf(rightPart.c_str(), "(%d,%c,%c)", &nextState, &writeSymbol, &direction);

        tm.addTransition(currentState, readSymbol, nextState, writeSymbol, direction);
    }

    string input;
    cin >> input;

    tm.defineInput(input);
    tm.execute();

    return 0;
}
