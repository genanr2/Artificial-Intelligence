#pragma once
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
#include <stdexcept>
using namespace std;

template <typename E> class LinkedBinaryTree; // forward declaration to be used when declaring TNode
template <typename E> class Position; // forward declaration to be used when declaring TNode
#define UNVISITED 1;

template <typename E>
class TNode { // a node of the tree
public:
	E		elem;					// element value
	TNode<E> *par;					// parent
	TNode<E> *left;					// left child
	TNode<E> *right;				// right child
	TNode<E>() : elem(), par(NULL), left(NULL), right(NULL) { } // default constructor
																															// Node(E &e) : elem(e), par(NULL), left(NULL), right(NULL) { } // constructor
	friend class LinkedBinaryTree<E>; // provide LinkedBinaryTree access
	friend class Position<E>;		// provide Position access
};
template <typename E>
class Position {					// position in the tree
//private:
public:
	TNode<E> *v;						// pointer to the node
	Position(TNode<E>* _v = NULL) : v(_v) { }		// constructor
	E& operator*()					// get element
	{
		return v->elem;
	}
	E& operator*() const                   		// get const element
	{
		return v->elem;
	}

	Position left() const				// get left child
	{
		return Position(v->left);
	}
	Position right() const				// get right child
	{
		return Position(v->right);
	}
	Position parent() const				// get parent
	{
		return Position(v->par);
	}
	bool isRoot() const				// root of the tree?
	{
		return (v->par == NULL);
	}
	bool isExternal() const				// an external node?
	{
		return v->left == NULL && v->right == NULL;
	}
	friend class LinkedBinaryTree<E>;			// give tree access
};
template <typename E>					// base element type
class LinkedBinaryTree {
protected:
	// insert Node declaration here...
public:
	// insert Position declaration here...
public:
	LinkedBinaryTree();					// constructor
	int size() const;					// number of nodes
	bool empty() const;					// is tree empty?
	Position<E> root() const;				// get the root
	void addRoot(const E& e = E());					// add root to empty tree
	void addLeftLeaf(const Position<E>& p, const E& e);		// expand external node
	void addRightLeaf(const Position<E>& p, const E& e);		// expand external node
	void preorderPrint() const;	// print nodes in preorder 
	void inorderPrint() const;	// print nodes in inorder 
	void postorderPrint() const;	// print nodes in postorder 
	void expandExternal(const Position<E>& p);		// expand external node
	Position<E> removeAboveExternal(const Position<E>& p);	// remove p and parent
																										// housekeeping functions such as copy constructor omitted...
private:
	void preorder(TNode<E>* v) const;	// preorder utility
	void inorder(TNode<E>* v) const;	// inorder utility
	void postorder(TNode<E>* v) const;	// postorder utility
	TNode<E>* _root;					// pointer to the root
	int n;						// number of nodes
};
template <typename E>
class Tree
{
	class Position; // a node position
	class PositionList; // a list of positions
public: // public functions
	int size() {}// const; // number of nodes
	bool empty() {}//const; // is tree empty?
	Position root() {}// const; // get the root
	PositionList positions() {} //const; // get positions of all nodes
};
/*
int depth .itshR(coootn(s)t) Tree& T, const Position& p) 
{
	return 0; // root has depth 0
else
	return 1 + depth(T, p.parent()); // 1 + (depth of parent)
}
*/
template <typename E> class SLinkedList;	// forward declaration to be used when declaring SNode
template <typename E>
class SNode {					// singly linked list node
public:
	E getElem() const { return elem; };
	SNode<E> *getnext() const { return next; };
private:
	E elem;					// linked list element value
	SNode<E> *next;				// next item in the list
	friend class SLinkedList<E>;		// provide SLinkedList access
};

template <typename E>
class SLinkedList {				// a singly linked list
public:
	SLinkedList();				// empty list constructor
	~SLinkedList();				// destructor
	bool empty() const;			// is list empty?
	E& front();					// return front element
	void addFront(const E& e);		// add to front of list
	void removeFront();			// remove front item list
	int size() const;					// list size
	SNode<E> * gethead() const { return head; };
private:
	SNode<E>* head;				// head of the list
	int     n;							// number of items
};


template <typename K, typename V>
class Entry {
public:
	typedef K Key;
	typedef V Value;
	Entry(const Key &k = Key(), const Value &v = Value())
		: _key(k), _value(v) {}
	const Key& key() const { return _key; }
	const Value& value() const { return _value; }
	void setKey(const Key& k) { _key = k; }
	void setValue(const Value& v) { _value = v; }
private:
	Key _key;
	Value _value;
};

template <typename E>
class SearchTree {					// a binary search tree
public: 						// public types
	typedef typename E::Key K;				// a key
	typedef typename E::Value V;			// a value
																		// class Iterator;					// an iterator/position
public:						// public functions
	SearchTree();					// constructor
	int size() const; 					// number of entries
	bool empty() const;					// is the tree empty?
	Position<E> find(const K& k);				// find entry with key k
	SLinkedList<E>*  findPath(const K& k);
	Position<E> insert(const K& k, const V& x);		// insert (k,x)
	void erase(const K& k);	// remove key k entry
													//void erase(const Iterator& p);			// remove entry at p
													// Iterator begin();					// iterator to first entry
													//Iterator end();					// iterator to end entry
	void printInorder() const;
protected:						// local utilities
											// BinaryTree<E> BinaryTree;			// linked binary tree
											//typedef typename BinaryTree::Position TPos;		// position in the tree
	Position<E> root() const;					// get virtual root
	Position<E> finder(const K& k, Position<E>& v);		// find utility
	Position<E> inserter(const K& k, const V& x);		// insert utility
	void inorder(Position<E>& v) const; // inorder print utility
	Position<E> eraser(Position<E>& v);				// erase utility
																						// Position restructure(const TPos& v);	// restructure
private: 						// member data
	LinkedBinaryTree<E> T;					// the binary tree
	int n;						// number of entries
public:
	// ...insert Iterator class declaration here
};

class Graph {
private:
	void operator =(const Graph&) {} // Protect assignment
	Graph(const Graph&) {} // Protect copy constructor
public:
	Graph() {} // Default constructor
	virtual ~Graph() {} // Base destructor
										 // Initialize a graph of n vertices
	virtual void Init(int n) = 0;
	// Return: the number of vertices and edges
	virtual int n() = 0;
	virtual int e() = 0;
	// Return vs first neighbor
	virtual int first(int v) = 0;
	// Return vs next neighbor
	virtual int next(int v, int w) = 0;
	// Set the weight for an edge
	// i, j: The vertices
	// wgt: Edge weight
	virtual void setEdge(int v1, int v2, int wght) = 0;
	// Delete an Нахимовский проспект, 58
	// i, j: The vertices
	virtual void delEdge(int v1, int v2) = 0;
	// Determine if an edge is in the graph
	// i, j: The vertices
	// Return: true if edge i,j has non-zero weight
	virtual bool isEdge(int i, int j) = 0;
	// Return an edges weight
	// i, j: The vertices
	// Return: The weight of edge i,j, or zero
	virtual int weight(int v1, int v2) = 0;
	// Get and Set the mark value for a vertex
	// v: The vertex
	// val: The value to set
	virtual int getMark(int v) = 0;
	virtual void setMark(int v, int val) = 0;
};

class Edge {
	int vert, wt;
public:
	Edge() { vert = -1; wt = -1; }
	Edge(int v, int w) { vert = v; wt = w; }
	int vertex() { return vert; }
	int weight() { return wt; }
};

class Graphm : public Graph {
private:
	int numVertex, numEdge; // Store number of vertices, edges
	int **matrix; // Pointer to adjacency matrix
	int *mark; // Pointer to mark array
public:
	Graphm(int numVert) // Constructor
	{
		Init(numVert);
	}
	~Graphm() 
	{ // Destructor
		delete[] mark; // Return dynamically allocated memory
		for (int i = 0; i<numVertex; i++)
			delete[] matrix[i];
		delete[] matrix;
	}
	void Init(int n) { // Initialize the graph
		int i;
		numVertex = n;
		numEdge = 0;
		mark = new int[n]; // Initialize mark array
		for (i = 0; i<numVertex; i++)
			mark[i] = UNVISITED;
		matrix = (int**) new int*[numVertex]; // Make matrix
		for (i = 0; i<numVertex; i++)
			matrix[i] = new int[numVertex];
		for (i = 0; i< numVertex; i++) // Initialize to 0 weights
			for (int j = 0; j<numVertex; j++)
				matrix[i][j] = 0;
	}
	int n() { return numVertex; } // Number of vertices
	int e() { return numEdge; } // Number of edges
															// Return first neighbor of "v"
	int first(int v) {
		for (int i = 0; i<numVertex; i++)
			if (matrix[v][i] != 0) return i;
		return numVertex; // Return n if none
	}
	// Return vs next neighbor after w
	int next(int v, int w) {
		for (int i = w + 1; i<numVertex; i++)
			if (matrix[v][i] != 0)
				return i;
		return numVertex; // Return n if none
	}

	// Set edge (v1, v2) to "wt"
	void setEdge(int v1, int v2, int wt) 
	{
		ASSERT(wt>0, "Illegal weight value");
		if (matrix[v1][v2] == 0) numEdge++;
		matrix[v1][v2] = wt;
	}
	void delEdge(int v1, int v2) { // Delete edge (v1, v2)
		if (matrix[v1][v2] != 0) numEdge--;
		matrix[v1][v2] = 0;
	}
	bool isEdge(int i, int j) // Is (i, j) an edge?
	{
		return matrix[i][j] != 0;
	}
	int weight(int v1, int v2) { return matrix[v1][v2]; }
	int getMark(int v) { return mark[v]; }
	void setMark(int v, int val) { mark[v] = val; }
};



/*
class Graphl : public Graph {
private:
	CList<Edge>** vertex; // List headers
//	CAtlList<Edge>** vertex2; // List headers
	CHttpResponse *aa;
	int numVertex, numEdge; // Number of vertices, edges
	int *mark; // Pointer to mark array
public:
	Graphl(int numVert)
	{
		Init(numVert);
	}
	Graphl() { // Destructor
		delete[] mark; // Return dynamically allocated memory
		for (int i = 0; i<numVertex; i++) delete[] vertex[i];
		delete[] vertex;
	}
	void Init(int n) {
		int i;
		numVertex = n;
		numEdge = 0;
		mark = new int[n]; // Initialize mark array
		for (i = 0; i<numVertex; i++) mark[i] = UNVISITED;
		// Create and initialize adjacency lists
		vertex = (CList<Edge>**) new CList<Edge>*[numVertex];
		for (i = 0; i<numVertex; i++)
			vertex[i] = new CList<Edge>();
	}
	int n() { return numVertex; } // Number of vertices
	int e() { return numEdge; } // Number of edges
	int first(int v) 
	{ // Return first neighbor of "v"
		if (vertex[v]->length() == 0)
			return numVertex; // No neighbor
		vertex[v]->moveToStart();
		Edge it = vertex[v]->getValue();
		return it.vertex();
	}
	// Get vs next neighbor after w
	int next(int v, int w) {
		Edge it;
		if (isEdge(v, w)) {
			if ((vertex[v]->currPos() + 1) < vertex[v]->length()) {
				vertex[v]->next();
				it = vertex[v]->getValue();
				return it.vertex();
			}
		}
		return n(); // No neighbor
	}
	// Set edge (i, j) to "weight"
	void setEdge(int i, int j, int weight) {
		Assert(weight>0, "May not set weight to 0");
		Edge currEdge(j, weight);
		if (isEdge(i, j)) { // Edge already exists in graph
			vertex[i]->remove();
			vertex[i]->insert(currEdge);
		}
		else { // Keep neighbors sorted by vertex index
			numEdge++;
			for (vertex[i]->moveToStart();
				vertex[i]->currPos() < vertex[i]->length();
				vertex[i]->next()) {
				Edge temp = vertex[i]->getValue();
				if (temp.vertex() > j) break;
			}
			vertex[i]->insert(currEdge);
		}
	}
	void delEdge(int i, int j) { // Delete edge (i, j)
		if (isEdge(i, j)) {
			vertex[i]->remove();
			numEdge--;
		}
	}
	bool isEdge(int i, int j) { // Is (i,j) an edge?
		Edge it;
		for (vertex[i]->moveToStart();
			vertex[i]->currPos() < vertex[i]->length();
			vertex[i]->next()) { // Check whole list
			Edge temp = vertex[i]->getValue();
			if (temp.vertex() == j) return true;
		}
		return false;
	}
	int weight(int i, int j) { // Return weight of (i, j)
		Edge curr;
		if (isEdge(i, j)) {
			curr = vertex[i]->getValue();
			return curr.weight();
		}
		else return 0;
	}
	int getMark(int v) { return mark[v]; }
	void setMark(int v, int val) { mark[v] = val; }
};
*/