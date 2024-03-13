#include <bits/stdc++.h>
using namespace std;

template <typename T>
int partition(vector<T>& arr, int low, int high) {
    int pivot = arr[high];
    int left = low;
    for (int i = low; i < high; i++) {
        if (arr[i] <= pivot) swap(arr[i], arr[left]), left++;
    }
    swap(arr[high], arr[left]);
    return left;
}
 
template <typename T>
void quickSort(vector<T>& arr, int low, int high) {
    if (low < high) {
        int pivot_location = partition(arr, low, high);
        quickSort(arr, low, pivot_location - 1);
        quickSort(arr, pivot_location + 1, high);
    }
}
