#ifndef COLORMAPTABLEBOX_H
#define COLORMAPTABLEBOX_H

#include <functional>
#include "iostream"

#include "itkCustomColormapFunction.h"
#include "itkJetColormapFunction.h"
#include "itkHotColormapFunction.h"
#include "itkGreyColormapFunction.h"
#include "itkHSVColormapFunction.h"
#include "itkCoolColormapFunction.h"
#include "itkWinterColormapFunction.h"
#include "itkCopperColormapFunction.h"
#include "itkSpringColormapFunction.h"
#include "itkRedColormapFunction.h"
#include "itkBlueColormapFunction.h"
#include "itkGreenColormapFunction.h"
#include "itkRGBPixel.h"

#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>

typedef void (*LUTFunctionPointer)(vtkSmartPointer<vtkLookupTable>);
typedef void (*TransFuncPointer)(double, double, vtkSmartPointer<vtkColorTransferFunction>);
static std::vector<std::string> stringVector = {"grey","hot","jet","HSV","cool","winter","copper","spring","red","blue","green"};

class PredefineLookUpTable{
public:
    //后续可以考虑使用泛型编程，更加简略！

    //设置颜色表，范围均为从0到255
    static void transToJETcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToHOTcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToGREYcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToHSVcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToCOOLcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToWINTERcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToCOPPERcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToSPRINGcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToREDcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToBLUEcolormap(vtkSmartPointer<vtkLookupTable> Lut);
    static void transToGREENcolormap(vtkSmartPointer<vtkLookupTable> Lut);

    //设置颜色转化函数（vtkColorTransferFunction），自定义最大和最小值
    static void greyTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void hotTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void jetTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void hsvTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void coolTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void winterTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void copperTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void springTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void redTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void blueTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);
    static void greeenTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans);


    //后续根据需求进行添加，参考：https://itk.org/Doxygen/html/search.php?query=ColormapFunction

};

static LUTFunctionPointer LUTfunctionArray[11] = {
    PredefineLookUpTable::transToGREYcolormap,
    PredefineLookUpTable::transToHOTcolormap,
    PredefineLookUpTable::transToJETcolormap,
    PredefineLookUpTable::transToHSVcolormap,
    PredefineLookUpTable::transToCOOLcolormap,
    PredefineLookUpTable::transToWINTERcolormap,
    PredefineLookUpTable::transToCOPPERcolormap,
    PredefineLookUpTable::transToSPRINGcolormap,
    PredefineLookUpTable::transToREDcolormap,
    PredefineLookUpTable::transToBLUEcolormap,
    PredefineLookUpTable::transToGREENcolormap
};

static TransFuncPointer TransFuncArray[11] = {
    PredefineLookUpTable::greyTransFunc,
    PredefineLookUpTable::hotTransFunc,
    PredefineLookUpTable::jetTransFunc,
    PredefineLookUpTable::hsvTransFunc,
    PredefineLookUpTable::coolTransFunc,
    PredefineLookUpTable::winterTransFunc,
    PredefineLookUpTable::copperTransFunc,
    PredefineLookUpTable::springTransFunc,
    PredefineLookUpTable::redTransFunc,
    PredefineLookUpTable::blueTransFunc,
    PredefineLookUpTable::greeenTransFunc
};

#endif // COLORMAPTABLEBOX_H
