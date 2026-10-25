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
        string treeHeader; generateTreeHeaderString(hTree.tree, treeHeader);
        writeModule io = writeModule(outputFilename);
        fstream plaintext(plaintextFilename);

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
        while (plaintext.get(ch)) for (size_t i = 0; i < hTree.codes[ch].myCode.size(); i++) io.writeBit(hTree.codes[ch].myCode.at(i)-'0');

        plaintext.close();

        if (io.getCtr() != 0) io.writePadding();
    }
};