#include <bits/stdc++.h>
using namespace std;
 
template <typename T>
class Stack{
public:
    virtual void push(const T& value) = 0;
    virtual T pop() = 0;
    virtual T peek() = 0;
    virtual int size() = 0;
    virtual bool isEmpty() = 0;
};
 
template <typename T>
class ArrayStack: Stack<T> {
private:
    int stackSize, capacity;
    static const int INITIAL_CAPACITY = 16;
    T* items;
 
public:
    ArrayStack() {
        stackSize = 0;
        capacity = INITIAL_CAPACITY;
        items = new T[INITIAL_CAPACITY];
    }
 
    ArrayStack(int n, const T& value) {
        stackSize = n;
        capacity = n * 2;
        items = new T[capacity];
        for (int i = 0; i < n; i++) items[i] = value;
    }
 
    ~ArrayStack() { delete[] items;}
 
    int size() override { return stackSize;}
 
    bool isEmpty() override { return stackSize == 0;}
 
    void push(const T& value) override {
        if (stackSize == capacity) {
            T* newArr = new T[2 * capacity];
            for (int i = 0; i < stackSize; i++) newArr[i] = items[i];
            delete[] items;
            items = newArr;
            capacity = capacity * 2;
            //delete[] newArr;
        }
        items[stackSize] = value;
        stackSize++;
    }
    T pop() override {
        if (!isEmpty()){
            stackSize--;
            if (stackSize == capacity / 2 && stackSize > INITIAL_CAPACITY) {
                T* newArr = new T[capacity / 2];
                for (int i = 0; i < stackSize; i++) newArr[i] = items[i];
                delete[] items;
                items = newArr;
                capacity = capacity / 2;
                //delete[] newArr;
            }
            return items[stackSize];
        }
        throw runtime_error("Stack is empty");
    }
    T peek() override {
        if (!isEmpty()) return items[stackSize - 1];
        else throw runtime_error("Stack is empty");
    }
};
 
template <typename T>
class ArrayQueue: Stack<T> {
private:
    int queueSize, capacity, beginning, end;
    static const int INITIAL_CAPACITY = 16;
    T* items;
 
public:
    ArrayQueue() {
        queueSize = 0;
        beginning = 0;
        end = -1;
        capacity = INITIAL_CAPACITY;
        items = new T[INITIAL_CAPACITY];
    }
 
    ArrayQueue(int n, const T& value) {
        queueSize = n;
        beginning = 0;
        end = n - 1;
        capacity = n * 2;
        items = new T[capacity];
        for (int i = 0; i < n; i++) items[i] = value;
    }
 
    ~ArrayQueue() { delete[] items;}
 
    int size() override { return queueSize;}
 
    bool isEmpty() override { return queueSize == 0;}
 
    void push(const T& value) override {
        if (queueSize == capacity) {
            T* newArr = new T[2 * capacity];
            for (int i = 0, j = beginning; i < queueSize; i++)
                newArr[i] = items[j], j = (j + 1) % capacity;
            delete[] items;
            items = newArr;
            capacity = capacity * 2;
            beginning = 0;
            end = queueSize - 1;
            //delete[] newArr;
        }
        end = (end + 1) % capacity;
        queueSize++;
        items[end] = value;
    }
    T pop() override {
        if (!isEmpty()){
            int retIndex = beginning;
            beginning = (beginning + 1) % capacity;
            queueSize--;
            return items[retIndex];
        }
        throw runtime_error("Stack is empty");
    }
    T peek() override {
        if (!isEmpty()) {
            return items[end];
        }
        else throw runtime_error("Stack is empty");
    }
};
 
int operationPriority(const string& op) {
    if (op[0] == '+' || op[0] == '-')
        return 1;
    else if (op[0] == '*' || op[0] == '/')
        return 2;
    else if (op[0] == 'm') // max/min operations
        return 3;
    else return -1;
}
 
bool leftAssociative(const string& op) {
    if (op[0] == '+' || op[0] == '-' || op[0] == '/' || op[0] == '*') return true;
    else return false;
}
 
int main() {
    ios::sync_with_stdio(false); cin.tie(); cout.tie(); //fast reading and writing
    ArrayStack <string> operations;
    ArrayQueue <string> output;
    string input;
    while (cin >> input) {
        if (input[0] == 'A') break;
        if ('0' <= input[0] && input[0] <= '9')
            output.push(input);
        else if (input[0] == 'm' || input[0] == '(')
            operations.push(input);
        else if (input[0] == ')') {
            while (!operations.isEmpty() && operations.peek()[0] != '(') output.push(operations.pop());
            if (!operations.isEmpty() && operations.peek()[0] == '(') operations.pop();
            if (!operations.isEmpty() && operations.peek()[0] == 'm') output.push(operations.pop());
        }
        else if (input[0] == ',')
            while (!operations.isEmpty() && operations.peek()[0] != '(') output.push(operations.peek()), operations.pop();
        else{
            while (!operations.isEmpty() &&
                operations.peek()[0] != '(' &&
                (operationPriority(input) <= operationPriority(operations.peek()) ||
                    (leftAssociative(input) && operationPriority(input) == operationPriority(operations.peek())))) {
                output.push(operations.pop());
            }
            operations.push(input);
        }
    }
    while (!operations.isEmpty()) {
        output.push(operations.peek()), operations.pop();
    }
    while (!output.isEmpty()) {
        cout << output.pop() << " ";
    }
    return 0;
}
