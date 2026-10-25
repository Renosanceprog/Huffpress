#pragma once
#include <string>
#include <fstream>
#include "HuffmanTree.hpp"

using namespace std;

class readModule
{
    private:
    ifstream *file;
    unsigned workingBuf;
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
        char locbuf;
        if (remBits == 0)
        {
            if (!file->get(locbuf)) return -1;
            isLast = (file->peek() == EOF);
            remBits = 8;
        }
        if (isLast && padding != 0 && remBits == 8 - padding) return -1;
        else
        {
            remBits--;
            workingBuf = (unsigned char) locbuf;
            return (workingBuf >> remBits) & 1;
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

class DecoderModule {
public:
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
    bool decodeFileAndRead(string filename)
    {
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }
        readModule io = readModule(&file);

        huffNode *tree = retrieveTree(io);

        while (true)
        {
            huffNode *p = tree;
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
    static bool decodeFileAndSave(string compressedFilename, string outputPlaintextFilename);
    static bool verifyStreamIntegrity(string originalTxt, string compressedHuf);
};