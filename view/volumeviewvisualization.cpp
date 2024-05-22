#include "volumeviewvisualization.h"
#include "ui_volumeviewvisualization.h"

VolumeViewVisualization::VolumeViewVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumeViewVisualization)
{
    ui->setupUi(this);
    for (auto it = stringVector.begin(); it != stringVector.end(); ++it) {
      ui->comboBox->addItem(QString::fromStdString(*it));
    }

    this->openGLView = new QVTKOpenGLNativeWidget();
    ui->verticalLayout->addWidget(this->openGLView);

    //设置volume render
    this->openGLView->SetRenderWindow(this->ThreeDWindow);
    this->ThreeDWindow->AddRenderer(this->ThreeDRenderer);
    this->ThreeDInteractor->SetInteractorStyle(style);
    this->ThreeDInteractor->SetRenderWindow(this->ThreeDWindow);

    this->ThreeDProperty->SetScalarOpacity(this->ThreeDPopacity);
    this->ThreeDVolume->SetMapper(this->ThreeDMapper);
    this->ThreeDVolume->SetProperty(this->ThreeDProperty);

    this->ThreeDRenderer->ResetCamera();
    this->ThreeDWindow->Render();
    this->openGLView->show();

    // 当窗口显示后，再调用displayThreeDImage函数
    connect(this, &VolumeViewVisualization::windowShown, this, &VolumeViewVisualization::displayThreeDImage);
}

VolumeViewVisualization::~VolumeViewVisualization()
{
    delete ui;
}

void VolumeViewVisualization::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    // 当窗口显示后，发出windowShown信号
    emit windowShown();
}

void VolumeViewVisualization::setThreeDImage(vtkSmartPointer<vtkImageData> imageData){
    std::cout << "setThreeDImage" << std::endl;
    this->threeDImageData->DeepCopy(imageData);
    this->threeDImageData->Modified();
}

void VolumeViewVisualization::displayThreeDImage(){
    std::cout << "displayThreeDImage" << std::endl;
    // 判断this->threeDImageData是否存在像素数据
    if(this->threeDImageData->GetScalarPointer() == nullptr){
        return;
    }
    this->ThreeDRenderer->RemoveActor(this->ThreeDVolume);
    this->ThreeDRenderer->RemoveActor(this->outlineActor);
    this->ThreeDRenderer->Modified();

    double scalarRange[2];
    this->threeDImageData->GetScalarRange(scalarRange);

    double minValue = scalarRange[0];
    double maxValue = scalarRange[1];

    //设置volume render

    //用于控制不透明度随着像素值的变化
    this->ThreeDPopacity->RemoveAllPoints();
    this->ThreeDPopacity->AddPoint(0.0,0.0);
    this->ThreeDPopacity->AddPoint(minValue,0.0);
    this->ThreeDPopacity->AddPoint(maxValue,1.0);
    this->ThreeDPopacity->ClampingOn();
    this->ThreeDPopacity->Modified();
    //用于控制不透明度随着梯度的变化
    this->volumeGradientOpacity->AddPoint(minValue, 0.0);
    this->volumeGradientOpacity->AddPoint(maxValue, 1.0);
    //用于控制颜色值（RGB）随着像素值的变化
    PredefineLookUpTable::greyTransFunc(minValue,maxValue,volumeColor);

    //体数据中每个像素的不透明度如何随着其梯度值的变化而变化
    this->ThreeDProperty->SetGradientOpacity(volumeGradientOpacity);
    //体数据中每个像素的不透明度如何随着其标量值的变化而变化
    this->ThreeDProperty->SetScalarOpacity(ThreeDPopacity);
    //体数据中每个像素的颜色如何随着其标量值的变化而变化
    this->ThreeDProperty->SetColor(volumeColor);
    this->ThreeDProperty->Modified();

    this->ThreeDMapper->RemoveAllInputs();
    this->ThreeDMapper->SetInputData(this->threeDImageData);
    this->ThreeDMapper->Update();

    this->ThreeDVolume->SetMapper(this->ThreeDMapper);
    this->ThreeDVolume->SetProperty(this->ThreeDProperty);
    this->ThreeDVolume->Update();

    this->ThreeDRenderer->AddActor(this->ThreeDVolume);//render中添加Volume

    // 三维方框
    this->outlineFilter->SetInputData(this->threeDImageData);
    this->outlineFilter->Update();
    this->outlineMapper->SetInputConnection(this->outlineFilter->GetOutputPort());
    this->outlineActor->SetMapper(this->outlineMapper);
    this->outlineActor->GetProperty()->SetColor(1.0, 1.0, 1.0); // 设置颜色为白色
    this->ThreeDRenderer->AddActor(this->outlineActor);

    this->ThreeDWindow->AddRenderer(this->ThreeDRenderer);
    this->ThreeDRenderer->GetActiveCamera()->SetFocalPoint(this->threeDImageData->GetCenter());
    this->openGLView->SetRenderWindow(this->ThreeDWindow);
    this->openGLView->GetRenderWindow()->Render();

    this->changeColorTransferFunction(ui->comboBox->currentIndex());

    this->ThreeDRenderer->ResetCamera();
    this->openGLView->GetRenderWindow()->Render();
    this->repaint();
}

void VolumeViewVisualization::changeColorTransferFunction(int index){
    TransFuncArray[index](this->threeDImageData->GetScalarRange()[0],this->threeDImageData->GetScalarRange()[1],volumeColor);

    ThreeDRenderer->Render();
    ThreeDWindow->Render();
    this->openGLView->GetRenderWindow()->Render();
}

void VolumeViewVisualization::on_comboBox_currentIndexChanged(int index)
{
    // 判断this->threeDImageData是否存在像素数据
    if(this->threeDImageData->GetScalarPointer() == nullptr){
        ui->comboBox->setCurrentIndex(0);
        return;
    }
    this->changeColorTransferFunction(index);
}
