#include <iostream>
#include <algorithm>  // std::max
#include <cstdlib>    // std::abs
#include <stack>
#include <vector>
#include <cassert>
using namespace std;
template<typename TKey, typename TValue>

// AVL Tree Class Implementation...
class avltree {

private:

	struct NODE {
		TKey   Key;
		TValue Value;
		int    Height;
		NODE*  Left;
		NODE*  Right;
	};

	NODE* Root;  // pointer to root node of tree (nullptr if empty)
	int   Size;  // # of nodes in the tree (0 if empty)

	
	// Helper function to make a full copy of tree. Calls 
	// insert to build new tree as original tree is traversed
	// in preorder fashion.
	void _copybinarysearchtree(NODE* cur) {
		
		if (cur == NULL) {
			return;
		}
		insert(cur->Key, cur->Value);
		_copybinarysearchtree(cur->Left);
		_copybinarysearchtree(cur->Right);
		
	}


	// Helper function for destructor to recurse till leafs of
	// tree to then delete the node. In other words, deletes
	// the tree in postorder fashion.
	void _deletebinarysearchtree(NODE* cur) {

		if (cur == nullptr) {
			return;
		}
		_deletebinarysearchtree(cur->Left);
		_deletebinarysearchtree(cur->Right);
		delete cur; // Delete very last node in tree.

  	}


	// Helper function to print tree inorder. Recurses till 
	// end of left most node, then prints, then recurses
	// back to parent node, prints, then recurses to right node
	// then prints.
  	void _inorder(NODE* cur) {
	
		if (cur == nullptr){
			return;
		}
		_inorder(cur->Left);
		cout << "         Key:  " << cur->Key << endl;
		cout << "         Value:  " << cur->Value << endl;
		cout << "-------------------------------" << endl;
		_inorder(cur->Right);

	}


	// 3 cases, N is the root, N is to the left of Parent,
	// and N is to right of Parent. In each case, perform a
	// right rotate by resigning the pointers for each node 
	// correctly, then pushing them onto stack to update
	// their heights.
	void _RightRotate(NODE* Parent, NODE* N) {

		// Preconditions
		assert(N != nullptr);
		assert(N->Left != nullptr);

		stack<NODE*> nodes; // Stack used to update heights at the end.

		if (Parent == nullptr) { // Node N is root.
			Root = N->Left;
			N->Left = Root->Right;
			Root->Right = N;
			// Push onto stack to update heights.
			nodes.push(Root);
			nodes.push(N);
		}
		else if (N->Key < Parent->Key) { // To the left.
			Parent->Left = N->Left;
			N->Left = N->Left->Right;
			Parent->Left->Right = N;
			// Push onto stack to update heights.
			nodes.push(Parent);
			nodes.push(Parent->Left);
			nodes.push(N);
		}
		else if (N->Key > Parent->Key) { // To the right.
			Parent->Right = N->Left;
			N->Left = N->Left->Right;
			Parent->Right->Right = N;
			// Push onto stack to update heights.
			nodes.push(Parent);
			nodes.push(Parent->Right);
			nodes.push(N);
		}

		// Update heights on stack
		while (!nodes.empty()) {
			NODE* cur = nodes.top();
			nodes.pop();
			int hL = (cur->Left == nullptr) ? -1 : cur->Left->Height;
			int hR = (cur->Right == nullptr) ? -1 : cur->Right->Height;
			int hCur = 1 + std::max(hL, hR);
			cur->Height = hCur;
		}

	}


	// 3 cases, N is the root, N is to the left of Parent,
	// and N is to right of Parent. In each case, perform a
	// left rotate by resigning the pointers for each node 
	// correctly, then pushing them onto stack to update
	// their heights.
	void _LeftRotate(NODE* Parent, NODE* N) {

		// Pre conditions
		assert(N != nullptr);
		assert(N->Right != nullptr);
    
		stack<NODE*> nodes; // Stack used to update heights at the end.

		if (Parent == nullptr) { // Node N is root.
			Root = N->Right;
			N->Right = Root->Left;
			Root->Left = N;
			// Push onto stack to update heights.
			nodes.push(Root);
			nodes.push(N);
		}
		else if (N->Key < Parent->Key) { // To the left.
			Parent->Left = N->Right;
			N->Right = N->Right->Left;
			Parent->Left->Left = N;
			// Push onto stack to update heights.
			nodes.push(Parent);
			nodes.push(Parent->Left);
			nodes.push(N);
		}
		else if (N->Key > Parent->Key) { // To the right.
			Parent->Right = N->Right;
			N->Right = N->Right->Left;
			Parent->Right->Left = N;
			// Push onto stack to update heights.
			nodes.push(Parent);
			nodes.push(Parent->Right);
			nodes.push(N);
		}

		// Update heights on stack
		while (!nodes.empty()) {
			NODE* cur = nodes.top();
			nodes.pop();
			int hL = (cur->Left == nullptr) ? -1 : cur->Left->Height;
			int hR = (cur->Right == nullptr) ? -1 : cur->Right->Height;
			int hCur = 1 + std::max(hL, hR);
			cur->Height = hCur;
		}

	}


	// Finds Pivots left and right subtree heights to determine
	// if left leaning or right leaning. If left leaning,
	// there are 2 cases, Right Rotate & Left-Right Rotate.
	// Similarly, if right leaning, there are 2 cases,
	// Left Rotate & Right-Left Rotate.
	void _rotateToFix(NODE* Parent, NODE* Pivot) {

		assert(Pivot != nullptr);
		// Get Pivots left and right subtree heights.
		int HL = (Pivot->Left == nullptr) ? -1 : Pivot->Left->Height;
		int HR = (Pivot->Right == nullptr) ? -1 : Pivot->Right->Height;
		assert(abs(HL - HR) > 1);

		// Pivots left subtree height should be greater than right subtree
		// height to be leaning left.
		if (HR < HL) { // Leaning left.
			
			int hL = (Pivot->Left->Left == nullptr) ? -1 : Pivot->Left->Left->Height;
			int hR = (Pivot->Left->Right == nullptr) ? -1 : Pivot->Left->Right->Height;
			// Check if conditions for right rotate are met.
			if (hL > hR) {
				_RightRotate(Parent, Pivot);
			}
			else {
				_LeftRotate(Pivot, Pivot->Left);
				_RightRotate(Parent, Pivot);
			}

		}
		else { // Leaning right.
		
			int hL = (Pivot->Right->Left == nullptr) ? -1 : Pivot->Right->Left->Height;
			int hR = (Pivot->Right->Right == nullptr) ? -1 : Pivot->Right->Right->Height;
			// Check if conditions for left rotate are met.
			if (hL < hR) {
				_LeftRotate(Parent, Pivot);
			}
			else {
				_RightRotate(Pivot, Pivot->Right);
				_LeftRotate(Parent, Pivot);	
			}

		}

	}


	// For test.cpp file to test cases
	void _inorder_keys(NODE* cur, std::vector<TKey>& V) {
		
		if (cur == nullptr) {
			return;
		}
		else {
			_inorder_keys(cur->Left, V);
			V.push_back(cur->Key);
			_inorder_keys(cur->Right, V);
		}
	
	}


	// For test.cpp file to test cases
	void _inorder_values(NODE* cur, std::vector<TValue>& V) {
		
		if (cur == nullptr) {
			return;
		}
		else {
			_inorder_values(cur->Left, V);
			V.push_back(cur->Value);
			_inorder_values(cur->Right, V);
		}
	
	}

	
	// For test.cpp file to test cases
	void _inorder_heights(NODE* cur, std::vector<int>& V) {

		if (cur == nullptr) {
			return;
		}
		else {
			_inorder_heights(cur->Left, V);
			V.push_back(cur->Height);
			_inorder_heights(cur->Right, V);
		}

	}


public:

	// Initializes class variables.
	avltree() {

		Root = nullptr;
		Size = 0;

	}


	// Initializes class variables, then calls helper function
	// to build a copy tree.
	avltree(const avltree& other) {

		// Initialize class variables.
		Root = nullptr;
		Size = 0;
		if (other.Root == nullptr) {
			return;
		}
		else {
			_copybinarysearchtree(other.Root);
		}

	}


	// Calls helper function to delete entire tree using
	// postorder. Then resets Root to nullptr.
	virtual ~avltree() {

		_deletebinarysearchtree(Root);
		Root = nullptr;

	}


	// Allows to assign tree into another variable.
	avltree& operator=(const avltree& other) {

		clear();
		_copybinarysearchtree(other.Root);
		return *this;

	}


	// Getter function to return Size class variable.
	int size() {	return Size;	}


	// Calls helper function _height and passes the root of tree.
	int height() {	

		if (Root == nullptr) {
			return -1;
		}
		else {
			return Root->Height;	
		}

	}


	// Calls helper function to delete the entire tree.
	// Then resets Root pointer and Size.
	void clear() {

		_deletebinarysearchtree(Root);
		Root = nullptr;
		Size = 0;

	}


	// Traverses tree until key is found, then returns.
	// If not found, return nullptr.
	TValue* search(TKey key) {

		NODE* temp = Root; // Temp node used to traverse.
		while (temp != nullptr) { // Loop till end of branch.
			if (temp->Key == key) {
				return &(temp->Value); // Return pointer to value to be read or written.
			}
			else if (key < temp->Key) { // Go left if key is less.
				temp = temp->Left;
			}
			else if (key > temp->Key) { // Go rigtht if key is more.
				temp = temp->Right;
			}
		}
		return nullptr; // Not found.

	}


	// First, checks if key already exists. If not, allocates
	// new node with new key and value. Then, inserts the 
	// node in correct subtree. Then goes though stack to update
	// each nodes height. If avl height condition broken, then
	// call rotateToFix.
	void insert(TKey key, TValue value) {

		// Keep track of traversing cur node and node prior.
		NODE* prev = nullptr;
		NODE* cur = Root;

		stack<NODE*> nodes;

		// Search to see if tree already contains key.
		while (cur != nullptr) {
		
			if (key == cur->Key) {	
				return;
			}

			nodes.push(cur);
			
			if (key < cur->Key) {
        			prev = cur;
        			cur = cur->Left;
      			}
			else {
        			prev = cur;
        			cur = cur->Right;
      			}

		}

		// Create new node to be inserted.
		NODE* newNode = new NODE();
		newNode->Key = key;
		newNode->Value = value;
		newNode->Height = 0;
		newNode->Left = nullptr;
		newNode->Right = nullptr;

		// Find which branch of previous node to be inserted.
		if (prev == nullptr) {
			Root = newNode;
		}
		else {

			if (key < prev->Key) {
				prev->Left = newNode;
			}
			else {
				prev->Right = newNode;
			}

		}

		Size++; // New node added.

		// Walk back up tree using stack and update heights.
		while (!nodes.empty()) {

			cur = nodes.top();
			nodes.pop();
			int HL, HR, newH;

			if (cur->Left == nullptr) {
				HL = -1;
			}
			else {
				HL = cur->Left->Height;
			}
			if (cur->Right == nullptr) {
				HR = -1;
			}
			else {
				HR = cur->Right->Height;
			}

			newH = 1 + max(HL, HR);
			cur->Height = newH;

			if (abs(HL - HR) > 1) { // Node broken.

				NODE* Parent = nullptr;
				// Parent is top of stack unless stack is empty.
				if (nodes.empty()) { 
					Parent = nullptr;
				}
				else {
					Parent = nodes.top();
				}
				_rotateToFix(Parent, cur); // Call helper to rotate.

			}

		}

	}


	// Traverses through tree until split node is found 
	// where both keys split direction. Then, two seperate
	// while loops to loop in each direction to increment
	// distance until each key is found.
	int distance(TKey k1, TKey k2) {

		if (search(k1) == nullptr || search(k2) == nullptr) { // Distance is -1 one does not exist.
			return -1;
		}
		else if (k1 == k2) { // Distance is 0 if equal.
			return 0;
		}

		int distance = 0;
		NODE* cur = Root;
		NODE* tempRoot = nullptr;

		// Find node where values split direction.
		while (cur != nullptr) { // Loop till end of branch.
		

			if (k1 == Root->Key || k2 == Root->Key) {
				tempRoot = Root;
				break;
			}
			else if (k1 < cur->Key && k2 < cur->Key) { // Go left if key is less.
				cur = cur->Left;
			}
			else if (k1 > cur->Key && k2 > cur->Key) { // Go rigtht if key is more.
				cur = cur->Right;
			}
			else { // k1 and k2 split direction.
				tempRoot = cur; // Hold the node then leave loop.
				break;
			}
		
		}
		//	Loop into key 1 direction and increment distance until found.
		while (cur != nullptr) {
			
			if (k1 == cur->Key) {
				break;
			}
			else if (k1 < cur->Key) {
				cur = cur->Left;
			}
			else if (k1 > cur->Key) {
				cur = cur->Right;
			}			
			distance++;

		}

		cur = tempRoot; // Reset to split node.
		//	Loop into key 2 direction and increment distance until found.
		while (cur != nullptr) {

			if (k2 == cur->Key) {
				break;
			}
			else if (k2 < cur->Key) {
				cur = cur->Left;
			}
			else if (k2 > cur->Key) {
				cur = cur->Right;
			}
			distance++;

		}

		return distance;

	}


	// Calls helper function to print the tree inorder.
	void inorder() {

		cout << "Inorder:" << endl;
		_inorder(Root);
		cout << endl;

	}


	// For test.cpp file to test cases
	vector<TKey> inorder_keys() {

		std::vector<TKey>  V;
		_inorder_keys(Root, V);
		return V;

	}


	// For test.cpp file to test cases
	vector<TValue> inorder_values() {

		std::vector<TValue>  V;
		_inorder_values(Root, V);
		return V;

	}


	// For test.cpp file to test cases
	vector<int> inorder_heights() {

		std::vector<int>  V;
		_inorder_heights(Root, V);
		return V;

	}

};