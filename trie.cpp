#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

struct TrieNode {
  TrieNode* next[26];
  bool isTerminal;
  TrieNode(): isTerminal(false) { for (auto &v: next) v = nullptr; }
};

struct Trie {
  TrieNode* root;

  Trie(): root(new TrieNode()) {};
  void insert(const string &word) {
    TrieNode* v = root;
    for (auto i: word) {
      if (!v->next[i - 'a'])
        v->next[i - 'a'] = new TrieNode();
      v = v->next[i - 'a'];
    }
    v->isTerminal = true;
  }

  bool search(const string &word) const {
    TrieNode* v = root;
    for (auto i: word) {
      if (!v->next[i - 'a']) return false;
      v = v->next[i - 'a'];
    }
    return v->isTerminal;
  }

  bool startsWith(const string &pref) const {
    TrieNode* v = root;
    for (auto i: pref) {
      if (!v->next[i - 'a']) return false;
      v = v->next[i - 'a'];
    }
    return true;
  }
};
