#include "compression.h"

int StringToBits(QString binarystring)
{
    int sum = 0, p = 1;
    for(int i = 0; i < binarystring.size(); ++i)
    {
        if(binarystring[i] == '1')
            sum |= p;
        p <<= 1;
    }
    return sum;
}

QString BitsToString(int Bits)
{
    QString binarystring = "";
    while(Bits)
    {
        binarystring += (Bits&1 ? '1' : '0');
        Bits >>= 1;
    }
    while(binarystring.size()<8) binarystring += '0';
    return binarystring;
}
Compression::Compression()
{

}

void Compression::DEL(Node* x)
{
    if(x==NULL) return;
    DEL(x->L),DEL(x->R);
    delete x;
}

void Compression::Weightmap_Init(QFile& in)
{
    QByteArray a;
    while(!in.atEnd())
    {
        a = in.read(1024);
        QString b = a;
        for(int i = 0; i < b.size(); ++i)
        {
            weightmap[b[i]]++;
        }
    }
}

Node* Compression::MakeNewNode(QChar ch)
{
    Node *Hnode = new Node;
    Hnode->C = ch;
    Hnode->weight = weightmap[ch];
    Hnode->L = Hnode->R = NULL;
    Hnode->leaf = 1;
    return Hnode;
}

void Compression::Container_Init()
{
    for(QMap<QChar,int>::iterator it = weightmap.begin(); it != weightmap.end(); it++)
    {
        Node *NewNode = MakeNewNode(it.key());
        container.push(NewNode);
    }
}

void Compression::HuffmanTree_Init()
{
    while(container.size()>1)
    {
        Node *A = container.top(); container.pop();
        Node *B = container.top(); container.pop();

        Node *Hnode = new Node;
        Hnode->leaf = 0;
        Hnode->C = 0;
        Hnode->L = A, Hnode->R = B;
        Hnode->weight = A->weight + B->weight;

        container.push(Hnode);
    }
}
