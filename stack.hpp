#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>

using namespace std;

template <typename T>
class Stack
{
    vector<T> stack;

public:
    Stack() {}

    Stack(const vector<T> &arr) : stack(arr) {}

    bool empty() { return stack.empty(); }

    int size() { return stack.size(); }

    void push(T val)
    {
        stack.push_back(val);
    }

    void pop()
    {
        if (stack.empty())
            throw runtime_error("Stack is empty");
        stack.pop_back();
    }

    T top()
    {
        if (stack.empty())
            throw runtime_error("Stack is empty");
        return stack.back();
    }

    void print()
    {
        for (const auto &val : stack)
            cout << val << " ";
        cout << "\n";
    }
};