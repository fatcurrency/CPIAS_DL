#include "singleviewslicevisualization.h"
#include "ui_singleviewslicevisualization.h"
#include <QMimeData>

SingleViewSliceVisualization::SingleViewSliceVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleViewSliceVisualization)
{
    ui->setupUi(this);
    setAcceptDrops(true); // 启用拖拽事件
    ui->horizontalSlider->setRange(0,0);
    ui->viewDirectionComboBox->addItem("Sagittal Plane");
    ui->viewDirectionComboBox->addItem("Coronal Plane");
    ui->viewDirectionComboBox->addItem("Transvers Plane");
    ui->currentSliceLabel->setText("0/0");

    this->rendererViewer->SetRenderWindow(this->renderWindow);
    this->rendererViewer->SetRenderer(this->renderer);
    this->rendererViewer->SetSliceOrientation(0);

    this->openGLView = new QVTKOpenGLNativeWidget();
    ui->verticalLayout->addWidget(this->openGLView);

    this->vtkOriginImage = vtkSmartPointer<vtkImageData>::New();
    this->rendererViewer->SetInputData(this->vtkOriginImage);
    this->openGLView->SetRenderWindow(this->renderWindow);

    this->style->SetInteractionModeToImageSlicing();
    this->renderer->GetActiveCamera()->ParallelProjectionOn();
    this->renderWindow->GetInteractor()->SetInteractorStyle(this->style);

    this->rendererViewer->Render();
    this->openGLView->show();
}

SingleViewSliceVisualization::~SingleViewSliceVisualization()
{
    delete ui;
}

ImageInfo SingleViewSliceVisualization::getImageInfo(vtkSmartPointer<vtkImageData> image)
{
    ImageInfo info;
    image->GetDimensions(info.dimensions);
    image->GetSpacing(info.spacing);
    image->GetOrigin(info.origin);
    image->GetCenter(info.center);
    double range[2];
    image->GetScalarRange(range);
    info.minValue = range[0];
    info.maxValue = range[1];
    info.dataType = image->GetScalarType();

    // 输出图像信息
    qDebug() << "Image dimensions: " << info.dimensions[0] << "x" << info.dimensions[1] << "x" << info.dimensions[2];
    qDebug() << "Image spacing: " << info.spacing[0] << "x" << info.spacing[1] << "x" << info.spacing[2];
    qDebug() << "Image origin: (" << info.origin[0] << "," << info.origin[1] << "," << info.origin[2] << ")";
    qDebug() << "Image center: (" << info.center[0] << "," << info.center[1] << "," << info.center[2] << ")";
    qDebug() << "Image scalar range: " << info.minValue << " - " << info.maxValue;
    qDebug() << "Image data type: " << info.dataType;

    return info;
}

void SingleViewSliceVisualization::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) { // 如果拖拽的数据包含URL（文件路径）
        event->acceptProposedAction(); // 接受拖拽事件
    }
}

void SingleViewSliceVisualization::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls()) {
        QString filePath = url.toLocalFile(); // 获取文件路径
        qDebug() << "Dropped file:" << filePath;
        if (filePath.endsWith(".nii") || filePath.endsWith(".nii.gz")) {
            qDebug() << "This is a NIfTI file";
            this->niftiPathString = filePath.toStdString();
            this->imageLoader.loadNiftiImage(this->niftiPathString);
            this->vtkOriginImage->DeepCopy(imageLoader.getVTKThreeDImage());
            this->vtkOriginImage->Modified();
            this->vtkOriginImageInfo = getImageInfo(this->vtkOriginImage);

            int index = ui->viewDirectionComboBox->currentIndex();
            ui->horizontalSlider->setRange(0,this->vtkOriginImageInfo.dimensions[index]-1);
            ui->horizontalSlider->setValue(this->vtkOriginImageInfo.dimensions[index]/2);
            this->rendererViewer->SetColorWindow(this->vtkOriginImageInfo.maxValue-this->vtkOriginImageInfo.minValue);
            this->rendererViewer->SetColorLevel((this->vtkOriginImageInfo.maxValue+this->vtkOriginImageInfo.minValue)/2);

            this->renderer->ResetCamera();
            this->rendererViewer->Render();
            this->openGLView->repaint();

            emit sendOriginalImageToSegmentation(this->vtkOriginImage);
        }
    }
}

void SingleViewSliceVisualization::on_horizontalSlider_valueChanged(int value)
{
    this->rendererViewer->SetSlice(value);
    ui->currentSliceLabel->setText(QString::number(value+1) + "/" + QString::number(ui->horizontalSlider->maximum()+1));
}

void SingleViewSliceVisualization::on_viewDirectionComboBox_currentIndexChanged(int index)
{
    this->rendererViewer->SetSliceOrientation(index);
    if(this->vtkOriginImage == nullptr)
        return;

    // 判断this->vtkOriginImage是否存在图像数据
    if (this->vtkOriginImage->GetScalarType() != -1){
        ui->horizontalSlider->setRange(0,this->vtkOriginImageInfo.dimensions[index]-1);
        int middleSlice = this->vtkOriginImageInfo.dimensions[index]/2;
        ui->horizontalSlider->setValue(middleSlice);
        ui->currentSliceLabel->setText(QString::number(middleSlice+1) + "/" + QString::number(ui->horizontalSlider->maximum()+1));
        this->renderer->ResetCamera();
        this->rendererViewer->Render();
        this->openGLView->repaint();
    }
}
