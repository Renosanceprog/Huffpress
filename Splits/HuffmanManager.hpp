#pragma once
#include <string>
#include "HuffmanTree.hpp"
#include "EncoderModule.hpp"
#include "DecoderModule.hpp"

class HuffmanManager {
private:
    string activeTxt;
    string activeHuf;
    bool txtLoaded;
    bool hufLoaded;
    bool pairVerified;
    HuffmanTree *currentTree;

public:
    HuffmanManager()
    {
        activeTxt = "";
        activeHuf = "";
        txtLoaded = false;
        hufLoaded = false;
        pairVerified = false;
        currentTree = NULL;
    };

    // State getters for UI locking
    bool isTxtLoaded(){ return txtLoaded; }
    bool isHufLoaded(){ return hufLoaded; }
    bool isPairVerified(){ return pairVerified; }
    string getTxtFilename(){ return activeTxt; }
    string getHufFilename(){ return activeHuf; }

    // Actions (Printless: return booleans/status instead of cout)
    bool loadPlaintext(string filename)
    {
        string fmtFname = ".\\Plaintexts\\"+filename+".txt";
        ifstream file(fmtFname);

        if (!file.is_open()) return false;
        else file.close();

        if (currentTree != NULL) delete currentTree;
        currentTree = new HuffmanTree(fmtFname);

        activeTxt = filename+".txt";
        txtLoaded = true;
        return true;
    }
    bool loadCompressed(string filename)
    {
        string fmtFname = ".\\Compressed\\"+filename+".huf";
        ifstream file(fmtFname, ios::in | ios::binary);

        if (!file.is_open()) return false;
        else file.close();

        activeHuf = filename+".huf";
        hufLoaded = true;
        return true;
    }
    
    bool buildTreeAndCodes(){return false;}
    bool executeCompression(string outName){return "";}
    bool executeDecompression(string outName){return "";}
    bool runIntegrityCheck(){return false;}
};