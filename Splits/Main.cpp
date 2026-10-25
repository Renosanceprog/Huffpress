#include <iostream>
#include <string>
#include "HuffmanManager.hpp"

using namespace std;

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
    cout<<"\033[H\033[2J";
    cout<<"========================================"<<endl;
    cout<<"   Huffpress! - Encode & Decode Suite   "<<endl;
    cout<<"========================================"<<endl;
    cout<<" Active .txt: " << (app.isTxtLoaded() ? app.getTxtFilename()+".txt" : "[None]")<<endl;
    cout<<" Active .huf: " << (app.isHufLoaded() ? app.getHufFilename()+".huf" : "[None]")<<endl;
    cout << " Status:      " << getStatusString(app.isTxtLoaded(), app.isHufLoaded(), app.isPairChecked(), app.getLastResult()) << endl;
    cout<<"----------------------------------------"<<endl;
    cout<<" [1] Load Plaintext (.txt)"<<endl;
    cout<<" [2] Load Compressed (.huf)"<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<"   Payload / Content  Preview & Stats   "<<endl;
    cout<<"----------------------------------------"<<endl;
    if (!(app.isTxtLoaded() || app.isHufLoaded())) cout<<"\e[0;38;5;240m";
        cout<<" [3] Read Loaded Text (Inspect in RAM)"<<endl;
        cout<<" [4] Run Character Frequency Analysis"<<endl;
        cout<<" [5] View Huffman Tree"<<endl;
        cout<<" [6] View Generated Codes";
        cout<<"\x1b[0;39m"<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<"          Save As  .txt / .huf          "<<endl;
    cout<<"----------------------------------------"<<endl;
    if (!app.isTxtLoaded()) cout<<"\e[0;38;5;240m";
    cout<<" [7] Compress File (Load .txt first)";
    cout<<"\x1b[0;39m"<<endl;
    if (!app.isHufLoaded()) cout<<"\e[0;38;5;240m";
    cout<<" [8] Decompress File (Load .huf first)";
    cout<<"\x1b[0;39m"<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<"Stastistics & Integrity  (Requires both)"<<endl;
    cout<<"----------------------------------------"<<endl;
    if (!(app.isTxtLoaded() && app.isHufLoaded())) cout<<"\e[0;38;5;240m";
    cout<<" [9]  View Compression Ratio & Stats"<<endl;
    cout<<" [10] Run Content Integrity Check";
    cout<<"\x1b[0;39m"<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<" [0] Exit"<<endl;
    cout<<"========================================"<<endl;
    cout<<"Enter choice: ";
    cin>>choice;
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
            cout<<"Enter the "<<((choice == 1) ? ".txt" : ".huf")<<" filename (Ensure it is in "<<((choice == 1) ? "./Plaintexts/" : "./Compressed/")<<"): ";
            cin>>field;
            if ((choice == 1) ? app.loadPlaintext(field) : app.loadCompressed(field)) printf("Succesfully loaded %s!\n", field.c_str());
            else printf("Failed to load %s! check if file exists in respective directory.\n", field.c_str());
            break;
        case 3:
        case 4:
        case 5:
        case 6:
            bool isPlain;
            if (app.isTxtLoaded() && app.isHufLoaded())
            {
                cout<<"Display for loaded .txt or .huf?\n[1] .txt\n[2] .huf\nEnter Option: ";
                cin>>opt;
                if (opt == 1) isPlain = true;
                else if (opt == 2) isPlain = false;
                else goto invalid;
            }
            else if (app.isTxtLoaded()) isPlain = true;
            else if (app.isHufLoaded()) isPlain = false;
            else continue;
            cout<<"\033[H\033[2J";
            cout<<"----------------------------------------"<<endl;
            cout<<"         Displaying  Statistics         "<<endl;
            cout<<"----------------------------------------"<<endl;
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
            cout<<"----------------------------------------"<<endl;
            break;
        case 7:
        case 8:
            cout<<"Enter the "<<((choice == 7) ? "compressed" : "plaintext")<<" filename (type NONE to use source filename): ";
            cin>>field;
            (choice == 7) ? app.executeCompression(field) : app.executeDecompression(field);
            cout<<"------------------------------------------------"<<endl;
            cout<<"Succesfully Saved file. Check respective Folder."<<endl;
            cout<<"------------------------------------------------"<<endl;
            break;
        case 9:
                    if (!(app.isTxtLoaded() && app.isHufLoaded()))
                    {
                        cout << "[Error] Load both files first.\n";
                        break;
                    }
                    unsigned long orig, comp;
                    double savings;
                    app.getCompressionStats(orig, comp, savings);
                    
                    cout<<"\n========================================"<<endl;
                    cout<<"       COMPRESSION STATISTICS REPORT    "<<endl;
                    cout<<"========================================"<<endl;
                    cout<<" Original Size:      " << orig << " bytes"<<endl;
                    cout<<" Compressed Size:    " << comp << " bytes"<<endl;
                    cout<<" Space Saved:        " << savings << "%"<<endl;
                    cout<<" Ratio:              " << (double)orig / (comp > 0 ? comp : 1) << ":1"<<endl;
                    cout<<"========================================"<<endl;
                    break;
    case 10:
    {
        if (!(app.isTxtLoaded() && app.isHufLoaded()))
        {
            cout<<"[Error] Load both files first."<<endl;
            break;
        }

        cout<<"\n========================================"<<endl;
        cout<<"       STREAM INTEGRITY DIAGNOSTIC      "<<endl;
        cout<<"========================================"<<endl;
        
        IntegrityReport rep = app.runIntegrityReport();

        if (rep.status == MATCH_OK)
        {
            cout<<" [PASSED] 100% Lossless Match Verified!"<<endl;
            cout<<"----------------------------------------"<<endl;
            cout<<" Bytes Successfully Decoded: " << rep.totalBytesVerified << " bytes"<<endl;
            cout<<" Total Bits Parsed Stream:   " << rep.totalBitsProcessed << " bits"<<endl;
            cout<<" Structural Integrity:       PASS (0 Mismatches)"<<endl;
        }
        else
        {
            cout<<" [FAILED] Integrity Check Failed!"<<endl;
            cout<<"----------------------------------------"<<endl;
            cout<<" Error Code:                 " << rep.status << ""<<endl;
            cout<<" Failed at Byte Index:       " << rep.mismatchIndex << ""<<endl;
            if (rep.status == ERROR_BYTE_MISMATCH)
            {
                cout<<" Expected Character:         '" << rep.expectedChar << "' (ASCII " << (int)rep.expectedChar << ")"<<endl;
                cout<<" Decoded Character:          '" << rep.actualChar << "' (ASCII " << (int)rep.actualChar << ")"<<endl;
            }
            else if (rep.status == ERROR_LENGTH_MISMATCH) cout<<" Reason:                     Stream lengths diverged (premature EOF)."<<endl;
        }
        cout<<"========================================"<<endl;
        break;
    }
        case 0:
            goto exit_menu;
            break;
        default:
            invalid:
            cout<<"invalid input! please pick from valid options..."<<endl;
            break;
        }
        cin.ignore();
        cin.get();
    }
    exit_menu:
    cout<<"Happy Coding! exiting program..."<<endl;
    return 0;
}