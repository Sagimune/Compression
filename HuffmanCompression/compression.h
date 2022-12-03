#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QVector>
#include <QMap>
#include <QFile>
#include <queue>

struct Node
{
    QChar C;
    int weight;
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

class Compression : public QMainWindow
{
private:
    QMap<QChar,int> weightmap;
    std::priority_queue<Node*,std::vector<Node*>,CMP> container;
    QMap<QChar,QString> passwordmap;
public:
    Compression();
    void Zip(QString path);
    void UnZip(QString path);
protected:
    void DEL(Node* root);
    void Weightmap_Init(QFile& in);
    Node *MakeNewNode(QChar ch);
    void Container_Init();
    void HuffmanTree_Init();
    void ZipPassword_Init(Node *root, QString password);
};

#endif // COMPRESSION_H
