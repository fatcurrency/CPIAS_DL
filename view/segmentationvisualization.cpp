#include "segmentationvisualization.h"
#include "ui_segmentationvisualization.h"
#include <QMimeData>

SegmentationVisualization::SegmentationVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SegmentationVisualization)
{
    ui->setupUi(this);
    setAcceptDrops(true); // 启用拖拽事件
    ui->horizontalSlider->setRange(0,0);
    ui->viewDirectionComboBox->addItem("Sagittal Plane");
    ui->viewDirectionComboBox->addItem("Coronal Plane");
    ui->viewDirectionComboBox->addItem("Transvers Plane");
    ui->currentSliceLabel->setText("0/0");

    this->renderWindow->SetNumberOfLayers(2);
    this->renderer->SetLayer(0);
    this->maskRenderer->SetLayer(1);
    this->renderWindow->AddRenderer(this->renderer);
    this->renderWindow->AddRenderer(this->maskRenderer);
    this->maskRenderer->GetActiveCamera()->ParallelProjectionOn();
    this->maskRenderer->InteractiveOff();

    this->rendererViewer->SetRenderWindow(this->renderWindow);
    this->rendererViewer->SetRenderer(this->renderer);
    this->rendererViewer->SetSliceOrientation(0);

    this->openGLView = new QVTKOpenGLNativeWidget();
    ui->verticalLayout->addWidget(this->openGLView);

    this->vtkMaskImage = vtkSmartPointer<vtkImageData>::New();
    this->sliceviewVtkOriginImage = vtkSmartPointer<vtkImageData>::New();
    this->vtkFusionImage = vtkSmartPointer<vtkImageData>::New();
    this->rendererViewer->SetInputData(this->sliceviewVtkOriginImage);
    this->openGLView->SetRenderWindow(this->renderWindow);

    this->style->SetInteractionModeToImageSlicing();
    this->renderer->GetActiveCamera()->ParallelProjectionOn();
    this->renderWindow->GetInteractor()->SetInteractorStyle(this->style);
    
    this->maskRenderer->SetActiveCamera(this->renderer->GetActiveCamera());// 直接共用一个camera
    this->rendererViewer->Render();
    this->openGLView->show();

    // 识别camera的改变
    connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    connections->Connect(this->renderer->GetActiveCamera(), vtkCommand::ModifiedEvent, this, SLOT(OnCameraModified()));
}

SegmentationVisualization::~SegmentationVisualization()
{
    delete ui;
}

// 根据一系列的离散点，绘制轮廓线，首尾相连
vtkSmartPointer<vtkActor> SegmentationVisualization::addCurveToRenderer(vtkRenderer* curveRenderer, std::vector<std::vector<double>>& pointsData) {
    vtkSmartPointer<vtkPolyData> curve = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    for (const auto& pointData : pointsData) {
        points->InsertNextPoint(pointData[0], pointData[1], pointData[2]);
    }

    curve->SetPoints(points);
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();

    for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
        line->GetPointIds()->SetId(0, i);
        line->GetPointIds()->SetId(1, (i + 1) % points->GetNumberOfPoints());
        lines->InsertNextCell(line);
    }

    curve->SetLines(lines);
    vtkSmartPointer<vtkPolyDataMapper> curveMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    curveMapper->SetInputData(curve);
    vtkSmartPointer<vtkActor> curveActor = vtkSmartPointer<vtkActor>::New();
    curveActor->SetMapper(curveMapper);
    curveActor->GetProperty()->SetColor(1,0,0); // 设置线条颜色
    curveActor->GetProperty()->SetLineWidth(2.0); // 设置线条粗细
    curveRenderer->AddActor(curveActor);

    return curveActor;
}

// 绘制所有的轮廓
void SegmentationVisualization::drawCurrentSliceContourLine(){
    if (this->maskSliceMat.empty()) {
        std::cout << "The mask image is empty" << std::endl;
        return;
    }
    // 遍历this->curveActorVec，删除renderer中的actor
    for (auto actor : this->curveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->curveActorVec.clear();

    // 获取当前切片索引
    int currentSliceIndex = this->rendererViewer->GetSlice();
    // 获取当前切片的切片轴
    int currentSliceOrientation = this->rendererViewer->GetSliceOrientation();
    // 获取当前切片的图像数据并提取轮廓
    cv::Mat img_data = this->maskSliceMat[currentSliceIndex];
    // 使用 cv::RETR_CCOMP，无需转化为 CV_8UC1
    // 使用 cv::RETR_TREE 或 cv::RETR_LIST ，需要 img_data.convertTo(img_data, CV_8UC1);
//    img_data.convertTo(img_data, CV_8UC1);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    // cv::RETR_TREE      cv::RETR_LIST
    cv::findContours(img_data, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    // 获取this->maskImage的spacing,origin
    double spacing[3];
    double origin[3];
    this->vtkMaskImage->GetSpacing(spacing);
    this->vtkMaskImage->GetOrigin(origin);

    for (int i = 0; i < contours.size(); i++) {
        std::vector<std::vector<double>> pointsData;
        for (int j = 0; j < contours[i].size(); j++) {
            switch (currentSliceOrientation) {
                case 0:
                    pointsData.push_back({currentSliceIndex*spacing[0]+origin[0],
                                          contours[i][j].x*spacing[1]+origin[1],
                                          contours[i][j].y*spacing[2]+origin[2]});
                    break;
                case 1:
                    pointsData.push_back({contours[i][j].x*spacing[0]+origin[0],
                                          currentSliceIndex*spacing[1]+origin[1],
                                          contours[i][j].y*spacing[2]+origin[2]});
                    break;
                case 2:
                    pointsData.push_back({contours[i][j].x*spacing[0]+origin[0],
                                          contours[i][j].y*spacing[1]+origin[1],
                                          currentSliceIndex*spacing[2]+origin[2]});
                    break;
                default:
                    break;
            }
        }
        auto curveAddingActor = addCurveToRenderer(this->maskRenderer, pointsData);
        this->curveActorVec.push_back(curveAddingActor);
    }

    this->rendererViewer->Render();
    this->openGLView->show();
}

// 鼠标拖拽事件
void SegmentationVisualization::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) { // 如果拖拽的数据包含URL（文件路径）
        event->acceptProposedAction(); // 接受拖拽事件
    }
}

// 拖拽文件复制到该窗口中，识别文件路径
void SegmentationVisualization::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls()) {
        QString filePath = url.toLocalFile(); // 获取文件路径
        qDebug() << "Dropped file:" << filePath;
        if (filePath.endsWith(".nii") || filePath.endsWith(".nii.gz")) {
            if(this->sliceviewVtkOriginImage == nullptr){
                std::cout << "Load the original image first" << std::endl;
                return;
            }

            this->niftiPathString = filePath.toStdString();
            this->imageLoader.loadNiftiImage(this->niftiPathString);
            this->vtkMaskImage->DeepCopy(imageLoader.getVTKThreeDImage());
            
            // 将this->vtkMaskImage的数据类型转换为unsigned int
            if (this->vtkMaskImage->GetScalarType() != VTK_UNSIGNED_INT){
                vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
                cast->SetInputData(this->vtkMaskImage);
                cast->SetOutputScalarTypeToUnsignedInt();
                cast->Update();
                this->vtkMaskImage->DeepCopy(cast->GetOutput());
            }
            this->vtkMaskImage->Modified();

            this->vtkMaskImageInfo = SingleViewSliceVisualization::getImageInfo(this->vtkMaskImage);
            // 检测this->vtkMaskImage和this->vtkOriginImage的维度以及spacing是否一致
            if (this->sliceviewVtkOriginImageInfo.dimensions[0] != this->vtkMaskImageInfo.dimensions[0] ||
                this->sliceviewVtkOriginImageInfo.dimensions[1] != this->vtkMaskImageInfo.dimensions[1] ||
                this->sliceviewVtkOriginImageInfo.dimensions[2] != this->vtkMaskImageInfo.dimensions[2] ||
                this->sliceviewVtkOriginImageInfo.spacing[0] != this->vtkMaskImageInfo.spacing[0] ||
                this->sliceviewVtkOriginImageInfo.spacing[1] != this->vtkMaskImageInfo.spacing[1] ||
                this->sliceviewVtkOriginImageInfo.spacing[2] != this->vtkMaskImageInfo.spacing[2]){
                std::cout << "The dimensions or spacing of the two images are not the same" << std::endl;
                return;
            }
            // 判断this->vtkMaskImage是否为二值图像，即只有0和1两个值
            if (this->vtkMaskImageInfo.minValue == 0 && this->vtkMaskImageInfo.maxValue == 1){
                std::cout << "This is a binary image" << std::endl;
                this->vtkMaskImageBinary = true;
            } else {
                std::cout << "This is a grayscale image" << std::endl;
                this->vtkMaskImageBinary = false;
                return;
            }

            // 图像融合，只保留mask中的像素值
            this->vtkFusionImage->DeepCopy(ImageProcessing::fuseOriginalImageByMask(this->sliceviewVtkOriginImage,this->vtkMaskImage,1));
            this->vtkFusionImage->Modified();
            SingleViewSliceVisualization::getImageInfo(this->vtkFusionImage);
            emit sendFusionImageToVolumeView(this->vtkFusionImage);

            if (this->sliceviewVtkOriginImage->GetScalarType() != -1){
                int index = ui->viewDirectionComboBox->currentIndex();
                ui->horizontalSlider->setRange(0,this->sliceviewVtkOriginImageInfo.dimensions[index]-1);
                ui->horizontalSlider->setValue(this->sliceviewVtkOriginImageInfo.dimensions[index]/2);
                this->rendererViewer->SetColorWindow(this->sliceviewVtkOriginImageInfo.maxValue-this->sliceviewVtkOriginImageInfo.minValue);
                this->rendererViewer->SetColorLevel((this->sliceviewVtkOriginImageInfo.maxValue+this->sliceviewVtkOriginImageInfo.minValue)/2);

                this->renderer->ResetCamera();
                this->rendererViewer->Render();
                this->openGLView->repaint();
            }

            this->maskSliceMat = ImageProcessing::GetSlices(this->vtkMaskImage,this->rendererViewer->GetSliceOrientation());
            this->drawCurrentSliceContourLine();
        }
    }
}

// 槽函数，同步接受slice_view加载得到的图像，作为mask轮廓的背景图像
void SegmentationVisualization::setSliceviewVtkOriginImage(vtkSmartPointer<vtkImageData> image){
    std::cout << "setSliceviewVtkOriginImage" << std::endl;
    this->sliceviewVtkOriginImage->DeepCopy(image);
    this->sliceviewVtkOriginImage->Modified();
    this->sliceviewVtkOriginImageInfo = SingleViewSliceVisualization::getImageInfo(this->sliceviewVtkOriginImage);

    // 遍历this->curveActorVec，删除renderer中的actor
    for (auto actor : this->curveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->curveActorVec.clear();

    if(this->sliceviewVtkOriginImage == nullptr)
        return;

    if (this->sliceviewVtkOriginImage->GetScalarType() != -1){
        int index = ui->viewDirectionComboBox->currentIndex();
        ui->horizontalSlider->setRange(0,this->sliceviewVtkOriginImageInfo.dimensions[index]-1);
        ui->horizontalSlider->setValue(this->sliceviewVtkOriginImageInfo.dimensions[index]/2);
        this->rendererViewer->SetColorWindow(this->sliceviewVtkOriginImageInfo.maxValue-this->sliceviewVtkOriginImageInfo.minValue);
        this->rendererViewer->SetColorLevel((this->sliceviewVtkOriginImageInfo.maxValue+this->sliceviewVtkOriginImageInfo.minValue)/2);

        this->renderer->ResetCamera();
        this->rendererViewer->Render();
        this->openGLView->repaint();
    }

}

// 暂时不使用该功能
void SegmentationVisualization::OnCameraModified() {
//    std::cout << "OnCameraModified" << std::endl;
}

// 切换不同的切片 Slice
void SegmentationVisualization::on_horizontalSlider_valueChanged(int value)
{
    this->rendererViewer->SetSlice(value);
    ui->currentSliceLabel->setText(QString::number(value+1) + "/" + QString::number(ui->horizontalSlider->maximum()+1));
    this->drawCurrentSliceContourLine();
}

// 切换不同的轴向 Orientation
void SegmentationVisualization::on_viewDirectionComboBox_currentIndexChanged(int index)
{
    this->rendererViewer->SetSliceOrientation(index);
    this->maskSliceMat = ImageProcessing::GetSlices(this->vtkMaskImage,this->rendererViewer->GetSliceOrientation());
    this->drawCurrentSliceContourLine();
    if(this->sliceviewVtkOriginImage == nullptr)
        return;

    // 判断this->vtkOriginImage是否存在图像数据
    if (this->sliceviewVtkOriginImage->GetScalarType() != -1){
        ui->horizontalSlider->setRange(0,this->sliceviewVtkOriginImageInfo.dimensions[index]-1);
        int middleSlice = this->sliceviewVtkOriginImageInfo.dimensions[index]/2;
        ui->horizontalSlider->setValue(middleSlice);
        ui->currentSliceLabel->setText(QString::number(middleSlice+1) + "/" + QString::number(ui->horizontalSlider->maximum()+1));
        this->renderer->ResetCamera();
        this->rendererViewer->Render();
        this->openGLView->repaint();
    }
}
