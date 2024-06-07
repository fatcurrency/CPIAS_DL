#ifndef SEGMENTATIONVISUALIZATION_H
#define SEGMENTATIONVISUALIZATION_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageViewer2.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkCamera.h>
#include <vtkImageCast.h>

#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkEventQtSlotConnect.h>

#include "basictool/imageloadingbox.h"
#include "singleviewslicevisualization.h"

namespace Ui {
class SegmentationVisualization;
}

class SegmentationVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit SegmentationVisualization(QWidget *parent = 0);
    ~SegmentationVisualization();
    vtkSmartPointer<vtkActor> addCurveToRenderer(vtkRenderer* curveRenderer, std::vector<std::vector<double>>& pointsData,int colorIndex = 0);
    void drawCurrentSliceContourLine();
    std::vector<std::vector<std::vector<double>>> extractingContours(vtkSmartPointer<vtkImageData> mask, std::vector<cv::Mat> matVec);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void sendFusionImageToVolumeView(vtkSmartPointer<vtkImageData> image);

public slots:
    void setSliceviewVtkOriginImage(vtkSmartPointer<vtkImageData> image);
    void OnCameraModified();

private slots:
    void on_horizontalSlider_valueChanged(int value);
    void on_viewDirectionComboBox_currentIndexChanged(int index);

private:
    Ui::SegmentationVisualization *ui;
    QVTKOpenGLNativeWidget* openGLView;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderer> maskRenderer;
    vtkNew<vtkImageViewer2> rendererViewer;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkNew<vtkInteractorStyleImage> style;

    ImageLoading imageLoader;
    vtkSmartPointer<vtkImageData> sliceviewVtkOriginImage;
    ImageInfo sliceviewVtkOriginImageInfo;
    vtkSmartPointer<vtkImageData> vtkMaskImage;
    ImageInfo vtkMaskImageInfo;
    std::vector<vtkSmartPointer<vtkImageData>> contourMaskVec; //所有轮廓mask（因为CV轮廓函数只能处理二值图像）
    vtkSmartPointer<vtkImageData> vtkFusionImage;
    vtkSmartPointer<vtkEventQtSlotConnect> connections;
    std::string niftiPathString;

    std::vector<cv::Mat> myocardialMaskSliceMat;
    std::vector<vtkSmartPointer<vtkActor>> myocardialCurveActorVec;

    std::vector<cv::Mat> lvVolumeMaskSliceMat;
    std::vector<vtkSmartPointer<vtkActor>> lvVolumeCurveActorVec;

    std::vector<cv::Mat> rvVolumeMaskSliceMat;
    std::vector<vtkSmartPointer<vtkActor>> rvVolumeCurveActorVec;

    std::vector<cv::Mat> otherMaskSliceMat;
    std::vector<vtkSmartPointer<vtkActor>> otherCurveActorVec;

};

#endif // SEGMENTATIONVISUALIZATION_H
