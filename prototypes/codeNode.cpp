#include <iostream>
#include <stdio.h>

using namespace std;

struct codeNode
{
    int myPadding;
    char myCode;
    bool isActive;
    codeNode()
    {
        isActive = false;
        myPadding = 0;
        myCode = 0;
    }
};

int main(int argc, char const *argv[])
{
    cout<<sizeof(codeNode);
    return 0;
}
