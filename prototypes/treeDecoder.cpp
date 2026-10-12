#include <iostream>
#include <stdio.h>

using namespace std;

class BTree{
    private:
//===== tree node unit ===============================================
    struct tNode
    {
        char data;
        tNode *left;
        tNode *right;
        tNode(char c)
        {
            data = c;
            left = NULL;
            right = NULL;
        }
        tNode()
        {
            data = '\0';
            left = NULL;
            right = NULL;
        }
    };

//===== Helper Functions =========================================
    void addNode(tNode *&p)
    {
        char c;
        int isInternal;
        if (p == NULL)
        {
            printf("start root? (1 to return): ", p);
            cin>>isInternal;
            if (isInternal) return;
            p = new tNode();
            addNode(p);
            return;
        }
        printf("create %p's left internal child? (1 to create leaf instead): ", p);
        cin>>isInternal;
        if (isInternal != 1)
        {
            p->left = new tNode();
            addNode(p->left);
        }
        else
        {
            printf("enter %p's left child char: ", p);
            cin>>c;
            p->left = new tNode(c);
        }
        printf("create %p's right internal child? (1 to create leaf instead): ", p);
        cin>>isInternal;
        if (isInternal != 1)
        {
            p->right = new tNode();
            addNode(p->right);
        }
        else
        {
            printf("enter %p's right child char: ", p);
            cin>>c;
            p->right = new tNode(c);
        }
    }
    void preOrder(tNode *p)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<p->data<<" -> ";
        else cout<<"()"<<" -> ";
        preOrder(p->left);
        preOrder(p->right);
    }
    void encodeTree(tNode *p)
    {
        if (p == NULL) return;
        if (p->left == NULL && p->right == NULL) cout<<1<<p->data;
        else cout<<"0";
        encodeTree(p->left);
        encodeTree(p->right);
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
    cout<<"=== Huffman tree decoder! ==="<<endl;
    cout<<"[1] Display decoded Tree"<<endl;
    cout<<"[2] Display encoded Tree"<<endl;
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
            cout<<"Tree decoded using Pre Order: \n";
            tree.printPreOrder();
            break;
        case 2:
            cout<<"Tree Encoded: \n";
            tree.printEncode();
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
