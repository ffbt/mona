#ifndef _MONA_COLLECTION_
#define _MONA_COLLECTION_

#include<types.h>

/*!
    \file  collection.h
    \brief collection utilities

    !!! in first use, you should debug this class. !!!

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/12/07 update:$Date$
*/

/*----------------------------------------------------------------------
    Bynary Tree
----------------------------------------------------------------------*/
template <class T> class BinaryTree {

  public:
    BinaryTree();
    virtual ~BinaryTree();

  public:
    dword size() const;
    T get(const dword key) const;
    bool contains(const dword key) const;
    void add(const dword key, const T element);

  private:
    struct Node {
        dword key;
        T element;
        Node* left;
        Node* right;
    };

    Node* root_;
    dword numberOfElements_;
    static const int NO_DATA = 0;

  private:
    void add(Node*& tree, const dword key, const T element);
    bool contains(const Node* tree, const dword key) const;
    void clear();
    void clear(Node*& tree);
    T get(const Node* tree, const dword key) const;
};

template <class T> BinaryTree<T>::BinaryTree() : root_(NO_DATA), numberOfElements_(0) {
}

template <class T> BinaryTree<T>::~BinaryTree() {
    clear();
}

template <class T> dword BinaryTree<T>::size() const {
    return numberOfElements_;
}

template <class T> T BinaryTree<T>::get(const dword key) const {
    return get(root_, key);
}

template <class T> void BinaryTree<T>::clear() {
    clear(root_);
}

template <class T> void BinaryTree<T>::add(const dword key, const T element) {

    add(root_, key, element);
    numberOfElements_++;
}

template <class T> bool BinaryTree<T>::contains(const dword key) const {
    return contains(root_, key);
}

template <class T> T BinaryTree<T>::get(const Node* tree, const dword key) const {

    if (key == tree->key) {
        return tree->element;
    } else if (key < tree->key) {
        return get(tree->left, key);
    } else {
        return get(tree->right, key);
    }
}

template <class T> void BinaryTree<T>::clear(Node*& tree) {

    if (tree != NO_DATA) {
        clear(tree->left);
        clear(tree->right);
        delete tree;
        tree = NO_DATA;
    }
}

template <class T> void BinaryTree<T>::add(Node*& tree, const dword key, const T element) {

    /* add */
    if (tree == NO_DATA) {
        tree = new Node;

        if (tree == NULL) {

            /* not implemented */
            for (;;);
        }

        tree->left    = NO_DATA;
        tree->right   = NO_DATA;
        tree->key     = key;
        tree->element = element;

    /* add to left */
    } else if (key < tree->key) {
        add(tree->left, key, element);

    /* add to right */
    } else {
        add(tree->right, key, element);
    }
}

template <class T> bool BinaryTree<T>::contains(const Node* tree, const dword key) const {

    if (tree == NO_DATA) {
        return false;
    } else if (key == tree->key) {
        return true;
    } else if (key < tree->key) {
        return contains(tree->left, key);
    } else {
        return contains(tree->right, key);
    }
}

/*----------------------------------------------------------------------
    HashMap
----------------------------------------------------------------------*/
template <class T> class HashMap : public Map {

  pulblic:
    HashMap();
    virtual ~HashMap();

  public:
    bool containsKey(const char* key) const;;
    bool containsValue(T value) const;
    T get(const char* key);
    T put(const char* key, T value);
    T remove(const char* key);
    dword size() const;



}

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
template <class T> class Map {

  public:
    bool containsKey(const char* key) const = 0;
    bool containsValue(T value)       const = 0;
    T get(const char* key)                  = 0;
    T put(const char* key, T value)         = 0;
    T remove(const char* key)               = 0;
    dword size()                      const = 0;
};

/*----------------------------------------------------------------------
    List Interface
----------------------------------------------------------------------*/
template <class T> class List {

  public:
    virtual void add(T element)              = 0;
    virtual T removeAt(size_t index)         = 0;
    virtual T remove(T element)              = 0;
    virtual T get(size_t index) const        = 0;
    virtual bool isEmpty() const             = 0;
    virtual size_t size() const              = 0;
    virtual bool hasElement(T element) const = 0;
};

/*----------------------------------------------------------------------
    HList Class
----------------------------------------------------------------------*/
template <class T> class HList : public List<T> {

  public:
    HList();
    HList(size_t size);
    HList(size_t size, size_t increase);
    virtual ~HList();

  public:
    void add(T element);
    T get(size_t index) const;
    T operator[](size_t index);
    T removeAt(size_t index);
    T remove(T element);
    size_t size() const;
    virtual bool isEmpty() const;
    bool hasElement(T element) const;

  private:
    T* data_;            /*! internal array     */
    size_t size_;        /*! size of liset      */
    size_t numElements_; /*! number of elements */
    size_t increase_;    /*! increase           */

    /* initilize */
    void init(size_t size, size_t increase);

};

/*!
    \brief constructor

    constructor default size is 5

    \author HigePon
    \date   create:2002/10/22 update:
*/
template <class T> HList<T>::HList() {

    init(5, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::HList(size_t size) {

    init(size, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list
    \param increase when resize this value used

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::HList(size_t size, size_t increase) {

    init(size, increase);
    return;
}

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> HList<T>::~HList() {

    /* release memory */
    delete[] data_;
    return;
}

/*!
    \brief isEmpty

    return is Empty or not

    \return true/false empty/has elements

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> bool HList<T>::isEmpty() const {

    return numElements_ == 0;
}

/*!
    \brief add element

    add element at the end of array

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> void HList<T>::add(T element) {

    /* if array is full */
    if (size_ == numElements_) {

        /* resize array */
        size_ += increase_;
        T* temp = new T[size_];

        /* copy original to new array */
        for (size_t i = 0; i < numElements_; i++) {
            temp[i] = data_[i];
        }
        delete[] data_;
        data_ = temp;
    }

    /* add element */
    data_[numElements_] = element;
    numElements_++;
    return;
}

/*!
    \brief get

    get element at index

    \param index index of element to get

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::get(size_t index) const {

    /* check range */
    if (index < 0 || index >=numElements_) {
        return (T)NULL;
    }
    return data_[index];
}

/*!
    \brief operator[]

    get element at index

    \param index index of element to get

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::operator[](size_t index) {

    return (this->get(index));
}

/*!
    \brief size

    return size of list

    \return size of list

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> size_t HList<T>::size() const {
    return numElements_;
}

/*!
    \brief remove element

    remove element at index

    \param index that removed

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::removeAt(size_t index) {

    /* check range */
    if (index < 0 || index >=numElements_) {

        /* do nothing */
        return (T)NULL;
    }

    /* save element to remove */
    T toRemove = data_[index];

    /* fix hole */
    for (size_t i = index; i < numElements_ - 1; i++) {
        data_[i] = data_[i + 1];
    }
    numElements_--;
    return toRemove;
}

/*!
    \brief remove element

    remove element

    \param element element to remove

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> T HList<T>::remove(T element) {

    for (dword i = 0; i < size(); i++) {

        /* element to remove found */
        if (data_[i] == element) {
            return (removeAt(i));
        }
    }

    return (T)NULL;
}

/*!
    \brief initilize

    set size of list & increase

    \author HigePon
    \date   create:2003/12/07 update:
*/
template <class T> void HList<T>::init(size_t size, size_t increase) {

    /* number of elements */
    numElements_ = 0;

    /* set size and increase */
    size_     = size     > 0 ? size : 5;
    increase_ = increase > 0 ? increase : 5;

    /* create internal array */
    data_ = new T[size_];
    return;
}

/*!
    \brief check list has the element

    \author HigePon
    \date   create:2003/12/21 update:
*/
template <class T> bool HList<T>::hasElement(T element) const {

    /* find element */
    for (dword i = 0; i < size(); i++) {

        if (data_[i] == element) {
            return true;
        }
    }

    /* not found */
    return false;
}

#endif
