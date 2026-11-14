#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    using TreeNode = AVLNode<Key, Value>;

    void replaceChild(TreeNode* parent, TreeNode* oldChild, TreeNode* newChild)
    {
        if (parent == nullptr) this->root_ = newChild;
        else if (parent->getLeft() == oldChild) parent->setLeft(newChild);
        else parent->setRight(newChild);
        if (newChild != nullptr) newChild->setParent(parent);
    }

    TreeNode* rotateLeft(TreeNode* node)
    {
        auto* pivot = node->getRight();
        auto* middle = pivot->getLeft();
        replaceChild(node->getParent(), node, pivot);
        node->setRight(middle);
        if (middle != nullptr) middle->setParent(node);
        pivot->setLeft(node);
        node->setParent(pivot);

        int pivotBalance = pivot->getBalance();
        int balance = node->getBalance() - 1 - (pivotBalance > 0 ? pivotBalance : 0);
        node->setBalance(balance);
        pivot->setBalance(pivotBalance - 1 + (balance < 0 ? balance : 0));
        return pivot;
    }

    TreeNode* rotateRight(TreeNode* node)
    {
        auto* pivot = node->getLeft();
        auto* middle = pivot->getRight();
        replaceChild(node->getParent(), node, pivot);
        node->setLeft(middle);
        if (middle != nullptr) middle->setParent(node);
        pivot->setRight(node);
        node->setParent(pivot);

        int pivotBalance = pivot->getBalance();
        int balance = node->getBalance() + 1 - (pivotBalance < 0 ? pivotBalance : 0);
        node->setBalance(balance);
        pivot->setBalance(pivotBalance + 1 + (balance > 0 ? balance : 0));
        return pivot;
    }

    TreeNode* rebalance(TreeNode* node)
    {
        if (node->getBalance() == -2) {
            if (node->getLeft()->getBalance() > 0) rotateLeft(node->getLeft());
            return rotateRight(node);
        }
        if (node->getRight()->getBalance() < 0) rotateRight(node->getRight());
        return rotateLeft(node);
    }


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    TreeNode* parent = nullptr;
    auto* current = static_cast<TreeNode*>(this->root_);
    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) current = current->getLeft();
        else if (current->getKey() < new_item.first) current = current->getRight();
        else {
            current->setValue(new_item.second);
            return;
        }
    }

    auto* node = new TreeNode(new_item.first, new_item.second, parent);
    if (parent == nullptr) {
        this->root_ = node;
        return;
    }
    if (new_item.first < parent->getKey()) parent->setLeft(node);
    else parent->setRight(node);

    while (parent != nullptr) {
        parent->updateBalance(node == parent->getLeft() ? -1 : 1);
        int balance = parent->getBalance();
        if (balance == 0) return;
        if (balance == -2 || balance == 2) {
            rebalance(parent);
            return;
        }
        node = parent;
        parent = parent->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    auto* node = static_cast<TreeNode*>(this->internalFind(key));
    if (node == nullptr) return;
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        nodeSwap(node, static_cast<TreeNode*>(this->predecessor(node)));
    }
    auto* parent = node->getParent();
    auto* child = node->getLeft() != nullptr ? node->getLeft() : node->getRight();
    int change = parent != nullptr && node == parent->getLeft() ? 1 : -1;
    replaceChild(parent, node, child);
    delete node;

    while (parent != nullptr) {
        parent->updateBalance(change);
        int balance = parent->getBalance();
        if (balance == -2 || balance == 2) parent = rebalance(parent);
        if (parent->getBalance() != 0) return;
        auto* subtree = parent;
        parent = subtree->getParent();
        change = parent != nullptr && subtree == parent->getLeft() ? 1 : -1;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
