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
void Compression::ZipPassword_Init(Node *x, QString s)
{
    if(x!=NULL&&x->leaf)
    {
        passwordmap[x->C] = s;
    }
    if(x->L!=NULL) ZipPassword_Init(x->L,s+'0');
    if(x->R!=NULL) ZipPassword_Init(x->R,s+'1');
}
void Compression::Zip(QString path)
{
    QFile openfile(path);
    if(!openfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL,QString("Warming"),QString("Can't open this file"));
        return;
    }
    Weightmap_Init(openfile);
    Container_Init();
    HuffmanTree_Init();
    ZipPassword_Init(container.top(),"");
    openfile.close();

    QFile savefile(path+".huffmanzip");
    savefile.open(QIODevice::WriteOnly);
    QDataStream out(&savefile);
    out<<(weightmap.size()==256 ? 0 : weightmap.size());
    for(QMap<QChar,int>::iterator it = weightmap.begin(); it != weightmap.end(); it++)
        out<<it.key()<<it.value();

    openfile.open(QIODevice::ReadOnly);
    QByteArray a;
    char ch = 0;
    int num = 0;
    while(!openfile.atEnd())
    {
        a = openfile.read(1024);
        QString b = a;
        for(int i = 0; i < b.size(); ++i)
        {
            QString S = passwordmap[b[i]];
            int Len = S.length();
            for(int j = 0; j < Len; ++j)
            {
                if(S[j]=='1') ch |= (1<<num);
                if((++num)==8)
                {
                    out<<ch;
                    num = ch = 0;
                }
            }
        }
    }
    if(num) out<<ch;

    openfile.close();
    savefile.close();

    DEL(container.top());
    container.pop();
    weightmap.clear();
    passwordmap.clear();
}
