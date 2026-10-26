#pragma once
#include <string>
#include <fstream>
#include "HuffmanTree.hpp"

using namespace std;

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
        file = ofstream(filename, ios::out | ios::binary);
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
    }
};

class EncoderModule {
public:
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
    void compressFile(string plaintextFilename, string outputFilename, HuffmanTree &hTree)
    {
        if (hTree.totalChars == 0) { cout<<"Error: File is empty."<<endl; return; }

        string treeHeader; generateTreeHeaderString(hTree.tree, treeHeader);
        writeModule io = writeModule(outputFilename);
        ifstream plaintext(plaintextFilename, ios::in | ios::binary);
        
        if (!plaintext.is_open()) { printf("Error opening plaintext for compression.\n"); return; }

        char* countPtr = (char*)&hTree.totalChars;
        for (size_t i = 0; i < sizeof(hTree.totalChars); i++) {
            io.writeByte(countPtr[i]);
        }

        for (size_t i = 0; i < treeHeader.length(); i++)
        {
            int bit = (treeHeader.at(i) == '0') ? 0 : 1;
            io.writeBit(bit);
            if (bit == 1) { io.writeByte(treeHeader.at(i+1)); i++; }
        }

        char ch;
        while (plaintext.get(ch)) for (size_t i = 0; i < hTree.codes[(unsigned char)ch].myCode.size(); i++) io.writeBit(hTree.codes[(unsigned char)ch].myCode.at(i)-'0');

        plaintext.close();

        if (io.getCtr() != 0) io.writePadding();
    }
};