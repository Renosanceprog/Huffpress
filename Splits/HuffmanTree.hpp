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
    unsigned char map[256];
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
    
    void displayTree(huffNode *p);
    void displayCodes(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<p->myChar<<": "<<path<<endl;
        displayCodes(p->left,path.append("0"));
        path.pop_back();
        displayCodes(p->right,path.append("1"));
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