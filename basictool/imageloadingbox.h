#ifndef IMAGELOADINGBOX_H
#define IMAGELOADINGBOX_H

#include <QString>
#include <QFile>
#include <QTemporaryFile>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkJoinSeriesImageFilter.h"

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include <experimental/filesystem>

#include "mydata.h"
#include "imageprocessingbox.h"

using ExtractFilterType = itk::ExtractImageFilter<FourDImageType, ThreeDImageType>;
using JoinSeriesFilterType = itk::JoinSeriesImageFilter<ThreeDImageType, FourDImageType>;

class ImageLoading{
public:
    ImageLoading();  // 构造函数
    ~ImageLoading();  // 析构函数
    bool iniResetdir; //初始化时是否立即将direction变为identity

    void loadDicomSeriesImage(std::string Pathstring);
    void loadNiftiImage(std::string Pathstring);
    ThreeDImageType::Pointer setIdentityDirection(bool identity);
    vtkSmartPointer<vtkImageData> itkTOvtkOriginImage(ThreeDImageType::Pointer itkImage);
    vtkSmartPointer<vtkImageData> getVTKThreeDImage();
    std::vector<vtkSmartPointer<vtkImageData>> itkTOvtkOriginFourDImage(FourDImageType::Pointer itkImage);
    std::vector<vtkSmartPointer<vtkImageData>> getVTKThreeDImageVec();
    bool getImage4DStatus();//获取加载图像是否为4D
    unsigned int getOriginFourDImageTimeSize();//获取第四维的大小（时间帧）

    // 提取指定时间帧
    static ThreeDImageType::Pointer extractSpecifiedTimeFrame(FourDImageType::Pointer itkFourDImage,unsigned int frameIndex = 0);
    static void getVTKimageInfPrint(vtkSmartPointer<vtkImageData> vtkImage);
    static void fillVTKimageInftoStruct(vtkSmartPointer<vtkImageData> vtkImage,vtkImageInf* infstruct);
    static void vtkTOitkImage(vtkSmartPointer<vtkImageData> vtkImage,ThreeDImageType::Pointer itkImage);
    static void saveVTKimageToNII(vtkSmartPointer<vtkImageData> vtkImage,std::string filename);
    static void saveVTKimageToNIIinFilepath(vtkSmartPointer<vtkImageData> vtkImage,std::string filepath);
    static void savePolarmapRowDatainFilepath(cardiacModelParameter CMP, std::vector<maxValuePointInf>& pointCloud, std::string& filepath);
    static void writeVectorToCSV(const std::vector<double>& data, const std::string& filename);
    static void getPolarmapSegmentContourFromSRC(const QString& resourcePath,vtkSmartPointer<vtkImageData> image);

private:
    FourDImageType::Pointer originFourDImage;
    FourDImageType::RegionType originFourDImageRegion;
    bool image4DStatus;//该图像是3D还是4D
    unsigned int imageTimeSize;

    // originThreeDImage和VTKOriginThreeDImage两个图像的作用是作为中间件，实现预处理，逐一处理四维图像中各个三维图像
    ThreeDImageType::Pointer originThreeDImage;
    vtkSmartPointer<vtkImageData> VTKOriginThreeDImage;
    // 如果加载的图像为静态的三维图像，这个容器就是只包含一个三维图像
    // 如果加载的图像是门控或者动态图像（带有时间帧的思维图像），那么这个容器包含多个三维图像
    // 主要是通过这个容器进行图像传递-》View 视窗
    std::vector<vtkSmartPointer<vtkImageData>> VTKOriginThreeDImageVec;
};

#endif // IMAGELOADINGBOX_H
