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

    this->segmentationLayout = new QVBoxLayout(ui->tab_2);
    this->segmentationView = new SegmentationVisualization(ui->tab_2);
    this->segmentationLayout->addWidget(this->segmentationView);
    this->segmentationLayout->setMargin(0);

    this->volumeLayout = new QVBoxLayout(ui->tab_3);
    this->volumeView = new VolumeViewVisualization(ui->tab_3);
    this->volumeLayout->addWidget(this->volumeView);
    this->volumeLayout->setMargin(0);

    // 将this->sliceView加载的图像同步到this->segmentationView作为mask 轮廓的背景显示
    connect(this->sliceView, &SingleViewSliceVisualization::sendOriginalImageToSegmentation, this->segmentationView, &SegmentationVisualization::setSliceviewVtkOriginImage);
    connect(this->segmentationView, &SegmentationVisualization::sendFusionImageToVolumeView, this->volumeView, &VolumeViewVisualization::setThreeDImage);
}

MainWindow::~MainWindow()
{
    delete this->sliceView;
    delete this->sliceLayout;
    delete this->segmentationView;
    delete this->segmentationLayout;
    delete this->volumeView;
    delete this->volumeLayout;
    delete ui;
}
