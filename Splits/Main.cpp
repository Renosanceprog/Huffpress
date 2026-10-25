#include <iostream>
#include <string>
#include "HuffmanManager.hpp"

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"
#define GRAY    "\033[90m"

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
    cout << CYAN << BOLD << "      Statistics & Integrity            " << RESET << endl;
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