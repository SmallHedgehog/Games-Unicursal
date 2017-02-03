#include "gamewindow.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QRegion>
#include <QPen>
#include <QMessageBox>
#include <QTimer>
#include <QToolTip>
#include <QPalette>
#include <QApplication>
#include <QIODevice>
#include <QtMath>
#include <QFileDialog>
#include <QStringList>
#include <QTextStream>
#include <QDebug>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    bColor = QColor(50, 50, 50);
    gridColor = QColor(240, 240, 240);
    VertexColor = QColor(120, 130, 120);
    DragLineColor = QColor(255, 255, 255);
    gridSize = QSize(20, 20);

    VertexRadius = 10;
    LineWidht = 2;
    HighColorVertexIndex = -1;
    HighColorVertex = QColor(120, 180, 0);
    IsHighColorLine = false;
    IsHasResult = false;
    PassedSteps = -1;
    PassLineColor = QColor(255, 180, 100);
    stretching = 0;
    SelectAllColor = QColor(180, 220, 50);
    IsDragAll = false;

    cur_mode = GameWindow::NORMALMODE;
    cur_state = GameWindow::NOSTATE;

    beginTimer = new QTimer(this);
    stopTimer  = new QTimer(this);

    InitWindow ();
    ConnectSiganlSlots ();
    InitData(QString("../Unicursal/test/result0.txt"));
}

void GameWindow::InitWindow ()
{
    setWindowIcon(QIcon(tr(":/images/res/pencil.ico")));

    setFixedSize (QSize(650, 700));
    CreateMenu ();
    CreateAction ();
    setTooltipStyle ();
    setMouseTracking (true);
    setAttribute (Qt::WA_DeleteOnClose, true);
}

void GameWindow::CreateMenu ()
{
    FileMenu = menuBar ()->addMenu (QString(tr("文件(&F)")));
    EditMenu = menuBar ()->addMenu (QString(tr("编辑(&E)")));
    HelpMenu = menuBar ()->addMenu (QString(tr("帮助(&H)")));
}

void GameWindow::CreateAction ()
{
    NewWindowAction = new QAction(QString(tr("新建窗口(&N)")), this);
    NewWindowAction->setShortcut (QKeySequence::New);

    OpenFileAction = new QAction(QString(tr("打开文件(&O)")), this);
    OpenFileAction->setShortcut (QKeySequence::Open);

    SaveFileAction = new QAction(QString(tr("保存文件(&S)")), this);
    SaveFileAction->setShortcut (QKeySequence::Save);

    QuitAction = new QAction(QString(tr("Quit(&X)")), this);
    QuitAction->setShortcut (QKeySequence(QString(tr("Ctrl+Q"))));

    QList<QAction*> actions;
    actions << NewWindowAction << OpenFileAction << SaveFileAction;
    FileMenu->addActions(actions);
    FileMenu->addSeparator ();
    FileMenu->addAction(QuitAction);

    EditAction = new QAction(QString(tr("编辑图形(&E)")), this);
    EditAction->setShortcut (QKeySequence(QString(tr("Ctrl+E"))));

    NormalAction = new QAction(QString(tr("NormalMode(&N)")), this);
    NormalAction->setShortcut (QKeySequence(QString(tr("Ctrl+Shift+N"))));

    ClearAction = new QAction(QString(tr("清空(&C)")), this);
    ClearAction->setShortcut (QKeySequence(QString(tr("Ctrl+Shift+C"))));

    NoAutoAction = new QAction(QString(tr("NoAutoAction(&S)")), this);
    NoAutoAction->setShortcut(QKeySequence(QString(tr("Ctrl+Shift+S"))));

    EditMenu->addAction(EditAction);
    EditMenu->addAction(NormalAction);
    EditMenu->addAction(ClearAction);
    EditMenu->addAction(NoAutoAction);

    HelpAction = new QAction(QString(tr("帮助")), this);
    HelpAction->setShortcut (QKeySequence(QString(tr("Ctrl+H"))));

    AboutAction = new QAction(QString(tr("关于(&A)...")), this);
    HelpMenu->addAction(HelpAction);
    HelpMenu->addAction(AboutAction);
}

void GameWindow::ConnectSiganlSlots ()
{
    // 设置编辑图形模式
    connect (EditAction, SIGNAL(triggered(bool)), this, SLOT(SetEditMode()));
    // 设置普通模式
    connect (NormalAction, SIGNAL(triggered(bool)), this, SLOT(SetNormalMode()));
    connect (NoAutoAction, SIGNAL(triggered(bool)), this, SLOT(SetNoAutoMode()));
    // 当前图形是否有解->调用相应slot函数
    connect (this, SIGNAL(info(GameWindow::HasResulted)), this, SLOT(getInfo(GameWindow::HasResulted)));
    // 开始求解定时器触发timeout信号
    connect (beginTimer, SIGNAL(timeout()), this, SLOT(beginTimerTimeout()));
    // 暂停求解
    connect (this, SIGNAL(stop()), this, SLOT(Stop_beginTimer()));
    // 导出文件
    connect(SaveFileAction, SIGNAL(triggered(bool)), this, SLOT(Export()));
    // 导入文件
    connect(OpenFileAction, SIGNAL(triggered(bool)), this, SLOT(Import()));
    // 关闭窗口
    connect(QuitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(HelpAction, SIGNAL(triggered(bool)), this, SLOT(Help()));
    connect(AboutAction, SIGNAL(triggered(bool)), this, SLOT(About()));
    // 清空当前图形
    connect(ClearAction, SIGNAL(triggered(bool)), this, SLOT(ClearAll()));
}

void GameWindow::beginTimerTimeout ()
{
    update();

    // 已求解完成
    if (PassedSteps == unicursal.Routes.size ()){
        // 停止计时
        beginTimer->stop ();
    }

    stretching += 0.001f;
    if (stretching >= 1){
        stretching = 0;
        PassedSteps++;
    }
}

void GameWindow::Stop_beginTimer ()
{
    beginTimer->stop ();
}

void GameWindow::SetEditMode ()
{
    cur_mode = GameWindow::EDITMODE;
    update ();
}

void GameWindow::SetNormalMode ()
{
    cur_mode = GameWindow::NORMALMODE;
    update ();
}

void GameWindow::SetNoAutoMode()
{
    cur_mode = GameWindow::CHALLENGE;
    clear();
    update();
}

void GameWindow::setTooltipStyle ()
{
    QFile qssFile(QString(":/toolTip.qss"));
    if (qssFile.open (QIODevice::ReadOnly)){
        // 设置QToolTip样式
        qApp->setStyleSheet (QString(qssFile.readAll ().data ()));

        qssFile.close ();
    }
}

void GameWindow::getInfo (GameWindow::HasResulted hasResulted)
{
    QMessageBox* InfoBox = new QMessageBox(this);
    InfoBox->setIconPixmap (QPixmap(QString(tr(":/images/pencil.ico"))));
    InfoBox->setWindowTitle (QString(tr("GameWindow::HasResulted")));
    switch (hasResulted)
    {
    case GameWindow::NoLine:
        InfoBox->setText (QString(tr("<p align='center'><font face='Consolas' size='5'>NO NOLINE</font></p>"
                                     "<font face='Consolas' size='4'>Get help from Help Menu!</font><br>")));
        break;
    case GameWindow::NO:
        InfoBox->setText (QString(tr("<p align='center'><font face='Consolas' size='5'>NO RESULT</font></p>"
                                     "<font face='Consolas' size='4'>Get help from Help Menu!</font><br>")));
        break;
    case GameWindow::YES:       // 有解
        // beginTimer定时器开始计时
        if (IsHasResult){
            PassedSteps = 0;    // 当前已走了多少边
            beginTimer->start ();
        }
        return ;
        break;
    case GameWindow::NoVertex:
        InfoBox->setText (QString(tr("<p align='center'><font face='Consolas' size='5'>NO VERTEX</font></p>"
                                     "<font face='Consolas' size='4'>Get help from Help Menu!</font><br>")));
        break;
    }
    InfoBox->setDefaultButton (QMessageBox::Ok);
    // InfoBox->setWindowFlags (Qt::Dialog);
    InfoBox->exec ();
}

QPoint GameWindow::midPoint (QPoint &p1, QPoint &p2, float &stretching)
{
    int x = stretching*(p2.x ()-p1.x ()) + p1.x ();
    int y = stretching*(p2.y ()-p1.y ()) + p1.y ();

    return QPoint(x, y);
}

double GameWindow::Distance (QPoint& p1, QPoint& p2)
{
    return sqrt((p2.x ()-p1.x ())*(p2.x ()-p1.x ()) +
                (p2.y ()-p1.y ())*(p2.y ()-p1.y ()));
}

QPoint GameWindow::Intersection (QPoint &p1, QPoint &p2, QPoint &ellipsePoint)
{
    // 围绕椭圆的四个顶点
    QPointF topPointF = QPointF(ellipsePoint.x ()-VertexRadius, ellipsePoint.y ()-VertexRadius);
    QPointF rightPointF = QPointF(ellipsePoint.x ()+VertexRadius, ellipsePoint.y ()-VertexRadius);
    QPointF bottomPointF = QPointF(ellipsePoint.x ()+VertexRadius, ellipsePoint.y ()+VertexRadius);
    QPointF leftPointF = QPointF(ellipsePoint.x ()-VertexRadius, ellipsePoint.y ()+VertexRadius);
    // 围绕椭圆的四条边
    QLineF topLine = QLineF(topPointF, rightPointF);
    QLineF rightLine = QLineF(rightPointF, bottomPointF);
    QLineF bottomLine = QLineF(bottomPointF, leftPointF);
    QLineF leftLine = QLineF(leftPointF, topPointF);

    QList<QLineF> LineLists;
    LineLists << topLine << rightLine << bottomLine << leftLine;
    QPointF IntersectedPointF;
    QLineF dataLine = QLineF(p1.x (), p1.y (), p2.x (), p2.y ());
    foreach (QLineF line, LineLists) {
        QLineF::IntersectType intersectType = line.intersect (dataLine, &IntersectedPointF);
        if (intersectType == QLineF::BoundedIntersection){
            break;
        }
    }

    QPoint IntersectedPoint = QPoint(IntersectedPointF.x (), IntersectedPointF.y ());
    return IntersectedPoint;
}

GameWindow::ObjectType GameWindow::Hit (QPoint point)
{
    // 如果击中顶点返回VertexObject
    for (int i=0; i<unicursal.Vertexs.size (); ++i){
        const VERTEX& v = unicursal.Vertexs[i];
        QRegion rg(v.Positon.x ()-VertexRadius, v.Positon.y ()-VertexRadius,
                   VertexRadius*2, VertexRadius*2, QRegion::Ellipse);
        if (rg.contains (point)){
            // 记录击中的顶点index
            HitVertexIndex = i;
            return GameWindow::VertexObject;
        }
    }
    // 如果击中相应顶点所连接的边则返回LineObject
    QMultiMap<int, int>::ConstIterator it = unicursal.lines.begin ();
    for (; it!=unicursal.lines.end (); ++it){
        // 连线的起始顶点的位置
        QPoint p1 = unicursal.Vertexs[unicursal.getVertexIndexById (it.key ())].Positon;
        // 连线的结束顶点的位置
        QPoint p2 = unicursal.Vertexs[unicursal.getVertexIndexById (it.value ())].Positon;

        double d1 = Distance (point, p1);
        double d2 = Distance (point, p2);
        double line_distance = Distance (p2, p1);
        if (d1+d2 - line_distance < MIN_DISTANCE){
            // 记录选中线的起始顶点和结束顶点
            SelectLineStartVertexId = it.key ();
            SelectLineEndVertexId = it.value ();

            return GameWindow::LineObject;
        }
    }

    return GameWindow::NoneObject;
}

bool GameWindow::CanDropVertex (QPoint &point)
{
    QRegion rg(point.x ()-VertexRadius, point.y ()-VertexRadius,
               VertexRadius*2, VertexRadius*2);
    QList<VERTEX>::ConstIterator it = unicursal.Vertexs.begin ();
    for (; it!=unicursal.Vertexs.end (); ++it){
        const VERTEX& v = (*it);
        QRegion temp_rg(v.Positon.x ()-VertexRadius, v.Positon.y ()-VertexRadius,
                        VertexRadius*2, VertexRadius*2);
        if (rg.intersects (temp_rg)){
            return false;
        }
    }
    return true;
}

bool GameWindow::event (QEvent *event)
{
    if (event->type () == QEvent::ToolTip){
        ObjectType type = Hit(mouseMovePoint);

        // QPalette palette = QPalette();
        // palette.setBrush (QPalette::Inactive, QPalette::ToolTipBase, QColor(VertexColor));
        // palette.setColor (QPalette::Inactive, QPalette::ToolTipBase, QColor(VertexColor));

        // QToolTip::setPalette (palette);

        if (type == GameWindow::VertexObject){
            QToolTip::showText (QCursor::pos (), QString("VERTEX NUMBER ID:%1")
                                .arg (QString::number (unicursal.Vertexs[HitVertexIndex].Id)));
        }else if (type == GameWindow::LineObject){
            QToolTip::showText (QCursor::pos (), QString("LINE FROM VN ID:%1 TO VN ID:%2")
                                .arg (QString::number (SelectLineStartVertexId))
                                .arg (QString::number (SelectLineEndVertexId)));
        }else{
            QToolTip::hideText ();
            event->ignore ();
        }
        return true;
    }
    return QWidget::event (event);
}

void GameWindow::paintEvent (QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint (QPainter::Antialiasing, true);
    painter.fillRect (rect(), bColor);

    drawGraphics (&painter);
}

void GameWindow::keyPressEvent (QKeyEvent *event)
{
    Qt::Key key = static_cast<Qt::Key>(event->key());
    Qt::KeyboardModifiers modifiers = event->modifiers();
    // 按下ESC键或者Q键时退出编辑模式(改为普通模式)
    if (event->key ()==Qt::Key_Escape || event->key ()==Qt::Key_Q){
        unicursal.setLineCount();
        cur_state = GameWindow::NOSTATE;
        emit NormalAction->triggered (true);
    }
    else if (event->key ()==Qt::Key_Space && cur_mode==GameWindow::NORMALMODE){   // 按下空格键时进行求解(需要在普通模式下)
        // 开始求解模式
        unicursal.clearRoutes ();
        clear ();
        cur_mode = GameWindow::STARTMODE;
        PassedSteps = -1;
        // 当前图形中没有顶点时
        if (unicursal.Vertexs.isEmpty ()){
            emit info(GameWindow::NoVertex);
            return ;
        }
        // 当前图形中没有边时
        if (unicursal.lines.isEmpty ()){
            emit info(GameWindow::NoLine);
            return ;
        }
        // 求解
        bool IsTOdd = false;
        IsHasResult = unicursal.IsCanResult(&IsTOdd);
        // 当前图形无解时
        if (!IsHasResult){
            emit info(GameWindow::NO);
        }else{  // 当前图形有解
            if (IsTOdd){    // 当前图形奇数点的个数为2
                unicursal.Solution(unicursal.getVertexOddId());
            }else{          // 当前图形的所有顶点的个数为偶数
                unicursal.Solution(0);
            }
            emit info(GameWindow::YES);
        }
    }
    else if (event->key ()==Qt::Key_Space && cur_mode==GameWindow::STARTMODE){
        // 普通模式
        cur_mode = GameWindow::NORMALMODE;
        // 暂停开始求解计时器
        emit stop();
    }
    else if (modifiers==Qt::ControlModifier && key==Qt::Key_A && cur_mode==GameWindow::EDITMODE){
        // 在编辑模式下按下Ctrl+A进入全选移动模式
        cur_state = GameWindow::SELETALL;
        update();
    }
}

void GameWindow::mousePressEvent (QMouseEvent *event)
{
    if (cur_mode == GameWindow::EDITMODE){
        Qt::MouseButton btn = event->button ();
        if (btn == Qt::LeftButton){     // 鼠标左键按下选择点或与Ctrl键组合添加点
            Qt::KeyboardModifiers Keyborad = event->modifiers ();
            if (Keyborad == Qt::NoModifier){            // 选择点进入拖动状态
                // 进入全选拖动状态
                if (cur_state == GameWindow::SELETALL){
                    if (event->pos().x()>=SelectAllNode.minX && event->pos().x()<=SelectAllNode.maxX
                            && event->pos().y()>=SelectAllNode.minY && event->pos().y()<=SelectAllNode.maxY){
                        SelectAllPoint = event->pos();
                        SelectAllEndPoint = event->pos();
                        IsDragAll = true;
                        setCursor(QCursor(Qt::ArrowCursor));
                    }else{
                        IsDragAll = false;
                        cur_state = GameWindow::NOSTATE;
                    }
                }else{
                    // 设置图形状态为拖动状态
                    GameWindow::ObjectType type = Hit(event->pos ());
                    if (type == VertexObject){
                        cur_state = GameWindow::DRAGGING;
                    }
                }
            }
            else if (Keyborad == Qt::ControlModifier){  // 添加点
                QPoint pt = event->pos ();
                if (CanDropVertex (pt)){
                    // 不需要修改pt的位置(pt为点的中心点)
                    unicursal.addVertex (pt);
                }
            }
            else if (Keyborad == Qt::ShiftModifier){    // 添加击中顶点的连线
                GameWindow::ObjectType type = Hit(event->pos ());
                if (type == VertexObject){
                    cur_state = GameWindow::LINING;
                    LineStartVertexIndex = HitVertexIndex;
                    LineEndPoint = event->pos ();
                }
            }
        }
    }else if (cur_mode == GameWindow::CHALLENGE){        // 非自动求解模式
        GameWindow::ObjectType type = Hit(event->pos());
        if (type == GameWindow::VertexObject)
        {
            int hitId = unicursal.Vertexs[HitVertexIndex].Id;
            if (HasPassedVertexs.isEmpty()){
                HasPassedVertexs.push_back(hitId);
            }else{
                // 需要和栈顶的顶点Id有联系
                const VERTEX& v = unicursal.Vertexs[unicursal.getVertexIndexById(HasPassedVertexs.top())];
                if (v.Neighbours.contains(hitId))
                {
                    // 加边
                    if (!(HasPassedRoutes.contains(v.Id, hitId)
                            || HasPassedRoutes.contains(hitId, v.Id))){
                        HasPassedRoutes.insert(v.Id, hitId);
                        HasPassedVertexs.push_back(hitId);
                    }
                }
            }
        }
    }
    update();
}

void GameWindow::mouseMoveEvent (QMouseEvent *event)
{
    SelectAllPoint = SelectAllEndPoint;
    SelectAllEndPoint = event->pos ();
    switch (cur_state)
    {
    case GameWindow::DRAGGING:  // 拖动顶点状态
        if (rect().contains (event->pos ())){
            QPoint pt = event->pos ();
            unicursal.Vertexs[HitVertexIndex].Positon = pt;
        }
        break;
    case GameWindow::LINING:    // 连线状态
        LineEndPoint = event->pos ();
        break;
    case GameWindow::SELETALL:  //全选移动状态
        if (event->pos().x()>=SelectAllNode.minX && event->pos().x()<=SelectAllNode.maxX
                && event->pos().y()>=SelectAllNode.minY && event->pos().y()<=SelectAllNode.maxY){
            setCursor(QCursor(Qt::SizeAllCursor));
            if (IsDragAll){
                for (int i=0; i<unicursal.Vertexs.size(); ++i){
                    unicursal.Vertexs[i].Positon += SelectAllEndPoint - SelectAllPoint;
                }
            }
        }else{
            setCursor(QCursor(Qt::ArrowCursor));
        }
        break;
    case GameWindow::NOSTATE:
        ObjectType type = Hit(event->pos ());
        // 选中顶点
        if (type == VertexObject){
            HighColorVertexIndex = HitVertexIndex;
        }else{
            HighColorVertexIndex = -1;
        }
        // 选中边
        if (type == LineObject){
            IsHighColorLine = true;
        }else{
            IsHighColorLine = false;
        }
        break;
    }

    mouseMovePoint = event->pos ();

    update();
}

void GameWindow::mouseReleaseEvent (QMouseEvent*)
{
    if (cur_state == LINING){
        GameWindow::ObjectType type = Hit(LineEndPoint);
        if (type == VertexObject){
           int LineEndVertexIndex = HitVertexIndex;
           int endId = unicursal.Vertexs[LineEndVertexIndex].Id;
           int startId = unicursal.Vertexs[LineStartVertexIndex].Id;
           if (endId == startId){
               // do something
           }else{
               unicursal.Vertexs[LineStartVertexIndex].Neighbours.insert (endId);
               unicursal.Vertexs[LineEndVertexIndex].Neighbours.insert (startId);
               if (!(unicursal.lines.contains(startId, endId)
                       || unicursal.lines.contains(endId, startId))){
                   unicursal.lines.insert (startId, endId);
               }
           }
        }
    }

    if (cur_state == GameWindow::SELETALL){
        //
    }else{
        cur_state = GameWindow::NOSTATE;
    }
    IsDragAll = false;

    update();
}

void GameWindow::mouseDoubleClickEvent (QMouseEvent *event)
{
    if (cur_mode==GameWindow::EDITMODE && event->button ()==Qt::LeftButton){
        ObjectType type = Hit(event->pos ());
        if (type == VertexObject){      // 击中的是顶点
            // 删除顶点与相连接的边
            unicursal.removeVertex (HitVertexIndex);
            HighColorVertexIndex = -1;
            update();
        }
        else if (type == LineObject){   // 选择的是相应顶点连接的边
            // 删边连接的顶点的相连接的顶点
            VERTEX& v1 = unicursal.Vertexs[unicursal.getVertexIndexById (SelectLineStartVertexId)];
            VERTEX& v2 = unicursal.Vertexs[unicursal.getVertexIndexById (SelectLineEndVertexId)];
            v1.Neighbours.remove (SelectLineEndVertexId);
            v2.Neighbours.remove (SelectLineStartVertexId);
            // 删除选中的边
            unicursal.lines.remove(SelectLineStartVertexId, SelectLineEndVertexId);
            unicursal.lines.remove(SelectLineEndVertexId, SelectLineStartVertexId);

            update();
        }
    }
}

void GameWindow::drawGraphics (QPainter *painter)
{
    switch (cur_mode)
    {
    case GameWindow::EDITMODE:
        editmode (painter);     // 绘制网格
        break;
    case GameWindow::NORMALMODE:
        break;
    default:
        break;
    }

    // 绘制正在画的线
    if (cur_state == GameWindow::LINING){
        painter->setPen (QPen(DragLineColor, 3));
        QPoint startPoint = unicursal.Vertexs[HitVertexIndex].Positon;
        // 需要改变startPoint和LineEndPoint
        painter->drawLine (startPoint, LineEndPoint);
        // QPoint newStartPoint = Intersection (startPoint, LineEndPoint, startPoint);
        // QPoint newLineEndPoint = Intersection (LineEndPoint, startPoint, LineEndPoint);
        // painter->drawLine (newStartPoint, newLineEndPoint);
    }

    // 绘制所有顶点
    QList<VERTEX>::ConstIterator it = unicursal.Vertexs.begin ();
    for (; it!=unicursal.Vertexs.end (); ++it){
        const VERTEX& v = (*it);
        painter->setPen (VertexColor);
        painter->setBrush (VertexColor);
        painter->drawEllipse (v.Positon.x ()-VertexRadius, v.Positon.y ()-VertexRadius,
                              VertexRadius*2, VertexRadius*2);
    }
    // 绘制所有与顶点相连的线
    QMultiMap<int, int>::ConstIterator its = unicursal.lines.begin ();
    for (; its!=unicursal.lines.end (); ++its){
        int startIndex = unicursal.getVertexIndexById (its.key ());
        int endIndex = unicursal.getVertexIndexById (its.value ());

        QPoint startPoint = unicursal.Vertexs[startIndex].Positon;
        QPoint endPoint = unicursal.Vertexs[endIndex].Positon;
        QPen pen;
        pen.setWidth (3);
        pen.setBrush (VertexColor);
        painter->setPen (pen);
        // 需要改变startPonit和endPoint
        // painter->drawLine (startPoint, endPoint);
        QPoint newStartPoint = Intersection (startPoint, endPoint, startPoint);
        QPoint newEndPoint = Intersection (endPoint, startPoint, endPoint);
        painter->drawLine (newStartPoint, newEndPoint);
    }

    // 求解模式
    if (cur_mode == GameWindow::STARTMODE)
    {
        painter->setPen (QPen(PassLineColor, 4));
        for (int i=0; i<unicursal.Routes.size (); ++i)
        {
            const LINE& l = unicursal.Routes[i];
            // 绘制已经经过的边
            if (i <= PassedSteps){
                QPoint startPoint = unicursal.Vertexs[unicursal.getVertexIndexById (l.VertexStart)].Positon;
                QPoint endPoint = unicursal.Vertexs[unicursal.getVertexIndexById (l.VertexEnd)].Positon;
                // 需要改变startPoint和endPoint
                QPoint newStartPoint = Intersection (startPoint, endPoint, startPoint);
                QPoint newEndPoint = Intersection (endPoint, startPoint, endPoint);
                painter->drawLine (newStartPoint, newEndPoint);
            }
            // 绘制正在经过的边
            else if (i == PassedSteps){
                QPoint startPoint = unicursal.Vertexs[unicursal.getVertexIndexById (l.VertexStart)].Positon;
                QPoint endPoint = unicursal.Vertexs[unicursal.getVertexIndexById (l.VertexEnd)].Positon;
                QPoint MidPoint = midPoint (startPoint, endPoint, stretching);
                // 需要改变startPoint
                QPoint newStartPoint = Intersection (startPoint, endPoint, startPoint);
                painter->drawLine (newStartPoint, MidPoint);
            }
        }
    }
    else if (cur_mode == GameWindow::CHALLENGE)
    {
        painter->setPen(QPen(QColor(255, 255, 255), 4));
        QMultiMap<int, int>::const_iterator it = HasPassedRoutes.constBegin();
        for (; it!=HasPassedRoutes.constEnd(); ++it){
            QPoint startPoint = unicursal.Vertexs[unicursal.getVertexIndexById(it.key())].Positon;
            QPoint endPoint = unicursal.Vertexs[unicursal.getVertexIndexById(it.value())].Positon;
            // 需要改变startPoint和endPoint
            QPoint newStartPoint = Intersection (startPoint, endPoint, startPoint);
            QPoint newEndPoint = Intersection (endPoint, startPoint, endPoint);
            painter->drawLine (newStartPoint, newEndPoint);
        }
    }

    // 高亮在无状态时的顶点
    if (HighColorVertexIndex != -1){
        const VERTEX& v = unicursal.Vertexs[HighColorVertexIndex];
        painter->setPen (HighColorVertex);
        painter->setBrush (HighColorVertex);
        painter->drawEllipse (v.Positon.x ()-VertexRadius, v.Positon.y ()-VertexRadius,
                              VertexRadius*2, VertexRadius*2);
        //
    }
    // 高亮在无状态时的边
    if (IsHighColorLine){
        painter->setPen (QPen(HighColorVertex, 3));
        QPoint startPoint = unicursal.Vertexs[unicursal.getVertexIndexById (SelectLineStartVertexId)].Positon;
        QPoint endPoint = unicursal.Vertexs[unicursal.getVertexIndexById (SelectLineEndVertexId)].Positon;
        // 需要改变startPonit和endPoint
        // painter->drawLine (startPoint, endPoint);
        QPoint newStartPoint = Intersection (startPoint, endPoint, startPoint);
        QPoint newEndPoint = Intersection (endPoint, startPoint, endPoint);
        painter->drawLine (newStartPoint, newEndPoint);
    }

    // 全选移动状态
    if (cur_state == GameWindow::SELETALL)
    {
        drawSelectAll(painter);
    }
}

void GameWindow::drawSelectAll(QPainter *painter)
{
    QList<VERTEX>::const_iterator it = unicursal.Vertexs.constBegin();
    if (it == unicursal.Vertexs.constEnd()){
        return ;
    }
    int minX = unicursal.Vertexs[0].Positon.x();
    int minY = unicursal.Vertexs[0].Positon.y();
    int maxX = minX;
    int maxY = minY;
    QPoint p1, p2, p3, p4;
    QVector<QPoint> lines;
    for (; it!=unicursal.Vertexs.constEnd(); ++it){
        const VERTEX& v = (*it);
        painter->setPen(QPen(SelectAllColor, 1, Qt::DashDotLine));
        p1.setX(v.Positon.x()-VertexRadius);
        p1.setY(v.Positon.y()-VertexRadius);
        p2.setX(v.Positon.x()+VertexRadius);
        p2.setY(v.Positon.y()-VertexRadius);
        p3.setX(v.Positon.x()+VertexRadius);
        p3.setY(v.Positon.y()+VertexRadius);
        p4.setX(v.Positon.x()-VertexRadius);
        p4.setY(v.Positon.y()+VertexRadius);
        lines << p1 << p2 << p2 << p3 << p3 << p4 << p4 << p1;
        painter->drawLines(lines);

        if (v.Positon.x() < minX){
            minX = v.Positon.x();
        }
        if (v.Positon.x() > maxX){
            maxX = v.Positon.x();
        }
        if (v.Positon.y() < minY){
            minY = v.Positon.y();
        }
        if (v.Positon.y() > maxY){
            maxY = v.Positon.y();
        }
    }
    // 绘制外接矩形
    setSelectAllNode(minX, minY, maxX, maxY);
    p1.setX(minX-VertexRadius);
    p1.setY(minY-VertexRadius);
    p2.setX(p1.x()+maxX-minX+VertexRadius*2);
    p2.setY(p1.y());
    p3.setX(p2.x());
    p3.setY(p1.y()+maxY-minY+VertexRadius*2);
    p4.setX(p1.x());
    p4.setY(p3.y());
    lines.clear();
    lines << p1 << p2 << p2 << p3 << p3 << p4 << p4 << p1;
    painter->drawLines(lines);
}

void GameWindow::editmode (QPainter *painter)
{
    QPen gridPen = QPen(gridColor);
    painter->setPen (gridPen);
    for (int x=0; x<width(); x+=gridSize.width ()){
        for (int y=0; y<height (); y+=gridSize.height ()){
            painter->drawPoint (x, y);
        }
    }
}

void GameWindow::setSelectAllNode(int minX, int minY, int maxX, int maxY)
{
    SelectAllNode.minX = minX;
    SelectAllNode.maxX = maxX;
    SelectAllNode.minY = minY;
    SelectAllNode.maxY = maxY;
}

void GameWindow::Export()
{
    if (cur_mode == GameWindow::NORMALMODE)
    {
        QString fileName = QFileDialog::getSaveFileName(this, QString("Export File"), QString("../Unicursal/test"));
        if (!fileName.isEmpty()){
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)){
                QTextStream in(&file);
                int VertexCounts = unicursal.Vertexs.size();
                int LineCounts = unicursal.getLineCount();

                in << VertexCounts << " " << LineCounts << "\n";
                QList<VERTEX>::const_iterator it = unicursal.Vertexs.constBegin();
                for (; it!=unicursal.Vertexs.constEnd(); ++it){
                    in << (*it).Positon.x() << " " << (*it).Positon.y() << "\n";
                }

                it = unicursal.Vertexs.constBegin();
                for (; it!=unicursal.Vertexs.constEnd(); ++it){
                    const VERTEX& v = (*it);
                    QSet<int>::const_iterator id = v.Neighbours.constBegin();
                    for (; id!=v.Neighbours.constEnd(); ++id){
                        in << v.Id << " " << (*id) << "\n";
                    }
                }
            }
        }
    }
}

void GameWindow::Import()
{
    if (cur_mode == GameWindow::EDITMODE)
    {
        QString fileName = QFileDialog::getOpenFileName(this, QString("Import File"), QString("../Unicursal/test/"), QString("text(*.txt)"));
        InitData(fileName);
    }
}

void GameWindow::InitData(QString fileName)
{
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)){
            QTextStream out(&file);

            unicursal.clear();

            QString str = out.readLine();
            QStringList strList = str.split(QString(" "));
            int VertexCounts = QString(strList[0]).toInt();
            // int LineCounts = QString(strList[1]).toInt();
            int Counts = 0;
            while (!str.isNull()){
                str = out.readLine();
                Counts++;
                if (!str.isEmpty()){
                    strList = str.split(QString(" "));
                    int number1 = QString(strList[0]).toInt();
                    int number2 = QString(strList[1]).toInt();
                    // 获取顶点的坐标
                    if (Counts <= VertexCounts){
                        QPoint point(number1, number2);
                        unicursal.addVertex(point);
                    }else{  // 获取边的信息
                        unicursal.Vertexs[unicursal.getVertexIndexById(number1)].Neighbours.insert(number2);
                        if (!(unicursal.lines.contains(number1, number2)
                                || unicursal.lines.contains(number2, number1))){
                            unicursal.lines.insert (number1, number2);
                        }
                    }
                }
            }
        }
    }
}

void GameWindow::Help()
{
    QFile file(QString("../Unicursal/README.md"));
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);

        QMessageBox::about(this, QString("Help"), in.readAll());
    }
}

void GameWindow::About()
{
    QFile file(QString("../Unicursal/About.md"));
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);

        QMessageBox::about(this, QString("About"), in.readAll());
    }
}

void GameWindow::clear ()
{
    HasPassedRoutes.clear ();
    HasPassedVertexs.clear ();
}

void GameWindow::ClearAll()
{
    unicursal.clear();
}

GameWindow::~GameWindow()
{

}
