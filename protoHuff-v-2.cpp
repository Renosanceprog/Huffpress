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
    //=======[CODE STRUCT]=============================
        struct codeNode
        {
            int myPadding;
            char myCode;
            bool isActive;
            codeNode()
            {
                isActive = false;
                myPadding = 0;
                myCode = 0;
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
//=======[TREE CREATION]==============================
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

    void createCodes(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL)
        {
            char b = 0;
            codes[p->myChar].isActive = true;
            for (size_t i = 0; i < path.size(); i++) b = (b << 1) | path.at(i);
            codes[p->myChar].myCode = b;
            codes[p->myChar].myPadding = 8 - path.size();
        }
        createCodes(p->left,path.append("0"));
        path.pop_back();
        createCodes(p->right,path.append("1"));
    }
    //========[IO Module]=======================
    class IOModule
    {
        private:
        unsigned char buf;
        int ctr;
        ofstream file;

        void writeToFile()
        {
            file.put(buf);
            resetBuf();
        }

        bool shiftAddBit(int bit)
        {
            ctr++;
            buf = (buf << 1) | bit;
            if (ctr == 8) return false;
            return true;
        }
        void resetBuf(){buf = 0; ctr = 0;}
        
        public:
        IOModule(string filename)
        {
            file = ofstream(filename+".huf", ios::out | ios::binary); 
            resetBuf();
        }
        ~IOModule()
        {
            file.close();
        }
        int getCtr() { return ctr; }
        void writeBit(int bit)
        {
            if (!shiftAddBit(bit)) writeToFile();
        }
        void writeByte(unsigned char byte)
        {
            for (int j = 7; j >= 0; j--)
            {
                int b = (byte >> j) & 1;
                if (!shiftAddBit(b)) writeToFile();
            }
        }
        void writePadding()
        {
            int padding = ctr;
            for (size_t i = 0; i < 8-padding; i++) writeBit(0);
            file.seekp(0);
            file.put(padding);
        }
    };
    //========[IO FUNCTIONS]=======================
    //========[FILE ENCODING]=======================
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
        string treeHeader; generateTreeHeaderString(tree, treeHeader);
        IOModule io = IOModule(filename);
        fstream plaintext(fname);

        io.writeByte(0);

        for (size_t i = 0; i < treeHeader.length(); i++)
        {
            int bit;
            if (treeHeader.at(i) == '0') bit = 0;
            else bit = 1;
            io.writeBit(bit);
            if (bit == 1) { io.writeByte(treeHeader.at(i+1)); i++; }
        }

        char ch;
        while (plaintext.get(ch)) for (size_t i = codes[ch].myPadding; i < 8; i++) io.writeBit((codes[ch].myCode >> i) & 1);

        plaintext.close();

        if (io.getCtr() != 0) io.writePadding();
    }
    //========[FILE DECODING]======================
    //========[CLASS ATTRIBUTES]===================
    char map[255] = {0};
    codeNode codes[255];
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
        createCodes(tree, "");
        cout<<endl;
        for (size_t i = 0; i < 256; i++)
        {
            if (i > 256) break;
            if (!codes[i].isActive) continue;
            else cout<<char(i)<<": "<<codes[i].myPadding<<endl;
        }
        
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
