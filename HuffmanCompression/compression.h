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

class Compression : public QMainWindow
{
    Q_OBJECT
private:
    QMap<unsigned char,unsigned int> weightmap;
    std::priority_queue<Node*,std::vector<Node*>,CMP> container;
    QMap<unsigned char,std::string> passwordmap;
public:
    Compression();
    double Zip(QString path);
    double UnZip(QString path);
protected:
    void DEL(Node* root);
    void Weightmap_Init(QFile& in);
    void Container_Init();
    void HuffmanTree_Init();
    void ZipPassword_Init(Node *root, std::string password);
signals:
    void error();
    void mysignal(double per);
public slots:
};

#endif // COMPRESSION_H
