#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include "avl.h"
#include "util.h"
using namespace std;


// given a string, breaks the string into individual tokens (words) based
// on spaces between the tokens.  Returns the tokens back in a vector.
// Example: "select * from students" would be tokenized into a vector
// containing 4 strings:  "select", "*", "from", "students".
vector<string> tokenize(string line) {

	vector<string> tokens;
	stringstream  stream(line);
	string token;

	while (getline(stream, token, ' ')) {
		tokens.push_back(token);
	}
	return tokens;

}


// given the meta filename, return a vector with all data in file
vector<string> createMetaVector(string tablename) {

	vector<string> metaVector;
	// open the file, make sure it opened, seekg to the given position,
	// loop and input values using >>, store into vector, return vector
	string filename = tablename + ".meta";
	ifstream data(filename, ios::in | ios::binary);

	if (!data.good()) {
		cout << "**Error: couldn't open data file '" << filename << "'." << endl;
		return metaVector;
	}

	// input all data from .meta to value
	// then, store .meta data into vector
	string value;
	data >> value;
	while (!data.eof()) {
		metaVector.push_back(value);
		data >> value;
	}
	return metaVector;

}


// populate tree with data with index column as key and line position of record as value
void populateAvlTree(string tablename, int recordSize, int numColumns, int indexVal, avltree<string, streamoff> &tree) {

	string   filename = tablename + ".data";
	ifstream data(filename, ios::in | ios::binary);
	if (!data.good()) {
		cout << "**Error: couldn't open data file '" << filename << "'." << endl;
		return;
	}

	// okay, read file record by record, and insert each value into tree:
	data.seekg(0, data.end);  // move to the end to get length of file:
	streamoff length = data.tellg();

	streamoff pos = 0;  // first record at offset 0:
	string    value;

	while (pos < length) {
		data.seekg(pos, data.beg);  // move to start of record:
		for (int i = 0; i < numColumns; ++i) { // read values, one per column
			data >> value;
			if (i == indexVal) { // insert into tree appropriate value and key pair
				tree.insert(value, pos);
			}
		}
		pos += recordSize;  // move offset to start of next record:
	}

}


// checks if there are errors in input and outputs appropriate message
// returns if input is valid or not
bool errorCheck(string tablename, vector<string> tokens, vector<string> columnVector) {

	// error checking...
	bool valid = true;
	if (tokens[0] != "select" && tokens[0] != "exit") {
		valid = false;
		cout << "Unknown query, ignored..." 
		<< endl;
	}
	else if (tokens[0] == "select" && tokens[1] != "*") {
		valid = false;
		for (unsigned i = 0; i < columnVector.size(); i++) {
			if (tokens[1] == columnVector[i]) {
				valid = true;
				break;
			}
		}
		if (valid == false) {
			cout << "Invalid select column, ignored..."
			<< endl;
		}
	}
	else if (tokens[2] != "from") {
		valid = false;
		cout << "Invalid select query, ignored..."
		<< endl;
	}
	else if (tokens[3] != tablename) {
		valid = false;
		cout << "Invalid table name, ignored..."
		<< endl;
	}
	else if (tokens[4] != "where") {
		valid = false;
		cout << "Invalid select query, ignored..."
		<< endl;
	}
	else if (tokens[5] != "") {
		valid = false;
		for (unsigned i = 0; i < columnVector.size(); i++) {
			if (tokens[5] == columnVector[i]) {
				valid = true;
				break;
			}
		}
		if (valid == false) {
			cout << "Invalid where column, ignored..."
			<< endl;
		}
	}
	else if (tokens[6] != "=") {
		valid = false;
		cout << "Invalid select query, ignored..."
		<< endl;
	}
	else if (tokens.size() != 8) {
		valid = false;
		cout << "Invalid select query, ignored..."
		<< endl;
	}
	return valid;

}


// print avl information
void printAvlInfo(vector<string> indexVector, vector<avltree<string, streamoff>> avlVector) {

	for (unsigned i = 0; i < indexVector.size(); i++ ) {
		cout << "Index column: "
		<< indexVector[i]
		<< endl;
		cout << "  Tree size: "
		<< avlVector[i].size()
		<< endl;
		cout << "  Tree height: "
		<< avlVector[i].height()
		<< endl;
	}

}


// go through both vectors, avl and linear until whereColumn and whereData is found, output not found otherwise
// then print all inforation related to whereColumn and whereData
void selectAll(string tablename, int offset, int numColumns, vector<string> tokens, vector<string> columnVector, 
			vector<string> indexVector, vector<avltree<string, streamoff>> avlVector, vector<string> linearVector) {

	bool found = false;
	string whereColumn = tokens[5];
	string whereData = tokens[7];

	// check if column is indexed
	for (unsigned i = 0; i < indexVector.size(); i++) {

		if (whereColumn == indexVector[i]) { // indexed column

			streamoff *dataPos = avlVector[i].search(whereData);
			if (dataPos == nullptr) { // whereData does not exist
				break;
			}
			found = true;
			vector<string> dataRecord;
			dataRecord = GetRecord(tablename, *dataPos, numColumns);
			
			// print record data
			for (unsigned j = 0; j < columnVector.size(); j++) {
				cout << columnVector[j]
				<< ": "
				<< dataRecord[j]
				<< endl;
			}

		}

	}

	if (found == false) {

		// check if column is not-indexed
		for (unsigned i = 0; i < linearVector.size(); i++) {

			if (whereColumn == linearVector[i]) {

				int matchColumn = 0;
				// find the matchColumn integer for LinearSearch function
				for (unsigned j = 0; j < columnVector.size(); j++) {
					if (whereColumn == columnVector[j]) {
						matchColumn = j + 1;
						break;
					}
				}
				vector<streamoff> dataPosVector = LinearSearch(tablename, offset, numColumns, whereData, matchColumn);
				if (dataPosVector.empty()) { // whereData does not exist
					break;
				}
				found = true;
				vector<string> dataRecord;

				// loop through all position of records given by linear search
				// then, create a record vector for each line found
				// finally, print the all data for each record line
				for (unsigned k = 0; k < dataPosVector.size(); k++) {

					dataRecord = GetRecord(tablename, dataPosVector[k], numColumns);
					for (unsigned j = 0; j < columnVector.size(); j++) {
						cout << columnVector[j]
						<< ": "
						<< dataRecord[j]
						<< endl;
					}

				}

			}

		}

	}
	if (found == false) { // not found in both vectors
		cout << "Not found..."
		<< endl;
	}

}


// go through both vectors, avl and linear until whereColumn and whereData is found, output not found otherwise
// then print specified selectColumn information related to whereColumn and whereData
void selectSpecific(string tablename, int offset, int numColumns, vector<string> tokens, vector<string> columnVector, 
			vector<string> indexVector, vector<avltree<string, streamoff>> avlVector, vector<string> linearVector) {

	bool found = false;
	string selectColumn = tokens[1];
	string whereColumn = tokens[5];
	string whereData = tokens[7];

	// check if column is indexed
	for (unsigned i = 0; i < indexVector.size(); i++) {

		if (whereColumn == indexVector[i]) { // indexed column

			streamoff *dataPos = avlVector[i].search(whereData);
			if (dataPos == nullptr) { // whereData does not exist
				break;
			}
			found = true;
			vector<string> dataRecord;
			dataRecord = GetRecord(tablename, *dataPos, numColumns);
			
			// print selected column data
			for (unsigned j = 0; j < columnVector.size(); j++) {
				if (selectColumn == columnVector[j]) {
					cout << columnVector[j]
					<< ": "
					<< dataRecord[j]
					<< endl;
					break;
				}
			}

		}

	}
	if (found == false) {

		// check if column is not-indexed
		for (unsigned i = 0; i < linearVector.size(); i++) {

			if (whereColumn == linearVector[i]) {

				int matchColumn = 0;
				// find the matchColumn integer for LinearSearch function
				for (unsigned j = 0; j < columnVector.size(); j++) {
					if (whereColumn == columnVector[j]) {
						matchColumn = j + 1;
						break;
					}
				}
				vector<streamoff> dataPosVector = LinearSearch(tablename, offset, numColumns, whereData, matchColumn);
				if (dataPosVector.empty()) { // whereData does not exist
					break;
				}
				found = true;
				vector<string> dataRecord;

				// loop through all position of records given by linear search
				// then, create a record vector for each line found
				// finally, print the all data for each record line
				for (unsigned k = 0; k < dataPosVector.size(); k++) {

					dataRecord = GetRecord(tablename, dataPosVector[k], numColumns);
					for (unsigned j = 0; j < columnVector.size(); j++) {
						if (selectColumn == columnVector[j]) {
							cout << columnVector[j]
							<< ": "
							<< dataRecord[j]
							<< endl;
							break;
						}
					}

				}						

			}

		}

	}
	if (found == false) { // not found in both vectors
		cout << "Not found..."
		<< endl;
	}

}


int main() {

	string tablename; // = "students";
	cout << "Welcome to myDB, please enter tablename> ";
	getline(cin, tablename);
	cout << "Reading meta-data..." << endl;

	// create vector with .meta data
	vector<string> metaVector = createMetaVector(tablename);
	int offset = stoi(metaVector[0]); // hold offset from .meta
	int numColumns = stoi(metaVector[1]); // hold number of columns from .meta
	cout << "Building index tree(s)..." << endl;

	// declare vectors for organization of necessary information..
	vector<string> columnVector; // holds all columns
	vector<string> linearVector; // holds non-indexed columns
	vector<string> indexVector; // holds indexed columns
	vector<avltree<string, streamoff>> avlVector; // holds avl trees for indexed columns

	int indexVal = 0; // which column is index
	// loop though meta data and if column is followed by 1, then it is indexed
	// if, column is followed by 0, then it is not-indexed
	for (unsigned i = 2; i < metaVector.size(); i = i + 2) {

		string key = metaVector[i]; // hold column
		string isIndex = metaVector[i+1]; // hold 0 or 1
		columnVector.push_back(key); // create column only vector

		// if indexed, then create new tree and populate
		// then, append to vectors to map column vector to corresponding tree in avl vector
		if ( isIndex == "1" ) {
			avltree<string, streamoff> newAvlTree;
			populateAvlTree(tablename, offset, numColumns, indexVal, newAvlTree);
			indexVector.push_back(key);
			avlVector.push_back(newAvlTree);
		}
		else if (isIndex == "0") { // if not-indexed, append column to linearsearch vector
			linearVector.push_back(key);
		}
		indexVal++;

	}

	printAvlInfo(indexVector, avlVector); // print avl tree info
	
	// main loop to input and execute queries from the user:
	string query;
	cout << endl;
	cout << "Enter query> ";
	getline(cin, query);
	while (query != "exit") {

		vector<string> tokens = tokenize(query); // hold user input
		
		// error checking...
		bool valid = errorCheck(tablename, tokens, columnVector);
		
		// no error found
		if (valid == true) {

			if ( tokens[1] == "*" ) { // select item is *, so select all
				selectAll(tablename, offset, numColumns, tokens, columnVector, indexVector, avlVector, linearVector);
			}
			else { // select item is a specific column
				selectSpecific(tablename, offset, numColumns, tokens, columnVector, indexVector, avlVector, linearVector);
			}

		}

		cout << endl;
		cout << "Enter query> ";
		getline(cin, query);

	}
	return 0;

}