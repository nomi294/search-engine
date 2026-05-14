#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cctype>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <set>
#include <sstream>
using namespace std;
unordered_map<string, set<int>> invertedIndex;

string filterText(string text)
{
    string cleanText;
    for (int i = 0; i < text.length(); i++)
    {
        if (isalpha(text[i]) || text[i] == ' ')
        {
            if (text[i] == ' ' && i + 1 < text.length() && text[i + 1] == ' ')
                continue;
            cleanText += tolower((unsigned char)text[i]);
        }
    }
    return cleanText;
}

vector<string> split_into_words(string text)
{
    vector<string> words;
    string temp;
    for (const char t : text)
    {
        if (t == ' ')
        {
            if (temp.length() > 0)
                words.push_back(temp);
            temp = "";
            continue;
        }
        temp += t;
    }
    if (temp.length() > 0)
        words.push_back(temp);
    return words;
}

void indexDocument(vector<string> &words, int docID)
{
    for (auto &word : words)
        if (!word.empty())
            invertedIndex[word].insert(docID);
}

vector<string> indexFile(string fileName, int docID)
{
    string line;
    vector<string> temp, words;
    ifstream myfile(fileName, ios::in);
    while (getline(myfile, line))
    {
        line = filterText(line);
        temp = split_into_words(line);
        words.insert(words.end(), temp.begin(), temp.end());
    }
    indexDocument(words, docID);
    myfile.close();
    return words;
}

set<int> intersection(set<int> &a, set<int> &b)
{
    set<int> result;
    set_intersection(a.begin(), a.end(), b.begin(), b.end(), inserter(result, result.begin()));
    return result;
}

vector<string> loadTextfromFile()
{
    string textFiles[5] = {"docs/input-1.txt", "docs/input-2.txt", "docs/input-3.txt", "docs/input-4.txt", "docs/input-5.txt"};
    vector<string> temp, words;
    for (int i = 0; i < 5; i++)
    {
        temp = indexFile(textFiles[i], i + 1);
        words.insert(words.end(), temp.begin(), temp.end());
    }
    return words;
}

class TrieNode
{
    char word;
    TrieNode *childs[26];
    bool isTerminalNode;

public:
    int frequency;
    TrieNode()
    {
        this->word = ' ';
        for (int i = 0; i < 26; i++)
        {
            childs[i] = nullptr;
        }
        this->isTerminalNode = false;
        this->frequency = 0;
    }

    void setWord(char word)
    {
        this->word = word;
    }

    char getWord()
    {
        return word;
    }

    void InsertChild(char c)
    {
        int index = c - 'a';
        if (childs[index] == nullptr)
        {
            this->childs[index] = new TrieNode();
            this->childs[index]->setWord(c);
        }
    }

    void removeChild(char c)
    {
        int index = c - 'a';
        if (childs[index] != nullptr && childs[index]->getWord() == c)
        {
            TrieNode *temp = childs[index];
            delete temp;
            childs[index] = nullptr;
        }
        else
        {
            cout << "Child not found\n";
        }
    }

    bool searchChild(char c)
    {
        int index = c - 'a';
        if (childs[index] != nullptr && childs[index]->getWord() == c)
        {
            cout << "Child found at index : " << index << endl;
            return true;
        }
        cout << "Child not found\n";
        return false;
    }

    TrieNode **getChild()
    {
        return childs;
    }

    void setTerminalNode()
    {
        this->isTerminalNode = true;
    }

    bool CheckTerminalNode()
    {
        return isTerminalNode;
    }

    TrieNode *findChild(char c)
    {
        int index = c - 'a';
        return childs[index];
    }

    void displayChildren()
    {
        cout << "Children : ";
        for (int i = 0; i < 26; i++)
            if (childs[i])
                cout << childs[i]->getWord() << " ";
        cout << endl;
    }

    void displayWord()
    {
        cout << "Word : " << word << endl;
    }

    void display()
    {
        displayWord();
        displayChildren();
    }

    ~TrieNode()
    {
        for (int i = 0; i < 26; i++)
            if (childs[i])
                delete childs[i];
    }
};

class Trie
{
    TrieNode *root, *current;

public:
    Trie()
    {
        this->root = new TrieNode();
        this->current = root;
    }

    void insert(string word)
    {
        TrieNode *next = root;
        current = root;
        for (int i = 0; i < word.length(); i++)
        {
            next = current->findChild(word[i]);
            if (next == nullptr)
            {
                current->InsertChild(word[i]);
                next = current->findChild(word[i]);
            }
            current = next;
        }

        if (next->CheckTerminalNode())
            next->frequency++;
        else
        {
            next->frequency = 1;
            next->setTerminalNode();
        }
    }

    bool searchHelper(TrieNode *node, string word, int index)
    {
        if (word.length() == index)
            return node->CheckTerminalNode();
        int charIndex = word[index] - 'a';
        TrieNode **temp = node->getChild();
        if (temp[charIndex] == nullptr)
            return false;
        return searchHelper(temp[charIndex], word, index + 1);
    }

    bool searchWord(string word)
    {
        return searchHelper(root, word, 0);
    }

    bool prefixSearchHepler(TrieNode *node, string prefix, int index)
    {
        if (index == prefix.length())
            return true;
        int charIndex = prefix[index] - 'a';
        TrieNode **temp = node->getChild();
        if (temp[charIndex] == nullptr)
            return false;
        else
            return prefixSearchHepler(temp[charIndex], prefix, index + 1);
    }

    bool prefixSearch(string prefix)
    {
        return prefixSearchHepler(root, prefix, 0);
    }

    void generateSuggestions(TrieNode *node, string word, vector<pair<string, int>> &suggestions)
    {
        if (node->CheckTerminalNode())
            suggestions.push_back({word, node->frequency});
        TrieNode **temp = node->getChild();
        for (int i = 0; i < 26; i++)
            if (temp[i] != nullptr)
            {
                char nextChar = 'a' + i;
                generateSuggestions(temp[i], word + nextChar, suggestions);
            }
    }

    TrieNode *reachPrefixNode(TrieNode *node, string prefix)
    {
        current = node;
        TrieNode *next = nullptr;
        for (int i = 0; i < prefix.length(); i++)
        {
            next = current->findChild(prefix[i]);
            if (next == nullptr)
                return next;
            current = next;
        }
        return current;
    }

    vector<string> autoComplete(string prefix)
    {
        TrieNode *prefixNode = reachPrefixNode(root, prefix);
        if (prefixNode == nullptr)
            return {};
        else
        {
            vector<pair<string, int>> suggestions;
            generateSuggestions(prefixNode, prefix, suggestions);
            sort(suggestions.begin(), suggestions.end(), [](auto &a, auto &b)
                 {
                     if (a.second != b.second)
                         return a.second > b.second;
                     return a.second>b.second; });
            vector<string> finalSuggestions;
            for (const auto &s : suggestions)
                finalSuggestions.emplace_back(s.first);
            int maxsize = 5;
            if (finalSuggestions.size() > 5)
                finalSuggestions.resize(maxsize);
            return finalSuggestions;
        }
    }

    void displayHelper(TrieNode *node, string curr)
    {
        if (!node)
            return;
        TrieNode **temp = node->getChild();
        for (int i = 0; i < 26; i++)
        {
            if (temp[i] != nullptr)
                displayHelper(temp[i], curr + char(i + 'a'));
        }
        if (node->CheckTerminalNode())
            cout << curr << endl;
    }

    void display()
    {
        displayHelper(root, "");
    }

    ~Trie()
    {
        delete root;
    }
};

int main()
{
    Trie T;
    vector<string> words = loadTextfromFile();
    unordered_map<int, string> documents = {
        {1, "docs/input-1.txt"},
        {2, "docs/input-2.txt"},
        {3, "docs/input-3.txt"},
        {4, "docs/input-4.txt"},
        {5, "docs/input-5.txt"}};
    set<int> resultDocs;
    map<string, vector<string>> history;
    string choice, search = "car", intro = R"(
         █████╗ ██╗   ██╗    ███████╗███████╗ █████╗ ██████╗  ██████╗██╗  ██╗
        ██╔══██╗██║   ██║    ██╔════╝██╔════╝██╔══██╗██╔══██╗██╔════╝██║  ██║
        ███████║██║   ██║    ███████╗█████╗  ███████║██████╔╝██║     ███████║
        ██╔══██║██║   ██║    ╚════██║██╔══╝  ██╔══██║██╔══██╗██║     ██╔══██║
        ██║  ██║╚██████╔╝    ███████║███████╗██║  ██║██║  ██║╚██████╗██║  ██║
        ╚═╝  ╚═╝ ╚═════╝     ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝                                                            
    )";
    //================[ Logo ]=======================
    cout << "\033[38;2;2;62;138m" << intro << "\033[0m\n";
    //==========[ Inserting words into Trie from docs folder having texts ]=============
    for (const string w : words)
    {
        T.insert(w);
    }
    words.clear();
    //===============================================
    do
    {
        cout << "\033[38;2;4;71;28m<< ===== MENU ===== >\033[0m" << endl
             << "\033[38;2;0;150;199m1.\033m \033[38;2;144;224;239mSearch\033[0m" << endl
             << "\033[38;2;0;150;199m2.\033m \033[38;2;144;224;239mShow search history\033[0m" << endl
             << "\033[38;2;0;150;199m3.\033m \033[38;2;144;224;239mClear history\033[0m" << endl
             << "\033[38;2;0;150;199m0.\033m \033[0m\033[38;2;144;224;239mExit\033[0m" << endl
             << "\033[38;2;144;224;239mEnter here : \033[0m";
        cin >> choice;
        cin.ignore();
        cout << "\033[38;2;0;150;199m===============================\033[0m\n";
        if (choice == "1")
        {
            cout << "Search : ";
            getline(cin, search);
            if (search.empty())
            {
                cout << "input not provided!\n";
                cout << "\033[38;2;0;150;199m===============================\033[0m\n";
                return 0;
            }
            words = split_into_words(filterText(search));
            resultDocs = invertedIndex[words[0]];
            for (int i = 1; i < words.size(); i++)
                resultDocs = intersection(resultDocs, invertedIndex[words[i]]);
            for (int id : resultDocs)
                cout << "Documents found : " << documents[id] << endl;
            cout << "\033[38;2;0;150;199m===============================\033[0m\nSuggestions about " << search << " :\n";
            if (!words.empty())
            {
                words = T.autoComplete(words.back());
                if (!words.empty())
                {
                    for (const string w : words)
                        cout << "\033[38;2;0;225;225;48;2;153;139;197m" << w << "\033[0m" << endl;
                    history[search] = words;
                }
                else
                {
                    cout << "\033[38;2;242;106;141mNo related results found!\033[0m" << endl;
                    cout << "\033[38;2;0;150;199m===============================\033[0m\n";
                }
            }
        }
        else if (choice == "2")
        {
            if (!history.empty())
            {
                cout << "Total Searches : " << history.size() << endl;
                for (const auto &h : history)
                {
                    cout << "\033[38;2;222;201;233msearch : " << h.first << "\nResults : \033[0m";
                    for (const auto &sug : h.second)
                    {
                        if (!sug.empty())
                            cout << "\033[38;2;222;201;233m" << sug << "\t\033[0m";
                    }
                    cout << "\n\033[38;2;0;150;199m===============================\033[0m\n";
                }
            }
            else
                cout << "\033[38;2;242;106;141mNo history to show!\033[0m" << endl;
            cout << "\033[38;2;0;150;199m===============================\033[0m\n";
        }
        else if (choice == "3")
        {
            if (!history.empty())
            {
                history.clear();
                cout << "\033[38;2;183;239;197mHistory cleared!\033[0m\n";
            }
            else
                cout << "\033[38;2;242;106;141mNo history to be cleared!\033[0m" << endl;
            cout << "\033[38;2;0;150;199m===============================\033[0m\n";
        }
        else if (choice == "0" || choice == "stop")
        {
            cout << "\033[38;2;183;239;197mExiting...\033[0m\n";
            cout << "\033[38;2;0;150;199m===============================\033[0m\n";
            break;
        }
        else
        {
            cout << "\033[38;2;242;106;141mInvalid Input! enter again\033[0m" << endl;
            cout << "\033[38;2;0;150;199m===============================\033[0m\n";
        }
    } while (true);
    return 0;
}