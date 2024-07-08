#include <bits/stdc++.h>
#include <algorithm>
#include <utility>
using namespace std;

template<
  class Key,
  class T,
  class Compare = std::less<Key>,
  class Allocator = std::allocator<std::pair<const Key, T>>>
class AVLTree {
  class Node;
  typedef std::pair<const Key, T> value_type;
  typedef AVLTree<Key, T, Compare, Allocator> tree_type;
  typedef std::shared_ptr<Node> NodePtr;

 public:
  class Iterator;
  class reversedIterator;

  AVLTree() : root(nullptr), head(nullptr), tail(nullptr) {}

  AVLTree(const std::initializer_list<value_type> &lst) {
    for (const auto &x : lst)
      insert(x);
  }

  T &operator[](const Key &key_) noexcept {
    insert({key_, T{}});
    return find(key_)->value;
  }

  [[nodiscard]] T &at(const Key &key_) {
    if (!contains(key_)) throw std::out_of_range("key does not exist");
    return find(key_)->value;
  }

  [[nodiscard]] const T &at(const Key &key_) const {
    if (!contains(key_)) throw std::out_of_range("key does not exist");
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
    NodePtr place = find(key_, [](NodePtr n) { n->height++; });
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
    NodePtr place = find(key_).curr;
    if (place == nullptr) return;
    if (key_ == head->key())
      head = successor(head);
    else if (key_ == tail->key())
      tail = predecessor(tail);
    --size_;

    // if 2 children
    while (place->left != nullptr && place->right != nullptr) {
      NodePtr succ = successor(place);

      swap(const_cast<Key&>(place->keyValuePair.first),
      const_cast<Key&>(succ->keyValuePair.first));

      swap(place->keyValuePair.second, succ->keyValuePair.second);

      place = succ;
    }
    // one or no children
    NodePtr
      child = place->left == nullptr ? place->right : place->left;

    if (child != nullptr) {
      swap(const_cast<Key&>(place->keyValuePair.first),
           const_cast<Key&>(child->keyValuePair.first));

      swap(place->keyValuePair.second, child->keyValuePair.second);
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
    NodePtr node = root;

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
    friend class AVLTree;
   public:
    Iterator(tree_type *tree, NodePtr node_)
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
    NodePtr curr;
    tree_type *parentTree;
  };

  class reversedIterator: public Iterator {
   public:
    reversedIterator(tree_type *tree, NodePtr node_)
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

  // Needed to resolve circular reference inside Node class,
  // weak_ptr is hard to implement at this stage
  ~AVLTree() {
    vector<NodePtr> toDestroy;
    toDestroy.reserve(size());
    for (auto it = begin(); it != end(); ++it)
      toDestroy.push_back(it.curr);
    for (auto& node: toDestroy)
      node->~Node();
  }

 private:
  NodePtr root, tail, head;
  size_t size_ = 0;
  Allocator alloc;

  [[nodiscard]] NodePtr
  subtreeMaximum(NodePtr node) const noexcept {
    while (node && node->right != nullptr)
      node = node->right;
    return node;
  }

  [[nodiscard]] NodePtr
  subtreeMinimum(NodePtr node) const noexcept {
    while (node && node->left != nullptr)
      node = node->left;
    return node;
  }

  [[nodiscard]] NodePtr findClosest(const Key &key_) const {
    NodePtr node = root;
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

  NodePtr successor(NodePtr node) {
    if (node == nullptr) {
      return nullptr;
    }
    if (node->right != nullptr) {
      return subtreeMinimum(node->right);
    } else {
      NodePtr y = node->parent;
      while (y != nullptr && y->parent != nullptr && node == y->right) {
        node = y;
        y = y->parent;
      }
      if (!y) return y;
      return (Compare{}(y->key(), node->key()) ? nullptr : y);
    }
  }

  NodePtr predecessor(NodePtr node) {
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
  [[nodiscard]] NodePtr find(const Key &key_, Action act) {
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

  void updateParentsChild(NodePtr oldChild, NodePtr newChild) {
    NodePtr parent = oldChild->parent;
    if (parent != nullptr && parent->left == oldChild)
      parent->left = newChild;
    else if (parent != nullptr && parent->right == oldChild)
      parent->right = newChild;
  }

  void recalculateHeight(NodePtr node) {
    while (node != nullptr) {
      node->height = max(node->rightHeight(), node->leftHeight()) + 1;
      node = node->parent;
    }
  }

  void rotateLeft(NodePtr localRoot) {
    NodePtr mid = localRoot->right;
    NodePtr midSubtree = mid->left;

    mid->left = localRoot;
    localRoot->right = midSubtree;

    updateParentsChild(localRoot, mid);

    mid->parent = localRoot->parent;
    localRoot->parent = mid;
    if (midSubtree != nullptr) midSubtree->parent = localRoot;

    localRoot->height =
      1 + std::max(localRoot->leftHeight(), localRoot->rightHeight());
    mid->height = 1 + std::max(mid->leftHeight(), mid->rightHeight());

    if (localRoot == root) root = mid;
  }

  void rotateRight(NodePtr localRoot) {
    NodePtr mid = localRoot->left;
    NodePtr midSubtree = mid->right;

    localRoot->left = midSubtree;
    mid->right = localRoot;

    updateParentsChild(localRoot, mid);

    mid->parent = localRoot->parent;
    localRoot->parent = mid;
    if (midSubtree != nullptr) midSubtree->parent = localRoot;

    localRoot->height =
      1 + std::max(localRoot->leftHeight(), localRoot->rightHeight());
    mid->height = 1 + std::max(mid->leftHeight(), mid->rightHeight());

    if (localRoot == root) root = mid;
  }

  void rotateRightLeft(NodePtr localRoot) {
    NodePtr mid = localRoot->left;
    NodePtr localLeaf = mid->right;

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

    mid->height = 1 + std::max(mid->leftHeight(), mid->rightHeight());
    localRoot->height =
      1 + std::max(localRoot->leftHeight(), localRoot->rightHeight());
    localLeaf->height =
      1 + std::max(localLeaf->leftHeight(), localLeaf->rightHeight());

    if (localRoot == root) root = localLeaf;
  }

  void rotateLeftRight(NodePtr localRoot) {
    NodePtr mid = localRoot->right;
    NodePtr localLeaf = mid->left;

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

    mid->height = 1 + std::max(mid->leftHeight(), mid->rightHeight());
    localRoot->height =
      1 + std::max(localRoot->leftHeight(), localRoot->rightHeight());
    localLeaf->height =
      1 + std::max(localLeaf->leftHeight(), localLeaf->rightHeight());

    if (localRoot == root) root = localLeaf;
  }

  void balance() {
    NodePtr node = root;
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
    NodePtr left, right, parent;
    value_type keyValuePair;

    Node() = delete;
    Node(value_type pair_, NodePtr parent)
      : keyValuePair(pair_), parent(std::move(parent)) {
      init();
    }
    explicit Node(value_type pair_)
      : keyValuePair(pair_) {
      init();
      parent = nullptr;
    }

    void init() {
      left = right =  nullptr;
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

    ~Node() {
      left = right = parent = nullptr;
    }
  };
};
