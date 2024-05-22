#include <bits/stdc++.h>
#define mod 1000000007
using namespace std;
typedef long long ll;

struct Node {
    ll sum;
    bool isAssigned;
    ll assigned;
    Node(ll sum, bool isAssigned, ll assigned): sum(sum), isAssigned(isAssigned), assigned(assigned) {};
};

//To change from sum segment tree to other segment tree, change func(ll a, ll b), assignOnNode(int v, int vl, int vr, ll x)
//and identity
struct SegmentTree {
    vector<Node> tree;
    ll sz;
    ll identity = 0;
    
    explicit SegmentTree(vector<ll>& a) {
        int n = 1;
        while (n < a.size()) n *= 2;

        tree.assign(2*n, Node(identity, false, 0));
        sz = tree.size() / 2;
        
        for (int i = 0; i < a.size(); i++)
            tree[n+i].sum = a[i];

        for (int i = n-1; i > 0; i--)
            recalculate(i);
    }
    
    explicit SegmentTree(int size) {
        int n = 1;
        while (n < size) n *= 2;
        
        tree.assign(2*n, Node(identity, false, 0));
        sz = tree.size() / 2;
    }

    void recalculate(int v) {
        tree[v].sum = func(tree[2*v].sum, tree[2*v + 1].sum);
    }

    ll action(int v, int vl, int vr, int ql, int qr) {
        if (vr < ql || qr < vl) return identity;
        if (ql <= vl && vr <= qr) return tree[v].sum;
        push(v,vl,vr);
        int vmid = (vl + vr) / 2;
        return func(action(2*v, vl, vmid, ql, qr), action(2*v + 1, vmid + 1, vr, ql, qr));
    }

    ll action (int ql, int qr) {
        return action(1, 1, sz, ql, qr);
    }

    void push(int v, int vl, int vr) {
        if (tree[v].isAssigned) {
            int vmid = (vl + vr) / 2;
            assignOnNode(2*v, vl, vmid, tree[v].assigned);
            assignOnNode(2*v + 1, vmid + 1, vr, tree[v].assigned);
            tree[v].assigned = 0;
            tree[v].isAssigned = false;
        }
    }

    void assignOnNode(int v, int vl, int vr, ll x) {
        //For min/max to add a value
        tree[v] = {tree[v].sum + x, true, tree[v].assigned + x};
        
        //For sum to add a value
        //tree[v] = {tree[v].sum + x * (vr - vl + 1), true, tree[v].assigned + x};
        
        //To set a value remove "tree[v].sum +", "tree[v].assigned +"
    }

    void assignOnSegment(int v, int vl, int vr, int ql, int qr, ll x) {
        if (vr < ql || qr < vl) return;
        if (ql <= vl && vr <= qr) {
            assignOnNode(v,vl,vr,x);
            return;
        }
        push(v,vl,vr);
        int vmid = (vl + vr) / 2;
        assignOnSegment(2*v, vl, vmid, ql, qr, x);
        assignOnSegment(2*v + 1, vmid + 1, vr, ql, qr, x);
        recalculate(v);
    }

    void assignOnSegment(int ql, int qr, ll x) {
        assignOnSegment(1, 1, sz, ql, qr, x);
    }
    
    void assignOnSingleNode(int q, ll x) {
        assignOnSegment(1, 1, sz, q, q, x);
    }

    ll func(ll a, ll b) {
        return a + b;
    }
};
