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
    cout<<sizeof(unsigned char)<<endl;
    cout<<sizeof(char)<<endl;
    cout<<sizeof(unsigned int)<<endl;
    cout<<sizeof(int)<<endl;
    cout<<sizeof(bool)<<endl;
    cout<<sizeof(codeNode)<<endl;
    return 0;
}
