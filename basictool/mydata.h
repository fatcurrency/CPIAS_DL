// mydata.h
#ifndef MYDATA_H
#define MYDATA_H

#include <QStringList>
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include <iostream>
#include <vector>

// 区域划分参数
struct cardiacModelParameter{
    int sectorCount;//扇形区域数量（圆柱及半球）
    int hemiRingCount;//半球环形区域数量(为了保证每次采样的点数一致，设置圆柱的采样层数也是这个)
    double cylinderMergeCount;//圆柱切片合并数量（相邻n个切片合并为一层进行最大值提取）
};

// 图像的基础参数信息，应该全局统一
struct MaskProperties {
    double center[3];
    double spacing[3];
    double origin[3];
    int extent[6];
    double bounds[6];
};

// 最大值点数据信息
struct maxValuePointInf{
    int x;
    int y;
    int z;
    double maxValue;
};

// 单个图像建模结果
struct modelResultStruct{
    vtkSmartPointer<vtkImageData> imageData;
    vtkSmartPointer<vtkImageData> maskData;
    cardiacModelParameter ModelParam;
    std::vector<maxValuePointInf> PointInfVec;
};

// 17节段数据排序
const int seventeenSegorder[] = {16, 15, 12, 13, 14, 11, 6, 7, 8, 9, 10, 5, 0, 1, 2, 3, 4};
// 全局变量
const QStringList seventeenStepList = QStringList() << "1" << "2" << "3" << "4" << "5" << "6"
                                                    << "7" << "8" << "9" << "10" << "11" << "12"
                                                    << "13" << "14" << "15" << "16" << "17";

const std::vector<QString> SeventeenSegmentNameString =
{"basal anterior", "basal anteroseptal", "basal inferoseptal", "basal inferior", "basal inferolateral", "basal anterolateral",
 "mid anterior", "mid anteroseptal", "mid inferoseptal", "mid inferior", "mid inferolateral", "mid anterolateral",
 "apical anterior", "apical septal", "apical inferior", "apical lateral", "apex"};

#endif // MYDATA_H
