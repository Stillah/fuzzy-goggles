vector<pair<int,int>> suffixArrayAndLCP(string s) {
    s += '$';
    int k = -1;
    int n = s.size();
    vector<int> c(n);
    vector<pair<int,int>> p(n);
    while(k < 0 || (1ll << k) < n) {
        vector<pair<pair<int, int>, int>> a(n);

        for (int i = 0; i < n; i++) {
            if (k != -1) a[i] = {{c[i], c[(i + (1ll << k)) % n]}, i};
            else a[i] = {{s[i], -1}, i};
        }

        sort(a.begin(), a.end());
        for (int i = 0; i < n; i++) p[i].first = a[i].second;
        c[p[0].first] = 0;

        for (int i = 1; i < n; i++) {
            c[p[i].first] = c[p[i-1].first];
            if (a[i].first != a[i-1].first)
                c[p[i].first]++;
        }
        k++;
    }

    k = 0;
    for (int i = 0; i < n - 1; i++) {
        int pi = c[i];
        int j = p[pi - 1].first;

        while(s[i + k] == s[j + k]) k++;
        p[pi].second = k;
        k = max(k - 1, 0);
    }
    return p;
}
