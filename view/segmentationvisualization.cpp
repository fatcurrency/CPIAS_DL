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
vtkSmartPointer<vtkActor> SegmentationVisualization::addCurveToRenderer(vtkRenderer* curveRenderer, std::vector<std::vector<double>>& pointsData, int colorIndex) {
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
    // 现在只实现三种线条颜色，对应mask中值为1，2，3，对应红绿蓝，其他值轮廓设置为白色
    if(colorIndex == 0){
        curveActor->GetProperty()->SetColor(1,0,0); // 设置线条颜色
    }else if (colorIndex == 1){
        curveActor->GetProperty()->SetColor(0,1,0); // 设置线条颜色
    }else if (colorIndex == 2){
        curveActor->GetProperty()->SetColor(0,0,1); // 设置线条颜色
    }else {
        curveActor->GetProperty()->SetColor(1,1,1); // 设置线条颜色
    }

    curveActor->GetProperty()->SetLineWidth(2.0); // 设置线条粗细
    curveRenderer->AddActor(curveActor);

    return curveActor;
}

// 绘制所有的轮廓
void SegmentationVisualization::drawCurrentSliceContourLine(){
    if (this->myocardialMaskSliceMat.empty() && this->lvVolumeMaskSliceMat.empty()
            && this->rvVolumeMaskSliceMat.empty() && this->otherMaskSliceMat.empty()) {
        std::cout << "The mask image is empty" << std::endl;
        return;
    }
    // 遍历this->myocardialCurveActorVec ，删除renderer中的actor
    for (auto actor : this->myocardialCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->myocardialCurveActorVec.clear();
    // 遍历this->lvVolumeCurveActorVec ，删除renderer中的actor
    for (auto actor : this->lvVolumeCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->lvVolumeCurveActorVec.clear();
    // 遍历this->rvVolumeCurveActorVec ，删除renderer中的actor
    for (auto actor : this->rvVolumeCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->rvVolumeCurveActorVec.clear();
    // 遍历this->otherCurveActorVec ，删除renderer中的actor
    for (auto actor : this->otherCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->otherCurveActorVec.clear();


    // 遍历this->contourMaskVec
    int contourMaskIndex = 0;
    // this->contourMaskVec表示轮廓mask图像容器，前三个分别为心肌（0），左心室血池（1），右心室血池（2）,剩下归类为其它
    for (auto contourMask : this->contourMaskVec) {
        if (contourMaskIndex == 0){
            auto pointsDataVec = extractingContours(contourMask, this->myocardialMaskSliceMat);
            for (auto pointsData : pointsDataVec){
                vtkSmartPointer<vtkActor> curveAddingActor = addCurveToRenderer(this->maskRenderer, pointsData,contourMaskIndex);
                this->myocardialCurveActorVec.push_back(curveAddingActor);
            }
        }else if (contourMaskIndex == 1) {
            auto pointsDataVec = extractingContours(contourMask, this->lvVolumeMaskSliceMat);
            for (auto pointsData : pointsDataVec){
                vtkSmartPointer<vtkActor> curveAddingActor = addCurveToRenderer(this->maskRenderer, pointsData,contourMaskIndex);
                this->lvVolumeCurveActorVec.push_back(curveAddingActor);
            }
        }else if (contourMaskIndex == 2) {
            auto pointsDataVec = extractingContours(contourMask, this->rvVolumeMaskSliceMat);
            for (auto pointsData : pointsDataVec){
                vtkSmartPointer<vtkActor> curveAddingActor = addCurveToRenderer(this->maskRenderer, pointsData,contourMaskIndex);
                this->rvVolumeCurveActorVec.push_back(curveAddingActor);
            }
        }else {
            auto pointsDataVec = extractingContours(contourMask, this->otherMaskSliceMat);
            for (auto pointsData : pointsDataVec){
                vtkSmartPointer<vtkActor> curveAddingActor = addCurveToRenderer(this->maskRenderer, pointsData,contourMaskIndex);
                this->otherCurveActorVec.push_back(curveAddingActor);
            }
        }
        contourMaskIndex++;
    }

    this->rendererViewer->Render();
    this->openGLView->show();
}

std::vector<std::vector<std::vector<double>>> SegmentationVisualization::extractingContours(vtkSmartPointer<vtkImageData> mask, std::vector<cv::Mat> matVec){
    // 判断mask是否为空
    if(mask == nullptr){
        return {};
    }
    int currentSliceIndex = this->rendererViewer->GetSlice(); // 获取当前切片索引
    int currentSliceOrientation = this->rendererViewer->GetSliceOrientation(); // 获取当前切片的切片轴
    cv::Mat img_data = matVec[currentSliceIndex]; // 获取当前切片的mask图像数据

    // 判断img_data是否存在轮廓
    if (cv::countNonZero(img_data) == 0){
        return {};
    }

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_data, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    // 获取this->maskImage的spacing,origin
    double spacing[3];
    double origin[3];
    mask->GetSpacing(spacing);
    mask->GetOrigin(origin);

    std::vector<std::vector<std::vector<double>>> pointsDataVec;
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
        pointsDataVec.push_back(pointsData);
    }
    return pointsDataVec;
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
            this->contourMaskVec.clear();
            this->contourMaskVec.resize(4);
            // 从sliceviewVtkOriginImageInfo中最小值循环到最大值
            for(int i = 1; i <= this->contourMaskVec.size(); i++){
                vtkSmartPointer<vtkImageData> multiMaskImage;
                if(i <= 3){
                    multiMaskImage = ImageProcessing::extractBinaryImage(this->vtkMaskImage,i);
                }else {
                    multiMaskImage = ImageProcessing::extractGreaterBinaryImage(this->vtkMaskImage,i);
                }

//                 this->contourMaskVec.push_back(multiMaskImage);
                 if(i == 1){
                     this->contourMaskVec[0] = multiMaskImage;
                     this->myocardialMaskSliceMat = ImageProcessing::GetSlices(multiMaskImage,this->rendererViewer->GetSliceOrientation());
                 }else if (i == 2) {
                     this->contourMaskVec[1] = multiMaskImage;
                     this->lvVolumeMaskSliceMat = ImageProcessing::GetSlices(multiMaskImage,this->rendererViewer->GetSliceOrientation());
                 }else if (i == 3) {
                     this->contourMaskVec[2] = multiMaskImage;
                     this->rvVolumeMaskSliceMat = ImageProcessing::GetSlices(multiMaskImage,this->rendererViewer->GetSliceOrientation());
                 }else{
                     this->contourMaskVec[3] = multiMaskImage;
                     this->otherMaskSliceMat = ImageProcessing::GetSlices(multiMaskImage,this->rendererViewer->GetSliceOrientation());
                 }
                 if(multiMaskImage != nullptr){
                     ImageLoading::saveVTKimageToNII(multiMaskImage,"maskTest"+std::to_string(i));
                 }
            }
            std::cout << "this->contourMaskVec size: " << this->contourMaskVec.size() << std::endl;

            // 图像融合，只保留mask中指定的像素值
            this->vtkFusionImage->DeepCopy(ImageProcessing::fuseOriginalImageByMask(this->sliceviewVtkOriginImage,this->vtkMaskImage,1));
            this->vtkFusionImage->Modified();
            SingleViewSliceVisualization::getImageInfo(this->vtkFusionImage);
            // 修改volume render可视化为mask区域内的图像
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
            this->drawCurrentSliceContourLine();
        }
    }
}

// 槽函数，同步接受slice_view加载得到的图像，作为mask轮廓的背景图像
void SegmentationVisualization::setSliceviewVtkOriginImage(vtkSmartPointer<vtkImageData> image){
    std::cout << "setSliceviewVtkOriginImage" << std::endl;
    this->myocardialMaskSliceMat.clear(); // 清除轮廓可视化
    this->lvVolumeMaskSliceMat.clear(); // 清除轮廓可视化
    this->rvVolumeMaskSliceMat.clear(); // 清除轮廓可视化
    this->otherMaskSliceMat.clear(); // 清除轮廓可视化

    this->sliceviewVtkOriginImage->DeepCopy(image);
    this->sliceviewVtkOriginImage->Modified();

    // volume render可视化原始图像
    emit sendFusionImageToVolumeView(this->sliceviewVtkOriginImage); // 清除之前的fusion volume render

    this->sliceviewVtkOriginImageInfo = SingleViewSliceVisualization::getImageInfo(this->sliceviewVtkOriginImage);

    // 遍历this->myocardialCurveActorVec ，删除renderer中的actor
    for (auto actor : this->myocardialCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->myocardialCurveActorVec.clear();
    // 遍历this->lvVolumeCurveActorVec ，删除renderer中的actor
    for (auto actor : this->lvVolumeCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->lvVolumeCurveActorVec.clear();
    // 遍历this->rvVolumeCurveActorVec ，删除renderer中的actor
    for (auto actor : this->rvVolumeCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->rvVolumeCurveActorVec.clear();
    // 遍历this->otherCurveActorVec ，删除renderer中的actor
    for (auto actor : this->otherCurveActorVec) {
        this->maskRenderer->RemoveActor(actor);
    }
    this->otherCurveActorVec.clear();

    this->maskRenderer->Clear();
    this->maskRenderer->Modified();

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
    this->contourMaskVec.resize(4);
    if(this->contourMaskVec[0] != nullptr){
        this->myocardialMaskSliceMat = ImageProcessing::GetSlices(this->contourMaskVec[0],this->rendererViewer->GetSliceOrientation());
    }
    if(this->contourMaskVec[1] != nullptr){
        this->lvVolumeMaskSliceMat = ImageProcessing::GetSlices(this->contourMaskVec[1],this->rendererViewer->GetSliceOrientation());
    }
    if(this->contourMaskVec[2] != nullptr){
        this->rvVolumeMaskSliceMat = ImageProcessing::GetSlices(this->contourMaskVec[2],this->rendererViewer->GetSliceOrientation());
    }
    if(this->contourMaskVec[3] != nullptr){
        this->otherMaskSliceMat = ImageProcessing::GetSlices(this->contourMaskVec[3],this->rendererViewer->GetSliceOrientation());
    }

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
