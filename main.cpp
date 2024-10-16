#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt
///  g++ main.cpp -o main && ./main < input.txt

using namespace std;

struct Transition {
    int nextState;
    char writeSymbol;
    char direction;
};

class RTM {
private:
    string input;
    vector<int> states;
    vector<char> inputAlphabet, tapeAlphabet;
    map<pair<int, char>, Transition> transitions;
    vector<char> tape1, tape2, tape3;
    int head1, head2, head3;
    int currentState;
    int finalState;

public:
    RTM(const vector<int>& stateList, const vector<char>& inputAlpha, 
                            const vector<char>& tapeAlpha, int startState, int acceptState) {
        states = stateList;
        inputAlphabet = inputAlpha;
        tapeAlphabet = tapeAlpha;
        currentState = startState;
        finalState = acceptState;

        tape1 = vector<char>(1000, 'B');
        tape2 = vector<char>(1000, 'B');
        tape3 = vector<char>(1000, 'B');

        for (int i = 0; i < (int)input.size(); ++i) {
            tape1[i] = input[i];
        }

        head1 = head2 = head3 = 0;
    }

    void defineInput(const string& inputStr)
    {
        input = inputStr;
        for (int i = 0; i < (int)input.size(); ++i) {
            tape1[i] = input[i];
        }
    }

    void printTransitions() {
        cout << "Transicoes:\n";
        for (const auto& transition : transitions) {
            int currentState = transition.first.first;
            char readSymbol = transition.first.second;
            const Transition& t = transition.second;

            cout << "(" << currentState << "," << readSymbol << ") -> ("
                << t.nextState << "," << t.writeSymbol << "," << t.direction << ")\n";
        }
    }


    void addTransition(int state, char readSymbol, int nextState, char writeSymbol, char direction) {
        transitions[{state, readSymbol}] = {nextState, writeSymbol, direction};
    }

    bool applyTransition() {
        if (head1 < 0 || head1 >= (int) tape1.size()) {
            cout << "Erro: Cabecote fora dos limites da fita!" << endl;
            return false;
        }
        
        char readSymbol = tape1[head1];
        auto key = make_pair(currentState, readSymbol);

        cout << currentState << " " <<readSymbol << endl;
        if (transitions.find(key) != transitions.end()) {
            Transition t = transitions[key];
            tape1[head1] = t.writeSymbol;
            currentState = t.nextState;
            head1 += (t.direction == 'R') ? 1 : -1;
            return true;
        } else {
            cout << "Erro: Nenhuma transicao encontrada para (" << currentState << ", " << readSymbol << ")." << endl;
            return false;
        }
    }


    void stage1() {
        while (currentState != finalState) {
            cout << "Estado atual: " << currentState << ", Simbolo lido: " << tape1[head1] << endl;
            if (!applyTransition()) {
                cout << "Erro: Nenhuma transicao encontrada." << endl;
                return;
            }
        }
        cout << "Final do estagio 1. Estado final: " << currentState << endl;
    }

    void stage2() {
        for (int i = 0; tape1[i] != 'B'; ++i) {
            tape3[i] = tape1[i];
        }
        cout << "Final do estagio 2. Saida copiada para fita 3." << endl;
    }

    void stage3() {
        cout << "Estagio 3: Inversao das transicoes." << endl;
    }

    void execute() {
        stage1();
        stage2();
        stage3();
    }

    void printTape() {
        cout << "Fita 1: ";
        for (char c : tape1) {
            if (c != 'B') cout << c;
        }
        cout << endl;
        cout << "Fita 2: ";
        for (char c : tape2) {
            if (c != 'B') cout << c;
        }
        cout << endl;
        cout << "Fita 3: ";
        for (char c : tape3) {
            if (c != 'B') cout << c;
        }
        cout << endl;
    }
};

int main() {
    int numStates, inputAlphabetSize, tapeAlphabetSize, numTransitions;

    cin >> numStates >> inputAlphabetSize >> tapeAlphabetSize >> numTransitions;

    vector<int> states(numStates);
    vector<char> inputAlphabet(inputAlphabetSize);
    vector<char> tapeAlphabet(tapeAlphabetSize);

    for (int i = 0; i < numStates; ++i) {
        cin >> states[i];
    }

    for (int i = 0; i < inputAlphabetSize; ++i) {
        cin >> inputAlphabet[i];
    }

    for (int i = 0; i < tapeAlphabetSize; ++i) {
        cin >> tapeAlphabet[i];
    }

    RTM tm(states, inputAlphabet, tapeAlphabet, states[0], states[numStates-1]);

    std::string line;
    std::getline(std::cin, line);
    for (int i = 0; i < numTransitions; ++i) {
        std::string line;
        std::getline(std::cin, line);
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) continue; 

        std::string leftPart = line.substr(0, equalPos);  
        std::string rightPart = line.substr(equalPos + 1); 

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

    tm.printTape();
    return 0;
}
