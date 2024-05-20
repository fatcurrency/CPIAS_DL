#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(600,600);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->tabWidget->setTabText(0,"2D Slice");
    ui->tabWidget->setTabText(1,"Segmentation");
    ui->tabWidget->setTabText(2,"Volume Render");

    this->sliceLayout = new QVBoxLayout(ui->tab_1);
    this->sliceView = new SingleViewSliceVisualization(ui->tab_1);
    this->sliceLayout->addWidget(this->sliceView);
    this->sliceLayout->setMargin(0);
}

MainWindow::~MainWindow()
{
    delete this->sliceView;
    delete this->sliceLayout;
    delete ui;
}