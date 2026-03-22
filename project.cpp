#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <windows.h>
#include <queue>
#include <map>
#include <set>

// getch() return values
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_SPACE 32

//windows colour code
const int BLACK = 0;
const int BLUE = 1;
const int GREEN = 2;
const int CYAN = 3;
const int RED = 4;
const int MAGENTA = 5;
const int BROWN = 6;
const int LIGHTGRAY = 7;
const int DARKGRAY = 8;
const int LIGHTBLUE = 9;
const int LIGHTGREEN = 10;
const int LIGHTCYAN = 11;
const int LIGHTRED = 12;
const int LIGHTMAGENTA = 13;
const int YELLOW = 14;
const int WHITE = 15;

using namespace std;
HANDLE hConsoleOut;

// User structure representing a Node in the Graph
struct User {
    int id;
    string name;
    vector<int> friends; // Adjacency List
};

vector<User> network;
int userIdCounter = 101;

// --- UI UTILITY FUNCTIONS (As per your design) ---

void initConsole() {
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

void gotoxy(int x, int y) {
    COORD c; 
    c.X = x; 
    c.Y = y;
    SetConsoleCursorPosition(hConsoleOut, c);
}

void setColor(int color) {
    SetConsoleTextAttribute(hConsoleOut, color);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100; 
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsoleOut, &info);
}

void showCursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 20; 
    info.bVisible = TRUE;
    SetConsoleCursorInfo(hConsoleOut, &info);
}

void drawBox(int left, int top, int width, int height) {
    int right = left + width;
    int bottom = top + height;
    gotoxy(left, top); 
    cout << char(201);
    gotoxy(right, top); 
    cout << char(187);
    gotoxy(left, bottom); 
    cout << char(200);
    gotoxy(right, bottom); 
    cout << char(188);
    for (int i = left + 1; i < right; i++) {
        gotoxy(i, top); 
        cout << char(205);
        gotoxy(i, bottom); 
        cout << char(205);
    }
    for (int i = top + 1; i < bottom; i++) {
        gotoxy(left, i); 
        cout << char(186);
        gotoxy(right, i); 
        cout << char(186);
    }
}

void centerBox(int width, int height, int &left, int &top) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsoleOut, &csbi);
    int screenWidth = csbi.srWindow.Right + 1;
    int screenHeight = csbi.srWindow.Bottom + 1;
    left = (screenWidth - width) / 2;
    top = (screenHeight - height) / 2;
    drawBox(left, top, width, height);
}

void loadingScreen(string text = "PROCESSING") {
    system("cls");
    int l, t;
    centerBox(60, 6, l, t);
    setColor(LIGHTCYAN);
    gotoxy(l + 20, t + 2); 
    cout << text;
    setColor(LIGHTGREEN);
    gotoxy(l + 5, t + 4);
    for (int i = 0; i < 50; i++) {
        cout << char(219);
        Sleep(15);
    }
    setColor(LIGHTGRAY);
}

void waitForBack(int l, int t) {
    hideCursor();
    setColor(LIGHTRED);
    gotoxy(l + 2, t); 
    cout << "[ < BACK ]";
    setColor(DARKGRAY); 
    cout << " (Press SPACE)";
    while (true) { 
        if (_getch() == KEY_SPACE) return; 
    }
}

// --- DATA LOGIC & FILE I/O ---

User* findUser(int id) {
    for (auto &u : network) if (u.id == id) return &u;
    return nullptr;
}

void saveToFile() {
    ofstream file("social_network.txt");
    file << userIdCounter << "\n";
    for (const auto& u : network) {
        file << u.id << "|" << u.name << "|";
        for (size_t i = 0; i < u.friends.size(); ++i) {
            file << u.friends[i] << (i == u.friends.size() - 1 ? "" : ",");
        }
        file << "\n";
    }
    file.close();
}

void loadFromFile() {
    ifstream file("social_network.txt");
    if (!file) return;
    file >> userIdCounter;
    string line;
    getline(file, line); // consume newline
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string part;
        User u;
        getline(ss, part, '|'); 
        u.id = stoi(part);
        getline(ss, u.name, '|');
        string friendList;
        getline(ss, friendList);
        stringstream ssf(friendList);
        string fId;
        while (getline(ssf, fId, ',')) {
            if (!fId.empty()) u.friends.push_back(stoi(fId));
        }
        network.push_back(u);
    }
}

// --- GRAPH ALGORITHMS ---

// 1. BFS for Shortest Path (Degree of Separation)
void calculateShortestPath() {
    system("cls");
    int l, t; 
    centerBox(60, 12, l, t);
    int id1, id2;
    showCursor();
    gotoxy(l+2, t+2); 
    cout << "Enter Source User ID: "; 
    cin >> id1;
    gotoxy(l+2, t+4); 
    cout << "Enter Target User ID: "; 
    cin >> id2;

    if (!findUser(id1) || !findUser(id2)) {
        setColor(LIGHTRED); 
        gotoxy(l+2, t+6); 
        cout << "One or both users not found!";
        waitForBack(l, t+10); 
        return;
    }

    loadingScreen("RUNNING BFS SEARCH");
    map<int, int> dist;
    map<int, int> parent;
    queue<int> q;

    q.push(id1);
    dist[id1] = 0;

    bool found = false;
    while (!q.empty()) {
        int curr = q.front(); 
        q.pop();
        if (curr == id2){ 
            found = true; 
            break; 
        }

        for (int neighbor : findUser(curr)->friends) {
            if (dist.find(neighbor) == dist.end()) {
                dist[neighbor] = dist[curr] + 1;
                parent[neighbor] = curr;
                q.push(neighbor);
            }
        }
    }

    system("cls");
    centerBox(60, 12, l, t);
    if (found) {
        setColor(LIGHTGREEN);
        gotoxy(l+5, t+3); 
        cout << "Shortest Degree of Separation: " << dist[id2];
        gotoxy(l+5, t+5); 
        cout << "Path Found (BFS logic): ";
        vector<int> path;
        for (int v = id2; v != id1; v = parent[v]) path.push_back(v);
        path.push_back(id1);
        reverse(path.begin(), path.end());
        for (size_t i = 0; i < path.size(); i++) cout << path[i] << (i == path.size() - 1 ? "" : " -> ");
    } 
    else {
        setColor(LIGHTRED); 
        gotoxy(l+5, t+4); 
        cout << "No connection exists between users.";
    }
    waitForBack(l, t+10);
}

// 2. BFS for Recommendation (Friends of Friends - Distance exactly 2)
void recommendFriends() {
    system("cls");
    int l, t; centerBox(60, 15, l, t);
    int id;
    showCursor();
    gotoxy(l+2, t+2); 
    cout << "Enter your User ID for Recommendations: "; 
    cin >> id;
    User* startNode = findUser(id);
    if (!startNode) {
        setColor(LIGHTRED); 
        gotoxy(l+2, t+4); 
        cout << "User not found!";
        waitForBack(l, t+8); 
        return;
    }

    loadingScreen("ANALYZING NETWORK");
    map<int, int> dist;
    queue<int> q;
    q.push(id);
    dist[id] = 0;

    vector<int> suggestions;
    set<int> myFriends(startNode->friends.begin(), startNode->friends.end());

    while (!q.empty()) {
        int curr = q.front(); 
        q.pop();
        if (dist[curr] >= 2) continue;

        for (int neighbor : findUser(curr)->friends) {
            if (dist.find(neighbor) == dist.end()) {
                dist[neighbor] = dist[curr] + 1;
                if (dist[neighbor] == 2) suggestions.push_back(neighbor);
                q.push(neighbor);
            }
        }
    }

    system("cls");
    centerBox(60, 10 + suggestions.size(), l, t);
    setColor(YELLOW); gotoxy(l+15, t+2); cout << "FRIEND RECOMMENDATIONS";
    setColor(LIGHTGRAY);
    int y = t+4;
    if (suggestions.empty()) {
        gotoxy(l+5, y); cout << "No new suggestions found.";
    } else {
        for (int sId : suggestions) {
            User* u = findUser(sId);
            gotoxy(l+5, y++); cout << "- " << u->name << " (ID: " << u->id << ")";
            if (y > t + 12) break; // limit display
        }
    }
    waitForBack(l, y + 2);
}

// 3. DFS for Discovery (K-steps)
void dfsDiscovery(int currId, int k, set<int>& visited, vector<int>& discovered) {
    if (k < 0) return;
    visited.insert(currId);
    discovered.push_back(currId);
    for (int neighbor : findUser(currId)->friends) {
        if (visited.find(neighbor) == visited.end()) {
            dfsDiscovery(neighbor, k - 1, visited, discovered);
        }
    }
}

void runDiscovery() {
    system("cls");
    int l, t; centerBox(60, 12, l, t);
    int id, k;
    showCursor();
    gotoxy(l+2, t+2); cout << "Enter Source User ID: "; cin >> id;
    gotoxy(l+2, t+4); cout << "Enter Steps (K): "; cin >> k;

    if (!findUser(id)) {
        setColor(LIGHTRED); gotoxy(l+2, t+6); cout << "User not found!";
        waitForBack(l, t+10); return;
    }

    loadingScreen("EXPLORING WITH DFS");
    set<int> visited;
    vector<int> result;
    dfsDiscovery(id, k, visited, result);

    system("cls");
    centerBox(65, 15, l, t);
    setColor(CYAN); gotoxy(l+5, t+2); cout << "USERS DISCOVERED WITHIN " << k << " STEPS:";
    setColor(LIGHTGRAY);
    int x = l+5, y = t+4;
    for (int rId : result) {
        if (rId == id) continue;
        gotoxy(x, y++); cout << "ID: " << rId << " | Name: " << findUser(rId)->name;
        if (y > t+13) break;
    }
    waitForBack(l, 14+t);
}

// --- MENU & MAIN UI ---

void drawMenu(int l, int t, int selected) {
    setColor(LIGHTCYAN);
    gotoxy(l + 15, t + 1); cout << "SOCIAL NETWORK ANALYZER";
    setColor(LIGHTGRAY);

    string options[] = {
        "Add User / Connection",
        "Show Social Graph",
        "Shortest Separation (BFS)",
        "Friend Recommendations",
        "Discover via K-Steps (DFS)",
        "Exit System"
    };

    for (int i = 0; i < 6; i++) {
        gotoxy(l + 10, t + 4 + i);
        if (i == selected) {
            setColor(224); // Highlight
            cout << " > " << options[i] << "               ";
        } else {
            setColor(i == 5 ? LIGHTRED : GREEN);
            cout << "   " << options[i] << "               ";
        }
    }
    setColor(DARKGRAY);
    gotoxy(l + 10, t + 12); cout << "[UP/DOWN] Move, [ENTER] Select";
}

int main() {
    initConsole();
    loadFromFile();
    loadingScreen("INITIALIZING GRAPH");

    int selected = 0;
    while (true) {
        system("cls");
        int l, t;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsoleOut, &csbi);
        l = (csbi.srWindow.Right - 60) / 2;
        t = (csbi.srWindow.Bottom - 15) / 2;

        hideCursor();
        drawBox(l, t, 60, 15);
        drawMenu(l, t, selected);

        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == KEY_UP) selected = (selected == 0) ? 5 : selected - 1;
            else if (key == KEY_DOWN) selected = (selected == 5) ? 0 : selected + 1;
        } else if (key == KEY_ENTER) {
            showCursor();
            if (selected == 0) { // Add User or Link
                system("cls");
                centerBox(60, 10, l, t);
                gotoxy(l+2, t+1); 
                cout << "1. Add New User";
                gotoxy(l+2, t+2); 
                cout << "2. Add Connection (Edge)";
                gotoxy(l+2, t+4); 
                cout << "Choice: ";
                int choice; 
                cin >> choice;
                if (choice == 1) {
                    User u; 
                    u.id = userIdCounter++;
                    gotoxy(l+2, t+6); 
                    cout << "Enter Name: "; 
                    cin.ignore(); 
                    getline(cin, u.name);
                    network.push_back(u);
                    saveToFile();
                    setColor(LIGHTGREEN); 
                    gotoxy(l+2, t+8); 
                    cout << "User Created with ID: " << u.id;
                } 
                else {
                    int id1, id2;
                    gotoxy(l+2, t+5); 
                    cout << "User ID 1: "; 
                    cin >> id1;
                    gotoxy(l+2, t+6); 
                    cout << "User ID 2: "; 
                    cin >> id2;
                    User* u1 = findUser(id1); 
                    User* u2 = findUser(id2);
                    if (u1 && u2) {
                        u1->friends.push_back(id2);
                        u2->friends.push_back(id1);
                        saveToFile();
                        setColor(LIGHTGREEN); 
                        gotoxy(l+2, t+8); 
                        cout << "Connection Established!";
                    } 
                    else {
                        setColor(LIGHTRED); 
                        gotoxy(l+2, t+8); 
                        cout << "Error: IDs not found.";
                    }
                }
                waitForBack(l, t+9);
            }
            else if (selected == 1) { // Show Graph
                system("cls");
                int height = max(10, (int)network.size() + 5);
                centerBox(70, height, l, t);
                int y = t+2;
                for (const auto& u : network) {
                    gotoxy(l+2, y++);
                    cout << u.id << " (" << u.name << ") -> Friends: ";
                    for (int fid : u.friends) cout << fid << " ";
                    if (y > t + height - 2) break;
                }
                waitForBack(l, t + height - 1);
            }
            else if (selected == 2) calculateShortestPath();
            else if (selected == 3) recommendFriends();
            else if (selected == 4) runDiscovery();
            else if (selected == 5) {
                loadingScreen("SAVING DATA");
                system("cls");
                break;
            }
        }
    }
    return 0;
}