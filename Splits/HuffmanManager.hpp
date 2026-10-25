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

        activeTxt = filename;
        txtLoaded = true;
        return true;
    }
    bool loadCompressed(string filename)
    {
        string fmtFname = ".\\Compressed\\"+filename+".huf";
        ifstream file(fmtFname, ios::in | ios::binary);

        if (!file.is_open()) return false;
        else file.close();

        activeHuf = filename;
        hufLoaded = true;
        return true;
    }

    void displayText(bool isPlain)
    {
        if (isPlain)
        {
            ifstream file(".\\Plaintexts\\"+activeTxt+".txt");
            string line;
            while (getline(file, line)) cout<<line<<endl;
            file.close();
        }
        else
        {
            DecoderModule app;
            app.decodeFileAndRead(".\\Compressed\\"+activeHuf+".huf");
        }
    }
    void displayFrequencies(bool isPlain)
    {
        if (isPlain) currentTree->displayFrequencies();
        else
        {
            DecoderModule app;
            string temploc = ".\\Compressed\\temp.txt";
            app.decodeFileAndSave(".\\Compressed\\"+activeHuf+".huf",temploc);
            HuffmanTree *temp = new HuffmanTree(temploc);
            temp->displayFrequencies();
            delete temp;
            remove(temploc.c_str());
        }
    }
    void displayTree(bool isPlain)
    {
        if (isPlain) currentTree->displayTree(currentTree->tree);
        else
        {
            ifstream file(".\\Compressed\\"+activeHuf+".huf", ios::in | ios::binary);
            readModule io = readModule(&file);
            DecoderModule app;
            huffNode *retrievedTree = app.retrieveTree(io);
            currentTree->displayTree(retrievedTree);
            file.close();
        }
    }
    void displayCodes(bool isPlain)
    {
        if (isPlain) currentTree->displayCodesRecursive(currentTree->tree,"");
        else
        {
            ifstream file(".\\Compressed\\"+activeHuf+".huf", ios::in | ios::binary);
            readModule io = readModule(&file);
            DecoderModule app;
            huffNode *retrievedTree = app.retrieveTree(io);
            currentTree->displayCodesRecursive(retrievedTree, "");
            file.close();
        }
    }
    bool executeCompression(string outName)
    {
        string fmtOutName;
        if (outName == "NONE") fmtOutName = ".\\Compressed\\"+activeTxt+".huf";
        else fmtOutName = ".\\Compressed\\"+outName+".huf";

        EncoderModule app;
        app.compressFile(".\\Plaintexts\\"+activeTxt+".txt", fmtOutName, *currentTree);
        return true;
    }
    bool executeDecompression(string outName)
    {
        string fmtOutName;
        if (outName == "NONE") fmtOutName = ".\\Plaintexts\\"+activeHuf+".txt";
        else fmtOutName = ".\\Plaintexts\\"+outName+".txt";

        DecoderModule app;
        return app.decodeFileAndSave(".\\Compressed\\"+activeHuf+".huf",fmtOutName);
    }
    bool runIntegrityCheck(){return false;}
};