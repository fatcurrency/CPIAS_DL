#include "colormapTableBox.h"

void PredefineLookUpTable::transToGREYcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Grey颜色表
    auto colormap = itk::Function::GreyColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToJETcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为JET颜色表
    auto colormap = itk::Function::JetColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToHOTcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Hot颜色表
    auto colormap = itk::Function::HotColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToHSVcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为HSV颜色表
    auto colormap = itk::Function::HSVColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToCOOLcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Cool颜色表
    auto colormap = itk::Function::CoolColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToWINTERcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Winter颜色表
    auto colormap = itk::Function::WinterColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToCOPPERcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Copper颜色表
    auto colormap = itk::Function::CopperColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToSPRINGcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Spring颜色表
    auto colormap = itk::Function::SpringColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToREDcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Red颜色表
    auto colormap = itk::Function::RedColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToBLUEcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Blue颜色表
    auto colormap = itk::Function::BlueColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::transToGREENcolormap(vtkSmartPointer<vtkLookupTable> Lut){
    //lut恢复初始化
    Lut->SetTableRange(0, 255);
    Lut->SetNumberOfTableValues(256);

    //将预定义的颜色表转换为Green颜色表
    auto colormap = itk::Function::GreenColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        Lut->SetTableValue(i, color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0, 1.0);
    }
}

void PredefineLookUpTable::greyTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Grey颜色表
    auto colormap = itk::Function::GreyColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::hotTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Hot颜色表
    auto colormap = itk::Function::HotColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::jetTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为JET颜色表
    auto colormap = itk::Function::JetColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::hsvTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为HSV颜色表
    auto colormap = itk::Function::HSVColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::coolTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Cool颜色表
    auto colormap = itk::Function::CoolColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::winterTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Winter颜色表
    auto colormap = itk::Function::WinterColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0;i<=255;i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed()/255.0, color.GetGreen()/255.0, color.GetBlue()/255.0);
    }
}

void PredefineLookUpTable::copperTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Copper颜色表
    auto colormap = itk::Function::CopperColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);
    for (int i = 0; i <= 255; i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }
}

void PredefineLookUpTable::springTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;
    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;
    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Spring颜色表
    auto colormap = itk::Function::SpringColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);

    for (int i = 0; i <= 255; i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }
}

void PredefineLookUpTable::redTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;

    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;

    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Red颜色表
    auto colormap = itk::Function::RedColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);

    for (int i = 0; i <= 255; i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }
}

void PredefineLookUpTable::blueTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;

    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;

    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Blue颜色表
    auto colormap = itk::Function::BlueColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);

    for (int i = 0; i <= 255; i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }
}

void PredefineLookUpTable::greeenTransFunc(double min, double max, vtkSmartPointer<vtkColorTransferFunction> colortrans){
    double step = (max - min) / 255.0;

    double pos[256];
    pos[0] = min;
    pos[255] = max;
    for (int i = 1; i < 255; i++) {
        pos[i] = pos[i - 1] + step;

    }

    colortrans->RemoveAllPoints();

    //将预定义的颜色表转换为Green颜色表
    auto colormap = itk::Function::GreenColormapFunction<float, itk::RGBPixel<unsigned char>>::New();
    colormap->SetMinimumInputValue(0.0);
    colormap->SetMaximumInputValue(255.0);

    for (int i = 0; i <= 255; i++) {
        auto color = colormap->operator()(i);
        colortrans->AddRGBPoint(pos[i], color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }
}
