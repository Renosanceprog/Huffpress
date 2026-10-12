#include <iostream>
#include <stdio.h>

using namespace std;

    struct Huffnode{
        char myChar;
        int myFreq;
        Huffnode *left, *right;
    };

class Queue{
    private:
    struct qNode
    {
        Huffnode *data;
        qNode *next;
        qNode(Huffnode *node)
        {
            data = node;
            next = NULL;
        }
    };
    
    
    qNode *f;
    int ctr;
    
    public:
    bool isEmpty(){return ctr == 0;}
    Queue()
    {
        f = NULL;
        ctr = 0;
    }

    void enqueue(Huffnode *data)
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

    Huffnode *dequeue()
    {
        if (isEmpty()) return NULL;
        
        Huffnode *val = f->data;
        qNode *del = f;

        f = f->next;

        delete del;
        ctr--;
        return val;
    }
};

