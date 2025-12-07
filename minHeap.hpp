#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

template <typename T>
class MinHeap
{
    vector<T> heap;

public:
    MinHeap() {}

    MinHeap(const vector<T> &arr) : heap(arr)
    {
        heapify();
    }

    int parent(int i) { return (i - 1) / 2; }
    int left(int i) { return 2 * i + 1; }
    int right(int i) { return 2 * i + 2; }
    int size() { return heap.size(); }

    void heapify()
    {
        if (heap.size() <= 1)
            return;
        for (int i = parent(heap.size() - 1); i >= 0; i--)
            heapifyDown(i);
    }

    void heapifyDown(int i)
    {
        int idx = i;
        int l = left(i);
        int r = right(i);
        int n = heap.size();

        if (l < n && heap[l] < heap[idx])
            idx = l;
        if (r < n && heap[r] < heap[idx])
            idx = r;

        if (idx != i)
        {
            swap(heap[i], heap[idx]);
            heapifyDown(idx);
        }
    }

    void heapifyUp(int i)
    {
        while (i > 0 && heap[i] < heap[parent(i)])
        {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }

    bool empty() { return heap.empty(); }

    void push(T val)
    {
        heap.push_back(val);
        heapifyUp(heap.size() - 1);
    }

    T top()
    {
        if (heap.empty())
            throw runtime_error("Heap is empty");
        return heap[0];
    }

    void pop()
    {
        if (heap.empty())
            throw runtime_error("Heap is empty");

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);
    }

    void update(int i, T newVal)
    {
        if (i >= 0 && i < heap.size())
        {
            T oldVal = heap[i];
            heap[i] = newVal;
            if (newVal < oldVal)
                heapifyUp(i);
            else
                heapifyDown(i);
        }
    }

    void print()
    {
        for (const auto &val : heap)
            cout << val << " ";
        cout << "\n";
    }
};