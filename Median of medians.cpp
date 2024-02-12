#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int SUBARRAY_LENGTH = 5;

int medianOfMedians(const vector<int>& arr, int index) {
    vector<int> medians, low, high;
    vector<vector<int>> subArrays(arr.size() / SUBARRAY_LENGTH + 1);
    int pivot = 0;

    for (int i = 0; i <= arr.size() / SUBARRAY_LENGTH; i++) {
        for (int j = 0; j < SUBARRAY_LENGTH; j++) {
            if (SUBARRAY_LENGTH * i + j < arr.size()) subArrays[i].push_back(arr[SUBARRAY_LENGTH * i + j]);
        }
    }
    for (auto subArray: subArrays) {
        quickSort(subArray, 0, subArray.size() - 1);
        if (!subArray.empty()) medians.push_back(subArray[subArray.size() / 2]);
    }

    if (medians.size() <= SUBARRAY_LENGTH) {
        quickSort(medians, 0, medians.size() - 1);
        pivot = medians[medians.size() / 2];
    }
    else pivot = medianOfMedians(medians, medians.size() / 2);

    for (int element : arr) {
        if (element < pivot) low.push_back(element);
        else if (element > pivot) high.push_back(element);
    }

    if (index < low.size()) return medianOfMedians(low, index);
    else if (index > low.size()) return medianOfMedians(high, index - low.size() - 1);
    else return pivot;
}
