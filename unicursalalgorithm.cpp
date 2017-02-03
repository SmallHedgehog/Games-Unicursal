#include "unicursalalgorithm.h"

#include <QDebug>

UnicursalAlgorithm::UnicursalAlgorithm()
{
    nSerial = 0;
}

int UnicursalAlgorithm::getVertexIndexById (int Id) const
{
    for (int i=0; i<Vertexs.size (); ++i){
        if (Vertexs[i].Id == Id){
            return i;
        }
    }

    return -1;
}

bool UnicursalAlgorithm::hasPassed (int Vertex1, int Vertex2) const
{
    return (PassedRoutes.contains(Vertex1, Vertex2) ||
            PassedRoutes.contains(Vertex2, Vertex1));
}

void UnicursalAlgorithm::addRoute (int Vertex1, int Vertex2)
{
    if (!hasPassed (Vertex1, Vertex2)){
        PassedRoutes.insert (Vertex1, Vertex2);
        LINE route = {
            Vertex1,
            Vertex2
        };
        Routes << route;
    }
}

void UnicursalAlgorithm::removeRoute (int Vertex1, int Vertex2)
{
    if (hasPassed (Vertex1, Vertex2)){
        PassedRoutes.remove(Vertex1, Vertex2);
        PassedRoutes.remove(Vertex2, Vertex1);

        if (!Routes.isEmpty ()){
            Routes.removeLast ();
        }
    }
}

void UnicursalAlgorithm::setLineCount ()
{
    LineCount = 0;
    QList<VERTEX>::ConstIterator it = Vertexs.constBegin ();
    for (; it!=Vertexs.constEnd (); ++it){
        LineCount += (*it).Neighbours.size ();
    }
    LineCount /= 2;
}

int UnicursalAlgorithm::getLineCount() const
{
    return LineCount;
}

bool UnicursalAlgorithm::Solution (int VertexStart)
{
    const VERTEX& vTex = Vertexs[getVertexIndexById (VertexStart)];

    // 获得vTex点的连接点
    QSet<int>::ConstIterator it;
    for (it=vTex.Neighbours.begin (); it!=vTex.Neighbours.end (); ++it){
        int NeighVertex = (*it);    // vTex的相连点
        if (hasPassed (vTex.Id, NeighVertex)){  // vTex点与NeighVertex点之间的连线已经走过
            continue;
        }else{  // vTex点与NeighVertex点之间的连线没有被走过
            // 记录走过的路线
            addRoute (vTex.Id, NeighVertex);
            // 如果已经走过的路线等于当前图形的路线总数->成功求解
            if (PassedRoutes.size () == LineCount){
                return true;
            }
        }
        // 已NeighVertex点为起点->求解
        bool result = Solution (NeighVertex);
        if (result){
            return true;
        }else{  // 无法走通->回溯
            removeRoute(vTex.Id, NeighVertex);
        }
    }

    return false;
}

bool UnicursalAlgorithm::IsCanResult(bool* IsTOdd)
{
    // 快速判断当前图形是否有解
    /*
     * 第一步: 判断当前图形是否为连通图
     * 第一步: 计算当前图形顶点的奇数个数
    */

    // 判断当前图形是否为连通图(广度优先搜索)
    int VertexCounts = Vertexs.size();
    if (VertexCounts > 0){
        // 记录顶点是否已被访问过
        bool* IsVisited = new bool[VertexCounts];
        memset(IsVisited, false, sizeof(bool) * VertexCounts);

        QQueue<VERTEX> Q;
        Q.push_back(Vertexs[0]);    // 顶点Id为0的入队列
        IsVisited[0] = true;        // Id为0的顶点已被访问过
        int id = 0, index = 0;
        while (!Q.isEmpty()){       // 当前队列非空
            VERTEX v = Q.first();
            Q.pop_front();
            // 遍历当前顶点的所有连接顶点
            QSet<int>::ConstIterator it = v.Neighbours.constBegin();
            for (; it!=v.Neighbours.constEnd(); ++it){
                // 如果当前顶点没有被访问过
                id = (*it);
                index = getVertexIndexById(id);
                if (!IsVisited[index]){
                    Q.push_back(Vertexs[index]);
                    IsVisited[index] = true;
                }
            }
        }

        // 判断当前图形是否为连通图
        for (int i=0; i<VertexCounts; ++i){
            // 有顶点还没有被访问过(非连通图)
            if (!IsVisited[i]){
                return false;
            }
        }
        delete [] IsVisited;

        // 计算当前图形顶点的奇数个数
        int OddCounts = 0;
        QList<VERTEX>::ConstIterator it = Vertexs.begin();
        for (; it!=Vertexs.end(); ++it){
            const VERTEX& v = (*it);
            // 顶点的个数为奇数
            if ((v.Neighbours.size() & 1)){
                OddCounts++;
                if (OddCounts == 1){
                    VertexOddId1 = v.Id;
                }else{
                    VertexOddId2 = v.Id;
                }
            }
        }
        if (OddCounts==0 || OddCounts==2){
            if (OddCounts == 2){
                *IsTOdd = true;
            }
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool UnicursalAlgorithm::Result ()
{
    setLineCount ();

    PassedRoutes.clear ();
    Routes.clear ();
    // 遍历所有点
    bool sign = false;
    QList<VERTEX>::ConstIterator it = Vertexs.begin ();
    for (; it!=Vertexs.end (); ++it){
        PassedRoutes.clear ();
        Routes.clear ();
        sign = Solution ((*it).Id);
        if (sign){
            //
            return true;
        }else{
            //
            continue;
        }
    }

    return false;
}

int UnicursalAlgorithm::getVertexOddId() const
{
    return VertexOddId1;
}

void UnicursalAlgorithm::addVertex (QPoint& Pos)
{
    VERTEX v;
    v.Id = nSerial;
    v.Positon = Pos;
    Vertexs << v;

    nSerial++;
}

void UnicursalAlgorithm::removeVertex (int Index)
{
    if (Index < 0 || Index >= Vertexs.size ()){
        return ;
    }else{
        VERTEX& RemoveV = Vertexs[Index];
        QSet<int>::ConstIterator it = RemoveV.Neighbours.begin ();
        for (; it!=RemoveV.Neighbours.end (); ++it){
            // 相邻点删除RemoveV
            VERTEX& NeighV = Vertexs[getVertexIndexById ((*it))];
            NeighV.Neighbours.remove (RemoveV.Id);
            // 删除边
            lines.remove(RemoveV.Id, NeighV.Id);
            lines.remove(NeighV.Id, RemoveV.Id);
        }

        Vertexs.removeAt (Index);
    }
}

void UnicursalAlgorithm::clear()
{
    Vertexs.clear();
    lines.clear();
    Routes.clear();
    LineCount = 0;
    nSerial   = 0;
    PassedRoutes.clear();
}

void UnicursalAlgorithm::clearRoutes ()
{
    Routes.clear ();
    PassedRoutes.clear ();
}

UnicursalAlgorithm::~UnicursalAlgorithm ()
{

}
