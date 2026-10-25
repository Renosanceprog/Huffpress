#include <iostream>
#include <string>
#include "HuffmanManager.hpp"

using namespace std;

int displayMenu(HuffmanManager app)
{
    int choice;
    cout<<"\033[H\033[2J";
    cout<<"========================================"<<endl;
    cout<<"   Huffpress! - Encode & Decode Suite   "<<endl;
    cout<<"========================================"<<endl;
    cout<<" Active .txt: " << (app.isTxtLoaded() ? app.getTxtFilename()+".txt" : "[None]")<<endl;
    cout<<" Active .huf: " << (app.isHufLoaded() ? app.getHufFilename()+".huf" : "[None]")<<endl;
    cout<<" Status:      " << (app.isPairVerified() ? "Verified Match [OK]" : "Pending")<<endl;
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
            break;
        case 7:
        case 8:
            cout<<"Enter the "<<((choice == 7) ? "compressed" : "plaintext")<<" filename (type NONE to use source filename): ";
            cin>>field;
            (choice == 7) ? app.executeCompression(field) : app.executeDecompression(field);
            break;
        case 9:
            
            break;
        case 10:
            //code
            break;
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