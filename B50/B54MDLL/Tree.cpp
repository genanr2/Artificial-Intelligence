#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#ifndef _STRING_
#include "string"
#endif
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include "tree.h"
template <typename E>
LinkedBinaryTree<E>::LinkedBinaryTree()			// constructor
	: _root(NULL), n(0) { }
template <typename E>
int LinkedBinaryTree<E>::size() const			// number of nodes
{
	return n;
}
template <typename E>
bool LinkedBinaryTree<E>::empty() const			// is tree empty?
{
	return size() == 0;
}

template <typename E>
Position<E> LinkedBinaryTree<E>::root() const // get the root
{
	return Position<E>(_root);
}

template <typename E>
void LinkedBinaryTree<E>::addRoot(const E& e)			// add root to empty tree
{
	_root = new TNode<E>;
	_root->elem = e;
	n = 1;
}

// expand external node
template <typename E>
void LinkedBinaryTree<E>::addLeftLeaf(const Position<E>& p, const E& e) {
	TNode<E>* v = p.v;					// p's node
	v->left = new TNode<E>;				// add a new left child
	v->left->elem = e;					// v is its parent
	v->left->par = v;					// v is its parent
	n++;								// one more node
}

// expand external node
template <typename E>
void LinkedBinaryTree<E>::addRightLeaf(const Position<E>& p, const E& e) {
	TNode<E>* v = p.v;					// p's node
	v->right = new TNode<E>;				// add a new left child
	v->right->elem = e;					// v is its parent
	v->right->par = v;					// v is its parent
	n++;								// one more node
}

template <typename E>
void LinkedBinaryTree<E>::preorderPrint() const {
	preorder(_root);
}

template <typename E>
void LinkedBinaryTree<E>::preorder(TNode<E> *v) const {
	if (v == NULL) return;
	cout << v->elem << endl;
	preorder(v->left);
	preorder(v->right);
}

template <typename E>
void LinkedBinaryTree<E>::inorderPrint() const {
	inorder(_root);
}

template <typename E>
void LinkedBinaryTree<E>::inorder(TNode<E> *v) const {
	if (v == NULL) return;
	inorder(v->left);
	cout << v->elem << endl;
	inorder(v->right);
}

template <typename E>
void LinkedBinaryTree<E>::postorderPrint() const {
	postorder(_root);
}

template <typename E>
void LinkedBinaryTree<E>::postorder(TNode<E> *v) const {
	if (v == NULL) return;
	postorder(v->left);
	postorder(v->right);
	cout << v->elem << endl;
}

// expand external node
template <typename E>
void LinkedBinaryTree<E>::expandExternal(const Position<E>& p) {
	TNode<E>* v = p.v;					// p's node
	v->left = new TNode<E>;					// add a new left child
	v->left->par = v;					// v is its parent
	v->right = new TNode<E>;				// and a new right child
	v->right->par = v;					// v is its parent
	n += 2;						// two more nodes
}


// remove p and parent
template <typename E>
Position<E> LinkedBinaryTree<E>::removeAboveExternal(const Position<E>& p) {
	TNode<E>* w = p.v;
	TNode<E>* v = w->par;			// get p's node and parent
	TNode<E>* sib = (w == v->left ? v->right : v->left);
	if (v == _root) {					// child of root?
		_root = sib;					// ...make sibling root
		sib->par = NULL;
	}
	else {
		TNode<E>* gpar = v->par;				// w's grandparent
		if (v == gpar->left) gpar->left = sib; 		// replace parent by sib
		else gpar->right = sib;
		sib->par = gpar;
	}
	delete w;
	delete v;					// delete removed nodes
	n -= 2;						// two fewer nodes
	return Position<E>(sib);
}



template <typename E>					// constructor
SearchTree<E>::SearchTree() : T(), n(0)
{
	T.addRoot();
}

template <typename E>					// get virtual root
Position<E> SearchTree<E>::root() const
{
	return T.root();
}

template <typename E>
int SearchTree<E>::size() const			// number of nodes
{
	return n;
}

template <typename E>
bool SearchTree<E>::empty() const			// is tree empty?
{
	return size() == 0;
}
template <typename E>
void SearchTree<E>::printInorder() const			// is tree empty?
{
	cout << "Keys in order: ";
	inorder(root());
	cout << endl;
}
template <typename E>
void SearchTree<E>::inorder(Position<E>& v) const			// is tree empty?
{
	if (v.isExternal()) return;
	inorder(v.left());
	cout << (*v).key() << ", ";
	inorder(v.right());
}
template <typename E>					// find utility
Position<E> SearchTree<E>::finder(const K& k, Position<E>& v) {
	Position<E> l = v.left(), r = v.right();
	if (v.isExternal()) return v;			// key not found
	if (k < (*v).key()) return finder(k, l);	// search left subtree
	else if ((*v).key() < k) return finder(k, r);	// search right subtree
	else return v;					// found it here
}
template <typename E>					// find entry with key k
Position<E> SearchTree<E>::find(const K& k) {
	Position<E> r = root();
	return finder(k, r);				// search from root
}
// TO DO: implement the finderPath function
template <typename E>					// find entry with key k
SLinkedList<E>* SearchTree<E>::findPath(const K& k) {
	// TO DO: you need to implement a function to return the entries of search path
	//         OK to create another member function to be called here if needed
	return nullptr;
}
template <typename E>					// insert utility
Position<E> SearchTree<E>::inserter(const K& k, const V& x) {
	Position<E> r = root();
	Position<E> v = finder(k, r);				// search from root
	if (!v.isExternal()) {			// key already exists?
		(*v).setValue(x);			// update value
	}
	else {
		T.expandExternal(v);		// add new internal node
		(*v).setKey(k);
		(*v).setValue(x);			// set entry
		n++;						// one more entry
	}
	return v;						// return insert position
}
template <typename E>					// insert (k,x)
Position<E> SearchTree<E>::insert(const K& k, const V& x)
{
	return inserter(k, x);
}
template <typename E>					// remove utility
Position<E> SearchTree<E>::eraser(Position<E>& v) {
	Position<E> w;
	if (v.left().isExternal())
		w = v.left();		// remove from left
	else if (v.right().isExternal())
		w = v.right();	// remove from right
	else {						// both internal?
		w = v.right();					// go to right subtree
		do { w = w.left(); } while (!w.isExternal());	// get leftmost node
		Position<E> u = w.parent();
		(*v).setKey((*u).key());
		(*v).setValue((*u).value());	// copy w's parent to v
	}
	n--;						// one less entry
	return T.removeAboveExternal(w);			// remove w and parent
}

template <typename E>					// remove key k entry
void SearchTree<E>::erase(const K& k) {
	Position<E> v = finder(k, root());	// search from virtual root
	if (v.isExternal())					// not found?
		throw out_of_range("Erase of nonexistent");
	eraser(v);						// remove it
}

template <typename E>
SLinkedList<E>::SLinkedList()			// constructor
	: head(NULL), n(0) { }

template <typename E>
bool SLinkedList<E>::empty() const		// is list empty?
{
	return head == NULL; // can also use return (n == 0);
}

template <typename E>
E& SLinkedList<E>::front() 		// return front element
{
	if (empty()) throw length_error("empty list");
	return head->elem;
}
template <typename E>
SLinkedList<E>::~SLinkedList()			// destructor
{
	while (!empty()) removeFront();
}
template <typename E>
void SLinkedList<E>::addFront(const E& e) {	// add to front of list
	SNode<E>* v = new SNode<E>;		// create new node
	v->elem = e;				// store data
	v->next = head;				// head now follows v
	head = v;				// v is now the head
	n++;
}
template <typename E>
void SLinkedList<E>::removeFront() {		// remove front item
	if (empty()) throw length_error("empty list");
	SNode<E>* old = head;			// save current head
	head = old->next;			// skip over old head
	delete old;				// delete the old head
	n--;
}
template <typename E>
int SLinkedList<E>::size() const {				// list size
	return n;
}
/*
int main() {

	SearchTree<Entry<string, string> > *bst1;
	SearchTree<Entry<string, string> > *bst2;
	Position<Entry<string, string> > p;
	SLinkedList<Entry<string, string>> *sll1;
	SLinkedList<Entry<string, string>> *sll2;
	cout << "==== datafile test ====" << endl;

	bst1 = buildBST_fromDatafile("datafile1.txt");
	bst2 = buildBST_fromDatafile("datafile2.txt");
	assert(bst1->size() == 171);
	assert(bst2->size() == 171);
	p = bst1->find("Steven");
	assert((*p).value() == "Hands of Stone");
	cout << "datafile1 is loaded successfully" << endl;
	p = bst2->find("Steven");
	assert((*p).value() == "Hands of Stone");
	cout << "datafile2 is loaded successfully" << endl;

	cout << "==== findPath test ====" << endl;
	sll1 = bst1->findPath("Steven");
	cout << "Steven: " << sll1->size() << " steps" << endl;
	sll2 = bst2->findPath("Steven");
	cout << "Steven: " << sll2->size() << " steps" << endl;
	printInReverseOder(sll2->gethead());

	delete(sll1); 	sll1 = NULL;
	delete(sll2);   sll2 = NULL;
	cout << "==== Your own name test ====" << endl;

	// TO DO : you need to search your own first name here
	string myownFirstName = "put your own first name here";

	sll1 = bst1->findPath(myownFirstName);
	cout << myownFirstName << ": " << sll1->size() << " steps" << endl;
	sll2 = bst2->findPath(myownFirstName);
	cout << myownFirstName << ": " << sll2->size() << " steps" << endl;
	printInReverseOder(sll2->gethead());
	delete(sll1);
	delete(sll2);
	delete(bst1);
	delete(bst2);
	return 0;
}*/
