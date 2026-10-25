#include <iostream>
#include <string>
#include "HuffmanManager.hpp"

using namespace std;

int displayMenu(HuffmanManager app)
{
    int choice;
    // Dynamic UI menu rendering based on app states...
    cout<<"\033[H\033[2J";
    cout<<"========================================"<<endl;
    cout<<"    HUFFMAN SUITE - PHASED ARCHITECTURE "<<endl;
    cout<<"========================================"<<endl;
    cout<<" Active .txt: " << (app.isTxtLoaded() ? app.getTxtFilename() : "[None]")<<endl;
    cout<<" Active .huf: " << (app.isHufLoaded() ? app.getHufFilename() : "[None]")<<endl;
    cout<<" Status:      " << (app.isPairVerified() ? "Verified Match [OK]" : "Pending")<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<" 1. Load Plaintext (.txt)"<<endl;
    cout<<" 2. Load Compressed (.huf)"<<endl;
    cout<<"----------------------------------------"<<endl;
    cout<<" 3. Read Loaded Text (Inspect in RAM)"<<endl;
    cout<<" 4. Run Character Frequency Analysis"<<endl;
    cout<<" 5. View Huffman Tree & Generated Codes"<<endl;
    cout<<"----------------------------------------"<<endl;
    if (app.isTxtLoaded()) cout<< " 6. Compress Plaintext -> .huf"<<endl;
    else cout<<"\e[0;38;5;240m"<<" 6. [LOCKED] Compress File (Load .txt first)"<<"\x1b[0;39m"<<endl;

    if (app.isHufLoaded()) cout<<" 7. Decompress .huf -> Plaintext"<<endl;
    else cout<<"\e[0;38;5;240m"<<" 7. [LOCKED] Decompress File (Load .huf first)"<<"\x1b[0;39m"<<endl;

    if (app.isTxtLoaded() && app.isHufLoaded())
    {
        cout<<" 8. View Compression Ratio & Stats"<<endl;
        cout<<" 9. Run Content Integrity Check"<<endl;
    }
    else
    {
        cout<<"\e[0;38;5;240m"<<" 8. [LOCKED] View Compression Ratio & Stats (Requires both files)"<<endl;
        cout<<"\e[0;38;5;240m"<<" 9. [LOCKED] Content Integrity Check (Requires both files)"<<"\x1b[0;39m"<<endl;
    }
    cout<<"----------------------------------------"<<endl;
    cout<<" 0. Exit"<<endl;
    cout<<"========================================"<<endl;
    cout<<"Enter choice: ";
    cin>>choice;
    return choice;
}

int main() {
    HuffmanManager app;
    string field;
    while (true)
    {
        switch (displayMenu(app))
        {
        case 1:
            cout<<"Enter the .txt filename (Ensure it is in ./Plaintexts/): ";
            cin>>field;
            if (app.loadPlaintext(field)) printf("Succesfully loaded %s!\n", field.c_str());
            else printf("Failed to load %s! check if file exists in respective directory.\n", field.c_str());
            break;
        case 2:
            cout<<"Enter the .huf filename (Ensure it is in ./Compressed/): ";
            cin>>field;
            if (app.loadCompressed(field)) printf("Succesfully loaded %s!\n", field.c_str());
            else printf("Failed to load %s! check if file exists in respective directory.\n", field.c_str());
            break;
        case 0:
            goto exit_menu;
            break;
        default:
            break;
        }
        cin.ignore();
        cin.get();
    }
    exit_menu:
    cout<<"Happy Coding! exiting program..."<<endl;
    return 0;
}