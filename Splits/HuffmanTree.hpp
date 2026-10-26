#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

struct huffNode
{
    unsigned char myChar;
    unsigned long long myFreq;
    huffNode *left;
    huffNode *right;
    huffNode(unsigned long long frq, unsigned char c)
    {
        myFreq = frq;
        myChar = c;
        left = NULL;
        right = NULL;
    }
    huffNode(unsigned long long frq)
    {
        myFreq = frq;
        myChar = '\0';
        left = NULL;
        right = NULL;
    }
    ~huffNode()
    {
        delete left;
        delete right;
    }
};

struct codeNode
{
    string myCode;
    bool isActive;
    codeNode() { isActive = false; }
};

class pQueue{
    private:
    struct qNode
    {
        huffNode *data;
        qNode *next;
        qNode(huffNode *node)
        {
            data = node;
            next = NULL;
        }
    };

    qNode *f;
    int ctr;

    public:
    bool isEmpty(){return ctr == 0;}
    int count(){return ctr;}
    pQueue()
    {
        f = NULL;
        ctr = 0;
    }

    void enqueue(huffNode *data)
    {
        qNode *newnode = new qNode(data);
        if (isEmpty() || data->myFreq < f->data->myFreq)
        {
            newnode->next = f;
            f = newnode;
        }
        else
        {
            qNode *p = f;
            while (p->next != NULL && data->myFreq >= p->next->data->myFreq) p = p->next;
            qNode *q = p->next;
            p->next = newnode;
            newnode->next = q;
        }
        ctr++;
    }

    huffNode *dequeue()
    {
        if (isEmpty()) return NULL;

        huffNode *val = f->data;
        qNode *del = f;

        f = f->next;

        delete del;
        ctr--;
        return val;
    }
};

double mySqrt(double val)
    {
        if (val <= 0) return 0;
        double guess = val / 2.0;
        if (guess == 0) guess = 1.0;
        for (int i = 0; i < 8; ++i) {
            guess = (guess + val / guess) / 2.0;
        }
        return guess;
    }

class HuffmanTree
{
public:
    unsigned long long map[256] = {0};
    codeNode codes[256];
    pQueue *treeQueue;
    huffNode *tree;
    string fname;
    unsigned long long totalChars;

    HuffmanTree(string filename)
    {
        treeQueue = NULL;
        tree = NULL;
        fname = filename;
        if (createOccurenceMap())
        {
            createQueue();
            createTree();
            createCodes(tree,"");
        }
        else return;
    }

    ~HuffmanTree()
    {
        delete tree;
        delete treeQueue;
    }

    bool createOccurenceMap()
    {
        ifstream file(fname, ios::in | ios::binary);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }

        char ch;
        while (file.get(ch)) {map[(unsigned char)ch]++; totalChars++;}

        file.close();
        return true;
    }

    void createQueue()
    {
        treeQueue = new pQueue();
        for (size_t i = 0; i < 256; i++)
        {
            if (map[i] == 0) continue;
            treeQueue->enqueue(new huffNode(map[i], (unsigned char)i));
        }
    }

    void createTree()
    {
        if (treeQueue->count() == 0) return;

        if (treeQueue->count() == 1)
        {
            huffNode *single = treeQueue->dequeue();
            tree = new huffNode(single->myFreq);
            tree->left = single;
            return;
        }

        while (treeQueue->count() != 1)
        {
            huffNode *l = treeQueue->dequeue();
            huffNode *r = treeQueue->dequeue();

            huffNode *newnode = new huffNode(l->myFreq + r->myFreq);
            newnode->left = l;
            newnode->right = r;
            treeQueue->enqueue(newnode);
        }
        tree = treeQueue->dequeue();
    }

    void createCodes(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL)
        {
            codes[p->myChar].isActive = true;
            codes[p->myChar].myCode = path.empty() ? "0" : path;
        }
        createCodes(p->left,path+"0");
        createCodes(p->right,path+"1");
    }

    void printHeatmappedText()
    {
        unsigned long maxFreq = 1;
        char ch;
        
        for (int i = 0; i < 256; i++) if (i != ' ' && i != '\n' && i != '\r' && i != '\t' && map[i] > maxFreq) maxFreq = map[i];

        double maxSqrt = mySqrt((double)maxFreq);
        if (maxSqrt == 0) maxSqrt = 1.0;

        ifstream file(fname);
        while (file.get(ch))
        {
            unsigned char uc = (unsigned char)ch;
            
            if (uc == ' ' || uc == '\n' || uc == '\r' || uc == '\t') {
                cout << ch;
                continue;
            }

            int freq = map[uc];

            double ratio = mySqrt((double)freq) / maxSqrt;
            if (ratio > 1.0) ratio = 1.0;

            int r = 0, g = 0, b = 0;
            if (ratio < 0.25) {
                r = 1; g = (int)((ratio / 0.25) * 4) + 1; b = 5;
            } else if (ratio < 0.50) {
                r = 1; g = 5; b = (int)((1.0 - (ratio - 0.25) / 0.25) * 4) + 1;
            } else if (ratio < 0.75) {
                r = (int)(((ratio - 0.50) / 0.25) * 4) + 1; g = 5; b = 1;
            } else {
                r = 5; g = (int)((1.0 - (ratio - 0.75) / 0.25) * 4) + 1; b = 1;
            }

            int ansiFgColor = 16 + (r * 36) + (g * 6) + b;

            printf("\x1b[38;5;%dm%c", ansiFgColor, ch);
        }
        cout << "\x1b[0m" << endl;
        file.close();
    }

    void printTreeRecursive(huffNode *node, string prefix, bool isLeft)
    {
        if (node == NULL) return;

        char left[4] = {(char)195,(char)196,(char)196,'\0'};
        char right[4] = {(char)192,(char)196,(char)196,'\0'};
        cout << "\033[90m" << prefix << "\033[0m";
        cout << "\033[36m" << (isLeft ? left : right) << "\033[0m";

        if (node->left == NULL && node->right == NULL)
        {
            cout << "\033[35m" << "['" << "\033[0m";
            if (node->myChar == '\n') cout << "\033[33m" << "\\n" << "\033[0m";
            else if (node->myChar == '\t') cout << "\033[33m" << "\\t" << "\033[0m";
            else if (node->myChar == '\r') cout << "\033[33m" << "\\r" << "\033[0m";
            else if (node->myChar == ' ') cout << "\033[33m" << "[space]" << "\033[0m";
            else cout << "\033[32m" << node->myChar << "\033[0m";
            cout << "\033[35m" << "']" << "\033[0m" << "\033[37m" << " (freq: " << "\033[33m" << node->myFreq << "\033[37m" << ")\033[0m\n";
        }
        else cout << "\033[33m" << "[*]" << "\033[37m" << " (freq: " << "\033[33m" << node->myFreq << "\033[37m" << ")\033[0m\n";

        char nextleft[4] = {(char)179,' ',' ','\0'};
        string nextPrefix = prefix + "\033[36m" + (isLeft ? nextleft : "    ") + "\033[0m";
        printTreeRecursive(node->left, nextPrefix, true);
        printTreeRecursive(node->right, nextPrefix, false);
    }

    void displayTree(huffNode *p)
    {
        if (p == NULL) return;

        if (p->left == NULL && p->right == NULL)
        {
            cout << "\033[35m" << "['" << "\033[0m";
            if (p->myChar == '\n') cout << "\033[33m" << "\\n" << "\033[0m";
            else if (p->myChar == '\t') cout << "\033[33m" << "\\t" << "\033[0m";
            else if (p->myChar == '\r') cout << "\033[33m" << "\\r" << "\033[0m";
            else if (p->myChar == ' ') cout << "\033[33m" << "[space]" << "\033[0m";
            else cout << "\033[32m" << p->myChar << "\033[0m";
            cout << "\033[35m" << "']" << "\033[0m" << "\033[37m" << " (freq: " << "\033[33m" << p->myFreq << "\033[37m" << ")\033[0m\n";
        }
        else cout << "\033[33m" << "[*]" << "\033[37m" << " (freq: " << "\033[33m" << p->myFreq << "\033[37m" << ")\033[0m\n";

        printTreeRecursive(p->left, "", true);
        printTreeRecursive(p->right, "", false);
    }

    void displayFrequencies()
    {
        for (size_t i = 0; i < 256; i++)
        {
            if (map[i] == 0) continue;
            if (i == '\n') cout<<"\\n: ";
            else if (i == '\t') cout<<"\\t: ";
            else if (i == '\r') cout<<"\\r: ";
            else if (i == ' ') cout<<"[space]: ";
            else cout<<(char)i<<": ";
            cout<<map[i]<<endl;
        }
    }
    void displayCodesRecursive(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) 
        {
            if (p->myChar == '\n') cout<<"\\n";
            else if (p->myChar == '\t') cout<<"\\t";
            else if (p->myChar == '\r') cout<<"\\r";
            else if (p->myChar == ' ') cout<<"[space]";
            else cout<<p->myChar;
            cout<<": "<<path<<endl;
        }
        displayCodesRecursive(p->left,path.append("0"));
        path.pop_back();
        displayCodesRecursive(p->right,path.append("1"));
    }

};