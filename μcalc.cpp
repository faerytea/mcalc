   // C++11
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

using namespace std;

struct internalExpressionFormat {
    list <char>      operators;
    list <long long> numbers;
    char             flag;        // for internal information
    internalExpressionFormat () {
        operators.clear();
        operators.clear();
        flag = 'n';               // n - normal
    }
};

bool higherPriority (char a, char b) {  // Compare sign priority 
    char aPriority, bPriority;          // true, if first sign (a) has a higher priority than second sign (b);
    switch (a) {
    case 1:       // Stop-operator; very low priority. It need for easily parsing
        aPriority = 0;
        break;
    case '+':
    case '-':
        aPriority = 1;
        break;
    case '*':
    case '/':
    case '%':
        aPriority = 2;
        break;
    }
    switch (b) {
    case 1:
        bPriority = 0;
        break;
    case '+':
    case '-':
        bPriority = 1;
        break;
    case '*':
    case '/':
    case '%':
        bPriority = 2;
        break;
    }
    if (aPriority > bPriority) {
        return true;
    }
    else {
        return false;
    }
};

internalExpressionFormat readExp () {  // read expression from stdin and translate to postfix form
    bool endOfStream = false; // if true, exp. parsed
    bool isLastOp = true;     // true, if last significant symbol was operator
    bool minus = false;       // true, if next number must be negativ
    bool isNum = false;       // true, if last significant symbol was number
    char nextChar;            // next char from cin
    long bracketsBalance = 0; // obvious
    long long tempNum;        // number, which reading now
    list <char> sideStack;    // stack for operators
    sideStack.clear();
    sideStack.push_back(1);   // stop-operator
    internalExpressionFormat res;
    while (!endOfStream) {
        nextChar = cin.get();
        switch (nextChar) {
        case '0':  // next char is part of number
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (isNum) {
                tempNum *= 10;
                tempNum += nextChar-'0';
            }
            else {
                isNum = true;
                tempNum = nextChar - '0';
                if (minus) {
                    tempNum *= -1;
                    minus = false;
                }
            }
            isLastOp = false;
            break;
        case '-':  // next char is '-'. Maybe it is part of negative number?
            if (isLastOp) {
                minus = ~minus;
                break;
            }
        case '+':  // next char is math operator (including '-')
        case '*':
        case '/':
        case '%':
            if (isNum) {  // Before it, we read number. This is end og number, so add number to parced exp.
                res.numbers.push_back(tempNum);
                res.operators.push_back('n');
                isNum = false;
            }             // Operator after operator. It is not good.
            if (isLastOp) {
                cerr<<"Parsing error - too many operators"<<endl;
                res.flag = 'e';
                return res;
                break;
            }             // translate operation
            if (!higherPriority(nextChar,sideStack.back())) {
                res.operators.push_back(sideStack.back());
                sideStack.pop_back();
            }
            sideStack.push_back(nextChar);
            isLastOp = true;
            break;
        case '(':
            bracketsBalance++;
            if (isNum) {  // Before it, we read number. This is end og number, so add number to parced exp.
                res.numbers.push_back(tempNum);
                res.operators.push_back('n');
                isNum = false;
            }
            sideStack.push_back('(');  // translate operation
            break;
        case ')':
            bracketsBalance--;
            if (isNum) {  // Before it, we read number. This is end og number, so add number to parced exp.
                res.numbers.push_back(tempNum);
                res.operators.push_back('n');
                isNum = false;
            }
            while (sideStack.back() != '(') {  // translate operation
                if (sideStack.size() == 1) {   // '(' wasn't found =(
                    cerr<<"Parsing error - lost left bracket"<<endl;
                    res.flag = 'e';
                    return res;
                    break;
                }
                res.operators.push_back(sideStack.back());
                sideStack.pop_back();
            }
            sideStack.pop_back();
            break;
        case 'q':  // q for quit
            res.flag = 'q';
        case '.':  // Just cruth for test. Don't need now
        case '\n': // End of input
            if (isNum) {
                res.numbers.push_back(tempNum);
                res.operators.push_back('n');
                isNum = false;
            }
            endOfStream = true;
            break;
        case ' ': // Ignoring all spaces
            break;
        default:  // Unexcepted symbol
            cerr<<"Parsing error - wrong symbol ('"<<nextChar<<"')"<<endl;
        }
    }
    while (!sideStack.empty()) {  // End of translation
        res.operators.push_back(sideStack.back());
        sideStack.pop_back();
    }
    if (bracketsBalance != 0) {
        cerr<<"Parsing error - brackets are not balanced."<<endl;
        res.flag = 'e';
    }
    return res;
};

long long calculate (internalExpressionFormat exp) {  // calculating exp. and gives an answer
    list <long long>::iterator itNum = exp.numbers.begin();
    list <char>::iterator      itOp  = exp.operators.begin();
    itOp++;
    long long a,b;
    char oper;
    if (exp.flag != 'n') {
        cerr<<"Something went wrong, calculation aborting"<<endl;
        return 0;
    }
    if (exp.operators.size() == 1) {     // There is only one operator in expression. It is stop. 
        cout<<"Nothing to do";
        return 0;
    }
    while (exp.operators.size() != 2) {
        while (*itOp == 'n') {  // Find first sign
            itOp++;
            itNum++;
        }
        oper = *itOp;           // Remember this sign
        if (oper == 1) {        // End while more then 2 (n1) operators? So bad.
            cerr<<"Calculating error - too many operands"<<endl;
            return 0;
        }
        itOp--;
        if ((itOp == exp.operators.begin())||(itNum == exp.numbers.begin())) {
            cerr<<"Calculating error - not enough operands"<<endl;
            return 0;
        }
        itOp--;
        itOp = exp.operators.erase(itOp);
        itOp++;
        itOp = exp.operators.erase(itOp); // replasing nn+ in exp with n
        itNum--;
        a = *itNum;                       // remember operands
        itNum = exp.numbers.erase(itNum); // and replasing one of them with result
        b = *itNum;
        switch (oper) { // calculating
        case '+':
            *itNum = a+b;
            break;
        case '-':
            *itNum = a-b;
            break;
        case '*':
            *itNum = a*b;
            break;
        case '/':
            if (b == 0) {
                cerr<<"Calculating error - division by zero"<<endl;
                return 0;
            }
            else {
                *itNum = a/b;
            }
            break;
        case '%':
            if (b == 0) {
                cerr<<"Calculating error - division by zero"<<endl;
                return 0;
            }
            else {
                *itNum = a%b;
            }
            break;
        }
    }
    return exp.numbers.front();
}

int main (int argc, const char* argv[]) {
    // Don't panic, it is just text 'How to'
    string help      = "This is calculator. It can calculate expression with decimal numbers, operations + - * / % and brackets.\nUsage: μcalc <command> [expression]\n\nCommands:\n\to - One expression\n\tm - More than one expressions\n\t    (enter q for quit)\n\tc - Get expression from arguments of command line // sadly, but don't working now\n\t    (for correct work you must separate operands and operators (including brackets) with whitespaces)\n\th - print help",
           operators = "Operators:\n\t+ addition\n\t- subtraction\n\t* multiplication\n\t/ integer division (round down)\n\t% modulo\n\t( and ) change priority\n\n* / % have higher priority than + -, but you can use brackets.",
           wrong     = "For help type \"μcalc h\"",
           hello     = "Print expression and press Enter";
    switch (argc) { 
        case 1: {          // No command - print help and exit
            cout<<help<<endl;
            break;
        }
        case 2: {          // It is a 'h', 'o' or 'm'
            switch (argv[1][0]) {
                case 'h': {       // h - print full help and exit
                    cout<<help<<endl<<'\n'<<operators<<endl;
                    break;
                }
                case 'o': {       // o - get expression from stdin and give a answer throgh stdout
                    cout<<hello<<endl<<"> ";
                    cout<<"\t= "<<calculate(readExp())<<endl;
                    break;
                }
                case 'm': {        // m - not one exp.
                    cout<<hello<<endl;
                    bool quitFlag = false;
                    do {
                        cout<<"> ";                                        // get next input
                        internalExpressionFormat tempExpForm = readExp();  // from stdit and
                        switch (tempExpForm.flag) {                        // parce it 
                            case 'q': {          // q for quit
                                quitFlag = true;
                                break;
                            }
                            case 'e': {          // catch a error
                                cout<<"You give wrong expression! Try again, if you want. "<<wrong<<endl;
                                break;
                            }
                            case 'n': {          // It is right exp., so calculate it
                                cout<<"\t= "<<calculate(tempExpForm)<<endl;
                                break;
                            }
                            default: {
                                cerr<<"Unknown error.";
                                cout<<"Something went wrong..."<<endl;
                                break;
                            }
                        }
                    } while (!quitFlag);
                    break;
                }
                default: {        // any other input is wrong
                    cout<<"Wrong argument ("<<argv[1]<<")! "<<wrong<<endl;
                    break;
                }
            }
            break;
        }
        default: {         // 3+ args; It is must be "c <expression>" (like "calc c ( 5 + 17 ) % 7"
            if (argv[1][0] != 'c') {
                cout<<"Wrong arguments ('"<<argv[1]<<"')! "<<wrong<<endl;
            }
            else {
                // Getting expression
                internalExpressionFormat expFromCL;
                list <char> sideStack;
                sideStack.clear();
                sideStack.push_back(1);
                for (int i = 2; i < argc; i++) {  // Translate to postfix form
                    string tempArg = argv[i];
                    if (tempArg.size() == 1) {
                        switch (argv[i][0]) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            expFromCL.numbers.push_back(tempArg[0]-'0');
                            expFromCL.operators.push_back('n');
                            break;
                        case '(':
                            sideStack.push_back('(');
                            break;
                        case ')':
                            while (sideStack.back() != '(') {
                                if (sideStack.size() == 1) {
                                    expFromCL.flag = 'e';
                                    break;
                                }
                                expFromCL.operators.push_back(sideStack.back());
                                sideStack.pop_back();
                            }
                            sideStack.pop_back();
                            break;
                        case '+':
                        case '-':
                        case '*':
                        case '/':
                        case '%':
                            if (!higherPriority(tempArg[0],sideStack.back())) {
                                expFromCL.operators.push_back(sideStack.back());
                                sideStack.pop_back();
                            }
                            sideStack.push_back(tempArg[0]);
                            break;
                        default:
                            cerr<<"Wrong input - '"<<tempArg[0]<<"'! It must was + - * / % or 0..9."<<endl;
                            cout<<"I can't parse "<<tempArg[0]<<endl;
                        }
                    }
                    else {
                        try {
                            long long t = stoll(argv[i]);
                            expFromCL.numbers.push_back(t);
                            expFromCL.operators.push_back('n');
                        }
                        catch (const out_of_range& e) {
                            cerr<<"Out of range: "<<e.what()<<endl;
                            cout<<"You gave too big number. I can't correct handle this, so programm now suicide."<<endl;
                            exit(0);
                        }
                        catch (const invalid_argument& e) {
                            cerr<<"Not a valid number: "<<e.what()<<endl;
                            exit(0);
                        }
                    }
                }
                while (!sideStack.empty()) {
                    expFromCL.operators.push_back(sideStack.back());
                    sideStack.pop_back();
                }
                if (expFromCL.flag == 'n') {
                    cout<<"\t= "<<calculate(expFromCL)<<endl;
                }
                else {
                    cout<<"Input wasn't parsed."<<endl;
                }
            }
        }
    }
    return 0;
}
