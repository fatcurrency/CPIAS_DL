#ifndef VOLUMEVIEWVISUALIZATION_H
#define VOLUMEVIEWVISUALIZATION_H

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)

#include <QWidget>
#include <QDebug>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkNamedColors.h>
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkImageData.h"
#include "vtkColorTransferFunction.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkCamera.h"
#include "vtkCursor3D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkOutlineFilter.h"

#include "singleviewslicevisualization.h"
#include "basictool/colormapTableBox.h"

namespace Ui {
class VolumeViewVisualization;
}

class VolumeViewVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit VolumeViewVisualization(QWidget *parent = 0);
    ~VolumeViewVisualization();
    void changeColorTransferFunction(int index);//通过索引修改颜色表

signals:
    void windowShown();

private slots:
    void showEvent(QShowEvent *event);

    void on_comboBox_currentIndexChanged(int index);

public slots:
    void setThreeDImage(vtkSmartPointer<vtkImageData> imageData);
    void displayThreeDImage();//3D渲染

private:
    Ui::VolumeViewVisualization *ui;
    QVTKOpenGLNativeWidget* openGLView;
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkImageData> threeDImageData;

    //三维图形数据
    vtkNew<vtkVolume> ThreeDVolume;
    vtkNew<vtkGPUVolumeRayCastMapper> ThreeDMapper;
    vtkNew<vtkVolumeProperty> ThreeDProperty;
    //颜色以及不透明度的控制设置
    vtkNew<vtkPiecewiseFunction> ThreeDPopacity;
    vtkNew<vtkPiecewiseFunction> volumeGradientOpacity;
    vtkNew<vtkColorTransferFunction> volumeColor;
    //渲染窗口的设置
    vtkNew<vtkGenericOpenGLRenderWindow> ThreeDWindow;
    vtkNew<vtkRenderer> ThreeDRenderer;
    vtkNew<vtkRenderWindowInteractor> ThreeDInteractor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;

    // 创建一个轮廓过滤器
    vtkNew<vtkOutlineFilter> outlineFilter;
    vtkNew<vtkPolyDataMapper> outlineMapper;
    vtkNew<vtkActor> outlineActor;
};

#endif // VOLUMEVIEWVISUALIZATION_H
