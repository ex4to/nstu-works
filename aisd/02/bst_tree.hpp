#include <climits>
#include <iostream>

template <class T>
class bst_tree {
 public:
  class Node {
   public:
    friend class bst_tree;

    Node(){};
    Node(T _val)
        : val(_val),
          Parent(nullptr),
          L_tree(nullptr),
          R_tree(nullptr),
          height(1){};

    T& operator*() { return val; }
    T get_val() { return val; }
    Node* getParent() { return Parent; }
    Node* get_R() { return R_tree; }
    Node* get_L() { return L_tree; }
    Node* get_P() { return Parent; }
    unsigned char get_H() { return height; }

    void set_H(unsigned char newH) { this->height = newH; }
    void setParent(Node* parent) { this->Parent = parent; }
    void set_R(Node* newR) { this->R_tree = newR; }
    void set_L(Node* newL) { this->L_tree = newL; }
    void set_P(Node* newP) { this->Parent = newP; }

   protected:
    T val;
    Node* Parent;
    Node* L_tree;
    Node* R_tree;
    unsigned char height;
  };

  class Iterator {
   public:
    friend class bst_tree;

    Iterator(Node* _node) : m_node(_node) {}

    T& operator*() { return m_node->val; }
    T& operator->() { return &m_node->val; }

    Node* get_node() { return m_node; }

    Iterator& operator++() {
      if (*this == nullptr) throw std::overflow_error("Haha");

      if (m_node->R_tree) {
        m_node = m_node->R_tree;

        while (m_node->L_tree) {
          m_node = m_node->L_tree;
        }
      } else {
        Node* parent = m_node->Parent;
        Node* lastOne = m_node;

        while (parent != nullptr && m_node == parent->R_tree) {
          m_node = parent;
          parent = m_node->Parent;
        }

        lastOne = m_node;
        if (parent != nullptr && m_node->R_tree != parent) m_node = parent;
        if (m_node == lastOne) m_node = nullptr;
      }

      return *this;
    }

    Iterator operator++(int) {
      if (*this == nullptr) throw std::overflow_error("Haha");      

      auto old = *this;
      ++(*this);
      return old;
    }

    Iterator& operator--() {
      if (m_node->L_tree) {
        Node* prev = m_node->L_tree;
        while (prev->R_tree) prev = prev->R_tree;
        m_node = prev;
      } else {
        Node* parent = m_node->Parent;

        while (parent && m_node == parent->L_tree) {
          m_node = parent;
          parent = parent->Parent;
        }
        m_node = parent;
      }

      return *this;
    }

    Iterator operator--(int) {
      auto old = *this;
      --(*this);
      return old;
    }
    /* m_node == other.m_node */
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
      return lhs.m_node == rhs.m_node;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
      return lhs.m_node != rhs.m_node;
    }

   protected:
    Node* m_node;
  };

  Iterator begin() const { return Iterator(find_min(root)); }
  Iterator rbegin() const { return Iterator(find_max(root)); }
  Iterator end() const { return Iterator(nullptr); }
  Iterator rend() const { return Iterator(nullptr); }
  Iterator it_root() const { return Iterator(root); }

#ifndef RECURSIVE
  Iterator find(const T& _val) {
    steps = 0;
    for (auto iter = begin(); iter != end(); iter++, steps++) {
      if (*iter == _val) return iter;
    }
    return end();
  }
#else
  Iterator find(const T& _val) {
      steps = -1;
      return find(root, _val);
  }

  Iterator find(Node* node, const T& _val) {
      steps++;
      if (node == nullptr) return end();
      if (node->val > _val) return find(node->R_tree, _val);
      if (node->val < _val) return find(node->L_tree, _val);
      return Iterator(node);
  }
#endif

  Node* find_min() const { return find_min(root); }
  Node* find_max() const { return find_max(root); }

  Node* find_min(Node* leaf) const {
    while (leaf && leaf->L_tree) leaf = leaf->L_tree;
    return leaf;
  }

  Node* find_max(Node* leaf) const {
    while (leaf->R_tree != nullptr) leaf = leaf->R_tree;
    return leaf;
  }

  Node* get_root() { return !root ? nullptr : root; }

#ifndef RECURSIVE
  void insert(Node* leaf, const T& _val) {
    steps = 1;
    Node* p = new Node(_val);
    Node* child = leaf;
    Node* parent = nullptr;

    while (child) {
      steps++;
      parent = child;
      if (_val < child->val) {
        child = child->L_tree;
      } else if (_val > child->val) {
        child = child->R_tree;
      } else {
        break;
      }
    }

    p->Parent = parent;

    if (_val < parent->val) {
      parent->L_tree = p;
      size++;
    } else if (_val > parent->val) {
      parent->R_tree = p;
      size++;
    }
  }
#elif
  void insert(Node* node, const T& _val) {
    steps++;

    if (node->val == _val) return;
    else if (node->val < _val) {
        if (node->L_tree) return insert(node->L_tree, _val);
        node->L_tree = new Node(_val);
        node->L_tree->Parent = node;
    } else {
        if (node->R_tree) return insert(node->R_tree, _val);
        node->R_tree = new Node(_val);
        node->R_tree->Parent = node;
    }

    size++;
  }
#endif

  void insert(const T& _val) {
    steps = 0;
    if (!root) {
      steps = 1;
      root = new Node(_val);
      root->Parent = nullptr;
      size++;
    } else {
      insert(root, _val);
    }
  }

  void erase(const T& value) {
    steps = 0;
    erase(find(value));
  }

  void erase(Iterator pos) {
    if (pos == end()) return;

    Node* node = pos.m_node;
    Node* parent = node->Parent;
    int cleared = 0;

    if (!parent) {
      if (root->R_tree) root = root->R_tree;
      if (root->L_tree) root = root->L_tree;
      if (!root->L_tree && !root->R_tree) cleared = 1;
    }

    if (!node->L_tree && !node->R_tree && !cleared) {
      (node == parent->L_tree ? parent->L_tree : parent->R_tree) = nullptr;
    } else if (node->L_tree && node->R_tree) {
      Node* newParent = find_min(node->R_tree);
      newParent->L_tree = node->L_tree;
      newParent->L_tree->Parent = newParent;
      node->R_tree->Parent = parent;

      if (parent) {
        (node == parent->L_tree ? parent->L_tree : parent->R_tree) =
            node->R_tree;
      }
    } else if (parent) {
      Node* child = (node->L_tree == nullptr ? node->R_tree : node->L_tree);
      child->Parent = parent;
      (node == parent->L_tree ? parent->L_tree : parent->R_tree) = child;
    }

    size--;
    delete node;
  }

  void inorder(Node* leaf) {
    using namespace std;

    if (leaf != nullptr) {
      inorder(leaf->L_tree);
      inorder(leaf->R_tree);
      cout << leaf->val << " ";
    }
  }

  void by_plus() {
    using std::cout;

    for (auto it = begin(); it != end(); it++) {
      cout << *it << " ";
    }
    cout << std::endl;
  }

  void by_minus() {
    using std::cout;

    for (auto it = rbegin(); it != rend(); it--) {
      cout << *it << " ";
    }
    cout << std::endl;
  }

  void display() {
    using namespace std;

    inorder(root);
    cout << endl;
  }

  void clear(Node* leaf) {
    if (leaf != nullptr && size > 0) {
      clear(leaf->L_tree);
      clear(leaf->R_tree);
      delete leaf;
    }
    size = 0;
  }

  const T& get_val(Node* leaf) { return leaf->val; }
  const T& get_val(Iterator leaf) { return leaf.m_node->val; }
  int CountNodes() { return steps; }

  int get_size() { return size; }
  int is_empty() { return root == nullptr; }

  void merge(const bst_tree& src) {
    for (auto item = src.it_root(); item != src.end(); item++) {
      insert(*item);
    }

    for (auto item = src.begin(); item != src.it_root(); item++) {
      insert(*item);
    }
  }

  bst_tree& operator=(const bst_tree& src) {
    clear(root);
    size = 0;
    root = nullptr;

    for (auto item = src.it_root(); item != src.end(); item++) {
      insert(*item);
    }

    for (auto item = src.begin(); item != src.it_root(); item++) {
      insert(*item);
    }

    return *this;
  }

  bst_tree() {
    root = nullptr;
    size = 0;
    steps = 0;
  };

  bst_tree(const bst_tree& src) : bst_tree() { *this = src; }

  ~bst_tree() { clear(root); };

 protected:
  Node* root;
  int size;
  int steps;
};
