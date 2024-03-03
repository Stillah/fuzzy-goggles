#include <bits/stdc++.h>
using namespace std;
unordered_map <string, bool> cache;
vector<string> words;
vector<string> out;
string s;
 
bool dp(int begin) {
    if (cache.count(s.substr(begin)) && cache[s.substr(begin)] || begin == s.length()) {
        out.push_back(s.substr(begin));
        return true;
    }
    else {
        for (const auto& word: words) {
            if (s.length() - begin >= word.length() && s.substr(begin, word.length()) == word) {
                if (dp(begin + word.length())) {
                    out.push_back(word);
                    return true;
                }
            }
        }
    }
    return false;
}
int main() {
    ios::sync_with_stdio(false); cin.tie(); cout.tie(); //fast input and output
 
    int n, k; cin >> n >> k;
    if (n == 0 || k == 0) return 0;
    for (int i = 0; i < n; i++) cin >> s, cache[s] = true, words.push_back(s);
    cin >> s;
    if (n == 1 && s == words[0]) cout << s << "\n";
 
    dp(0);
    for (int i = out.size() - 1; i >= 0; i--) cout << out[i] << " ";
    
    return 0;
}
