#include <iostream>
#include <stdio.h>

using namespace std;

void char_to_bit(char buf)
{
    for (int j = 7; j >= 0; j--)
    {
        int b = (buf >> j) & 1;
        cout<<b;
    }
    cout<<endl;
}

char shiftAddBit(char buf, int bit)
{
    return (buf << 1) | bit;
}

int main(int argc, char const *argv[])
{
    while (true)
    {
        char buf = 0;
        char c;
        int bit;
        cout<<"New Character"<<endl;
        for (int i = 0; i < 8; i++)
        {
            cout<<"1 or 0?: ";
            cin>>c;
            if (c == '1') buf = shiftAddBit(buf, 1);
            else buf = shiftAddBit(buf, 0);
            char_to_bit(buf);
        }
        cout<<"buf in plaintext: "<<buf<<endl;
    }
    return 0;
}
