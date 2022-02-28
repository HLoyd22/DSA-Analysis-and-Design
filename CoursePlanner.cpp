//============================================================================
// Name        : CoursePlanner.cpp
// Author      : Hillary Loyd
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> 

#include "CSVparser.hpp"

using namespace std;

const unsigned int DEFAULT_SIZE = 10;

struct Course {
    string courseNumber; 
    string courseName;
    string preReqs;
};

class HashTable {

private:
    struct Node {
        Course course;
        unsigned int key;
        Node *next;

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        Node(Course acourse) : Node() {
            course = acourse;
        }

        Node(Course acourse, unsigned int aKey) : Node(acourse) {
            key = aKey;
        }
    };

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(string key); //had to add this because the key was not working properly allows for the key to be a string 

public:
    HashTable();
    HashTable(unsigned int size);
    void Insert(Course course);
    void PrintAll();
    Course Search(string courseNumber);
};

HashTable::HashTable() {
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    nodes.resize(tableSize);
}

unsigned int HashTable::hash(string courseNumber) {

    return atoi(courseNumber.substr(4).c_str()) % tableSize; 
    //used this instead of the one that we were given. This is because when I used the other one the values for the key were at 0. This still a problem as only two of the course only cycle through.

}

void HashTable::Insert(Course course) {
    unsigned key = hash(course.courseNumber.c_str()); //set key by hashing the course's courseNumber.

    cout << course.courseNumber.c_str() << endl; 
    //I added this to show that the information was actually pulling from the file and that all courses are present however were having the problem with duplicate keys there for not showing.

    Node* oldNode = &(nodes.at(key));//Set the oldNode to the nodes at the key.

    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));
    }

    else if (oldNode->key == UINT_MAX) { //if the oldNode key is equal to the UINT_MAX
        oldNode->key = key; //update the key
        oldNode->course = course; //update the course
        oldNode->next = nullptr; //update the next
    }
    else {
        while (oldNode->next != nullptr) {
            oldNode = oldNode->next; 
        }
    }
}

void HashTable::PrintAll() {

    cout << "Here is your schedule: " << endl;

    for (auto nodeSearch = nodes.begin(); nodeSearch != nodes.end(); ++nodeSearch) {

        if (nodeSearch->key != UINT_MAX) { //if the key is NOT EQUAL to UINT_MAX.
            cout << nodeSearch->course.courseNumber << ", " << nodeSearch->course.courseName << endl; //Print courseNumber and courseName.

            Node* node = nodeSearch->next; //node to search->next

            while (node != nullptr) {
                cout << nodeSearch->course.courseNumber << ", " << nodeSearch->course.courseName << endl; //Print courseNumber and courseName.

                node = node->next; //node to node->next
            }
        }
    }
}



Course HashTable::Search(string courseNumber) {
    Course course;

    unsigned key = hash(courseNumber); //set the key by hashing the courseNumber.

    Node* node = &(nodes.at(key)); //node is set to the nodes at the key.
     // if node is EQUAL to nullptr and the node key is EQUAL to the UINT_MAX returns course
    if (node == nullptr || node->key == UINT_MAX) {
        return course;
    }
    // if no entry found for the key sets the course to  node's course and then returns the course
    if (node != nullptr && node->key != UINT_MAX && node->course.courseNumber.compare(courseNumber) == 0) {
        course = node->course;
        return course;
    }

    while(node != nullptr) { //if the node is NOT EQUAL to nullptr
        if (node->key != UINT_MAX && node->course.courseNumber.compare(courseNumber) == 0) {
            return node->course; //returns the node's course
        }

        node = node->next; //set node to node's next
    }

    return course;
}

//============================================================================
// Static methods used for testing
//============================================================================
//print's the addtional information for individual courses needed for the menu and based off users input
void displayCourse(Course course) {
        cout << course.courseNumber << ", " << course.courseName << endl;
        if (course.preReqs.size() > 0) {
            cout << "Prerequsits: " << course.preReqs << endl;
        }
        else {
            cout << "Prerequsits: None needed for this course." << endl;
        }
        return;
}

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the course read
 */
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of courses
            Course course;
            course.courseName = file[i][1];
            course.courseNumber = file[i][0];
            course.preReqs = file[i][2];


             
            // push this bid to the end
            hashTable->Insert(course);
        }
    }
    catch (csv::Error& e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}


/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, courseKey;
    switch (argc) {

    default:
        csvPath = "eCourse_Planner.csv";
        courseKey = "CSCI100";
    }


    // Define a hash table to hold all the bids
    HashTable* courseTable{};

    Course course;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Data Structure" << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            courseTable = new HashTable(); //creates a new Hastable

            // Complete the method call to load the bids
            loadCourses(csvPath, courseTable); // loads the courses from the CSV path using the function that we created earlier.
            break;

        case 2:
            courseTable->PrintAll(); //uses the print all function to print the courses from the CSV. In our case it only prints out the first two in the CSV.
            break;

        case 3:

            cout << "What course do you want to know about? " << endl; //asks the user what course they would like to search for

            cin >> courseKey; //user's input for the course they would like to search for and becomes the course key

            course = courseTable->Search(courseKey); //sets the course to the search for the course key

            // if the courseNumber is not empty then uses the displayCOurse function to display the courseNumber, courseName and then the Prerequisits needed for the class.
            if (!course.courseNumber.empty()) {
                displayCourse(course);
            }

            break;

        case 9:
            //ends the program
            cout << "Thank you for using the course planner!" << endl;
            break;
        }
    }
    return 0;
}
