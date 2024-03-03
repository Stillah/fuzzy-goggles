#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
 
int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr); //fast input and output
    int n, w; cin >> n >> w;
    vector<int> weight(n+1), cost(n+1), ans;
    vector<vector<int>> dp(n+1,vector<int>(w+1));
    for (int i = 1; i <= n; i++) cin >> weight[i];
    for (int i = 1; i <= n; i++) cin >> cost[i];
 
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= w; j++) {
            if (weight[i] <= j) dp[i][j] = max(dp[i-1][j], cost[i] + dp[i-1][j-weight[i]]);
            else dp[i][j] = dp[i-1][j];
        }
    }
 
    int column = w;
    for (int i = n; i > 0; i--) {
        for (int j = column; j > 0; j--) {
            if (dp[i-1][j] == dp[i][j]) break;
            if (dp[i][j-1] < dp[i][j]) {
                column -= weight[i];
                ans.push_back(i);
                break;
            }
        }
    }
 
    sort(ans.begin(), ans.end());
    cout << ans.size() << "\n";
    for (auto x: ans) cout << x << " ";
    return 0;
}
