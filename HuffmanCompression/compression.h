#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QVector>
#include <QMap>
#include <QFile>
#include <queue>
#include <string>
#include <ctime>

struct Node
{
    unsigned char C;
    unsigned int weight;
    Node *L,*R;
    bool leaf;
};
struct CMP
{
    bool operator() (Node* A,Node *B)
    {
        return A->weight > B->weight;
    }
};

struct ComparisonNode
{
    unsigned int Code,Len;
    unsigned char C;
};
class Comcmp
{
public:
    bool operator() (const ComparisonNode &a, const ComparisonNode &b)
    {
        return a.Len==b.Len ? a.C < b.C : a.Len<b.Len;
    }
};

class Compression : public QMainWindow
{
private:
    QMap<unsigned char,unsigned int> weightmap;
    std::priority_queue<Node*,std::vector<Node*>,CMP> container;
    QMap<unsigned char,std::string> passwordmap;
    QVector<ComparisonNode> Q;
public:
    Compression();
    void Zip(QString path);
    void UnZip(QString path);
protected:
    void DEL(Node* root);
    void Weightmap_Init(QFile& in);
    void Container_Init();
    void HuffmanTree_Init();
    void ZipPassword_Get(Node *root, unsigned int len);
    void ZipPassword_Init(bool type);
};

#endif // COMPRESSION_H
