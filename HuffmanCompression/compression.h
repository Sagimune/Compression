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
    int weight;
    Node *L,*R;
    bool leaf;
};
struct CMP
{
    bool operator() (Node* A,Node *B)
    {
        return A->weight==B->weight ? A->C > B->C : A->weight > B->weight;
    }
};

class Compression : public QMainWindow
{
private:
    QMap<unsigned char,int> weightmap;
    std::priority_queue<Node*,std::vector<Node*>,CMP> container;
    QMap<unsigned char,std::string> passwordmap;
public:
    Compression();
    void Zip(QString path);
    void UnZip(QString path);
protected:
    void DEL(Node* root);
    void Weightmap_Init(QFile& in);
    void Container_Init();
    void HuffmanTree_Init();
    void ZipPassword_Init(Node *root, std::string password);
};

#endif // COMPRESSION_H
