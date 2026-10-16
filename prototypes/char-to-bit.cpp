#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, char const *argv[])
{
    char c;
    while (true)
    {
        for (int i = 7; i >= 0; i--)
        {
            int bit = (c >> i) & 1;
            cout<<bit;
        }
        cout<<endl;
        if (c == '`') break;
        cin>>c;
    }

    return 0;
}
