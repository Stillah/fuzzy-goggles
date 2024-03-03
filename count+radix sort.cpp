#include <bits/stdc++.h>
using namespace std;
 
 
template <typename T>
void RuslanGatiatullin_radix_str(vector<vector<T>>& arr, int d, int maxElement, int pos) {
    vector<vector<T>> ret(arr.size(),vector<T>(3));
    long long digit = 1;
    for (int j = 0; j <= log10(maxElement) + 1; j++) {
        vector<int> count(d);
        for (int i = 0; i < arr.size(); i++)
            count[(arr[i][pos] / digit) % d]++;
        for (int i = 1; i < d; i++)
            count[i] += count[i-1];
        for (int i = arr.size() - 1; i >= 0; i--) {
            ret[count[ (arr[i][pos] / digit) % d ] - 1 ] = arr[i];
            count[ (arr[i][pos] / digit) % d ]--;
        }
        arr = ret;
        digit *= d;
    }
}
 
template <unsigned int minElement = 0, unsigned int maxElement = 100, typename T>
vector<vector<T>> RuslanGatiatullin_count_str(vector<vector<T>> arr) {
    vector<int> numCount(maxElement - minElement + 1);
    unordered_map<int,vector<vector<T>>> additionalData;
    vector<vector<T>> ret(arr.size());
 
    for (int i = 0; i < arr.size(); i++) {
        arr[i][0] -= minElement;
        numCount[arr[i][0]]++;
        additionalData[arr[i][0]].push_back({});
        for (int j = 0; j < arr[i].size(); j++) additionalData[arr[i][0]].back().push_back(arr[i][j]);
    }
    for (int i = maxElement - minElement - 1; i >= 0; i--) numCount[i] = numCount[i+1] + numCount[i];
    for (auto& x: additionalData)
        if (x.second.size() > 1) RuslanGatiatullin_radix_str(x.second, 10, 100000, 1);
 
    for (int i = arr.size() - 1; i >= 0; i--) {
        ret[numCount[arr[i][0]] - 1] = additionalData[arr[i][0]].back();
        additionalData[arr[i][0]].pop_back();
        numCount[arr[i][0]]--;
    }
    return ret;
}
int main() {
    ios::sync_with_stdio(false); cin.tie(); cout.tie(); //fast input and output
 
    int n; cin >> n;
    vector<vector<int>> bids(n,vector<int>(3));
    for (int i = 0; i < n; i++) cin >> bids[i][0] >> bids[i][1], bids[i][2] = i + 1;
    vector<vector<int>> sortedArray = RuslanGatiatullin_count_str(bids);
    for (int i = 0; i < n; i++) cout << sortedArray[i][2] << " ";
    return 0;
}
