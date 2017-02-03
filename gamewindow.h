#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "unicursalalgorithm.h"
#include "commonref.h"
#include "nodecommonref.h"
#include <QStack>

class QMenu;
class QAction;
class QColor;
class QTimer;

// 处理鼠标悬停事件时记录选中时的顶点Id(选中顶点和边)
struct mouseMoveHitVertexId
{
    int startId;
    int endId;
};
typedef struct mouseMoveHitVertexId MMHVertexId;

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = 0);
    ~GameWindow();

public:
    // 窗口模式
    enum Mode{
        EDITMODE,   // 编辑图形模式
        STARTMODE,  // 自动求解模式
        CHALLENGE,  // 非自动求解模式
        NORMALMODE
    };
    // 图形状态
    enum State{
        DRAGGING,   // 拖动图形模式
        LINING,     // 连线模式
        SELETALL,   // 全选移动模式
        NOSTATE
    };

    // 定义窗口中图形的类型
    enum ObjectType{
        VertexObject,   // 顶点
        LineObject,     // 顶点之间的连线
        NoneObject      // None
    };

    // 当前图形是否有解
    enum HasResulted{
        NoVertex,   // 无顶点
        NO,         // 无解
        YES,        // 有解
        NoLine      // 无边
    };

protected:
    void paintEvent (QPaintEvent*);
    void mousePressEvent (QMouseEvent* event);
    void mouseMoveEvent (QMouseEvent* event);
    void mouseReleaseEvent (QMouseEvent*);
    void mouseDoubleClickEvent (QMouseEvent* event);
    void keyPressEvent (QKeyEvent* event);
    // 处理鼠标悬停事件
    bool event (QEvent *event);

private:
    void InitWindow();      //设置窗口属性
    void CreateMenu();      //设置菜单
    void CreateAction();    //设置菜单项
    void drawGraphics(QPainter* painter);
    void editmode(QPainter* painter);
    void drawSelectAll(QPainter* painter);
    void ConnectSiganlSlots();
    ObjectType Hit(QPoint point);             // 当鼠标左键按下时返回击中的图形类型
    bool CanDropVertex(QPoint& point);        // 判断当前画的点是否和已画的点冲突
    double Distance(QPoint& p1, QPoint& p2);  // 计算两个点之间的距离
    QPoint Intersection(QPoint& p1, QPoint& p2, QPoint& ellipsePoint); // 计算顶点与直线的交点
    QPoint midPoint(QPoint& p1, QPoint& p2, float& stretching);        // 绘制两点之间的连线时的中间某一点
    // 设置QToolTip的样式
    void setTooltipStyle();
    void setSelectAllNode(int minX, int minY, int maxX, int maxY);
    void clear();
    void InitData(QString fileName);

private:
    QMenu *FileMenu, *EditMenu, *HelpMenu;
    // FileMenu
    QAction *NewWindowAction, *OpenFileAction, *SaveFileAction, *QuitAction;
    // EditMenu
    QAction *EditAction, *NormalAction, *ClearAction, *NoAutoAction;
    // HelpMenu
    QAction *HelpAction, *AboutAction;

    Mode cur_mode;          // 当前窗口模式[编辑图形模式、开始求解模式]
    State cur_state;        // 当前图形状态
    QColor bColor;          // 背景颜色
    QColor VertexColor;     // 顶点颜色
    int VertexRadius;       // 顶点半径
    QColor DragLineColor;        // 正在拖动线的颜色
    int LineWidht;               // 线宽
    int HitVertexIndex;          // 鼠标左击按下时记录击中顶点的index
    int LineStartVertexIndex;    // 连线时的开始顶点
    QPoint LineEndPoint;         // 连线的结束位置
    int HighColorVertexIndex;    // 在无状态时(当鼠标移动时高亮击中的顶点)
    QColor HighColorVertex;      // 在无状态时高亮击中的顶点的颜色
    QColor PassLineColor;        // 求解模式下画线时的颜色

    QColor SelectAllColor;       // 全选模式下绘制方框颜色
    NODE SelectAllNode;
    QPoint SelectAllPoint;
    QPoint SelectAllEndPoint;
    bool IsDragAll;

    // QPoint SelectLineVertexs; // 当选中一条边时(记录边的开始顶点和结束顶点)
    int SelectLineStartVertexId;
    int SelectLineEndVertexId;

    bool IsHighColorLine;   // 当选中一条边时(高亮)
    bool IsHasResult;       // 当前图形是否有解
    int PassedSteps;        // 记录求解时经过的步数
    QTimer* beginTimer;     // 开始求解的定时器(需要当前图形有解时)
    QTimer* stopTimer;      // 暂停求解
    float   stretching;     // 绘制两个顶点间的连线的步长

    QStack<int> HasPassedVertexs;
    QMultiMap<int, int> HasPassedRoutes;

    // 处理鼠标悬停事件
    QPoint mouseMovePoint;  // 记录鼠标移动的位置
    MMHVertexId mmhVertexId;

    UnicursalAlgorithm unicursal;

    // 编辑模式
    QColor gridColor;       // 网格颜色
    QSize  gridSize;        // 网格间距

public slots:
    void SetEditMode();
    void SetNormalMode();
    void SetNoAutoMode();
    void getInfo(GameWindow::HasResulted hasResulted);
    void beginTimerTimeout();
    void Stop_beginTimer();
    void Export();          // 导出
    void Import();          // 导入
    void Help();
    void About();
    void ClearAll();

signals:
    void info(GameWindow::HasResulted hasResulted);
    void stop();
};

#endif // GAMEWINDOW_H
