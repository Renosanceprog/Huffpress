#include <iostream>
#include <string>
#include <fstream>

using namespace std;

//================================================================
//=======[HUFFMAN CORE & TREE CREATION]===========================
//================================================================
//--------[TREE NODE FOR HUFFMAN TREE]----------------------------
struct huffNode
{
    unsigned char myChar;
    unsigned long long myFreq;
    huffNode *left;
    huffNode *right;
    huffNode(unsigned long long frq, unsigned char c)
    {
        myFreq = frq;
        myChar = c;
        left = NULL;
        right = NULL;
    }
    huffNode(unsigned long long frq)
    {
        myFreq = frq;
        myChar = '\0';
        left = NULL;
        right = NULL;
    }
    ~huffNode()
    {
        delete left;
        delete right;
    }
};
//--------[SIMPLE STRUCT FOR CODE MAP]----------------------------
struct codeNode
{
    string myCode;
    bool isActive;
    codeNode() { isActive = false; }
};
//--------[PRIORITY QUEUE FOR TREE CREATION PROCESS]--------------
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
//--------[sqrt helper for heatmap creation]----------------------------
double mySqrt(double val)
    {
        if (val <= 0) return 0;
        double guess = val / 2.0;
        if (guess == 0) guess = 1.0;
        for (int i = 0; i < 8; ++i) {
            guess = (guess + val / guess) / 2.0;
        }
        return guess;
    }
//--------[MAIN CLASS FOR HUFFMAN CORE]---------------------------------
class HuffmanTree
{
public:
    unsigned long long map[256] = {0};
    codeNode codes[256];
    pQueue *treeQueue;
    huffNode *tree;
    string fname;
    unsigned long long totalChars;

    HuffmanTree(string filename)
    {
        treeQueue = NULL;
        tree = NULL;
        fname = filename;
        if (createOccurenceMap())
        {
            createQueue();
            createTree();
            createCodes(tree,"");
        }
        else return;
    }

    ~HuffmanTree()
    {
        delete tree;
        delete treeQueue;
    }

    bool createOccurenceMap()
    {
        ifstream file(fname, ios::in | ios::binary);
        if (!file.is_open())
        {
            printf("File not found.\n");
            return false;
        }

        char ch;
        while (file.get(ch)) {map[(unsigned char)ch]++; totalChars++;}

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
        if (treeQueue->count() == 0) return;

        if (treeQueue->count() == 1)
        {
            huffNode *single = treeQueue->dequeue();
            tree = new huffNode(single->myFreq);
            tree->left = single;
            return;
        }

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

    void createCodes(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL)
        {
            codes[p->myChar].isActive = true;
            codes[p->myChar].myCode = path.empty() ? "0" : path;
        }
        createCodes(p->left,path+"0");
        createCodes(p->right,path+"1");
    }

    void printHeatmappedText()
    {
        unsigned long maxFreq = 1;
        char ch;
        
        for (int i = 0; i < 256; i++) if (i != ' ' && i != '\n' && i != '\r' && i != '\t' && map[i] > maxFreq) maxFreq = map[i];

        double maxSqrt = mySqrt((double)maxFreq);
        if (maxSqrt == 0) maxSqrt = 1.0;

        ifstream file(fname);
        while (file.get(ch))
        {
            unsigned char uc = (unsigned char)ch;
            
            if (uc == ' ' || uc == '\n' || uc == '\r' || uc == '\t') {
                cout << ch;
                continue;
            }

            int freq = map[uc];

            double ratio = mySqrt((double)freq) / maxSqrt;
            if (ratio > 1.0) ratio = 1.0;

            int r = 0, g = 0, b = 0;
            if (ratio < 0.25) {
                r = 1; g = (int)((ratio / 0.25) * 4) + 1; b = 5;
            } else if (ratio < 0.50) {
                r = 1; g = 5; b = (int)((1.0 - (ratio - 0.25) / 0.25) * 4) + 1;
            } else if (ratio < 0.75) {
                r = (int)(((ratio - 0.50) / 0.25) * 4) + 1; g = 5; b = 1;
            } else {
                r = 5; g = (int)((1.0 - (ratio - 0.75) / 0.25) * 4) + 1; b = 1;
            }

            int ansiFgColor = 16 + (r * 36) + (g * 6) + b;

            printf("\x1b[38;5;%dm%c", ansiFgColor, ch);
        }
        cout << "\x1b[0m" << endl;
        file.close();
    }

    void printTreeRecursive(huffNode *node, string prefix, bool isLeft)
    {
        if (node == NULL) return;

        char left[4] = {(char)195,(char)196,(char)196,'\0'};
        char right[4] = {(char)192,(char)196,(char)196,'\0'};
        cout << "\033[90m" << prefix << "\033[0m";
        cout << "\033[36m" << (isLeft ? left : right) << "\033[0m";

        if (node->left == NULL && node->right == NULL)
        {
            cout << "\033[35m" << "['" << "\033[0m";
            if (node->myChar == '\n') cout << "\033[33m" << "\\n" << "\033[0m";
            else if (node->myChar == '\t') cout << "\033[33m" << "\\t" << "\033[0m";
            else if (node->myChar == '\r') cout << "\033[33m" << "\\r" << "\033[0m";
            else if (node->myChar == ' ') cout << "\033[33m" << "[space]" << "\033[0m";
            else cout << "\033[32m" << node->myChar << "\033[0m";
            cout << "\033[35m" << "']" << "\033[0m" << "\033[37m" << " (freq: " << "\033[33m" << node->myFreq << "\033[37m" << ")\033[0m\n";
        }
        else cout << "\033[33m" << "[*]" << "\033[37m" << " (freq: " << "\033[33m" << node->myFreq << "\033[37m" << ")\033[0m\n";

        char nextleft[4] = {(char)179,' ',' ','\0'};
        string nextPrefix = prefix + "\033[36m" + (isLeft ? nextleft : "    ") + "\033[0m";
        printTreeRecursive(node->left, nextPrefix, true);
        printTreeRecursive(node->right, nextPrefix, false);
    }

    void displayTree(huffNode *p)
    {
        if (p == NULL) return;

        if (p->left == NULL && p->right == NULL)
        {
            cout << "\033[35m" << "['" << "\033[0m";
            if (p->myChar == '\n') cout << "\033[33m" << "\\n" << "\033[0m";
            else if (p->myChar == '\t') cout << "\033[33m" << "\\t" << "\033[0m";
            else if (p->myChar == '\r') cout << "\033[33m" << "\\r" << "\033[0m";
            else if (p->myChar == ' ') cout << "\033[33m" << "[space]" << "\033[0m";
            else cout << "\033[32m" << p->myChar << "\033[0m";
            cout << "\033[35m" << "']" << "\033[0m" << "\033[37m" << " (freq: " << "\033[33m" << p->myFreq << "\033[37m" << ")\033[0m\n";
        }
        else cout << "\033[33m" << "[*]" << "\033[37m" << " (freq: " << "\033[33m" << p->myFreq << "\033[37m" << ")\033[0m\n";

        printTreeRecursive(p->left, "", true);
        printTreeRecursive(p->right, "", false);
    }

    void displayFrequencies()
    {
        for (size_t i = 0; i < 256; i++)
        {
            if (map[i] == 0) continue;
            if (i == '\n') cout<<"\\n: ";
            else if (i == '\t') cout<<"\\t: ";
            else if (i == '\r') cout<<"\\r: ";
            else if (i == ' ') cout<<"[space]: ";
            else cout<<(char)i<<": ";
            cout<<map[i]<<endl;
        }
    }
    void displayCodesRecursive(huffNode *p, string path)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) 
        {
            if (p->myChar == '\n') cout<<"\\n";
            else if (p->myChar == '\t') cout<<"\\t";
            else if (p->myChar == '\r') cout<<"\\r";
            else if (p->myChar == ' ') cout<<"[space]";
            else cout<<p->myChar;
            cout<<": "<<path<<endl;
        }
        displayCodesRecursive(p->left,path.append("0"));
        path.pop_back();
        displayCodesRecursive(p->right,path.append("1"));
    }
};
//================================================================
//=======[ENCODING FUNCTIONS AND MODELS]==========================
//================================================================
//--------[BIT WRITER MODULE]----------------------
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
//--------[FILE HUF ENCODING MODULE]----------------------------
class EncoderModule
{
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
//================================================================
//=======[DECODING FUNCTIONS AND MODELS]==========================
//================================================================
//--------[INTEGRITY ENUMS]----------------------
enum IntegrityStatus {
    MATCH_OK = 0,
    ERROR_FILE_NOT_FOUND,
    ERROR_TREE_BUILD_FAILED,
    ERROR_LENGTH_MISMATCH,
    ERROR_BYTE_MISMATCH,
    ERROR_BITSTREAM_CORRUPT
};
//--------[REPORT STRUCT FOR REPORT DATA]----------------------------
struct IntegrityReport {
    IntegrityStatus status;
    unsigned long totalBytesVerified = 0;
    unsigned long totalBitsProcessed = 0;
    long mismatchIndex = -1;       // Position in the stream where it failed (-1 if none)
    char expectedChar = '\0';      // What the plaintext file had
    char actualChar = '\0';        // What the decoded bitstream produced
};
//--------[BIT READER MODULE]----------------------------
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
//--------[HUF-ENCODED DECODER MODULE]----------------------------
class DecoderModule
{
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
//================================================================
//=======[APP CONTROLLER AND BACKEND]=============================
//================================================================
//--------[APP CLASS]----------------------------
class HuffmanManager
{
private:
    string activeTxt;
    string activeHuf;
    bool txtLoaded;
    bool hufLoaded;
    HuffmanTree *currentTree;
    IntegrityStatus integrityStatus;
    bool isChecked;

public:
    HuffmanManager()
    {
        activeTxt = "";
        activeHuf = "";
        txtLoaded = false;
        hufLoaded = false;
        currentTree = NULL;
        integrityStatus = ERROR_FILE_NOT_FOUND;
        isChecked = false;
    };
//--------[GETTERS]----------------------------
    bool isTxtLoaded(){ return txtLoaded; }
    bool isHufLoaded(){ return hufLoaded; }
    bool isPairChecked(){ return isChecked; }
    IntegrityStatus getLastResult() { return integrityStatus; }
    string getTxtFilename(){ return activeTxt; }
    string getHufFilename(){ return activeHuf; }
//--------[FILE LOADERS]----------------------------
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
//--------[DISPLAY TEXT IN RAM]----------------------------
    void displayText(bool isPlain)
    {
        if (isPlain) currentTree->printHeatmappedText();
        else
        {
            DecoderModule app;
            string temploc = ".\\Compressed\\temp.txt";
            app.decodeFileAndSave(".\\Compressed\\"+activeHuf+".huf",temploc);
            HuffmanTree *temp = new HuffmanTree(temploc);
            temp->printHeatmappedText();
            delete temp;
            remove(temploc.c_str());
        }
    }
//--------[DISPLAY CHARACTER FREQUENCIES]----------------------------
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
//--------[DISPLAY TREE]----------------------------
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
//--------[SAVE .TXT AS .HUF]----------------------------
    bool executeCompression(string outName)
    {
        string fmtOutName;
        if (outName == "NONE") fmtOutName = ".\\Compressed\\"+activeTxt+".huf";
        else fmtOutName = ".\\Compressed\\"+outName+".huf";

        EncoderModule app;
        app.compressFile(".\\Plaintexts\\"+activeTxt+".txt", fmtOutName, *currentTree);
        return true;
    }
//--------[SAVE .HUF AS .TXT]----------------------------
    bool executeDecompression(string outName)
    {
        string fmtOutName;
        if (outName == "NONE") fmtOutName = ".\\Plaintexts\\"+activeHuf+".txt";
        else fmtOutName = ".\\Plaintexts\\"+outName+".txt";

        DecoderModule app;
        return app.decodeFileAndSave(".\\Compressed\\"+activeHuf+".huf",fmtOutName);
    }
//--------[VERIFY IF FILE PAIRS ARE SAME CONTENT]----------------------------
    void verifyPairs()
    {
        if (!(isTxtLoaded() && isHufLoaded())) {
            isChecked = false;
            return;
        }
        DecoderModule app;
        integrityStatus = app.verifyStreamIntegrity(".\\Plaintexts\\"+activeTxt+".txt",".\\Compressed\\"+activeHuf+".huf");
        isChecked = true;
    }
//--------[COMPRESSION STATS]----------------------------
    void getCompressionStats(unsigned long &origSize, unsigned long &compSize, double &savingsPercent)
    {
        string txtPath = ".\\Plaintexts\\" + activeTxt + ".txt";
        string hufPath = ".\\Compressed\\" + activeHuf + ".huf";

        ifstream txtFile(txtPath, ios::binary | ios::ate);
        if (!txtFile.is_open()) return;
        origSize = txtFile.tellg();
        txtFile.close();

        ifstream hufFile(hufPath, ios::binary | ios::ate);
        if (!hufFile.is_open()) return;
        compSize = hufFile.tellg();
        hufFile.close();


        if (origSize == 0) savingsPercent = 0.0;
        else savingsPercent = (1.0 - (double)compSize / (double)origSize) * 100.0;
    }
//--------[REPORT METHOD]----------------------------
    IntegrityReport runIntegrityReport()
    {
        DecoderModule app;
        return app.runDetailedDiagnostics(".\\Plaintexts\\" + activeTxt + ".txt",".\\Compressed\\" + activeHuf + ".huf");
    }
};
//==================================================================
//=======[MAIN ENTRY POINT AND UI ACTIONS]==========================
//==================================================================
//--------[COLOR MACROS]----------------------------
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[95m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define GRAY    "\033[90m"

//--------[status helper for UI]----------------------------
string getStatusString(bool txt, bool huf, bool checked, IntegrityStatus res)
{
    if (!txt || !huf) return "Pending (Load both files)";
    switch(res)
    {
        case MATCH_OK:                  return "Verified Match [OK]";
        case ERROR_LENGTH_MISMATCH:     return "FAILED: Length Mismatch";
        case ERROR_BYTE_MISMATCH:       return "FAILED: Byte Content Error";
        case ERROR_BITSTREAM_CORRUPT:   return "FAILED: Corrupt Bitstream";
        default:                        return "FAILED: Unknown Error";
    }   
}

int displayMenu(HuffmanManager app)
{
    int choice;
    cout << "\033[H\033[2J";

    cout << CYAN << BOLD;
    cout << "========================================" << endl;
    cout << "   Huffpress! - Encode & Decode Suite   " << endl;
    cout << "========================================" << RESET << endl;

    cout << WHITE << " Active .txt: " << RESET
         << (app.isTxtLoaded() ? GREEN + app.getTxtFilename() + ".txt" + RESET : GRAY "[None]" RESET) << endl;

    cout << WHITE << " Active .huf: " << RESET
         << (app.isHufLoaded() ? GREEN + app.getHufFilename() + ".huf" + RESET : GRAY "[None]" RESET) << endl;

    cout << WHITE << " Status:      " << RESET
         << getStatusString(app.isTxtLoaded(), app.isHufLoaded(), app.isPairChecked(), app.getLastResult()) << endl;

    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << YELLOW << BOLD << " [1] " << RESET << "Load Plaintext (.txt)" << endl;
    cout << YELLOW << BOLD << " [2] " << RESET << "Load Compressed (.huf)" << endl;

    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << CYAN << BOLD << "   Payload / Content Preview & Stats    " << RESET << endl;
    cout << BLUE << "----------------------------------------" << RESET << endl;

    if (!(app.isTxtLoaded() || app.isHufLoaded())) cout << GRAY;
    cout << " [3] Read Loaded Text (Inspect in RAM)" << RESET << endl;
    if (!(app.isTxtLoaded() || app.isHufLoaded())) cout << GRAY;
    cout << " [4] Run Character Frequency Analysis" << RESET << endl;
    if (!(app.isTxtLoaded() || app.isHufLoaded())) cout << GRAY;
    cout << " [5] View Huffman Tree" << RESET << endl;
    if (!(app.isTxtLoaded() || app.isHufLoaded())) cout << GRAY;
    cout << " [6] View Generated Codes" << RESET << endl;

    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << CYAN << BOLD << "          Save As .txt / .huf           " << RESET << endl;
    cout << BLUE << "----------------------------------------" << RESET << endl;

    if (!app.isTxtLoaded()) cout << GRAY;
    else cout << GREEN;
    cout << " [7] Compress File (Load .txt first)" << RESET << endl;

    if (!app.isHufLoaded()) cout << GRAY;
    else cout << GREEN;
    cout << " [8] Decompress File (Load .huf first)" << RESET << endl;

    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << CYAN << BOLD << "         Statistics & Integrity         " << RESET << endl;
    cout << BLUE << "----------------------------------------" << RESET << endl;

    if (!(app.isTxtLoaded() && app.isHufLoaded())) cout << GRAY;
    else cout << MAGENTA;
    cout << " [9]  View Compression Ratio & Stats" << RESET << endl;

    if (!(app.isTxtLoaded() && app.isHufLoaded())) cout << GRAY;
    else cout << MAGENTA;
    cout << " [10] Run Content Integrity Check" << RESET << endl;

    cout << BLUE << "----------------------------------------" << RESET << endl;
    cout << RED << BOLD << " [0] Exit" << RESET << endl;
    cout << BLUE << "========================================" << RESET << endl;

    cout << CYAN << BOLD << "Enter choice: " << RESET;
    cin >> choice;
    return choice;
}

int main()
{
    HuffmanManager app;
    string field;
    int choice;
    int opt;

    while (true)
    {
        app.verifyPairs();
        choice = displayMenu(app);

        switch (choice)
        {
        case 1:
        case 2:
            cout << CYAN << "Enter the "
                 << ((choice == 1) ? ".txt" : ".huf")
                 << " filename (Ensure it is in "
                 << ((choice == 1) ? "./Plaintexts/" : "./Compressed/")
                 << "): " << RESET;

            cin >> field;

            if ((choice == 1) ? app.loadPlaintext(field) : app.loadCompressed(field))
                printf(GREEN "Successfully loaded %s!\n" RESET, field.c_str());
            else
                printf(RED "Failed to load %s! Check if file exists in respective directory.\n" RESET, field.c_str());

            break;

        case 3:
        case 4:
        case 5:
        case 6:
            bool isPlain;

            if (app.isTxtLoaded() && app.isHufLoaded())
            {
                cout << YELLOW << "Display for loaded .txt or .huf?\n"
                     << "[1] .txt\n"
                     << "[2] .huf\n"
                     << "Enter Option: " << RESET;
                cin >> opt;

                if (opt == 1) isPlain = true;
                else if (opt == 2) isPlain = false;
                else goto invalid;
            }
            else if (app.isTxtLoaded()) isPlain = true;
            else if (app.isHufLoaded()) isPlain = false;
            else continue;

            cout << "\033[H\033[2J";
            cout << BLUE << "----------------------------------------" << endl;
            cout << CYAN << BOLD << "         Displaying Statistics          " << RESET << endl;
            cout << BLUE << "----------------------------------------" << RESET << endl;

            switch (choice)
            {
            case 3:
                app.displayText(isPlain);
                break;

            case 4:
                app.displayFrequencies(isPlain);
                break;

            case 5:
                app.displayTree(isPlain);
                break;

            case 6:
                app.displayCodes(isPlain);
                break;
            }

            cout << BLUE << "----------------------------------------" << RESET << endl;
            break;

        case 7:
        case 8:
            cout << CYAN << "Enter the "
                 << ((choice == 7) ? "compressed" : "plaintext")
                 << " filename (type NONE to use source filename): "
                 << RESET;

            cin >> field;

            (choice == 7) ? app.executeCompression(field) : app.executeDecompression(field);

            cout << GREEN << "------------------------------------------------" << endl;
            cout << "Successfully Saved file. Check respective Folder." << endl;
            cout << "------------------------------------------------" << RESET << endl;
            break;

        case 9:
            if (!(app.isTxtLoaded() && app.isHufLoaded()))
            {
                cout << RED << "[Error] Load both files first.\n" << RESET;
                break;
            }

            unsigned long orig, comp;
            double savings;

            app.getCompressionStats(orig, comp, savings);

            cout << MAGENTA << "\n========================================" << endl;
            cout << BOLD << "       COMPRESSION STATISTICS REPORT    " << RESET << endl;
            cout << MAGENTA << "========================================" << RESET << endl;

            cout << WHITE << " Original Size:      " << CYAN << orig << " bytes" << endl;
            cout << WHITE << " Compressed Size:    " << CYAN << comp << " bytes" << endl;
            cout << WHITE << " Space Saved:        " << GREEN << savings << "%" << endl;
            cout << WHITE << " Ratio:              " << YELLOW
                 << (double)orig / (comp > 0 ? comp : 1) << ":1" << endl;

            cout << MAGENTA << "========================================" << RESET << endl;
            break;

        case 10:
        {
            if (!(app.isTxtLoaded() && app.isHufLoaded()))
            {
                cout << RED << "[Error] Load both files first." << RESET << endl;
                break;
            }

            cout << MAGENTA << "\n========================================" << endl;
            cout << BOLD << "       STREAM INTEGRITY DIAGNOSTIC      " << RESET << endl;
            cout << MAGENTA << "========================================" << RESET << endl;

            IntegrityReport rep = app.runIntegrityReport();

            if (rep.status == MATCH_OK)
            {
                cout << GREEN << " [PASSED] 100% Lossless Match Verified!" << RESET << endl;
                cout << GREEN << "----------------------------------------" << RESET << endl;
                cout << WHITE << " Bytes Successfully Decoded: " << GREEN
                     << rep.totalBytesVerified << " bytes" << endl;
                cout << WHITE << " Total Bits Parsed Stream:   " << GREEN
                     << rep.totalBitsProcessed << " bits" << endl;
                cout << WHITE << " Structural Integrity:       " << GREEN
                     << "PASS (0 Mismatches)" << RESET << endl;
            }
            else
            {
                cout << RED << " [FAILED] Integrity Check Failed!" << RESET << endl;
                cout << RED << "----------------------------------------" << RESET << endl;
                cout << WHITE << " Error Code:                 " << RED
                     << rep.status << RESET << endl;
                cout << WHITE << " Failed at Byte Index:       " << RED
                     << rep.mismatchIndex << RESET << endl;

                if (rep.status == ERROR_BYTE_MISMATCH)
                {
                    cout << WHITE << " Expected Character:         " << YELLOW
                         << "'" << rep.expectedChar << "' (ASCII "
                         << (int)rep.expectedChar << ")" << RESET << endl;

                    cout << WHITE << " Decoded Character:          " << YELLOW
                         << "'" << rep.actualChar << "' (ASCII "
                         << (int)rep.actualChar << ")" << RESET << endl;
                }
                else if (rep.status == ERROR_LENGTH_MISMATCH)
                {
                    cout << YELLOW
                         << " Reason: Stream lengths diverged (premature EOF)."
                         << RESET << endl;
                }
            }

            cout << MAGENTA << "========================================" << RESET << endl;
            break;
        }

        case 0:
            goto exit_menu;
            break;

        default:
        invalid:
            cout << RED << "Invalid input! Please pick from valid options..." << RESET << endl;
            break;
        }

        cin.ignore();
        cin.get();
    }

exit_menu:
    cout << GREEN << "Happy Coding! Exiting program..." << RESET << endl;
    return 0;
}