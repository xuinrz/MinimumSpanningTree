#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(920,700);
    this->setWindowIcon(QIcon(":/res/icon.png"));
    this->setWindowTitle("最小生成树算法可视化");

    cas=new Canvas();
    cas->setParent(this);
    cas->move(50,30);

    ui->lineEdit->setText("请选择一种算法");
    QVector<QLineEdit*> es;
    es.push_back(ui->lineEdit_3);
    es.push_back(ui->lineEdit_4);
    es.push_back(ui->lineEdit_5);
    es.push_back(ui->lineEdit_6);
    es.push_back(ui->lineEdit_7);
    es.push_back(ui->lineEdit_8);
    //事件：开始运行算法
    QObject::connect(ui->pushButton_4,&QPushButton::clicked,this,[=](){
        if(!(this->cas->gv->buildMatrix()))return;
        if(choice==1){
            this->cas->resetcolor();
            this->cas->gv->kruskal(ui->verticalSlider->value(),es);
        }else if(choice==2){
            this->cas->resetcolor();
            this->cas->gv->prim2(ui->verticalSlider->value(),es);
        }else{
            QMessageBox::information(this,
                                     tr("最小生成树"),
                                     tr("请选择算法后运行"),
                                     QMessageBox::Ok);
        }
    });

    //事件：清空画布
    QObject::connect(ui->pushButton_3,&QPushButton::clicked,this,[=](){
        this->cas->clear();
    });

    //事件 改变速度
    connect(ui->verticalSlider,SIGNAL(valueChanged(int)),this,SLOT(setspeed(int)));



    //事件：选择Kruskal
    QObject::connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        choice=1;
        setfakecode(choice);
        ui->lineEdit->setText("当前：Kruskal算法");
    });
    //事件：选择Prim
    QObject::connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        choice=2;
        setfakecode(choice);
        ui->lineEdit->setText("当前：Prim算法");
    });


}

void MainWindow::setfakecode(int choice){
    if(choice==1){
        ui->lineEdit_3->setText("   Sort E edges by increasing weight");
        ui->lineEdit_4->setText("   T = {}");
        ui->lineEdit_5->setText("   for (i = 0; i < edges.length && T.length< n-1; i++) ");
        ui->lineEdit_6->setText("           if adding e = edgelist[i] does not form a cycle");
        ui->lineEdit_7->setText("                   add e to T");
        ui->lineEdit_8->setText("           else ignore e");
    }
    else if(choice==2){
        ui->lineEdit_3->setText("   T = {V0}");
        ui->lineEdit_4->setText("   enqueue edges connected to V0 in PQ (by inc weight)");
        ui->lineEdit_5->setText("   while (!PQ.isEmpty)");
        ui->lineEdit_6->setText("       if (vertex v linked with e = PQ.remove ∉ T)");
        ui->lineEdit_7->setText("           T = T ∪ {v, e}, enqueue edges connected to v");
        ui->lineEdit_8->setText("       else ignore e");

    }
}

void MainWindow::setspeed(int val){
    double d = val;
    this->cas->gv->speedRate = d/5;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen(QColor(100,100,100,50));
    pen.setWidth(5);
    painter.setPen(pen);
    QBrush brush(QColor(255,255,255));
    painter.setBrush(brush);
    QRect rect=this->rect();
    painter.drawRoundedRect(rect,0,0);
}
MainWindow::~MainWindow()
{
    delete ui;
}
