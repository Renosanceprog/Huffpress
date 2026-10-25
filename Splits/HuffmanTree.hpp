#pragma once
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct huffNode
{
    unsigned char myChar;
    unsigned long myFreq;
    huffNode *left;
    huffNode *right;
    huffNode(int frq, unsigned char c)
    {
        myFreq = frq;
        myChar = c;
        left = NULL;
        right = NULL;
    }
    huffNode(int frq)
    {
        myFreq = frq;
        myChar = '\0';
        left = NULL;
        right = NULL;
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

class HuffmanTree {
public:
    unsigned long map[256] = {0};
    codeNode codes[256];
    pQueue *treeQueue;
    huffNode *tree;
    string fname;

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

    bool createOccurenceMap()
    {
        ifstream file(fname, ios::in | ios::binary);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }

        char ch;
        while (file.get(ch)) map[(unsigned char)ch]++;

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
    
    void printTreeRecursive(huffNode *node, string prefix, bool isLeft)
    {
        if (node == NULL) return;

        cout << prefix;
        cout << (isLeft ? "|-- " : "L__ ");

        if (node->left == NULL && node->right == NULL)
        {
            cout << "['";
            if (node->myChar == '\n') cout << "\\n";
            else if (node->myChar == '\t') cout << "\\t";
            else if (node->myChar == '\r') cout << "\\r";
            else if (node->myChar == ' ') cout << "[space]";
            else cout << node->myChar;
            cout << "'] (freq: " << node->myFreq << ")\n";
        }
        else cout << "[*] (freq: " << node->myFreq << ")\n";

        string nextPrefix = prefix + (isLeft ? "|   " : "    ");
        printTreeRecursive(node->left, nextPrefix, true);
        printTreeRecursive(node->right, nextPrefix, false);
    }

    void displayTree(huffNode *p)
    {
        if (p == NULL) return;

        if (p->left == NULL && p->right == NULL)
        {
            cout << "['";
            if (p->myChar == '\n') cout << "\\n";
            else if (p->myChar == '\t') cout << "\\t";
            else if (p->myChar == '\r') cout << "\\r";
            else if (p->myChar == ' ') cout << "[space]";
            else cout << p->myChar;
            cout << "'] (freq: " << p->myFreq << ")\n";
        }
        else cout << "[*] (freq: " << p->myFreq << ")\n";

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
        if (p->left == NULL && p->right == NULL) cout<<p->myChar<<": "<<path<<endl;
        displayCodesRecursive(p->left,path.append("0"));
        path.pop_back();
        displayCodesRecursive(p->right,path.append("1"));
    }

    void createCodes(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL)
        {
            codes[p->myChar].isActive = true;
            codes[p->myChar].myCode = path;
        }
        createCodes(p->left,path.append("0"));
        path.pop_back();
        createCodes(p->right,path.append("1"));
    }
};