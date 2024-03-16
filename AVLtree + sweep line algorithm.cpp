//Ruslan Gatiatullin
#include <bits/stdc++.h>
#include <algorithm>
#include <utility>
using namespace std;
typedef long long ll;

// Quicksort algorithm from
// https://forkettle.ru/vidioteka/programmirovanie-i-set/algoritmy-i-struktury-dannykh/108-sortirovka-i-poisk-dlya-chajnikov/1010-metod-khoara-bystraya-sortirovka-quick-sort
template< typename BidirectionalIterator, typename Compare >
void quick_sort(BidirectionalIterator first, BidirectionalIterator last, Compare cmp ) {
    if( first != last ) {
        BidirectionalIterator left  = first;
        BidirectionalIterator right = last;
        BidirectionalIterator pivot = left++;
        while( left != right ) {
            if( cmp( *left, *pivot ) ) {
                ++left;
            } else {
                while( (left != --right) && cmp( *pivot, *right ) )
                    ;
                std::iter_swap( left, right );
            }
        }
        --left;
        std::iter_swap( first, left );

        quick_sort( first, left, cmp );
        quick_sort( right, last, cmp );
    }
}

int partition (double *a, int p, int r) {
    double x = *(a+r);
    int i = p - 1;
    int j;
    double tmp;
    for (j = p; j < r; j++) {
        if (*(a+j) <= x) {
            i++;
            tmp = *(a+i);
            *(a+i) = *(a+j);
            *(a+j) = tmp;
        }
    }
    tmp = *(a+r);
    *(a+r) = *(a+i+1);
    *(a+i+1) = tmp;
    return i + 1;
}

void quicksort (double *a, int p, int r) {
    int q;
    if (p < r) {
        q = partition (a, p, r);
        quicksort (a, p, q-1);
        quicksort (a, q+1, r);
    }
}
template< typename BidirectionalIterator >
inline void quick_sort( BidirectionalIterator first, BidirectionalIterator last ) {
    quick_sort(first, last, std::less_equal<typename std::iterator_traits<BidirectionalIterator>::value_type>());
}

template<typename T>
class segment {
public: //first is 0 if (x1, y1) is the beginning and 1 otherwise
    bool isLeft{}, reversed{};
    pair<T, T> segBegin, segEnd;
    segment() = default;
    segment(T x1, T y1, T x2, T y2, bool position, bool rev): segBegin(x1, y1), segEnd(x2, y2), isLeft(position),
                                                              reversed(rev){};
    bool operator<(segment& seg) {
        if (segBegin.second == seg.segBegin.second) return segBegin.first < seg.segBegin.first;
        return segBegin.second < seg.segBegin.second;
    }
    bool operator>(segment& seg) {
        if (segBegin.second == seg.segBegin.second) return segBegin.first > seg.segBegin.first;
        return segBegin.second > seg.segBegin.second;
    }
    bool operator<=(segment& seg) {
        if (segBegin.second == seg.segBegin.second) return segBegin.first <= seg.segBegin.first;
        return segBegin.second <= seg.segBegin.second;
    }
    bool operator>=(segment& seg) {
        if (segBegin.second == seg.segBegin.second) return segBegin.first >= seg.segBegin.first;
        return segBegin.second >= seg.segBegin.second;
    }
    bool operator==(segment& seg) {
        return segBegin.second == seg.segBegin.second;
    }
    bool operator!=(segment& seg) {
        return !(segBegin.first == seg.segBegin.first && segBegin.second == seg.segBegin.second &&
            segEnd.first == seg.segEnd.first && segEnd.second == seg.segEnd.second && isLeft == seg.isLeft);
    }
    void displaySegment() {
        if (!reversed) {
            if (isLeft)
                cout << segBegin.first << " " << segBegin.second << " " << segEnd.first << " " << segEnd.second << "\n";
            else
                cout << segEnd.first << " " << segEnd.second << " " << segBegin.first << " " << segBegin.second << "\n";
        }
        else {
            if (isLeft)
                cout << segEnd.first << " " << segEnd.second << " " << segBegin.first << " " << segBegin.second << "\n";
            else
                cout << segBegin.first << " " << segBegin.second << " " << segEnd.first << " " << segEnd.second << "\n";
        }
    }
};

template<typename T>
class AVLTree;

template<typename T>
class Node {
    T value;
    int height;
    int index;
    Node<T>* left;
    Node<T>* right;
    Node<T>* parent;
public:
    Node(): value(nullptr), parent(nullptr), height(0), index(0), left(nullptr), right(nullptr) {};
    Node(T value, Node<T>* parent): value(std::move(value)), index(0), parent(parent), height(1), left(nullptr), right(nullptr) {};
    explicit Node(T value): value(std::move(value)), index(0), parent(nullptr), height(1), left(nullptr), right(nullptr) {};

    int leftHeight() {
        if (left == nullptr) return 0;
        else return left->height;
    }
    int rightHeight() {
        if (right == nullptr) return 0;
        else return right->height;
    }
    int bf() { return leftHeight() - rightHeight();}

    T val() const {return value;}
    friend AVLTree<T>;
};

template<typename T> // T should have comparison operators
class AVLTree {
    Node<T>* root;
    int count = 0;
private:
    template<typename Action>
    Node<T>* findValue(T value, Action act) {
        Node<T>* node = root;
        act(node);

        while (node != nullptr) { //maybe just ">"
            if (value >= node->value && node->right != nullptr) {
                node = node->right;
                act(node);
            }
            else if (value < node->value && node->left != nullptr) {
                node = node->left;
                act(node);
            }
            else break;
        }
        return node;
    }

    void updateParentsChild(Node<T>* oldChild, Node<T>* newChild) {
        Node<T>* parent = oldChild->parent;
        if (parent != nullptr && parent->left == oldChild)
            parent->left = newChild;
        else if (parent != nullptr && parent->right == oldChild)
            parent->right = newChild;
    }

    void recalculateHeight(Node<T>* node) {
        while(node != nullptr) {
            node->height = max(node->rightHeight(), node->leftHeight()) + 1;
            node = node->parent;
        }
    }

    void rotateLeft(Node<T>* localRoot) {
        Node<T>* mid = localRoot->right;
        Node<T>* midSubtree = mid->left;

        mid->left = localRoot;
        localRoot->right = midSubtree;

        updateParentsChild(localRoot, mid);

        mid->parent = localRoot->parent;
        localRoot->parent = mid;
        if (midSubtree != nullptr) midSubtree->parent = localRoot;

        localRoot->height = 1 + max(localRoot->leftHeight(), localRoot->rightHeight());
        mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());

        if (localRoot == root) root = mid;
    }

    void rotateRight(Node<T>* localRoot) {
        Node<T>* mid = localRoot->left;
        Node<T>* midSubtree = mid->right;

        localRoot->left = midSubtree;
        mid->right = localRoot;

        updateParentsChild(localRoot, mid);

        mid->parent = localRoot->parent;
        localRoot->parent = mid;
        if (midSubtree != nullptr) midSubtree->parent = localRoot;

        localRoot->height = 1 + max(localRoot->leftHeight(), localRoot->rightHeight());
        mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());

        if (localRoot == root) root = mid;
    }

    void rotateRightLeft(Node<T>* localRoot) {
        Node<T>* mid = localRoot->left;
        Node<T>* localLeaf = mid->right;

        mid->right = localLeaf->left;
        localRoot->left = localLeaf->right;
        localLeaf->left = mid;
        localLeaf->right = localRoot;

        updateParentsChild(localRoot, localLeaf);

        localLeaf->parent = localRoot->parent;
        mid->parent = localLeaf;
        localRoot->parent = localLeaf;

        if (localRoot->left != nullptr) localRoot->left->parent = localRoot;
        if (mid->right != nullptr) mid->right->parent = mid;

        mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());
        localRoot->height = 1 + max(localRoot->leftHeight(), localRoot->rightHeight());
        localLeaf->height = 1 + max(localLeaf->leftHeight(), localLeaf->rightHeight());

        if (localRoot == root) root = localLeaf;
    }

    void rotateLeftRight(Node<T>* localRoot) {
        Node<T>* mid = localRoot->right;
        Node<T>* localLeaf = mid->left;

        mid->left = localLeaf->left;
        localRoot->right = localLeaf->right;
        localLeaf->right = mid;
        localLeaf->left = localRoot;

        updateParentsChild(localRoot, localLeaf);

        localLeaf->parent = localRoot->parent;
        mid->parent = localLeaf;
        localRoot->parent = localLeaf;

        if (localRoot->right != nullptr) localRoot->right->parent = localRoot;
        if (mid->left != nullptr) mid->left->parent = mid;

        mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());
        localRoot->height = 1 + max(localRoot->leftHeight(), localRoot->rightHeight());
        localLeaf->height = 1 + max(localLeaf->leftHeight(), localLeaf->rightHeight());

        if (localRoot == root) root = localLeaf;
    }

    void balance() {
        Node<T>* node = root;
        while(true) {
            if (node->left != nullptr && abs(node->left->bf()) > 1)
                node = node->left;
            else if (node->right != nullptr && abs(node->right->bf()) > 1)
                node = node->right;
            else break;
        }

        if (node->bf() < -1 && node->right->right != nullptr)
            rotateLeft(node);
        else if (node->bf() > 1 && node->left->left != nullptr)
            rotateRight(node);
        else if (node->bf() > 1)
            rotateRightLeft(node);
        else if (node->bf() < -1)
            rotateLeftRight(node);
    }

public:
    AVLTree(): root(nullptr) {}; //should make destructor but doesn't matter here
    Node<T>* treeMaximum(Node<T>* node) {
        while (node->right != nullptr)
            node = node->right;
        return node;
    }

    Node<T>* treeMinimum(Node<T>* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    Node<T>* findValue(T value) {
        Node<T>* node = root;

        while (node != nullptr) {
            if (value >= node->value && node->right != nullptr && node->value != value)
                node = node->right;
            else if (value < node->value && node->left != nullptr && node->value != value)
                node = node->left;
            else break;
        }
        return node;
    }

    Node<T>* findClosest(T value) {
        Node<T>* node = root;
        while (node != nullptr) {
            if (value == node->value) break;
            else if (value > node->value && node->right != nullptr)
                node = node->right;
            else if (value < node->value && node->left != nullptr)
                node = node->left;
            else break;
        }
        return node;
    }

    Node<T>* successor(Node<T>* node) {
        if (node == nullptr) return nullptr;
        if (node->right != nullptr)
            return treeMinimum(node->right);
        else {
            auto y = node->parent;
            while(y != nullptr && y->parent != nullptr && node == y->right) {
                node = y;
                y = y->parent;
            }
            return y <= node? y: nullptr;
        }
    }

    Node<T>* predecessor(Node<T>* node) {
        if (node == nullptr) return nullptr;
        if (node->left != nullptr)
            return treeMaximum(node->left);
        else {
            auto y = node->parent;
            while(y != nullptr && y->parent != nullptr && node == y->left) {
                node = y;
                y = y->parent;
            }
            return y >= node? y: nullptr;
        }
    }

    void add(T value) {
        if (root == nullptr) {
            root = new Node<T>(value);
            return;
        }

        Node<T>* place = findValue(value, [](Node<T>* n) {n->height++;}); //pls work
        if (place->value > value)
            place->left = new Node<T>(value, place);
        else if (place->value <= value)
            place->right = new Node<T>(value, place);

        balance();
    }

    void remove(T value) {
        Node<T>* place = findValue(value);
        if (place == nullptr) return;

        //if 2 children
        while (place->left != nullptr && place->right != nullptr) {
            Node<T>* succ = successor(place); //perhaps need to make unique_ptr

            swap(place->value, succ->value);
            place = succ;
        }
        //one or no children
        Node<T>* child = place->left == nullptr? place->right: place->left;

        if (child != nullptr) {
            swap(place->value, child->value);
            delete(child);
            place->right = nullptr;
            place->left = nullptr;
            recalculateHeight(place);
        }
        else {
            if (root == place) root = nullptr;
            updateParentsChild(place, nullptr);
            recalculateHeight(place);
            delete(place);
        }


    }

    void traversePostOrder(Node<T>* node) {
        if (node == nullptr) {
            return;
        }

        traversePostOrder(node->left);
        traversePostOrder(node->right);

        display(node);

        count++;
        node->index = count;
    }

    void traversePostOrder() { traversePostOrder(root); }

    void display(Node<T>* node) {
        cout << node->value << " ";
        cout << (node->left == nullptr ? -1: node->left->index) << " ";
        cout << (node->right == nullptr ? -1: node->right->index) << "\n";
    }

};

template <typename T>
bool intersect(segment<T> seg1, segment<T> seg2) {
    T x1 = seg1.segBegin.first, y1 = seg1.segBegin.second, x2 = seg1.segEnd.first, y2 = seg1.segEnd.second;
    T x3 = seg2.segBegin.first, y3 = seg2.segBegin.second, x4 = seg2.segEnd.first, y4 = seg2.segEnd.second;



    //one or 2 lines are straight up
    if (x2 - x1 == 0 || x4 - x3 == 0) {
        if (x2 - x1 == x4 - x3 && x1 == x3 && x2 == x4) return true;

        if (x2 - x1 == 0) {
            double k2 = 1.0 * (y4 - y3) / (x4 - x3);
            if (k2 * x1 - k2 * x3 + y3 >= min(y1, y2) && k2 * x1 - k2 * x3 + y3 <= max(y1, y2)
                && max(x3, x4) >= x1 && min(x3, x4) <= x1)
                return true;
        }

        if (x4 - x3 == 0) {
            double k1 = 1.0 * (y2 - y1) / (x2 - x1);
            if (k1 * x3 - k1 * x1 + y1 >= min(y3, y4) && k1 * x3 - k1 * x1 + y1 <= max(y3, y4)
                && max(x1, x2) >= x3 && min(x1, x2) <= x3)
                return true;
        }

        return false;
    }
    //lines are parallel or equal
    double k1 = 1.0 * (y2 - y1) / (x2 - x1);
    double k2 = 1.0 * (y4 - y3) / (x4 - x3);

    if (k1 == k2) {
        if (k1 * x1 - k2 * x3 - y1 + y3 == 0 && min(x1, x2) <= max(x3, x4) && max(x1, x2) >= min(x3, x4)) return true;
        else return false;
    }

    double interPoint = (k1 * x1 - k2 * x3 - y1 + y3) / (k1 - k2);
    interPoint = round(interPoint * 1000) / 1000;
    if (interPoint < min(x1, x2) || interPoint < min(x3, x4) || interPoint > max(x1, x2) || interPoint > max(x3, x4))
        return false;
    return true;
}

template <typename T>
void intersectionFound(segment<T> seg1, segment<T> seg2) {
    cout << "INTERSECTION\n";
    seg1.displaySegment();
    seg2.displaySegment();
    exit(0);
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);
    //Possible bugs: duplicated values
    int n, x1, x2, y1, y2; cin >> n;
    vector<segment<int>> segments(2*n);
    for (int i = 0; i < n; i++) {
        cin >> x1 >> y1 >> x2 >> y2;
        if (x1 <= x2) {
            segments[i] = segment<int>(x1, y1, x2, y2, true, false);
            segments[i + n] = segment<int>(x2, y2, x1, y1, false, false);
        }
        else {
            segments[i + n] = segment<int>(x2, y2, x1, y1, true, true);
            segments[i] = segment<int>(x1, y1, x2, y2, false, true);
        }
    }

    quick_sort(segments.begin(), segments.end(), [](segment<int>& seg1, segment<int>& seg2){
        if (seg1.segBegin.first != seg2.segBegin.first) return seg1.segBegin.first < seg2.segBegin.first;
        else if (seg1.isLeft != seg2.isLeft) return seg1.isLeft > seg2.isLeft;
        return seg1.segBegin.second < seg2.segBegin.second;});
    AVLTree<segment<int>> derevo;
    set<int> a;
    a.lower_bound(43);

    for (int i = 0; i < 2*n; i++) {
        if (segments[i].isLeft) {
            derevo.add(segments[i]);
            auto pred = derevo.predecessor(derevo.findClosest(segments[i]));
            auto succ = derevo.successor(derevo.findValue(segments[i]));

            if (pred != nullptr && intersect(segments[i], pred->val()))
                intersectionFound(segments[i], pred->val());
            if (succ != nullptr && intersect(segments[i], succ->val()))
                intersectionFound(segments[i], succ->val());
        }

        else {
            swap(segments[i].segBegin, segments[i].segEnd);
            segments[i].isLeft = true;
            auto pred = derevo.predecessor(derevo.findClosest(segments[i]));
            auto succ = derevo.successor(derevo.findValue(segments[i]));

            if (succ != nullptr && pred != nullptr && intersect(pred->val(), succ->val()))
                intersectionFound(pred->val(), succ->val());

            derevo.remove(segments[i]);
        }

    }

    cout << "NO INTERSECTIONS\n";
    return 0;
}
