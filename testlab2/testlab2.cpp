#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>

using namespace std;


class BinaryUtils {
public:
    static vector<bool> convertToBinary(int decimalNumber, int size) {
        vector<bool> bits(size, false);
        bool isNegative = decimalNumber < 0;
        if (isNegative) {
            decimalNumber = -decimalNumber;
        }

        for (int i = 0; i < size; ++i) {
            bits[size - 1 - i] = (decimalNumber >> i) & 1;
        }

        if (isNegative) {
            for (int i = 0; i < size; ++i) {
                bits[i] = !bits[i];
            }
            for (int i = size - 1; i >= 0; --i) {
                if (!bits[i]) {
                    bits[i] = true;
                    break;
                }
                bits[i] = false;
            }
        }

        return bits;
    }

    static int convertToDecimal(const vector<bool>& bits) {
        int size = bits.size();
        int decimalNumber = 0;
        bool isNegative = bits[0];

        if (isNegative) {
            for (int i = 1; i < size; ++i) {
                decimalNumber |= (!bits[i]) << (size - 1 - i);
            }
            decimalNumber = -(decimalNumber + 1);
        }
        else {
            for (int i = 1; i < size; ++i) {
                decimalNumber |= bits[i] << (size - 1 - i);
            }
        }

        return decimalNumber;
    }
};

class BinaryArithmetic {
public:
    static vector<bool> add(const vector<bool>& a, const vector<bool>& b, int& carry) {
        int size = a.size();
        vector<bool> result(size, false);
        carry = 0;

        for (int i = size - 1; i >= 0; --i) {
            int sum = a[i] + b[i] + carry;
            result[i] = sum & 1;
            carry = sum >> 1;
        }

        return result;
    }

    static vector<bool> subtract(const vector<bool>& a, const vector<bool>& b, bool& borrow) {
        int size = a.size();
        vector<bool> result(size, false);
        borrow = false;

        for (int i = size - 1; i >= 0; --i) {
            bool diff = a[i] ^ b[i] ^ borrow;
            borrow = (!a[i] && borrow) || (!a[i] && b[i]) || (b[i] && borrow);
            result[i] = diff;
        }

        return result;
    }

    static vector<bool> shiftLeft(const vector<bool>& bits) {
        int size = bits.size();
        vector<bool> result(size, false);

        for (int i = 0; i < size - 1; ++i) {
            result[i] = bits[i + 1];
        }
        result[size - 1] = false;

        return result;
    }
};


class BinaryNumber {
private:
    vector<bool> bits;
    int size;

public:
    BinaryNumber(int decimalNumber, int size) : size(size) {
        if (size != 8 && size != 16 && size != 32) {
            throw invalid_argument("Invalid binary number size. Allowed sizes: 8, 16, 32.");
        }
        bits = BinaryUtils::convertToBinary(decimalNumber, size);
    }

    string getBinaryString() const {
        string binaryString;
        for (int i = 0; i < size; ++i) {
            binaryString += bits[i] ? '1' : '0';
        }
        return binaryString;
    }

    int getDecimalNumber() const {
        return BinaryUtils::convertToDecimal(bits);
    }

    BinaryNumber operator+(const BinaryNumber& other) const {
        if (size != other.size) {
            throw invalid_argument("Binary numbers must be of the same size for addition.");
        }
        int carry = 0;
        vector<bool> resultBits = BinaryArithmetic::add(bits, other.bits, carry);

        if (carry) {
            throw overflow_error("Overflow occurred during addition.");
        }

        BinaryNumber result(0, size);
        result.bits = resultBits;
        return result;
    }

    BinaryNumber operator-(const BinaryNumber& other) const {
        if (size != other.size) {
            throw invalid_argument("Binary numbers must be of the same size for subtraction.");
        }

        bool borrow = false;
        vector<bool> resultBits = BinaryArithmetic::subtract(bits, other.bits, borrow);

        BinaryNumber result(0, size);
        result.bits = resultBits;
        return result;
    }

    BinaryNumber operator*(const BinaryNumber& other) const {
        if (size != other.size) {
            throw invalid_argument("Binary numbers must be of the same size for multiplication.");
        }

        BinaryNumber result(0, size);
        BinaryNumber shifted = *this;

        for (int i = size - 1; i >= 0; --i) {
            if (other.bits[i]) {
                result = result + shifted;
            }
            shifted.bits = BinaryArithmetic::shiftLeft(shifted.bits);
        }

        if (result.bits[0] != bits[0] && result.bits[0] != other.bits[0]) {
            throw overflow_error("Overflow occurred during multiplication.");
        }

        return result;
    }

    bool operator==(const BinaryNumber& other) const {
        return bits == other.bits;
    }

    bool operator!=(const BinaryNumber& other) const {
        return !(*this == other);
    }

    bool operator<(const BinaryNumber& other) const {
        if (size != other.size) {
            throw invalid_argument("Binary numbers must be of the same size for comparison.");
        }

        int thisDecimal = BinaryUtils::convertToDecimal(bits);
        int otherDecimal = BinaryUtils::convertToDecimal(other.bits);

        return thisDecimal < otherDecimal;
    }

    bool operator>(const BinaryNumber& other) const {
        return other < *this;
    }

    bool operator<=(const BinaryNumber& other) const {
        return !(other < *this);
    }

    bool operator>=(const BinaryNumber& other) const {
        return !(*this < other);
    }
};


/*
=======================================================================================================================================================================

--------------------------------------------------------------------ЛАБОРАТОРНАЯ НОМЕР 2-------------------------------------------------------------------------------

=======================================================================================================================================================================
*/

class Node {
public:
    BinaryNumber data; // число
    Node* next; // ссылка на следующий
    Node(const BinaryNumber& data) : data(data), next(nullptr) {} // конструктор
};

class Stack {
private:
    Node* top; // верхний элемент в стеке

public: 
    Stack() : top(nullptr) {} //конструктор

    ~Stack() { // деструктор
        while (top != nullptr) {
            Node* temp = top;
            top = top->next;
            delete temp;
        }
    }

    void push(const BinaryNumber& num) { // добавить элемент
        Node* newNode = new Node(num);
        newNode->next = top;
        top = newNode;
    }

    void pop() { // удалить верхний элемент
        if (top == nullptr) {
            throw underflow_error("Stack underflow: no elements to pop");
        }
        Node* temp = top;
        top = top->next;
        delete temp;
    }

    BinaryNumber peek() const { // просто получить верхний (без удаления)
        if (top == nullptr) {
            throw underflow_error("Stack underflow: no elements to peek");
        }
        return top->data;
    }

    bool isEmpty() const { // проверка на пустоту
        return top == nullptr;
    }
};


class Calculator {
private:
    BinaryNumber performOperation(const BinaryNumber& a, const BinaryNumber& b, char op) {
        switch (op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        default:
            throw invalid_argument("Invalid operation. Allowed operations: +, -, *.");
        }
    }

public:
    BinaryNumber evaluatePostfix(const string& expression, int size) {
        istringstream iss(expression); // делим строку на токены
        string token; // токен
        Stack stack;
        
        while (iss >> token) { // пробегаемся по токенам
            if (token == "+" || token == "-" || token == "*") { // если встретили арифмитическое действие
                if (stack.isEmpty()) { // стек пустой?
                    throw runtime_error("Invalid postfix expression: insufficient operands for operation " + token);
                }
                BinaryNumber b = stack.peek(); // поулчаем верхний элемент из стека
                stack.pop(); // удаляем элемент

                if (stack.isEmpty()) { // стек пустой?
                    throw runtime_error("Invalid postfix expression: insufficient operands for operation " + token);
                }
                BinaryNumber a = stack.peek();
                stack.pop();
                

                stack.push(performOperation(a, b, token[0])); // выполняем операцию
            }
            else { // если встретили число
                try {
                    int decimalValue = stoi(token); // преобразуем в число
                    stack.push(BinaryNumber(decimalValue, size)); // добавляем в стек
                }
                catch (...) {
                    throw invalid_argument("Invalid token in expression: " + token);
                }
            }
        }

        if (stack.isEmpty()) { // обработка ошибки
            throw runtime_error("Invalid postfix expression: no result in the stack");
        }

        BinaryNumber result = stack.peek(); // очищение стека, получение результата
        stack.pop();

        if (!stack.isEmpty()) { // обработка ошибки
            throw runtime_error("Invalid postfix expression: more than one result remains on the stack");
        }

        return result;
    }
};

int main() {
    Calculator calculator;
    cout << "Enter the postfix expression: ";
    string postfixExpr;
    getline(cin, postfixExpr);
    int size;
    cout << "Enter the binary number size (8, 16, or 32): ";
    cin >> size;



    if (size != 8 && size != 16 && size != 32) {
        throw invalid_argument("Invalid binary number size. Allowed sizes: 8, 16, 32.");
    }

    try {
        BinaryNumber result = calculator.evaluatePostfix(postfixExpr, size);
        cout << "Binary Result: " << result.getBinaryString() << endl;
        cout << "Decimal Result: " << result.getDecimalNumber() << endl;
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}

