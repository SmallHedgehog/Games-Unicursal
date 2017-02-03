#ifndef UNICURSALALGORITHM_H
#define UNICURSALALGORITHM_H

#include <QList>
#include <QMultiMap>
#include <QSet>
#include <QPoint>
#include <QQueue>

// 点的属性
struct vertex
{
    int Id;                 // 点的编号
    QSet<int> Neighbours;   // 与其连通的点编号的集合
    QPoint Positon;         // 在界面上的坐标
};
typedef struct vertex VERTEX;

// 线的属性
struct line
{
    int VertexStart;        // 线端点的开始点的编号
    int VertexEnd;          // 线端点的结束点的编号
};
typedef struct line LINE;

class UnicursalAlgorithm
{
public:
    UnicursalAlgorithm();
    ~UnicursalAlgorithm();

public:
    bool Result();                        // 遍历所有点求解
    bool Solution(int VertexStart);       // 回溯解决一笔画图形问题(从点的编号为VertexStart开始)
    bool IsCanResult(bool* IsTOdd);       // 求解当前图形是否有解(其中需要计算出顶点的个数为奇数的顶点Id)
    void addVertex(QPoint& Pos);          // 增加一个点
    int getVertexIndexById(int Id) const;
    void removeVertex(int Index);         // 移除一个点
    void setLineCount();                  // 设置图形中连线的总数
    int getLineCount() const;             // 获取图形中连线的总数
    int getVertexOddId() const;
    void clear();
    void clearRoutes();

private:
    bool hasPassed(int Vertex1, int Vertex2) const;     // 判断Vertex1和Vertex2点的连线是否已经走过
    void addRoute(int Vertex1, int Vertex2);            // 记录走过的路线
    void removeRoute(int Vertex1, int Vertex2);         // 回溯时需要移除已经走过的路线

public:
    QList<VERTEX> Vertexs;             // 所有的点
    QMultiMap<int, int> lines;         // 所有的连线(使用点的编号)
    QList<LINE> Routes;                // 有解时的所有连线

private:
    int LineCount;                     // 连线总数
    QMultiMap<int, int> PassedRoutes;  // 已走过的路线

    int nSerial;
    // 如果当前图形有且只有两个奇数点(记录这两个顶点的Id)
    int VertexOddId1;
    int VertexOddId2;
};

#endif // UNICURSALALGORITHM_H
