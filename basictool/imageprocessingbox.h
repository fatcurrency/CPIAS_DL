#ifndef IMAGEPROCESSINGBOX_H
#define IMAGEPROCESSINGBOX_H

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkAffineTransform.h"
#include "itkFlipImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageFileWriter.h"

#include <vtkImageFlip.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkImageReslice.h>
#include <vtkImageShiftScale.h>
#include <vtkImageMathematics.h>
#include <vtkExtractVOI.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDelaunay3D.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkMassProperties.h>
#include <opencv2/opencv.hpp>
#include "mydata.h"


using PixelType = float; //像素值数据类型
using ThreeDImageType = itk::Image<PixelType, 3>; //3D图像
using FourDImageType = itk::Image<PixelType, 4>; //4D图像（门控或者动态图像）
using SeriesIdContainer = std::vector<std::string>; //series列表
using FileNamesContainer = std::vector<std::string>; //文件名列表
using VTKToITKFilterType = itk::VTKImageToImageFilter<ThreeDImageType>; //vtk转itk
using WriterType = itk::ImageFileWriter<ThreeDImageType>; //文件写入

//存储图像的信息
struct vtkImageInf{
    int dimension[3];
    int extent[6];
    double spacing[3];
    double origin[3];
    double bound[6];
    double center[3];
    double scalarRange[2];
};

using TransformType = itk::AffineTransform<double, 3>;
using ResampleThreeDImageFilterType = itk::ResampleImageFilter<ThreeDImageType, ThreeDImageType>;

using PixelType = float;//像素值数据类型
using ThreeDImageType = itk::Image<PixelType, 3>;//3D图像
using AffineTransformType = itk::AffineTransform<double, 3>;//仿射变换

const double xAxisArr[3] = {1, 0, 0};
const itk::AffineTransform<double, 3>::InputVectorType xAxis(xAxisArr);
const itk::FlipImageFilter<ThreeDImageType>::FlipAxesArrayType xFlipAxis(xAxisArr);

const double yAxisArr[3] = {0, 1, 0};
const itk::AffineTransform<double, 3>::InputVectorType yAxis(yAxisArr);
const itk::FlipImageFilter<ThreeDImageType>::FlipAxesArrayType yFlipAxis(yAxisArr);

const double zAxisArr[3] = {0, 0, 1};
const itk::AffineTransform<double, 3>::InputVectorType zAxis(zAxisArr);
const itk::FlipImageFilter<ThreeDImageType>::FlipAxesArrayType zFlipAxis(zAxisArr);

const float DefaultPixelVal = 0;

class ImageProcessing{
public:
    ImageProcessing();  // 构造函数
    ~ImageProcessing();  // 析构函数

    //itk
    static void copyThreeDImage(ThreeDImageType::Pointer input, ThreeDImageType::Pointer output);
    static void resetDirIdentityThreeDImage(ThreeDImageType::Pointer input);
    static void rotateXYZThreeDImage(ThreeDImageType::Pointer input,double xAngle,double yAngle,double zAngle);
    static void rotateImage90Degrees(ThreeDImageType::Pointer input,TransformType::InputVectorType axis,bool LorR = true);

    //暂时先不考虑旋转矩阵的问题，使用时nullptr替代即可
    //vtk图像翻转
    static void mirrorFlipVTKimage(vtkSmartPointer<vtkImageData> imageData,int axis);
    //vtk图像旋转（旋转90度）
    static void rotateNinetyVTKimage(vtkSmartPointer<vtkImageData> imageData,int axis,bool clockwise=true);
    //vtk图像旋转（旋转任意角度）
    static void rotateVTKimage(vtkSmartPointer<vtkImageData> imageData, double x, double y, double z,bool extend = false);

    //将vtkImageData转化为各向同性(取最小的spacing作为同步数据)
    static void setImageIsotropic(vtkSmartPointer<vtkImageData> imageData);
    static void setImageVecIsotropic(std::vector<vtkSmartPointer<vtkImageData>> imageDataVec);

    //返回两个图像中范围中的最值（两个最小值取最小值，两个最大值取最大值）
    static std::tuple<double, double> getMaxScalarRange(vtkSmartPointer<vtkImageData> image1,vtkSmartPointer<vtkImageData> image2);

    //根据缩放因子，将image所有像素值都乘上该因子
    static void setImageScaleValue(vtkSmartPointer<vtkImageData> image, double scaleFactor);

    //将image图像归一化到范围0~1之间
    static void scaleImageToUnitInterval(vtkSmartPointer<vtkImageData> image);

    //最大密度投影合并多个图像（MIP）
    static vtkSmartPointer<vtkImageData> setImageVecMIP(std::vector<vtkSmartPointer<vtkImageData>> imageVec);

    // 遍历 image 的所有像素，并对每个像素进行四舍五入,针对float类型图像
    static void roundFloatImage(vtkSmartPointer<vtkImageData> image);

    // 提取所选区域的图像
    static vtkSmartPointer<vtkImageData> extractImageVOI(vtkSmartPointer<vtkImageData> image, std::vector<int> extent);
    // 恢复填充到原始图像大小，填充元素自定义
    static vtkSmartPointer<vtkImageData> fillRestoreImage(vtkSmartPointer<vtkImageData> inImage, vtkSmartPointer<vtkImageData> outImage);

    // 采样指导图像中添加mask，即分割区域外设置为-1，其它保持原有采样值
    static vtkSmartPointer<vtkImageData> addSamplingGuidedImageMask(vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkImageData> mask);

    // 对image只保留mask中值为value的区域,其它设置为-1，返回一个新的image
    static vtkSmartPointer<vtkImageData> extractImageRegionByMaskValue(vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkImageData> mask, int value);

    // 遍历的方法查询最大值像素点的位置与像素值
    static maxValuePointInf getMaxPointInf(vtkSmartPointer<vtkImageData> image);

    // 计算三维点云的凸包的体积
    static std::vector<double> calculateLeftVentricularVolume(std::vector<std::vector<maxValuePointInf>> maxValueVecVec);

    // 判断变量是否存在于容器之中,存在,直接返回容器，不存在则添加（容器排序保持从小到大）
    static std::vector<double> addAndSortIfNotPresent(std::vector<double> container, double value);
    // 判断变量是否存在于容器之中,存在,返回容器中索引值，不存在返回-1
    static int findIndexIfPresent(const std::vector<double>& container, double value);

    static std::vector<cv::Mat> GetSlices(vtkSmartPointer<vtkImageData> image, int axis);

    // 对image只保留mask中值为value的区域,其它设置为0，返回一个新的image
    static vtkSmartPointer<vtkImageData> fuseOriginalImageByMask(vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkImageData> mask, int value);

};

#endif // IMAGEPROCESSINGBOX_H
