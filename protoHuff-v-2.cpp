#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>

using namespace std;

class huffTree{
    private:
//=======[TREE NODE]==========================
    struct huffNode
    {
        char myChar;
        int myFreq;
        huffNode *left;
        huffNode *right;
        huffNode(int frq, char c)
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
//=======[PRIORITY QUEUE]==========================
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
    bool createOccurenceMap()
    {
        ifstream file(fname);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }

        char ch;
        while (file.get(ch)) map[ch]++;

        file.close();
        return true;
    }

    void createQueue()
    {
        treeQueue = new pQueue();
        for (size_t i = 0; i < 256; i++)
        {
            if (map[i] == 0) continue;
            treeQueue->enqueue(new huffNode(map[i], i));
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

    void preOrder(huffNode *p)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) printf("(%c, %d) -> ", p->myChar, p->myFreq);
        else printf("(%d) -> ", p->myFreq);
        preOrder(p->left);
        preOrder(p->right);
    }

    void codesTraverse(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<p->myChar<<": "<<path<<endl;
        codesTraverse(p->left,path.append("0"));
        path.pop_back();
        codesTraverse(p->right,path.append("1"));
    }
    //========[IO FUNCTIONS]=======================
    class IOModule
    {
        private:
        unsigned char buf;
        int ctr;

        public:
        IOModule(){ resetBuf(); }
        void resetBuf(){buf = 0; ctr = 0;}
        int getCtr() { return ctr; }
        unsigned char getBuf() { return buf; }
        bool shiftAddBit(int bit)
        {
            ctr++;
            buf = (buf << 1) | bit;
            if (ctr == 8) return false;
            return true;
        }
    };
    void generateTreeHeaderString(huffNode *p, string &s)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) {
            s.append("1");
            s.push_back(p->myChar);
        }
        else s.append("0");
        generateTreeHeaderString(p->left, s);
        generateTreeHeaderString(p->right,s);
        return;
    }
    void generateFile(string filename)
    {
        string treeHeader; 
        ofstream file(filename+".huf", ios::out | ios::binary);
        IOModule io = IOModule();
        file.put(0);
        generateTreeHeaderString(tree, treeHeader);
        for (size_t i = 0; i < treeHeader.size(); i++)
        {
            int bit;
            if (treeHeader.at(i) == '0') bit = 0;
            else bit = 1;
            if (!io.shiftAddBit(bit))
            {
                file.put(io.getBuf());
                io.resetBuf();
            }
            if (bit == 1){
                for (int j = 7; j >= 0; j--)
                {
                    int b = (treeHeader.at(i+1) >> j) & 1;
                    if (!io.shiftAddBit(b))
                    {
                        file.put(io.getBuf());
                        io.resetBuf();
                    }
                }
                i++;
            }
        }
        if (io.getCtr() != 0)
        {
            int padding = io.getCtr();
            for (size_t i = 0; i < 8-padding; i++) io.shiftAddBit(0);
            file.put(io.getBuf());
            file.seekp(0);
            file.put(padding);
        }
        file.close();
    }
    //========[CLASS ATTRIBUTES]===================
    char map[255] = {0};
    pQueue *treeQueue;
    huffNode *tree;
    string fname;

    public:
    huffTree(string filename)
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

    void displayTree()
    {
        preOrder(tree);
        cout<<"\e[4D    \n";
    }

    void displayCodes()
    {
        codesTraverse(tree, "");
        return;
    }
    void encodeText()
    {
        string s;
        cout<<"Enter encoded filename: ";
        cin>>s;
        generateFile(".\\exports\\"+s);
    }
};

int main(int argc, char const *argv[])
{
    huffTree tree = huffTree("plaintext.txt");
    tree.displayTree();
    tree.displayCodes();
    tree.encodeText();
    return 0;
}
