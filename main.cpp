#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <ctime>
#include <limits>

using namespace std;

// Helper function to convert a string to lowercase
string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Trie Node for Book Search
struct TrieNode {
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord = false;
};

class Trie {
public:
    TrieNode* root;

    Trie() { root = new TrieNode(); }

    // Insert a book title into the Trie
    void insert(const string& book) {
        TrieNode* node = root;
        string lowerBook = toLowerCase(book); // Ensure case insensitivity
        for (char c : lowerBook) {
            if (!node->children[c])
                node->children[c] = new TrieNode();
            node = node->children[c];
        }
        node->isEndOfWord = true;
    }

    // Search for a prefix in the Trie
    vector<string> search(const string& prefix) {
        vector<string> results;
        TrieNode* node = root;
        string lowerPrefix = toLowerCase(prefix);
        for (char c : lowerPrefix) {
            if (!node->children[c]) return results;
            node = node->children[c];
        }
        dfs(node, lowerPrefix, results);
        return results;
    }

    ~Trie() { clear(root); }

private:
    void dfs(TrieNode* node, string current, vector<string>& results) {
        if (node->isEndOfWord) results.push_back(current);
        for (auto& [c, child] : node->children)
            dfs(child, current + c, results);
    }

    void clear(TrieNode* node) {
        for (auto& [c, child] : node->children)
            clear(child);
        delete node;
    }
};

// User Structure
struct User {
    string name;
    vector<string> issuedBooks;
};

// Library Management System
class Library {
    Trie bookTrie;
    unordered_map<string, int> availableBooks;  // Book title -> count
    unordered_map<string, int> totalBooks;     // Book title -> total copies count
    unordered_map<string, User> users;         // User name -> User data
    priority_queue<pair<time_t, string>, vector<pair<time_t, string>>, greater<>> overdueQueue; // Overdue books
    unordered_map<string, string> overdueUserMap; // Book title -> User name

public:
    // Add a book
    void addBook(const string& title, int count) {
        bookTrie.insert(title);
        availableBooks[title] += count;  // Increment available count
        totalBooks[title] += count;      // Maintain total count
        cout << "Book added: " << title << " (Count: " << count << ")" << endl;
    }

    // Search for books
    void searchBooks(const string& prefix) {
        vector<string> results = bookTrie.search(prefix);
        if (results.empty()) {
            cout << "No books found with prefix \"" << prefix << "\"." << endl;
        } else {
            cout << "Books found: ";
            for (const string& book : results) 
                cout << book << " (Available: " << availableBooks[book] << "), ";
            cout << endl;
        }
    }

    // Add a user
    void addUser(const string& name) {
        if (users.find(name) == users.end()) {
            users[name] = {name, {}};
            cout << "User added: " << name << endl;
        } else {
            cout << "User already exists." << endl;
        }
    }

    // Issue a book
    void issueBook(const string& userName, const string& bookTitle) {
        if (users.find(userName) == users.end()) {
            cout << "User does not exist." << endl;
            return;
        }
        if (availableBooks[bookTitle] > 0) {
            users[userName].issuedBooks.push_back(bookTitle);
            availableBooks[bookTitle]--;  // Decrement available count
            overdueQueue.push({time(0) + 7 * 24 * 60 * 60, bookTitle});  // 7 days from now
            overdueUserMap[bookTitle] = userName;
            cout << "Book issued to " << userName << ": " << bookTitle << endl;
        } else {
            cout << "Book not available: " << bookTitle << endl;
        }
    }

    // Return a book
    void returnBook(const string& userName, const string& bookTitle) {
        if (users.find(userName) == users.end()) {
            cout << "User does not exist." << endl;
            return;
        }
        auto& books = users[userName].issuedBooks;
        auto it = find(books.begin(), books.end(), bookTitle);
        if (it != books.end()) {
            books.erase(it);
            availableBooks[bookTitle]++;  // Increment available count
            overdueUserMap.erase(bookTitle); // Remove from overdue mapping
            cout << "Book returned: " << bookTitle << endl;
        } else {
            cout << "Book not issued to user: " << bookTitle << endl;
        }
    }

    // Display overdue books
    void displayOverdueBooks() {
        time_t now = time(0);
        if (overdueQueue.empty()) {
            cout << "No overdue books." << endl;
            return;
        }
        cout << "Overdue Books: " << endl;
        while (!overdueQueue.empty() && overdueQueue.top().first <= now) {
            string bookTitle = overdueQueue.top().second;
            cout << bookTitle << " (User: " << overdueUserMap[bookTitle] << ")" << endl;
            overdueQueue.pop();
        }
    }

    // Display books issued to a user
    void displayUserBooks(const string& userName) {
        if (users.find(userName) == users.end()) {
            cout << "User does not exist." << endl;
            return;
        }
        cout << "Books issued to " << userName << ": ";
        for (const string& book : users[userName].issuedBooks)
            cout << book << ", ";
        cout << endl;
    }
};

// Main Function
int main() {
    Library library;
    int choice;

    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Add User\n";
        cout << "3. Search Books\n";
        cout << "4. Issue Book\n";
        cout << "5. Return Book\n";
        cout << "6. Display Overdue Books\n";
        cout << "7. Display User's Issued Books\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Handle newline

        switch (choice) {
            case 1: {
                string bookTitle;
                int count;
                cout << "Enter book title: ";
                getline(cin, bookTitle);
                cout << "Enter number of copies: ";
                cin >> count;
                library.addBook(bookTitle, count);
                break;
            }
            case 2: {
                string userName;
                cout << "Enter user name: ";
                getline(cin, userName);
                library.addUser(userName);
                break;
            }
            case 3: {
                string prefix;
                cout << "Enter prefix to search for books: ";
                getline(cin, prefix);
                library.searchBooks(prefix);
                break;
            }
            case 4: {
                string userName, bookTitle;
                cout << "Enter user name: ";
                getline(cin, userName);
                cout << "Enter book title: ";
                getline(cin, bookTitle);
                library.issueBook(userName, bookTitle);
                break;
            }
            case 5: {
                string userName, bookTitle;
                cout << "Enter user name: ";
                getline(cin, userName);
                cout << "Enter book title: ";
                getline(cin, bookTitle);
                library.returnBook(userName, bookTitle);
                break;
            }
            case 6: {
                library.displayOverdueBooks();
                break;
            }
            case 7: {
                string userName;
                cout << "Enter user name: ";
                getline(cin, userName);
                library.displayUserBooks(userName);
                break;
            }
            case 8: {
                cout << "Exiting Library Management System. Goodbye!\n";
                break;
            }
            default: {
                cout << "Invalid choice. Please try again.\n";
                break;
            }
        }
    } while (choice != 8);

    return 0;
}
