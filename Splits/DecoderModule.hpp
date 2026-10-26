#pragma once
#include <string>
#include <fstream>
#include "HuffmanTree.hpp"

using namespace std;


enum IntegrityStatus {
    MATCH_OK = 0,
    ERROR_FILE_NOT_FOUND,
    ERROR_TREE_BUILD_FAILED,
    ERROR_LENGTH_MISMATCH,
    ERROR_BYTE_MISMATCH,
    ERROR_BITSTREAM_CORRUPT
};

struct IntegrityReport {
    IntegrityStatus status;
    unsigned long totalBytesVerified = 0;
    unsigned long totalBitsProcessed = 0;
    long mismatchIndex = -1;       // Position in the stream where it failed (-1 if none)
    char expectedChar = '\0';      // What the plaintext file had
    char actualChar = '\0';        // What the decoded bitstream produced
};

class readModule
{
    private:
    ifstream *file;
    unsigned char workingBuf;
    int remBits;

    public:
    unsigned long long totalChars;
    readModule(ifstream *filename)
    {
        workingBuf = 0;
        remBits = 0;
        file = filename;
        file->read((char*)&totalChars, sizeof(totalChars));
    }

    int readBit()
    {
        if (remBits == 0)
        {
            char locbuf;
            if (!file->get(locbuf)) return -1;
            workingBuf = (unsigned char) locbuf;
            remBits = 8;
        }
        remBits--;
        return (workingBuf >> remBits) & 1;
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
    huffNode* retrieveTree(readModule &io)
    {
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

    bool decodeFileAndSave(string compressedFilename, string outputPlaintextFilename)
    {
        ifstream ifile(compressedFilename, ios::in | ios::binary);
        if (!ifile.is_open())
        {
            printf("File not found.\n");
            return false;
        }
        readModule io = readModule(&ifile);

        huffNode *tree = retrieveTree(io);

        if (!tree) { ifile.close(); return false; }

        ofstream ofile(outputPlaintextFilename, ios::out | ios::binary);
        if (!ofile.is_open())
        {
            ifile.close();
            delete tree;
            return false;
        }

        unsigned long long decodedCount = 0;
        while (decodedCount < io.totalChars)
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
            ofile.put(p->myChar);
            decodedCount++;
        }
        ofile.close();
        ifile.close();
        delete tree;
        return true;
    }

    IntegrityStatus verifyStreamIntegrity(string originalTxt, string compressedHuf)
    {
        ifstream ofile(originalTxt, ios::in | ios::binary);
        if (!ofile.is_open()) return ERROR_FILE_NOT_FOUND;

        ifstream cfile(compressedHuf, ios::in | ios::binary);
        if (!cfile.is_open()) return ERROR_FILE_NOT_FOUND;

        readModule io(&cfile);

        huffNode *tree = retrieveTree(io);
        if (tree == NULL) return ERROR_TREE_BUILD_FAILED;

        unsigned long long decodedCount = 0;
        while (decodedCount < io.totalChars)
        {
            char raw_ch;
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

            bool hufFinished = (bit == -1);
            bool txtFinished = !(ofile.get(raw_ch));
            unsigned char ch = (unsigned char)raw_ch;

            if (txtFinished != hufFinished) return ERROR_LENGTH_MISMATCH;

            if (p->left != NULL || p->right != NULL) return ERROR_BITSTREAM_CORRUPT;

            if (ch != p->myChar) return ERROR_BYTE_MISMATCH;

            decodedCount++;
        }

        char dummy;
        if (ofile.get(dummy)) return ERROR_LENGTH_MISMATCH;

        delete tree;
        return MATCH_OK;
    }

    IntegrityReport runDetailedDiagnostics(string originalTxt, string compressedHuf)
    {
        IntegrityReport report;
        ifstream ofile(originalTxt, ios::in | ios::binary);
        if (!ofile.is_open()) { report.status = ERROR_FILE_NOT_FOUND; return report; }

        ifstream cfile(compressedHuf, ios::in | ios::binary);
        if (!cfile.is_open()) { report.status = ERROR_FILE_NOT_FOUND; return report; }

        readModule io(&cfile);

        huffNode *tree = retrieveTree(io);
        if (tree == NULL) { report.status = ERROR_TREE_BUILD_FAILED; return report; }

        long byteIndex = 0;
        unsigned long bitsReadCount = 0;
        unsigned long long decodedCount = 0;

        while (decodedCount < io.totalChars)
        {
            char raw_ch;
            huffNode *p = tree;
            int bit;
            
            while (true)
            {
                if (p->left == NULL && p->right == NULL) break;
                bit = io.readBit();
                if (bit == -1) break;
                bitsReadCount++;
                if (bit == 0) p = p->left;
                else p = p->right;
            }

            bool hufFinished = (bit == -1);
            bool txtFinished = !(ofile.get(raw_ch));
            unsigned char ch = (unsigned char)raw_ch;

            if (txtFinished != hufFinished)
            {
                report.status = ERROR_LENGTH_MISMATCH;
                report.mismatchIndex = byteIndex;
                delete tree;
                return report;
            }

            if (p->left != NULL || p->right != NULL)
            {
                report.status = ERROR_BITSTREAM_CORRUPT;
                report.mismatchIndex = byteIndex;
                delete tree;
                return report;
            }

            if (ch != p->myChar)
            {
                report.status = ERROR_BYTE_MISMATCH;
                report.mismatchIndex = byteIndex;
                report.expectedChar = ch;
                report.actualChar = p->myChar;
                delete tree;
                return report;
            }

            byteIndex++;
            decodedCount++;
            report.totalBytesVerified = byteIndex;
            report.totalBitsProcessed = bitsReadCount;
        }

        char dummy;
        if (ofile.get(dummy))
        {
            report.status = ERROR_LENGTH_MISMATCH;
            report.mismatchIndex = byteIndex;
            delete tree;
            return report;
        }

        report.status = MATCH_OK;
        delete tree;
        return report;
    }
};