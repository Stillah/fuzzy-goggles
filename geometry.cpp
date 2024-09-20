#include <bits/stdc++.h>
#include <iomanip>
using namespace std;
typedef long long ll;
const ll mod = 1e9 + 7;
const double eps = 1e-9;

template <typename T>
struct point {
  T x, y;
  point(): x(0), y(0) {};
  point(T _x, T _y): x(_x), y(_y) {};


  point operator+(point<T> other) {
    return {x + other.x, y + other.y};
  }

  point operator-(point<T> other) {
    return {x - other.x, y - other.y};
  }

  bool operator==(point<T> other) {
    return (x == other.x && y == other.y);
  }

  bool operator<(point<T> other) {
    if (other.x != x) return x < other.x;
    return y < other.y;
  }

};

template <typename T>
T cross(point<T> p1, point<T> p2) {
  ll a = p1.x * p2.y - p2.x * p1.y;
  return a;
}


bool mid(point<ll> a, point<ll> b, point<ll> c) {
  vector<point<ll>> v{a,b,c};
  std::sort(v.begin(), v.end());
  return (v[1] == c);
}

void solve() {
  int n; cin >> n;
  vector<point<ll>> polygon(n);
  for (int i = 0; i < n; i++)
    cin >> polygon[i].x >> polygon[i].y;
  
  polygon.push_back(polygon[0]);

  ll ans = 0;
  for (int i = 0; i < n; i++)
    ans += ::cross(polygon[i], polygon[i+1]);

  cout << abs(ans) << "\n";
}

int main() {
  ios::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);
  int t = 1; //cin >> t;
  while (t--) solve();
  return 0;
}
