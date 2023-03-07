#include "graphcanvas.h"

GC::GC()
{
    this->setMouseTracking(true);
    this->setRenderHint(QPainter::Antialiasing);
    this->setStyleSheet("padding:0px;border:0px");

    myGraphicsScene = new QGraphicsScene();
    myGraphicsScene->setBackgroundBrush(Qt::transparent);
    myGraphicsScene->setSceneRect(0,0,590,390);
    this->setScene(myGraphicsScene);
    this->move(5,5);

    root=new Vex(QPointF(100,150),10, vexID++);
    myGraphicsScene->addItem(root->nameTag);
    myGraphicsScene->addItem(root);
    vexes.push_back(root);
}

void GC::mousePressEvent(QMouseEvent *event)
{
    int onVex=false;
    int onWhichVex = 0;
    for(int i=0;i<vexes.size();i++)
    {
        if(event->pos().rx()>=vexes[i]->center.rx()-20 && event->pos().rx()<=vexes[i]->center.rx()+20 &&
                event->pos().ry()>=vexes[i]->center.ry()-20 && event->pos().ry()<=vexes[i]->center.ry()+20){
            onVex=true;
            onWhichVex = i;
            break;
        }
    }
    if(onVex){
        if(!isCreating){
            isCreating=true;
            strtVex =vexes[onWhichVex];
        }
        else{
            Vex* endVex = vexes[onWhichVex];
            if(endVex == strtVex) {
                isCreating=false;
                return;
            }
            strtVex->nexts.push_back(endVex);
            addLine(strtVex,endVex);
            clearSketch();
            isCreating=false;
        }
    }
    else{
        if(isCreating){
            Vex* endVex=addVex(event->pos());
            strtVex->nexts.push_back(endVex);
            addLine(strtVex,endVex);
            clearSketch();
            isCreating=false;}
        else{
            clearSketch();
            addVex(event->pos());
            isCreating=false;}

    }
}

void GC::mouseMoveEvent(QMouseEvent *event){
    if(isCreating){
        clearSketch();
        sketchLine(strtVex->pos() + strtVex->rect().center(), event->pos());
    }
    //    if(event->pos())
}

Vex* GC::addVex(QPointF center, qreal radius)
{
    Vex *newVex = new Vex(center, radius, vexID++);
    myGraphicsScene->addItem(newVex);
    myGraphicsScene->addItem(newVex->nameTag);
    newVex->showAnimation();
    vexes.push_back(newVex);
    return newVex;
}

Line* GC::addLine(Vex *start, Vex *end)
{

    bool ok;
    int i = QInputDialog::getInt(this, tr("最小生成树算法可视化"),
                                 tr("请输入边权值："), 1, 1, 10000, 1, &ok);
    if (!ok)
        i = 1;
    QString s = QString::number(i, 10);

    Line * line=new Line(start,end,i);
    lines.push_back(line);
    myGraphicsScene->addItem(line);

    QGraphicsSimpleTextItem *weight;
    weight = new QGraphicsSimpleTextItem();
    weight->setPos((start->center + end->center)/2);
    weight->setFont(QFont("Corbel", 15, QFont::Normal, true));
    weight->setText(s);
    weight->setBrush(Qt::black);
    weight->setFlags(QGraphicsItem::ItemIsSelectable);
    myGraphicsScene->addItem(weight);
    return line;
}

void GC::sketchLine(QPointF start, QPointF end){
    QGraphicsLineItem *newLine = new QGraphicsLineItem(start.x(), start.y(), end.x(), end.y());
    QPen pen;
    pen.setWidth(3);
    pen.setStyle(Qt::DashLine);
    pen.setBrush(QColor(58, 143, 192, 100));
    pen.setCapStyle(Qt::RoundCap);
    newLine->setPen(pen);
    scene()->addItem(newLine);
    sketchItem = newLine;
}

void GC::clearSketch(){
    if(sketchItem != nullptr){
        scene()->removeItem(sketchItem);
        sketchItem = nullptr;
    }
}
void GC::addAnimation(QTimeLine *ani){
    aniQueue.push_back(ani);
    if(!onAni){
        onAni = true;
        nextAni();
    }
}

void GC::nextAni(){
    if(aniQueue.size() > 0){
        QTimeLine *next = aniQueue.front();
        curAni = next;
        aniQueue.pop_front();
        connect(next, &QTimeLine::finished, [=](){nextAni(); next->deleteLater();});
        next->setDuration(next->duration() / speedRate);
        next->start();
    }
    else{
        onAni = false;
        curAni = nullptr;
    }
}

void GC::stopAnimation(){
    aniQueue.clear();
}

void GC::recoverlinesandvexes(){
    for (Line* a: coverlines) {
        myGraphicsScene->removeItem(a);
    }
    for (Vex* a: vexes) {
        myGraphicsScene->removeItem(a);
        a->setBrush(QColor(108,100,205));
        myGraphicsScene->addItem(a);
    }

}

bool GC::buildMatrix()
{
    int vnumber = vexes.size();
    matrix = std::vector<std::vector<int>>(vnumber,std::vector<int>(vnumber));
    for (int i = 0; i < lines.size(); ++i) {
        matrix[vexes.indexOf(lines[i]->getstart())][vexes.indexOf(lines[i]->getend())] = lines[i]->getweight();
        matrix[vexes.indexOf(lines[i]->getend())][vexes.indexOf(lines[i]->getstart())] = lines[i]->getweight();
    }

    int vis[vnumber];
    for (int i = 0; i < vnumber; ++i) {
        vis[i]=0;
    }
    std::queue<int> q;
    q.push(0);
    while (!q.empty()) {
        int v = q.front();
        //访问v
        q.pop();
        if (!vis[v]) {
            for (int i = 0; i < matrix[v].size(); i++) {
                if (matrix[v][i]!=0&&!vis[i]) {
                    q.push(i);
                }
            }
        }
        vis[v] = 1;
    }
    for (int i = 0; i < vnumber; ++i) {
        if(vis[i]==0){
            QMessageBox::warning(this,
                                 tr("最小生成树"),
                                 tr("该图不连通！"),
                                 QMessageBox::Ok);
            return false;
        }
    }
    return true;
};
QTimeLine* GC::coverline(Vex *start, Vex *end, QColor color)
{
    QTimeLine *timeLine = new QTimeLine(200, this);
    timeLine->setFrameRange(0, 200);
    Line * line=new Line(start,end,color);

    connect(timeLine, &QTimeLine::frameChanged, timeLine, [=](int frame){
        myGraphicsScene->addItem(line);
        coverlines.push_back(line);
    });
    return timeLine;
}

QTimeLine* GC::hightlightcode(QLineEdit* e)
{
    QTimeLine *timeLine = new QTimeLine(500, this);
    timeLine->setFrameRange(0, 200);
    connect(timeLine, &QTimeLine::frameChanged, timeLine, [=](int frame){
        if(frame<30)e->setStyleSheet("background-color:rgba(100,100,100,100)");
        if(frame>170)e->setStyleSheet("background-color:rgba(0,0,0,0)");
    });
    return timeLine;
}
//克鲁斯卡尔算法最小生成树
void GC::kruskal(double speed, QVector<QLineEdit*> es) {
    int sum = 0;
    speedRate = speed/5;
    qDebug()<<"边"<<speedRate;
    for (int var = 0; var < lines.size(); ++var) {
        qDebug()<<"边"<<vexes.indexOf(lines[var]->getstart())<<","<<vexes.indexOf(lines[var]->getend())<<"权="<<lines[var]->getweight();
    }
    int i, start, end, elem, k;
    int visited[vexes.size()];
    //每个顶点配置一个标记值
    int component[vexes.size()];
    int num = 0;
    //初始状态下，每个顶点的标记都不相同
    for (i = 0; i < vexes.size(); i++) {
        component[i] = i;
        visited[i] = 0;
    }
    //根据权值，对所有边进行升序排序
    addAnimation(hightlightcode(es[0]));
    for (int j=1; j<lines.size(); j++)
    {
        int key = lines[j]->getweight();
        Line* keystrc = lines[j];
        int i = j-1;
        while (i>=0 && lines[i]->getweight()>key)
        {
            lines[i+1] = lines[i];
            i--;
        }
        lines[i+1] = keystrc;
    }

    qDebug()<<"排序后：=======================";
    for (int var = 0; var < lines.size(); ++var) {
        qDebug()<<"边"<<vexes.indexOf(lines[var]->getstart())<<","<<vexes.indexOf(lines[var]->getend())<<"权="<<lines[var]->getweight();
    }
    addAnimation(hightlightcode(es[1]));
    //遍历所有的边
    for (i = 0; i < lines.size(); i++) {
        addAnimation(hightlightcode(es[2]));
        //找到当前边的两个顶点在 component 数组中的位置下标
        start = vexes.indexOf(lines[i]->getstart());
        end = vexes.indexOf(lines[i]->getend());
        //如果顶点位置存在且顶点的标记不同，说明不在一个集合中，不会产生回路
        if (component[start] != component[end]) {
            sum+=lines[i]->getweight();
            //记录该边，作为最小生成树的组成部分
            addAnimation(hightlightcode(es[3]));
            addAnimation(hightlightcode(es[4]));
            addAnimation(coverline(lines[i]->getstart(), lines[i]->getend(), QColor(160,200,100)));
            if(!visited[start]){
                addAnimation(lines[i]->getstart()->visit());
                qDebug()<<"点"<<start;
                visited[start] = 1;
            }
            if(!visited[end]){
                addAnimation(lines[i]->getend()->visit());
                qDebug()<<"点"<<end;
                visited[end] = 1;
            }


            qDebug()<<"访问"<<"边"<<vexes.indexOf(lines[i]->getstart())<<","<<vexes.indexOf(lines[i]->getend())<<"权="<<lines[i]->getweight();
            //计数+1
            num++;

            //将新加入生成树的顶点标记全部改为一样的
            elem = component[end];
            for (k = 0; k < vexes.size(); k++) {
                if (component[k] == elem) {
                    component[k] = component[start];
                }
            }
            //如果选择的边的数量和顶点数相差1，证明最小生成树已经形成，退出循环
            if (num == vexes.size() - 1) {
                break;
            }
        }
        else {
            addAnimation(hightlightcode(es[5]));
            addAnimation(coverline(lines[i]->getstart(), lines[i]->getend(), QColor(230,230,230)));
        }
    }
}

void GC::prim(double speed){
    speedRate = speed/5;
    int sign[vexes.size()]; // 标记sign数组 初始化全为0 ，1表示已经加入已选列表
    for (int i = 0; i < vexes.size(); ++i) {
        sign[i]=0;
    }

    int sum=0; // 最小生成树权值之和 初始值为0
    /* 默认从第一个点开始 */
    sign[0]=1;
    addAnimation(vexes[0]->visit());
    int counter=1; // 计算当前状态已加入最小生成树队列的个数

    int flagX=-1,flagY=-1,minNodeValue=1000000;
    while(counter!=vexes.size()) {
        flagX=-1,flagY=-1,minNodeValue=1000000; // 每一次都要初始化
        /* 每一次循环都是一次贪心 当前局势的一种最优解 */
        for(int i=0; i<vexes.size(); i++) {
            if(sign[i]==0) continue; // 如果当前为加入队列 直接下一次循环 其实这是一个笨方法
            for(int j=0; j<vexes.size(); j++) {
                // 在查找下一个连接点时 发现这个连接点已经在里面了（或者为0，即不可达） 我们直接跳过 我能说这是一个笨方法么？但似乎只能这样
                if(sign[j]==1||matrix[i][j]==0) {
                    if(matrix[i][j]!=0 && !sign[j])
                        addAnimation(coverline(vexes[i],vexes[j],QColor(230,230,230)));
                    continue;
                }
                if(matrix[i][j]<minNodeValue){
                    // 更小的一个进行标记
                    flagX=i;
                    flagY=j;
                    minNodeValue=matrix[i][j];
                }
            }
        }
        /* 一轮查找后 */
        addAnimation(coverline(vexes[flagX],vexes[flagY],QColor(160,200,100)));
        qDebug()<<"连边"<<flagX<<","<<flagY;
        addAnimation(vexes[flagY]->visit());
        sign[flagY]=1; // 谁该标1 要清楚
        counter++; // 找到加一
        sum+=matrix[flagX][flagY]; // 这里其实不用担心指针越界 如果是连通图 在一轮查找下来 肯定能找到一个最小的
    }
}


struct edge
{
    int from,to,w;///保存最小生成树的形成路径
    friend bool operator <(edge n1,edge n2)///重载运算符，自定义优先队列的排序规则
    {
        return n1.w>n2.w;
    }
};

void GC::prim2(double speed, QVector<QLineEdit*> es){
    speedRate = speed/5;
    int vnumber = vexes.size();
    int flag[vnumber];
    memset(flag,0,sizeof(flag));
    edge temp,temp_1;
    std::priority_queue<edge>PQ;

    for (int i = 0; i < vnumber; ++i) {
        for (int j = 0; j < vnumber; ++j) {
            qDebug()<<matrix[i][j];
        }
        qDebug()<<"/n";
    }
    for (int i = 0; i < vnumber; ++i) {
        if(matrix[0][i]!=0){
            edge e;
            e.from = 0;
            e.to = i;
            e.w = matrix[0][i];
            qDebug()<<e.w<<"入队";
            PQ.push(e);}
    }
    addAnimation(hightlightcode(es[0]));
    addAnimation(hightlightcode(es[1]));

    flag[0]=1;

    for (int i = 1; i < vnumber; ++i)
    {
        addAnimation(hightlightcode(es[2]));
        while(!PQ.empty()&&flag[PQ.top().to]==1)//如果数据合理，此处不会爆栈
        {
            temp = PQ.top();
            addAnimation(hightlightcode(es[5]));
            addAnimation(coverline(vexes[temp.from], vexes[temp.to], QColor(230,230,230)));
            PQ.pop();
        }


        if(i==1) addAnimation(vexes[0]->visit());


        addAnimation(hightlightcode(es[3]));
        addAnimation(hightlightcode(es[4]));
        temp=PQ.top();
        PQ.pop();

        addAnimation(coverline(vexes[temp.from], vexes[temp.to], QColor(160,200,100)));

        if(!flag[temp.from]){
            addAnimation(vexes[temp.from]->visit());
            qDebug()<<"点"<<temp.from;
        }
        if(!flag[temp.to]){
            addAnimation(vexes[temp.to]->visit());
            qDebug()<<"点"<<temp.to;
        }


        flag[temp.to]=1;//将该点标记为已访问
        qDebug()<<"访问"<<temp.to;




        for(int i=0;i<vnumber;++i)
        {
            if(matrix[temp.to][i]!=0&&!flag[i])
            {
                addAnimation(hightlightcode(es[3]));
                temp_1.from=temp.to;
                temp_1.to=i;
                temp_1.w=matrix[temp.to][i];
                PQ.push(temp_1);
            }
        }
    }
    qDebug()<<"for结束";

}

void GC::init()
{
    if(myGraphicsScene != nullptr)
    {
        delete myGraphicsScene;
        myGraphicsScene = nullptr;
    }

    aniQueue.clear();
    onAni = false;
    curAni = nullptr;

    vexID = 0;
    isCreating = false;

    vexes.clear();
    lines.clear();
    preVexes.clear();
    leaves.clear();
    nullVexes.clear();
    leafLines.clear();

    strtVex = nullptr;
    sketchItem = nullptr;

    myGraphicsScene = new QGraphicsScene();
    myGraphicsScene->setBackgroundBrush(Qt::transparent);
    myGraphicsScene->setSceneRect(0,0,590,390);
    this->setScene(myGraphicsScene);

    root=new Vex(QPointF(100,150),10, vexID++);
    myGraphicsScene->addItem(root->nameTag);
    myGraphicsScene->addItem(root);
    vexes.push_back(root);
}

//*******************************************************************************************

Vex::Vex(QPointF _center, qreal _r, int nameID, QGraphicsItem *parent) :
    QGraphicsEllipseItem(_center.x()-20, _center.y()-20, 40, 40, parent),
    center(_center),
    radius(_r){
    nameText = QString::asprintf("V%d", nameID);
    nameTag = new QGraphicsSimpleTextItem();
    nameTag->setPos(center + QPointF(radius, - radius - QFontMetrics(nameFont).height()));
    nameTag->setFont(nameFont);
    nameTag->setText(nameText);
    nameTag->setZValue(this->zValue());
    nameTag->setBrush(Qt::black);
    nameTag->setFlags(QGraphicsItem::ItemIsSelectable);
    this->setPen(Qt::NoPen);
    this->setBrush(regBrush);
}

Vex::Vex(QPointF _center, qreal _r, QGraphicsItem *parent) :
    QGraphicsEllipseItem(_center.x()-20, _center.y()-20, 40, 40, parent),
    center(_center),
    radius(_r){
    nameText = "nullptr";
    nameTag = new QGraphicsSimpleTextItem();
    nameTag->setPos(center + QPointF(radius, - radius - QFontMetrics(nameFont).height()));
    nameTag->setFont(nameFont);
    nameTag->setText(nameText);
    nameTag->setZValue(this->zValue());
    nameTag->setBrush(Qt::black);
    QPen pen(QColor(108,166,205));
    pen.setStyle(Qt::DashLine);
    pen.setWidth(3);
    this->setPen(pen);
    this->setBrush(QColor(255,255,255));
}

void Vex::setName(QString s)
{
    nameText = s;
    nameTag = new QGraphicsSimpleTextItem();
    nameTag->setPos(this->center + QPointF(10, - 10 - QFontMetrics(nameFont).height()));
    nameTag->setFont(nameFont);
    nameTag->setText(nameText);
    nameTag->setZValue(this->zValue());
    nameTag->setBrush(Qt::black);
    nameTag->setFlags(QGraphicsItem::ItemIsSelectable);
}

QTimeLine* Vex::visit()
{
    QTimeLine *timeLine = new QTimeLine(500, this);
    timeLine->setFrameRange(0, 200);
    QEasingCurve curve = QEasingCurve::OutBounce;
    qreal baseRadius = 26;
    qreal difRadius = -6;
    connect(timeLine, &QTimeLine::frameChanged, timeLine, [=](int frame){
        this->setBrush(visitedBrush);
        qreal curProgress = curve.valueForProgress(frame / 200.0);
        qreal curRadius = baseRadius + difRadius * curProgress;//20
        this->setRect(QRectF(center.x() - curRadius, center.y() - curRadius, curRadius * 2, curRadius * 2));
    });
    return timeLine;
}

void Vex::showAnimation(){
    QTimeLine *timeLine = new QTimeLine(500, this);
    timeLine->setFrameRange(0, 200);
    QEasingCurve curve = QEasingCurve::OutBounce;
    qreal baseRadius = 26;
    qreal difRadius = -6;
    connect(timeLine, &QTimeLine::frameChanged, timeLine, [=](int frame){
        qreal curProgress = curve.valueForProgress(frame / 200.0);
        qreal curRadius = baseRadius + difRadius * curProgress;//20
        this->setRect(QRectF(center.x() - curRadius, center.y() - curRadius, curRadius * 2, curRadius * 2));
    });
    curAnimation = timeLine;
    startAnimation();
}

void Vex::startAnimation(){
    if(curAnimation != nullptr){
        curAnimation->start();
    }
}

//*******************************************************************************************

Line::Line(Vex *start, Vex *end, QGraphicsItem *parent) :
    QGraphicsLineItem(parent),
    startVex(start),
    endVex(end){
    //Set display effect
    defaultPen.setWidth(lineWidth);
    defaultPen.setStyle(lineStyle);
    defaultPen.setCapStyle(capStyle);
    defaultPen.setColor(defaultColor);
    this->setPen(defaultPen);
    this->setLine(startVex->center.rx(),startVex->center.ry(),endVex->center.rx(),endVex->center.ry());
    this->setZValue(-2);
}
Line::Line(Vex *start, Vex *end, int _weight, QGraphicsItem *parent) :
    QGraphicsLineItem(parent),
    weight(_weight),
    startVex(start),
    endVex(end){
    //Set display effect
    defaultPen.setWidth(lineWidth);
    defaultPen.setStyle(lineStyle);
    defaultPen.setCapStyle(capStyle);
    defaultPen.setColor(defaultColor);
    this->setPen(defaultPen);
    this->setLine(startVex->center.rx(),startVex->center.ry(),endVex->center.rx(),endVex->center.ry());
    this->setZValue(-2);
}
Line::Line(Vex *start, Vex *end, QColor color, QGraphicsItem *parent):
    QGraphicsLineItem(parent),
    startVex(start),
    endVex(end){
    defaultPen.setWidth(lineWidth);
    defaultPen.setStyle(lineStyle);
    defaultPen.setCapStyle(capStyle);
    defaultPen.setColor(color);
    this->setPen(defaultPen);
    this->setLine(startVex->center.rx(),startVex->center.ry(),endVex->center.rx(),endVex->center.ry());
    this->setZValue(-1);

}








