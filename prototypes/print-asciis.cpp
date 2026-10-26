#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

int main(int argc, char const *argv[])
{
    cout<<"  ";
    for (size_t i = 0; i < 10; i++) cout<<i+1<<" ";
    cout<<endl;
    
    for (size_t i = 32; i < 256; i++)
    {
        printf("%c ", (char)i);
        if (i % 10 == 0) printf("\n%-3d", (i+1)/10);
    }
    
    cout<<endl;
    char arr[3] = {(char)192,(char)196,(char)196};
    cout << arr;

    return 0;
}
