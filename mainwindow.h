#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>

#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void setspeed(int val);

private:
    Ui::MainWindow *ui;
    Canvas * cas;//子窗口,用于放置画布
    int choice = 0;//记录选用的算法，1为k，2为p
    void paintEvent(QPaintEvent *);
    void setfakecode(int choice);
};
#endif // MAINWINDOW_H
