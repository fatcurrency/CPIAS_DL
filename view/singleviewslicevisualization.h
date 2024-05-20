#ifndef SINGLEVIEWSLICEVISUALIZATION_H
#define SINGLEVIEWSLICEVISUALIZATION_H

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

#include "basictool/imageloadingbox.h"

struct ImageInfo
{
    int dimensions[3]; // 图像的维度
    double spacing[3]; // 像素间距
    int dataType; // 像素数据类型
    double minValue; // 数据最小值
    double maxValue; // 数据最大值
};

namespace Ui {
class SingleViewSliceVisualization;
}

class SingleViewSliceVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit SingleViewSliceVisualization(QWidget *parent = 0);
    ~SingleViewSliceVisualization();
    ImageInfo getImageInfo(vtkSmartPointer<vtkImageData> image);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_viewDirectionComboBox_currentIndexChanged(int index);

private:
    Ui::SingleViewSliceVisualization *ui;
    QVTKOpenGLNativeWidget* openGLView;
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkImageViewer2> rendererViewer;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkNew<vtkInteractorStyleImage> style;

    ImageLoading imageLoader;
    vtkSmartPointer<vtkImageData> vtkOriginImage;
    ImageInfo vtkOriginImageInfo;
    std::string niftiPathString;
};

#endif // SINGLEVIEWSLICEVISUALIZATION_H
