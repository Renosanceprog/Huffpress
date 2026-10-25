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
    //=======[CODE STRUCT]=============================
        struct codeNode
        {
            string myCode;
            bool isActive;
            codeNode() { isActive = false; }
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
            codes[p->myChar].isActive = true;
            codes[p->myChar].myCode = path;
        }
        createCodes(p->left,path.append("0"));
        path.pop_back();
        createCodes(p->right,path.append("1"));
    }
    //========[IO Module]=======================
    class writeModule
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
        writeModule(string filename)
        {
            file = ofstream(filename+".huf", ios::out | ios::binary);
            resetBuf();
        }
        ~writeModule()
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
    class readModule
    {
        private:
        ifstream *file;
        char workingBuf;
        int remBits;
        bool isLast;

        public:
        int padding;
        readModule(ifstream *filename)
        {
            workingBuf = 0;
            remBits = 0;
            char pd;
            file = filename;
            if (!file->get(pd) || (pd < 0 || pd > 7)) padding = -1;
            else padding = pd;
            isLast = false;
        }

        int readBit()
        {
            if (remBits == 0)
            {
                if (!file->get(workingBuf)) return -1;
                isLast = (file->peek() == EOF);
                remBits = 8;
            }
            if (isLast && padding != 0 && remBits == 8 - padding) return -1;
            else
            {
                remBits--;
                return (static_cast<unsigned char>(workingBuf) >> remBits) & 1;
            }
        }
        char readByte() {
            char val = 0;
            for (int i = 0; i < 8; i++) {
                int b = readBit();
                if (b == -1) break;
                val = (val << 1) | b;
            }
            return val;
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
        writeModule io = writeModule(filename);
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
        while (plaintext.get(ch)) for (size_t i = 0; i < codes[ch].myCode.size(); i++) io.writeBit(codes[ch].myCode.at(i)-'0');

        plaintext.close();

        if (io.getCtr() != 0) io.writePadding();
    }
    //========[FILE DECODING]======================
    huffNode* retrieveTree(readModule &io) {
        int bit = io.readBit();
        if (bit == -1) return NULL;

        if (bit == 1)
        {
            char ch = io.readByte();
            return new huffNode(0, ch);
        }
        else
        {
            huffNode* node = new huffNode(0);
            node->left = retrieveTree(io);
            node->right = retrieveTree(io);
            return node;
        }
    }
    bool decodeFile(string filename)
    {
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }
        readModule io = readModule(&file);

        decodedtree = retrieveTree(io);

        while (true)
        {
            huffNode *p = decodedtree;
            int bit;
            while (true)
            {
                if (p->left == NULL && p->right == NULL) break;
                bit = io.readBit();
                if (bit == -1) break;
                if (bit == 0) p = p->left;
                else p = p->right;
            }
            if (bit == -1) break;
            cout<<p->myChar;
        }
        return true;
    }
    //========[CLASS ATTRIBUTES]===================
    unsigned char map[255] = {0};
    codeNode codes[255];
    pQueue *treeQueue;
    huffNode *tree;
    huffNode *decodedtree;
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
            else cout<<char(i)<<": "<<codes[i].myCode<<endl;
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
    void decodeText()
    {
        string s;
        cout<<"Enter filename to decode: ";
        cin>>s;
        decodeFile(".\\exports\\"+s+".huf");
    }
};

int main(int argc, char const *argv[])
{
    huffTree tree = huffTree("lorem.txt");
    //tree.displayTree();
    //tree.displayCodes();
    //tree.encodeText();
    tree.decodeText();
    return 0;
}
