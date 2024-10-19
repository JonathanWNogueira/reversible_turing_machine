#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

struct Transition {
    int nextState;
    char writeSymbol;
    char direction;
};

struct Quadruple {
    int state;           // Estado atual
    char symbol;         // Símbolo lido ou escrito
    int nextState;       // Estado para onde vai
    char direction;      // 'N' para não mover a cabeça, 'L' para esquerda, 'R' para direita
};

struct Tape {
    vector<char> content;
    int headPosition;
};

pair<Quadruple, Quadruple> breaksQuintupleApart(pair<int, char> current, Transition t) {
    // Primeira quádrupla: leitura e escrita do símbolo, mas ainda sem mover a cabeça
    Quadruple firstQuadruple = {
        current.first,    // Estado atual
        current.second,   // Símbolo lido
        t.nextState,      // Estado intermediário ou próximo estado
        t.writeSymbol     // Estado a ser escrito
    };

    // Segunda quádrupla: representa o movimento da cabeça após a escrita
    Quadruple secondQuadruple = {
        t.nextState,      // Estado intermediário
        t.writeSymbol,    // Símbolo escrito na fita
        t.nextState,      // Próximo estado final
        t.direction       // Direção em que a cabeça se move
    };

    return make_pair(firstQuadruple, secondQuadruple);
}

pair<Quadruple, Quadruple> revertsQuadruple(pair<Quadruple, Quadruple> quads)
{
    // Extraindo as quádruplas a serem revertidas
    Quadruple firstQuadruple = quads.first;
    Quadruple secondQuadruple = quads.second;

    // Revertendo a segunda quádrupla (que se torna a primeira no sentido reverso)
    Quadruple reversedFirst = {
        secondQuadruple.nextState, // Estado final se torna o estado inicial
        secondQuadruple.symbol,    // Símbolo escrito na fita
        secondQuadruple.state,     // Volta ao estado intermediário (ou original)
        secondQuadruple.direction  // Sem movimento da cabeça ao reverter
    };

    // Revertendo a primeira quádrupla (que se torna a segunda no sentido reverso)
    Quadruple reversedSecond = {
        firstQuadruple.nextState,  // Estado intermediário se torna o estado final
        firstQuadruple.symbol,     // Símbolo lido originalmente
        firstQuadruple.state,      // Volta ao estado inicial
        firstQuadruple.direction   // Reverter a direção do movimento (L -> R, R -> L)
    };

    // Para garantir reversão correta, trocamos 'L' por 'R' e vice-versa:
    if (reversedFirst.direction == 'L') {
        reversedFirst.direction = 'R';
    } else if (reversedFirst.direction == 'R') {
        reversedFirst.direction = 'L';
    }

    return make_pair(reversedFirst, reversedSecond);
}

class RTM {
 private:
    string input;
    vector<int> states;
    vector<char> inputAlphabet, tapeAlphabet;
    map<pair<int, char>, Transition> transitions;
    map<pair<int, char>, Quadruple> reversibleTransitions;
    map<pair<int, char>, Quadruple> reversedTransitions;
    Tape inputTape, historyTape, outputTape;
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

        inputTape = {vector<char>(1000, 'B'), 0};
        historyTape = {vector<char>(1000, 'B'), 0};
        outputTape = {vector<char>(1000, 'B'), 0};
    }

    void defineInput(const string& inputStr) {
        input = inputStr;
        for (int i = 0; i < (int) input.size(); i++) {
            inputTape.content[i] = input[i];
        }
    }

    void addTransition(int state, char readSymbol, int nextState, char writeSymbol, char direction) {
        transitions[{state, readSymbol}] = {nextState, writeSymbol, direction};
    }

    pair<int,char> getActualKey()
    {
        char readSymbol = inputTape.content[inputTape.headPosition];
        auto key = make_pair(currentState, readSymbol);

        return key;
    }

    Transition getTransition(pair<int,char> key)
    {
        if (transitions.find(key) != transitions.end()) {
            Transition t = transitions[key];
            return t;
        }
        return {0, 'N', 'N'};
    }

    void printQuadruple(pair<Quadruple, Quadruple> q)
    {
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

    bool applyTransition(pair<Quadruple, Quadruple> transition)
    {
        if (inputTape.headPosition < 0 || inputTape.headPosition >= (int) inputTape.content.size()) {
            cout << "Erro: Cabecote fora dos limites da fita! === " << inputTape.headPosition << endl;
            return false;
        }
        else if (transition.first.direction == 'N')
        {
            cout << "Erro: transicao nao existente" << endl;
            return false;
        }
        pair<int,char> keyMove = getActualKey();
        // printf("KEY : %c, %c, HEAD: %d\n",'0' + keyMove.first, keyMove.second, historyTape.headPosition);
        historyTape.content[historyTape.headPosition] = '0' + keyMove.first;
        historyTape.headPosition++;
        historyTape.content[historyTape.headPosition] = keyMove.second;
        historyTape.headPosition++;

        Quadruple first = transition.first;
        Quadruple second = transition.second;

        printQuadruple(transition);

        inputTape.content[inputTape.headPosition] = first.direction;

        if (second.direction == 'L') {
            inputTape.headPosition--;
        } else if (second.direction == 'R') {
            inputTape.headPosition++;
        }


        currentState = second.nextState;

        return true;
    }

    bool applyReversedTransition(pair<Quadruple, Quadruple> transition)
    {
        Quadruple first = transition.first;
        Quadruple second = transition.second;

        printQuadruple(transition);

        // Step 1: Revert the second quadruple first (move head back)
        if (first.direction == 'L') {
            inputTape.headPosition--;
        } else if (first.direction == 'R') {
            inputTape.headPosition++;
        }

        // Step 2: Revert the first quadruple (write the original symbol)
        inputTape.content[inputTape.headPosition] = second.symbol;
        
        return true;
    }

    void stage1() {
        while (currentState != finalState) {
            cout << "Estado atual: " << currentState << ", Simbolo lido: " << inputTape.content[inputTape.headPosition] << endl;
            if(applyTransition(breaksQuintupleApart(getActualKey(), getTransition(getActualKey()))))
            { } else {
                cout << "deu merda" << endl;
                break;
            };
        }
        cout << "Final do estagio 1. Estado final: " << currentState << endl;
    }

    void stage2() {
        for (int i = 0; i < (int)inputTape.content.size(); i++) {
            outputTape.content[i] = inputTape.content[i];
        }

        cout << "Final do estagio 2. Saida copiada para fita 3." << endl;
    }
    
    void stage3() {
        cout << "Estagio 3: Inversao das transicoes." << endl;
        // Percorre a historyTape e aplica as transições revertidas
        for (int i = historyTape.headPosition - 2; i >= 0; i-=2) {
            int keyState = historyTape.content[i] - '0';
            char keySymbol = historyTape.content[i+1];
            pair<int,char> key = make_pair(keyState,keySymbol);

            // cout << historyTape.content[i] << " " << historyTape.content[i+1] << endl;

            // Revertendo as transições salvas no histórico (use o método `revertsQuadruple`)
            pair<Quadruple, Quadruple> reversedTransition = revertsQuadruple(breaksQuintupleApart(key, getTransition(key)));
            applyReversedTransition(reversedTransition);
            historyTape.content[i] = 'B';
            historyTape.content[i+1] = 'B';
        }
    }

    void execute() {
        stage1();
        cout << "Fita 2: ";
        for (char c : historyTape.content) {
            if (c != 'B') 
            {
                printf("%c", c) << c;
            }
        }
        cout << endl;
        stage2();
        stage3();
    }

    void printTape() {
        cout << "Fita 1: ";
        for (char c : inputTape.content) {
            if (c != 'B') cout << c;
        }
        cout << endl;
        cout << "Fita 2: ";
        for (char c : historyTape.content) {
            if (c != 'B') 
            {
                printf("%d", c) << c;
            }
        }
        cout << endl;
        cout << "Fita 3: ";
        for (char c : outputTape.content) {
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

    for (int i = 0; i < numStates; i++) {
        cin >> states[i];
    }

    for (int i = 0; i < inputAlphabetSize; i++) {
        cin >> inputAlphabet[i];
    }

    for (int i = 0; i < tapeAlphabetSize; i++) {
        cin >> tapeAlphabet[i];
    }

    RTM tm(states, inputAlphabet, tapeAlphabet, states[0], states[numStates-1]);

    std::string line;
    std::getline(std::cin, line);
    for (int i = 0; i < numTransitions; i++) {
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
