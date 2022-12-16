#include "compression.h"
#include <QDebug>

unsigned long long INTMAX = 4294967295ull;

Compression::Compression()
{

}

void Compression::DEL(Node* x)
{
    if(x==NULL) return;
    DEL(x->L),DEL(x->R);
    delete x;
    x = NULL;
}

void Compression::Weightmap_Init(QFile& in)
{
    QByteArray a;
    while(!in.atEnd())
    {
        a = in.read(1024);
        std::string b = a.toStdString();
        for(unsigned int i = 0; i < b.length(); ++i)
        {
            weightmap[b[i]]++;
        }
    }
}

void Compression::Container_Init()
{
    for(QMap<unsigned char,unsigned int>::iterator it = weightmap.begin(); it != weightmap.end(); it++)
    {
        Node *Hnode = new Node;
        Hnode->C = it.key();
        Hnode->weight = weightmap[it.key()];
        Hnode->L = Hnode->R = NULL;
        Hnode->leaf = 1;
        container.push(Hnode);
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
        Hnode->weight = (unsigned long long)A->weight + (unsigned long long)B->weight > INTMAX ? INTMAX : A->weight+B->weight;

        container.push(Hnode);
    }
}

void Compression::ZipPassword_Init(Node *x, std::string s)
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
        QMessageBox::warning(NULL,QString("警告"),QString("文件打开失败"));
        return;
    }
    if(openfile.size()==0)
    {
        QMessageBox::warning(NULL,QString("警告"),QString("文件为空"));
        return;
    }
    clock_t Begin = clock();
    Weightmap_Init(openfile);
    Container_Init();
    HuffmanTree_Init();
    ZipPassword_Init(container.top(),"");

    while(path.size()&&path.right(1)!='/') path.chop(1);
    QFile savefile(path+"haffuman.tmp");

    savefile.open(QIODevice::WriteOnly);
    QDataStream out(&savefile);
    out<<(int)0;
    out<<(weightmap.size()==256 ? 0 : weightmap.size());
    for(QMap<unsigned char,unsigned int>::iterator it = weightmap.begin(); it != weightmap.end(); it++)
        out<<it.key()<<it.value();

    openfile.seek(0);
    QByteArray a;
    unsigned char ch = 0;
    int num = 0;
    while(!openfile.atEnd())
    {
        a = openfile.read(1024);
        std::string b = a.toStdString();
        for(unsigned int i = 0; i < b.length(); ++i)
        {
            std::string S = passwordmap[b[i]];
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
    if(num)
    {
        out<<ch;
        savefile.seek(0);
        out<<num;
    }

    openfile.close();
    savefile.close();

    qDebug()<<"ZipTime:"<<QString::number(double(clock()-Begin)/CLOCKS_PER_SEC);

    DEL(container.top());
    container.pop();
    weightmap.clear();
    passwordmap.clear();
}

void Compression::UnZip(QString path)
{
    if(path.right(11)!=".huffmanzip")
    {
        QMessageBox::warning(NULL,QString("警告"),QString("此文件非哈夫曼压缩文件 "));
        return;
    }
    QFile openfile(path);
    if(!openfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL,QString("警告"),QString("文件打开失败 "));
        return;
    }
    if(openfile.size()==0)
    {
        QMessageBox::warning(NULL,QString("警告"),QString("文件为空"));
        return;
    }

    clock_t Begin = clock();
    QDataStream in(&openfile);
    int Num; in>>Num;
    int n; in>>n; if(n==0) n=256;
    for(int i = 1; i <= n; ++i)
    {
        unsigned char ch; in>>ch;
        in>>weightmap[ch];
    }
    Container_Init();
    HuffmanTree_Init();

    path.chop(11);
    path.insert(path.lastIndexOf('.'),"(NEW)");
    QFile savefile(path);
    savefile.open(QIODevice::WriteOnly);
    QDataStream out(&savefile);
    Node *x = container.top();
    while(!in.atEnd())
    {
        unsigned char ch; in>>ch;
        int tot = 8;
        if(in.atEnd()&&Num) tot = Num;
        for(int i = 0; i < tot; ++i)
        {
            x =((ch>>i)&1) ? x->R : x->L;
            if(x->leaf)
            {
                out<<x->C;
                x = container.top();
            }
        }
    }
    openfile.close();
    savefile.close();

    qDebug()<<"UnzipTime:"<<QString::number(double(clock()-Begin)/CLOCKS_PER_SEC);

    DEL(container.top());
    container.pop();
    weightmap.clear();
}
