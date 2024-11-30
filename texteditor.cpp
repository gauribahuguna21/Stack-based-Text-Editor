#include <fstream>
#include <stack>
#include <iostream>
using namespace std;

class Node {
public:
    string data;
    Node* next;
    bool modified; // To track changes for highlighting
    Node(string data) {
        this->data = data;
        this->next = NULL;
        this->modified = false;
    }
};

// Utility function to display the text
void display(Node* root) {
    cout << "Current text:" << endl;
    int i = 0;
    while (root != NULL) {
        cout << ++i << "> " << root->data << endl;
        root = root->next;
    }
    cout << endl;
}

class Editor {
public:
    // Add a new line
    void add_line(Node*& root) {
        string line;
        cout << "Enter the line to insert:" << endl;
        getline(cin, line);

        Node* temp = new Node(line);
        temp->modified = true; // Mark as modified
        if (root == NULL) {
            root = temp;
            return;
        }

        Node* root_ref = root;
        while (root_ref->next != NULL)
            root_ref = root_ref->next;
        root_ref->next = temp;
    }

    // Insert a line at a specific index
    void add_line_at(Node*& root, int index) {
        string line;
        cout << "Enter the line to insert:" << endl;
        getline(cin, line);

        Node* temp = new Node(line);
        temp->modified = true;

        if (index == 1) {
            temp->next = root;
            root = temp;
            return;
        }

        Node* root_ref = root;
        for (int i = 1; i < index - 1 && root_ref != NULL; i++) {
            root_ref = root_ref->next;
        }

        if (root_ref == NULL) {
            cout << "Invalid index!" << endl;
            delete temp;
            return;
        }

        temp->next = root_ref->next;
        root_ref->next = temp;
    }

    // Replace a specific line
    void replace_line(Node*& root, int line_no) {
        string line;
        cout << "Enter the new line:" << endl;
        getline(cin, line);

        Node* root_ref = root;
        for (int i = 1; i < line_no && root_ref != NULL; i++) {
            root_ref = root_ref->next;
        }

        if (root_ref == NULL) {
            cout << "Invalid index!" << endl;
            return;
        }

        root_ref->data = line;
        root_ref->modified = true; // Mark as modified
    }

    // Interchange two lines
    void interchange_lines(Node*& root, int index1, int index2) {
        if (index1 == index2) return;

        Node *node1 = NULL, *node2 = NULL, *root_ref = root;
        for (int i = 1; root_ref != NULL; i++) {
            if (i == index1) node1 = root_ref;
            if (i == index2) node2 = root_ref;
            root_ref = root_ref->next;
        }

        if (node1 == NULL || node2 == NULL) {
            cout << "Invalid indexes!" << endl;
            return;
        }

        swap(node1->data, node2->data);
        node1->modified = node2->modified = true;
    }

    // Delete a specific line
    void delete_line(Node*& root, int index) {
        if (root == NULL || index <= 0) {
            cout << "Invalid index!" << endl;
            return;
        }

        if (index == 1) {
            Node* temp = root;
            root = root->next;
            delete temp;
            return;
        }

        Node* root_ref = root;
        for (int i = 1; i < index - 1 && root_ref != NULL; i++) {
            root_ref = root_ref->next;
        }

        if (root_ref == NULL || root_ref->next == NULL) {
            cout << "Invalid index!" << endl;
            return;
        }

        Node* temp = root_ref->next;
        root_ref->next = temp->next;
        delete temp;
    }

    // Search and replace functionality
    void search_and_replace(Node*& root, const string& search, const string& replace) {
        Node* root_ref = root;
        bool found = false;

        while (root_ref != NULL) {
            if (root_ref->data.find(search) != string::npos) {
                size_t pos = 0;
                while ((pos = root_ref->data.find(search, pos)) != string::npos) {
                    root_ref->data.replace(pos, search.length(), replace);
                    pos += replace.length();
                }
                root_ref->modified = true; // Mark as modified
                found = true;
            }
            root_ref = root_ref->next;
        }

        if (found) {
            cout << "All occurrences of \"" << search << "\" replaced with \"" << replace << "\"." << endl;
        } else {
            cout << "No occurrences of \"" << search << "\" found." << endl;
        }
    }

    // Save the file
    void save_file(Node* root, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open file '" << filename << "' for writing!" << endl;
            return;
        }

        while (root != NULL) {
            file << root->data << endl;
            root = root->next;
        }
        file.close();
        cout << "File saved successfully as '" << filename << "'!" << endl;
    }

    // Undo functionality
    void push_root(Node* root, stack<Node*>& st) {
        Node* head = NULL;
        Node* root_ref = root;

        while (root_ref != NULL) {
            Node* temp = new Node(root_ref->data);
            if (head == NULL) {
                head = temp;
            } else {
                Node* head_ref = head;
                while (head_ref->next != NULL)
                    head_ref = head_ref->next;
                head_ref->next = temp;
            }
            root_ref = root_ref->next;
        }
        st.push(head);
    }

    void undo(Node*& root, stack<Node*>& undoStack) {
        if (undoStack.size() > 1) {
            undoStack.pop(); // Remove current state
            root = undoStack.top(); // Revert to previous state
        } else {
            cout << "UNDO unavailable..." << endl;
        }
    }

    // Display with highlighting modified lines
    void display_with_highlight(Node* root) {
        cout << "Current text (Modified lines marked with *):" << endl;
        int i = 0;
        while (root != NULL) {
            cout << ++i << "> " << (root->modified ? "* " : "") << root->data << endl;
            root = root->next;
        }
        cout << endl;
    }

    void begin(Node*& root) {
    stack<Node*> undoStack;
    push_root(root, undoStack);

    int select;
    string filename, search, replace;
    while (true) {
        cout << "Enter your Selection:\n"
             << "1. Insert line\n"
             << "2. Insert line at index\n"
             << "3. Replace a line\n"
             << "4. Interchange two lines\n"
             << "5. Delete line\n"
             << "6. Search and Replace word\n"
             << "7. Display text with highlights\n"
             << "8. Save File\n"
             << "9. Undo changes\n"
             << "-1. Exit\n>> ";
        cin >> select;
        cin.ignore();

        switch (select) {
            case 1:
                add_line(root);
                push_root(root, undoStack);
                break;
            case 2: {
                int index;
                cout << "Enter index: ";
                cin >> index;
                cin.ignore();
                add_line_at(root, index);
                push_root(root, undoStack);
                break;
            }
            case 3: {
                int index;
                cout << "Enter line number to replace: ";
                cin >> index;
                cin.ignore();
                replace_line(root, index);
                push_root(root, undoStack);
                break;
            }
            case 4: {
                int index1, index2;
                cout << "Enter the first line number: ";
                cin >> index1;
                cout << "Enter the second line number: ";
                cin >> index2;
                cin.ignore();
                interchange_lines(root, index1, index2);
                push_root(root, undoStack);
                break;
            }
            case 5: {
                int index;
                cout << "Enter line number to delete: ";
                cin >> index;
                cin.ignore();
                delete_line(root, index);
                push_root(root, undoStack);
                break;
            }
            case 6:
                cout << "Enter the word to search: ";
                getline(cin, search);
                cout << "Enter the word to replace with: ";
                getline(cin, replace);
                search_and_replace(root, search, replace);
                push_root(root, undoStack);
                break;
            case 7:
                display_with_highlight(root);
                break;
            case 8:
                cout << "Enter filename to save: ";
                getline(cin, filename);
                save_file(root, filename);
                break;
            case 9: // Undo functionality
                undo(root, undoStack);
                break;
            case -1:
                cout << "Editor exited..." << endl;
                return;
            default:
                cout << "Invalid option. Try again!" << endl;
        }
    }
}

    // Function to load a file into the linked list
void load_from_file(Node*& root, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        Node* temp = new Node(line);
        if (root == NULL) {
            root = temp;
        } else {
            Node* root_ref = root;
            while (root_ref->next != NULL)
                root_ref = root_ref->next;
            root_ref->next = temp;
        }
    }
    file.close();
    cout << "File loaded successfully!" << endl;
}

};

int main() {
    Node* root = NULL;
    Editor editor;

    // Load text from a file
    string filename;
    cout << "Enter the filename to load (or leave blank to start fresh): ";
    getline(cin, filename);
    if (!filename.empty()) {
        editor.load_from_file(root, filename);
    }

    editor.begin(root);
    return 0;
}


