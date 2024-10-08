#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll mod = 1e9 + 7;
const ll maxN = 1e6+10;
vector<ll> fact(maxN, 1);

ll power(ll x, ll n) {
  if (!n) return 1;
  ll u = power(x, n/2);
  u = (u * u) % mod;
  if (n & 1) u = (u * x) % mod;
  return u;
}

ll inverse(ll x) {
  return power(x % mod, mod-2);
}

ll nCk(ll n, ll k) {
  return fact[n] * inverse(fact[k] * fact[n - k] % mod) % mod;
}

int main() {
  for (ll i = 1; i < maxN; i++)
    fact[i] = (i * fact[i-1]) % mod;
}
