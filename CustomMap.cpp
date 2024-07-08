#include <bits/stdc++.h>
#include <algorithm>
#include <utility>
using namespace std;

template<
  class Key,
  class T,
  class Compare = less<Key>,
  class Allocator = allocator<pair<const Key, T>>>
class AVLTree {
  typedef pair<const Key, T> value_type;
  typedef AVLTree<Key, T, Compare, Allocator> tree_type;
  class Node;

 public:
  class Iterator;
  class reversedIterator;

  AVLTree() : root(nullptr), head(nullptr), tail(nullptr) {}

  AVLTree(const initializer_list<value_type> &lst) {
    for (const auto &x : lst)
      insert(x);
  }

  T &operator[](const Key &key_) noexcept {
    insert({key_, T{}});
    return find(key_)->value;
  }

  [[nodiscard]] T &at(const Key &key_) {
    if (!contains(key_)) throw out_of_range("key does not exist");
    return find(key_)->value;
  }

  [[nodiscard]] const T &at(const Key &key_) const {
    if (!contains(key_)) throw out_of_range("key does not exist");
    return find(key_)->value;
  }

  void insert(const value_type &pair_) {
    auto key_ = pair_.first;
    if (contains(key_)) return;

    ++size_;
    if (root == nullptr) {
      root = allocate_shared<Node>(alloc, pair_);
      head = root;
      tail = root;
      return;
    }
    shared_ptr<Node>
      place = find(key_, [](shared_ptr<Node> n) { n->height++; });
    if (Compare{}(place->key(), key_))
      place->right = allocate_shared<Node>(alloc, pair_, place);
    else
      place->left = allocate_shared<Node>(alloc, pair_, place);
    balance();

    if (key_ < head->key())
      head = predecessor(head);
    else if (key_ > tail->key())
      tail = successor(tail);
  }

  void erase(const Key &key_) {
    shared_ptr<Node> place = find(key_);
    if (place == nullptr) return;
    if (key_ == head->key_)
      head = successor(head);
    else if (key_ == tail->key_)
      tail = predecessor(tail);
    --size_;

    // if 2 children
    while (place->left != nullptr && place->right != nullptr) {
      shared_ptr<Node> succ = successor(place);

      swap(place->key_, succ->key_);
      place = succ;
    }
    // one or no children
    shared_ptr<Node>
      child = place->left == nullptr ? place->right : place->left;

    if (child != nullptr) {
      swap(place->key_, child->key_);
      place->right = nullptr;
      place->left = nullptr;
      recalculateHeight(place);
    } else {
      if (root == place) root = nullptr;
      updateParentsChild(place, nullptr);
      recalculateHeight(place);
    }
  }

  [[nodiscard]] Iterator find(Key key_) {
    shared_ptr<Node> node = root;

    while (node != nullptr) {
      if (Compare{}(key_, node->key()) && node->left != nullptr
        && node->key() != key_)
        node = node->left;
      else if (!Compare{}(key_, node->key()) && node->right != nullptr
        && node->key() != key_)
        node = node->right;
      else
        break;
    }
    if (node && node->key() == key_) return Iterator(this, node);
    else
      return Iterator(this, nullptr);
  }

  [[nodiscard]] bool contains(Key key_) noexcept {
    return find(key_) != end();
  }

  [[nodiscard]] inline bool empty() const noexcept {
    return root == nullptr;
  }

  [[nodiscard]] inline size_t size() const noexcept {
    return size_;
  }

  void clear() noexcept {
    size_ = 0;
    root = nullptr;
    tail = nullptr;
    head = nullptr;
  }

  [[nodiscard]] Iterator begin() noexcept {
    return Iterator(this, subtreeMinimum(root));
  }

  [[nodiscard]] reversedIterator rbegin() noexcept {
    return reversedIterator(this, subtreeMaximum(root));
  }

  [[nodiscard]] Iterator end() noexcept {
    return Iterator(this, nullptr);
  }

  [[nodiscard]] reversedIterator rend() noexcept {
    return reversedIterator(this, nullptr);
  }

  class Iterator {
   public:
    Iterator(tree_type *tree, shared_ptr<Node> node_)
      : parentTree(tree), curr(std::move(node_)) {}
    Iterator(const Iterator &iter_) = default;
    Iterator(Iterator &&iter_) = default;
    Iterator &operator=(const Iterator &iter_) = default;

    Iterator operator++() {
      if (!curr) return *this;
      curr = parentTree->successor(curr);
      return *this;
    }
    Iterator operator++(int) {
      auto temp = *this;
      if (!curr) return *this;
      curr = parentTree->successor(curr);
      return temp;
    }
    Iterator operator--() {
      if (!curr) curr = parentTree->tail;
      else
        curr = parentTree->predecessor(curr);

      assert(curr != nullptr);
      return *this;
    }
    Iterator operator--(int) {
      auto temp = *this;

      if (!curr) curr = parentTree->tail;
      else
        curr = parentTree->predecessor(curr);

      assert(curr != nullptr);
      return temp;
    }
    value_type &operator*() {
      return curr->keyValuePair;
    }
    value_type &operator->() {
      return curr->keyValuePair;
    }
    bool operator==(Iterator it) {
      return curr == it.curr && parentTree == it.parentTree;
    }
    bool operator!=(Iterator it) {
      return !(curr == it.curr && parentTree == it.parentTree);
    }

   protected:
    shared_ptr<Node> curr;
    tree_type *parentTree;
  };

  class reversedIterator: public Iterator {
   public:
    reversedIterator(tree_type *tree, shared_ptr<Node> node_)
      : Iterator(tree, node_) {}
    reversedIterator operator++() {
      if (!this->curr) return *this;
      this->curr = this->parentTree->predecessor(this->curr);
      return *this;
    }
    reversedIterator operator++(int) {
      reversedIterator temp = *this;
      if (!this->curr) return *this;
      this->curr = this->parentTree->predecessor(this->curr);
      return temp;
    }
    reversedIterator operator--() {
      if (!this->curr) this->curr = this->parentTree->head;
      else
        this->curr = this->parentTree->successor(this->curr);

      assert(this->curr != nullptr);
      return *this;
    }
    reversedIterator operator--(int) {
      auto temp = *this;

      if (!this->curr) this->curr = this->parentTree->head;
      else
        this->curr = this->parentTree->successor(this->curr);

      assert(this->curr != nullptr);
      return temp;
    }
  };

  ~AVLTree() {
    clear();
  }

 private:
  shared_ptr<Node> root, tail, head;
  size_t size_ = 0;
  Allocator alloc;

  [[nodiscard]] shared_ptr<Node>
  subtreeMaximum(shared_ptr<Node> node) const noexcept {
    while (node && node->right != nullptr)
      node = node->right;
    return node;
  }

  [[nodiscard]] shared_ptr<Node>
  subtreeMinimum(shared_ptr<Node> node) const noexcept {
    while (node && node->left != nullptr)
      node = node->left;
    return node;
  }

  [[nodiscard]] shared_ptr<Node> findClosest(const Key &key_) const {
    shared_ptr<Node> node = root;
    while (node != nullptr) {
      if (key_ == node->key_) break;
      else if (key_ > node->key_ && node->right != nullptr)
        node = node->right;
      else if (key_ < node->key_ && node->left != nullptr)
        node = node->left;
      else
        break;
    }
    return node;
  }

  shared_ptr<Node> successor(shared_ptr<Node> node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->right != nullptr) {
      return subtreeMinimum(node->right);
    } else {
      shared_ptr<Node> y = node->parent;
      while (y != nullptr && y->parent != nullptr && node == y->right) {
        node = y;
        y = y->parent;
      }
      if (!y) return y;
      return (Compare{}(y->key(), node->key()) ? nullptr : y);
    }
  }

  shared_ptr<Node> predecessor(shared_ptr<Node> node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->left != nullptr) {
      return subtreeMaximum(node->left);
    } else {
        auto y = node->parent;
        while (y != nullptr && y->parent != nullptr && node == y->left) {
          node = y;
          y = y->parent;
        }
      if (!y) return y;
      return (Compare{}(y->key(), node->key()) ? y : nullptr);
    }
  }

  template<typename Action>
  [[nodiscard]] shared_ptr<Node> find(const Key &key_, Action act) {
    auto node = root;
    act(node);

    while (node != nullptr) {  // maybe just ">"
      if (Compare{}(key_, node->key()) && node->left != nullptr) {
        node = node->left;
        act(node);
      } else if (!Compare{}(key_, node->key()) && node->right != nullptr) {
          node = node->right;
          act(node);
      } else {
          break;
        }
    }
    return node;
  }

  void
  updateParentsChild(shared_ptr<Node> oldChild, shared_ptr<Node> newChild) {
    shared_ptr<Node> parent = oldChild->parent;
    if (parent != nullptr && parent->left == oldChild)
      parent->left = newChild;
    else if (parent != nullptr && parent->right == oldChild)
      parent->right = newChild;
  }

  void recalculateHeight(shared_ptr<Node> node) {
    while (node != nullptr) {
      node->height = max(node->rightHeight(), node->leftHeight()) + 1;
      node = node->parent;
    }
  }

  void rotateLeft(shared_ptr<Node> localRoot) {
    shared_ptr<Node> mid = localRoot->right;
    shared_ptr<Node> midSubtree = mid->left;

    mid->left = localRoot;
    localRoot->right = midSubtree;

    updateParentsChild(localRoot, mid);

    mid->parent = localRoot->parent;
    localRoot->parent = mid;
    if (midSubtree != nullptr) midSubtree->parent = localRoot;

    localRoot->height =
      1 + max(localRoot->leftHeight(), localRoot->rightHeight());
    mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());

    if (localRoot == root) root = mid;
  }

  void rotateRight(shared_ptr<Node> localRoot) {
    shared_ptr<Node> mid = localRoot->left;
    shared_ptr<Node> midSubtree = mid->right;

    localRoot->left = midSubtree;
    mid->right = localRoot;

    updateParentsChild(localRoot, mid);

    mid->parent = localRoot->parent;
    localRoot->parent = mid;
    if (midSubtree != nullptr) midSubtree->parent = localRoot;

    localRoot->height =
      1 + max(localRoot->leftHeight(), localRoot->rightHeight());
    mid->height = 1 + max(mid->leftHeight(), mid->rightHeight());

    if (localRoot == root) root = mid;
  }

  void rotateRightLeft(shared_ptr<Node> localRoot) {
    shared_ptr<Node> mid = localRoot->left;
    shared_ptr<Node> localLeaf = mid->right;

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
    localRoot->height =
      1 + max(localRoot->leftHeight(), localRoot->rightHeight());
    localLeaf->height =
      1 + max(localLeaf->leftHeight(), localLeaf->rightHeight());

    if (localRoot == root) root = localLeaf;
  }

  void rotateLeftRight(shared_ptr<Node> localRoot) {
    shared_ptr<Node> mid = localRoot->right;
    shared_ptr<Node> localLeaf = mid->left;

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
    localRoot->height =
      1 + max(localRoot->leftHeight(), localRoot->rightHeight());
    localLeaf->height =
      1 + max(localLeaf->leftHeight(), localLeaf->rightHeight());

    if (localRoot == root) root = localLeaf;
  }

  void balance() {
    shared_ptr<Node> node = root;
    while (true) {
      if (node->left != nullptr && abs(node->left->bf()) > 1)
        node = node->left;
      else if (node->right != nullptr && abs(node->right->bf()) > 1)
        node = node->right;
      else
        break;
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

  class Node {
   public:
    int height;
    shared_ptr<Node> left, right, parent;
    value_type keyValuePair;

    Node() = delete;
    Node(value_type pair_, shared_ptr<Node> parent)
      : keyValuePair(pair_), parent(std::move(parent)) {
      init();
    }
    explicit Node(value_type pair_)
      : keyValuePair(pair_) {
      init();
      parent = nullptr;
    }

    void init() {
      left = nullptr;
      right = nullptr;
      height = 1;
    }

    [[nodiscard]] Key key() const {
      return keyValuePair.first;
    }

    int leftHeight() {
      if (left == nullptr) return 0;
      else
        return left->height;
    }

    int rightHeight() {
      if (right == nullptr) return 0;
      else
        return right->height;
    }

    int bf() { return leftHeight() - rightHeight(); }
  };
};
