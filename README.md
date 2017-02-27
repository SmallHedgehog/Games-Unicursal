# 一笔画程序

---

## 开发环境与依赖

 - IDE: Qt creator4
 - 开发框架：Qt4
 - 开发语言: C++

## 实现效果图
![俄罗斯方块游戏实现效果图](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/0.png)

## 文件描述

- res/目录下是程序使用的资源文件
- test/目录下是程序使用的测试文件
- commonref.h文件是程序中定义的const values，比如说窗口大小等描述
- configure.h文件是配置文件，程序中暂时还没有使用
- nodecommonref.h文件是定义些公共引用结构体
- main.cpp程序入口函数

## 类文件描述

**unicursalalgorithm类**

该类描述的是怎样表示程序中的点和相关点连接的线以及该怎样求解一笔画问题。

**gamewindow类**

该类是用来图形展示我们的图形以及动态展示一笔画搜索。

## 程序详细描述

程序中定义有多种模式和状态，在不同模式和状态下支持相应 的功能。模式有编辑图形模式、自动求解模式、非自动求解模 式、普通模式。状态有：拖动图形状态、连线状态、全选移动 状态。

- 编辑图形模式：在该模式下可以编辑当前图形，包括添加顶点(Ctrl+Mouse:leftButton)、边(Shift+ Mouse:leftButton)和删除顶点和边(选中Mouse:Dou- bleClick)，程序中的状态是在该模式下工作的，拖动图形状态(选中：鼠标左击移动)、连线状态：鼠标 选中顶点移动鼠标，然后释放鼠标(释放的位置是另) 一个顶点、全选移动状态(Ctrl+A)：拖动当前图形。

- 自动求解模式：程序默认的模式，在编辑模式下按下Q或ESC键可进入该模式。当处于该模式下时，按下Space键->程序自动求解问题。

- 非自动求解模式：处于该模式下，玩家可以自行求解问题。

## 展示

普通模式下

![](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/1.png)

编辑图形模式下

![](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/2.png)

非自动求解模式下

![](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/3.png)

高亮显示与提示

![](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/4.png)

全选状态下

![](https://github.com/SmallHedgehog/Games-Unicursal/blob/master/res/5.png)
