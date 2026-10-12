#include <iostream>
#include <stdio.h>

using namespace std;

class BTree{
    private:
//===== tree node unit ===============================================
    struct tNode
    {
        char data;
        int freq;
        tNode *left;
        tNode *right;
        tNode(char c, int frq)
        {
            data = c;
            freq = frq;
            left = NULL;
            right = NULL;
        }
        tNode(int frq)
        {
            data = '\0';
            freq = frq;
            left = NULL;
            right = NULL;
        }
    };

//===== Helper Functions =========================================
    void addNode(tNode *&p)
    {
        char c;
        int frq;
        if (p == NULL)
        {
            cout<<"enter root frq (-1 to return): ";
            cin>>frq;
            p = new tNode(frq);
            addNode(p);
            return;
        }
        printf("enter %d's left child frq (-1 to create leaf node and go to right): ", p->freq);
        cin>>frq;
        if (frq != -1)
        {
            p->left = new tNode(frq);
            addNode(p->left);
        }
        else
        {
            printf("enter %d's left child frq: ", p->freq);
            cin>>frq;
            printf("enter %d's left child char: ", p->freq);
            cin>>c;
            p->left = new tNode(c, frq);
        }
        printf("enter %d's right child value (-1 to create leaf node and return): ", p->freq);
        cin>>frq;
        if (frq != -1)
        {
            p->right = new tNode(frq);
            addNode(p->right);
        }
        else
        {
            printf("enter %d's right child frq: ", p->freq);
            cin>>frq;
            printf("enter %d's right child char: ", p->freq);
            cin>>c;
            p->right = new tNode(c, frq);
        }
    }
    void encodeTree(tNode *p)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<1<<p->data;
        else cout<<"0";
        encodeTree(p->left);
        encodeTree(p->right);
    }
    void preOrder(tNode *p)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<p->data<<" -> ";
        else cout<<"()"<<" -> ";
        preOrder(p->left);
        preOrder(p->right);
    }
//===== BTree variable ===========================================
    tNode *root;

    public:

    BTree() { root = NULL; }

    void BuildTree()
    {
        addNode(root);
    }
    void printPreOrder()
    {
        preOrder(root);
        cout<<"\e[4D    ";
    }
    void printEncode()
    {
        encodeTree(root);
    }
};

int menu()
{
    int opt;
    printf("\033[H\033[2J");
    cout<<"=== Huffman tree encoder! ==="<<endl;
    cout<<"[1] Display encoded Tree"<<endl;
    cout<<"[2] Display Tree using Pre Order"<<endl;
    cout<<"[3] Create Simple Binary Tree"<<endl;
    cout<<"[0] Quit"<<endl;
    cout<<endl<<"Choose Option: ";
    cin>>opt;
    return opt;
}

int main(int argc, char const *argv[])
{
    BTree tree;
    while (true)
    {
        switch (menu())
        {
        case 1:
            cout<<"Tree Encoded using Pre Order: \n";
            tree.printEncode();
            break;
        case 2:
            cout<<"Tree Traversal using Pre Order: \n";
            tree.printPreOrder();
            break;
        case 3:
            tree.BuildTree();
            break;
        case 0:
            goto exit_menu;
            break;
        default:
            break;
        }
        cout<<"\n[Press Enter to Continue]";
        cin.ignore();
        cin.get();
    }
    exit_menu:
    return 0;
}

