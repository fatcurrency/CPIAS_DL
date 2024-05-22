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
    vtkSmartPointer<vtkActor> addCurveToRenderer(vtkRenderer* curveRenderer, std::vector<std::vector<double>>& pointsData);
    void drawCurrentSliceContourLine();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

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

    vtkSmartPointer<vtkEventQtSlotConnect> connections;

    std::string niftiPathString;
    bool vtkMaskImageBinary;
    std::vector<cv::Mat> maskSliceMat;
    std::vector<vtkSmartPointer<vtkActor>> curveActorVec;

};

#endif // SEGMENTATIONVISUALIZATION_H
