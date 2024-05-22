#include "imageProcessingBox.h"

ImageProcessing::ImageProcessing() {

}

ImageProcessing::~ImageProcessing(){

}

void ImageProcessing::copyThreeDImage(ThreeDImageType::Pointer input, ThreeDImageType::Pointer output){
    // 从input复制到output
    output->SetRegions(input->GetLargestPossibleRegion());
    output->SetOrigin(input->GetOrigin());
    output->SetSpacing(input->GetSpacing());
    output->SetDirection(input->GetDirection());
    output->Allocate();

    itk::ImageRegionIterator<ThreeDImageType> inputIter(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<ThreeDImageType> outputIter(output, input->GetLargestPossibleRegion());

    while (!inputIter.IsAtEnd())
    {
        outputIter.Set(inputIter.Get());
        ++inputIter;
        ++outputIter;
    }
}

void ImageProcessing::rotateXYZThreeDImage(ThreeDImageType::Pointer input,double xAngle,double yAngle,double zAngle){
    auto center = itk::Point<double, 3>(); // 创建旋转中心点对象
    for (unsigned int i = 0; i < 3; ++i){
        center[i] = input->GetOrigin()[i] + input->GetSpacing()[i] * input->GetLargestPossibleRegion().GetSize()[i] / 2.0;
        std::cout << center[i] << std::endl;
    }

    // 创建仿射变换对象并设置旋转和平移变换
    const auto transform = AffineTransformType::New();
    transform->SetCenter(center);
    transform->Rotate3D(zAxis, xAngle * itk::Math::pi / 180.0,true);
    transform->Rotate3D(yAxis, yAngle * itk::Math::pi / 180.0,true);
    transform->Rotate3D(xAxis, zAngle * itk::Math::pi / 180.0,true);
    transform->Modified();
    itk::Matrix<double, 3, 3> rotateMatrix = transform->GetMatrix();

    //将input图像direction恢复为单位矩阵
    ThreeDImageType::DirectionType inputImageDirection = input->GetDirection();//input图像的原始direction
    itk::Matrix<double, 3, 3> identityMatrix = inputImageDirection.GetIdentity();
    input->SetDirection(identityMatrix);

//    itk::Matrix<double, 3, 3> inverseDirection = static_cast<itk::Matrix<double, 3, 3>>(inputImageDirection.GetInverse());//求逆矩阵
//    itk::Matrix<double, 3, 3> transformMatrix = inputImageDirection * rotateMatrix;//inputImageDirection * inverseDirection * transform->GetMatrix()

    transform->SetMatrix(rotateMatrix);//rotateMatrix
    transform->Modified();

    std::cout << "rotateMatrix:" << std::endl;
    std::cout << rotateMatrix << std::endl;

    auto resample = itk::ResampleImageFilter<ThreeDImageType, ThreeDImageType>::New();
    resample->SetInput(input); // 设置输入图像
    resample->SetReferenceImage(input); // 设置参考图像
    resample->UseReferenceImageOn(); // 使用参考图像
    resample->SetSize(input->GetLargestPossibleRegion().GetSize());// 设置输出图像大小
    resample->SetOutputOrigin(input->GetOrigin()); //设置origin
    resample->SetOutputSpacing(input->GetSpacing()); //设置spacing
    resample->SetDefaultPixelValue(DefaultPixelVal); // 设置默认像素值

    itk::BSplineInterpolateImageFunction <ThreeDImageType, double>::Pointer BSplineInterpolator =
            itk::BSplineInterpolateImageFunction <ThreeDImageType, double>::New();//B样条插值
    resample->SetInterpolator(BSplineInterpolator); // 设置插值器
    resample->SetTransform(transform); // 将仿射变换对象传递给重采样器
    resample->Update();

    copyThreeDImage(resample->GetOutput(),input);
    input->SetDirection(inputImageDirection * rotateMatrix);
    std::cout << "rotateXYZThreeDImage:" << input << std::endl;
}

void ImageProcessing::rotateImage90Degrees(ThreeDImageType::Pointer input,TransformType::InputVectorType axis,bool LorR){
    const ThreeDImageType::SizeType & size = input->GetLargestPossibleRegion().GetSize(); // 获取输入图像的大小
    const auto origin = input->GetOrigin(); // 获取输入图像的原点
    const auto spacing = input->GetSpacing(); // 获取输入图像的像素间距
    auto center = itk::Point<double, 3>(); // 创建旋转中心点对象
    ThreeDImageType::SizeType resampleSize; //输出图像大小
    ThreeDImageType::SpacingType resamplespacing; //输出图像spacing
    double angle;
    if(axis == xAxis){
        center[0] = origin[0] + spacing[0] * (size[0]-1) / 2.0;

        if(LorR == true){
            angle = 90.0;
            center[1] = origin[1] + spacing[1] * (size[1]-1) / 2.0;
            center[2] = origin[2] + spacing[2] * (size[2]-1) / 2.0;
            std::cout << origin[1] << ",," << spacing[1] << ",," << size[1]  << ",," << center[1] << std::endl;
            std::cout << origin[2] << ",," << spacing[2] << ",," << size[2]  << ",," << center[2] << std::endl;

        }else{
            angle = -90.0;
            center[1] = origin[1] + spacing[1] * (size[1]-1) / 2.0;
            center[2] = origin[2] + spacing[2] * (size[2]-1) / 2.0;
        }

        resampleSize = {size[0],size[2],size[1]};
        const double resamplespacingVec[3] = {spacing[0], spacing[2], spacing[1]};
        resamplespacing = resamplespacingVec;

    }else if (axis == yAxis) {
        center[1] = origin[1] + spacing[1] * (size[1]-1) / 2.0;

        if(LorR == true){
            angle = -90.0;
            center[0] = origin[0] + spacing[0] * (size[0]-1) / 2.0;
            center[2] = origin[0] + spacing[0] * (size[0]-1) / 2.0;
        }else{
            angle = 90.0;
            center[0] = origin[2] + spacing[2] * (size[2]-1) / 2.0;
            center[2] = origin[2] + spacing[2] * (size[2]-1) / 2.0;
        }

        resampleSize = {size[2],size[1],size[0]};
        const double resamplespacingVec[3] = {spacing[2], spacing[1], spacing[0]};
        resamplespacing = resamplespacingVec;
    }else if (axis == zAxis) {
        center[2] = origin[2] + spacing[2] * (size[2]-1) / 2.0;

        if(LorR == true){
            angle = 90.0;
            center[0] = origin[0] + spacing[0] * (size[0]-1) / 2.0;
            center[1] = origin[0] + spacing[0] * (size[0]-1) / 2.0;
        }else{
            angle = -90.0;
            center[0] = origin[1] + spacing[1] * (size[1]-1) / 2.0;
            center[1] = origin[1] + spacing[1] * (size[1]-1) / 2.0;
        }

        resampleSize = {size[1],size[0],size[2]};
        const double resamplespacingVec[3] = {spacing[1], spacing[0], spacing[2]};
        resamplespacing = resamplespacingVec;
    }else {
        return;
    }
    std::cout << input << std::endl;
    for (unsigned int i = 0; i < 3; ++i){
       std::cout << "center[" << i << "]:" << center[i] << std::endl;
    }

    auto resample = ResampleThreeDImageFilterType::New();
    resample->SetInput(input); // 设置输入图像
    resample->SetOutputOrigin(input->GetOrigin());
    resample->SetOutputDirection(input->GetDirection());
    resample->SetOutputSpacing(resamplespacing);
    resample->SetSize(resampleSize); // 设置输出图像大小
    resample->SetDefaultPixelValue(0); // 设置默认像素值

    itk::NearestNeighborInterpolateImageFunction<ThreeDImageType, double>::Pointer NearestInterpolator =
            itk::NearestNeighborInterpolateImageFunction<ThreeDImageType, double>::New();//最近邻插值
    resample->SetInterpolator(NearestInterpolator); // 设置插值器

    // 创建仿射变换对象并设置旋转和平移变换
    const auto transform = TransformType::New();
    transform->SetCenter(center);
    //pre代表的是“在现有变换之前应用旋转变换”还是“在现有变换之后应用旋转变换”的逻辑顺序(Rotate3D的第三个参数)
    transform->Rotate3D(axis, angle * itk::Math::pi / 180.0,true);//正数为顺时针旋转（以quickview视图为参考）

    // 将仿射变换对象传递给重采样器
    resample->SetTransform(transform);
    resample->Update();

    copyThreeDImage(resample->GetOutput(),input);
}

void ImageProcessing::resetDirIdentityThreeDImage(ThreeDImageType::Pointer input){
    ThreeDImageType::DirectionType inputImageDirection = input->GetDirection();//输入图像direction
    itk::Matrix<double, 3, 3> identityMatrix = inputImageDirection.GetIdentity();//单位矩阵identity
    //输入图像direction等于单位矩阵，无需操作
    if(inputImageDirection == identityMatrix){
        std::cout << "This is already the identity matrix!" << std::endl;
        return;
    }
    // 创建旋转中心点对象
    auto center = itk::Point<double, 3>();
    for (unsigned int i = 0; i < 3; ++i){
        center[i] = input->GetOrigin()[i] + input->GetSpacing()[i] * input->GetLargestPossibleRegion().GetSize()[i] / 2.0;
        std::cout << center[i] << std::endl;
    }
    const auto transform = AffineTransformType::New();
    transform->SetCenter(center);

    //将输入图像设置为单位矩阵direction
    input->SetDirection(identityMatrix);
    itk::Matrix<double, 3, 3> inverseDirection = static_cast<itk::Matrix<double, 3, 3>>(inputImageDirection.GetInverse());//求逆矩阵
    transform->SetMatrix(inverseDirection);//转换为逆矩阵，即还原操作
    transform->Modified();

    auto resample = itk::ResampleImageFilter<ThreeDImageType, ThreeDImageType>::New();
    resample->SetInput(input); // 设置输入图像
    resample->SetReferenceImage(input); // 设置参考图像
    resample->UseReferenceImageOn(); // 使用参考图像
    resample->SetSize(input->GetLargestPossibleRegion().GetSize());// 设置输出图像大小
    resample->SetOutputOrigin(input->GetOrigin()); //设置origin
    resample->SetOutputSpacing(input->GetSpacing()); //设置spacing
    resample->SetDefaultPixelValue(DefaultPixelVal); // 设置默认像素值

    itk::BSplineInterpolateImageFunction <ThreeDImageType, double>::Pointer BSplineInterpolator =
            itk::BSplineInterpolateImageFunction <ThreeDImageType, double>::New();//B样条插值
    resample->SetInterpolator(BSplineInterpolator); // 设置插值器
    resample->SetTransform(transform); // 将仿射变换对象传递给重采样器
    resample->Update();

    copyThreeDImage(resample->GetOutput(),input);
    input->SetDirection(identityMatrix);//恢复为初始单位阵
    std::cout << "resetDirIdentityThreeDImage:" << input << std::endl;
}

//镜像翻转
void ImageProcessing::mirrorFlipVTKimage(vtkSmartPointer<vtkImageData> imageData,int axis){
    vtkSmartPointer<vtkImageFlip> flipFilter = vtkSmartPointer<vtkImageFlip>::New();
    flipFilter->SetInputData(imageData);
    flipFilter->SetFilteredAxis(axis);// 0 = x-axis, 1 = y-axis, 2 = z-axis
    flipFilter->Update();

    // 获取镜像后的图像数据
    imageData->DeepCopy(flipFilter->GetOutput());
}

//90度旋转
void ImageProcessing::rotateNinetyVTKimage(vtkSmartPointer<vtkImageData> imageData, int axis,bool clockwise){
    //旋转角度，限制为90
    double angle = 90.0;
    if(clockwise == false){
        angle = -angle;
    }

    //获取图像的基础信息
    double center[3];
    imageData->GetCenter(center);
    double spacing[3];
    imageData->GetSpacing(spacing);
    double origin[3];
    imageData->GetOrigin(origin);
    int extent[6];
    imageData->GetExtent(extent);

    //转换函数
    vtkNew<vtkTransform> transform;
    transform->Translate(center[0], center[1], center[2]);

    switch(axis) {
        case 0: // X轴
            transform->RotateWXYZ(angle, 0, 0, 1);
            break;
        case 1: // Y轴
            transform->RotateWXYZ(angle, 0, 1, 0);
            break;
        case 2: // Z轴
            transform->RotateWXYZ(angle, 1, 0, 0);
            break;
        default:
            std::cout << "Error: The wrong index value was passed." << std::endl;
            return;
    }
    transform->Translate(-center[0], -center[1], -center[2]);

//    std::cout << "rotate center:" << center[0] << "," << center[1] << "," << center[2] << std::endl;
//    std::cout << "spacing:" << spacing[0] << "," << spacing[1] << "," << spacing[2] << std::endl;
//    std::cout << "origin:" << origin[0] << "," << origin[1] << "," << origin[2] << std::endl;
//    std::cout << "extent:" << extent[0] << "," << extent[1] << "," << extent[2] << ","
//                           << extent[3] << "," << extent[4] << "," << extent[5]<< std::endl;

    vtkNew<vtkImageReslice> reslice;
    reslice->SetInputData(imageData);
    reslice->SetResliceTransform(transform);
    //重采样插值方法：SetInterpolationModeToNearestNeighbor （最近邻） SetInterpolationModeToCubic （样条）
    reslice->SetInterpolationModeToNearestNeighbor();

    switch(axis) {
        case 0: // X轴
            reslice->SetOutputSpacing(spacing[1],spacing[0],spacing[2]);
            reslice->SetOutputOrigin(center[0]-(center[1]-origin[1]),center[1]-(center[0]-origin[0]),origin[2]);
            reslice->SetOutputExtent(extent[2],extent[3],extent[0],extent[1],extent[4],extent[5]);
            reslice->Update();
            break;
        case 1: // Y轴
            reslice->SetOutputSpacing(spacing[2],spacing[1],spacing[0]);
            reslice->SetOutputOrigin(center[0]-(center[2]-origin[2]),origin[1],center[2]-(center[0]-origin[0]));
            reslice->SetOutputExtent(extent[4],extent[5],extent[2],extent[3],extent[0],extent[1]);
            reslice->Update();
            break;
        case 2: // Z轴
            reslice->SetOutputSpacing(spacing[0],spacing[2],spacing[1]);
            reslice->SetOutputOrigin(origin[0],center[1]-(center[2]-origin[2]),center[2]-(center[1]-origin[1]));
            reslice->SetOutputExtent(extent[0],extent[1],extent[4],extent[5],extent[2],extent[3]);
            reslice->Update();
            break;
        default:
            std::cout << "Invalid axis index. Must be 0, 1, or 2." << std::endl;
            return;
    }

    imageData->DeepCopy(reslice->GetOutput());
}
//extend默认参数为false，表示不扩展图像
void ImageProcessing::rotateVTKimage(vtkSmartPointer<vtkImageData> imageData, double x, double y, double z,bool extend){
    //获取图像的基础信息
    double center[3];
    imageData->GetCenter(center);
    double spacing[3];
    imageData->GetSpacing(spacing);
    double origin[3];
    imageData->GetOrigin(origin);
    int extent[6];
    imageData->GetExtent(extent);

    //转换函数
    vtkNew<vtkTransform> transform;
    transform->Translate(center[0], center[1], center[2]);
    transform->RotateWXYZ(y, 0, 1, 0);
    transform->RotateWXYZ(z, 1, 0, 0);
    transform->RotateWXYZ(x, 0, 0, 1);
    transform->Translate(-center[0], -center[1], -center[2]);

//    std::cout << "rotate center:" << center[0] << "," << center[1] << "," << center[2] << std::endl;
//    std::cout << "spacing:" << spacing[0] << "," << spacing[1] << "," << spacing[2] << std::endl;
//    std::cout << "origin:" << origin[0] << "," << origin[1] << "," << origin[2] << std::endl;
//    std::cout << "extent:" << extent[0] << "," << extent[1] << "," << extent[2] << ","
//                           << extent[3] << "," << extent[4] << "," << extent[5]<< std::endl;

    vtkNew<vtkImageReslice> reslice;
    reslice->SetInputData(imageData);
    reslice->SetResliceTransform(transform);
    //重采样插值方法：SetInterpolationModeToNearestNeighbor （最近邻） SetInterpolationModeToCubic （样条）
    reslice->SetInterpolationModeToCubic();
    reslice->SetOutputSpacing(spacing[0],spacing[1],spacing[2]);
    reslice->SetOutputOrigin(center[0]-(center[0]-origin[0]),center[1]-(center[1]-origin[1]),center[2]-(center[2]-origin[2]));
//    reslice->SetOutputOrigin(origin[0],origin[1],origin[2]);

    if(extend == true){
        extent[4] = extent[4] - static_cast<int>(0.2*(extent[5]-extent[4]));
        extent[5] = extent[5] + static_cast<int>(0.2*(extent[5]-extent[4]));
    }

    reslice->SetOutputExtent(extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);
    reslice->Update();

    imageData->DeepCopy(reslice->GetOutput());
}

void ImageProcessing::setImageIsotropic(vtkSmartPointer<vtkImageData> imageData){
    //判断image3D是否为空
    if (imageData == nullptr) {
        std::cout << "image3D is nullptr" << std::endl;
    }
    // 判断imageData的spacing是否为等间距
    double spacing[3];
    imageData->GetSpacing(spacing);
    double epsilon = 1e-5;
    if (std::abs(spacing[0] - spacing[1]) < epsilon && std::abs(spacing[1] - spacing[2]) < epsilon) {
        std::cout << "imageData is isotropic" << std::endl;
        return;
    }
    // 获取spacing最小值
    double minSpacing = spacing[0];
    for (int i = 1; i < 3; ++i) {
        if (spacing[i] < minSpacing) {
            minSpacing = spacing[i];
        }
    }
    // 将spacing设置为等间距
    spacing[0] = minSpacing;
    spacing[1] = minSpacing;
    spacing[2] = minSpacing;
    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInputData(imageData);
    reslice->SetInterpolationModeToCubic(); // 使用样条插值
    reslice->SetOutputSpacing(spacing);
    reslice->Update();
    imageData->DeepCopy(reslice->GetOutput());
    imageData->Modified();
}

void ImageProcessing::setImageVecIsotropic(std::vector<vtkSmartPointer<vtkImageData>> imageDataVec){
    for (unsigned int i = 0;i<imageDataVec.size();i++) {
        setImageIsotropic(imageDataVec[i]);
    }
}

//返回两个图像中范围中的最值（两个最小值取最小值，两个最大值取最大值）
std::tuple<double, double> ImageProcessing::getMaxScalarRange(vtkSmartPointer<vtkImageData> image1,vtkSmartPointer<vtkImageData> image2){
    //获取图像1的最大值和最小值
    double image1Range[2];
    image1->GetScalarRange(image1Range);
    //获取图像2的最大值和最小值
    double image2Range[2];
    image2->GetScalarRange(image2Range);

    //打印图像1的最大值和最小值
    std::cout << "image1Range[0]:" << image1Range[0] << std::endl;
    std::cout << "image1Range[1]:" << image1Range[1] << std::endl;
    //打印图像2的最大值和最小值
    std::cout << "image2Range[0]:" << image2Range[0] << std::endl;
    std::cout << "image2Range[1]:" << image2Range[1] << std::endl;

    //返回两个图像中范围中的最值（两个最小值取最小值，两个最大值取最大值）
    return std::make_tuple(std::min(image1Range[0], image2Range[0]), std::max(image1Range[1], image2Range[1]));
}

void ImageProcessing::setImageScaleValue(vtkSmartPointer<vtkImageData> image, double scaleFactor){
    std::cout << "setImageScaleValue" << std::endl;
    vtkSmartPointer<vtkImageShiftScale> shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
    shiftScale->SetInputData(image);
    shiftScale->SetShift(0);
    shiftScale->SetScale(scaleFactor);
    shiftScale->Update();
    image->DeepCopy(shiftScale->GetOutput());
    image->Modified();
}

void ImageProcessing::scaleImageToUnitInterval(vtkSmartPointer<vtkImageData> image){
    std::cout << "scaleImageToUnitInterval" << std::endl;
    // 将image图像归一化到范围0~1之间
    double* range = image->GetScalarRange();
    double min = range[0];
    double max = range[1];

    vtkSmartPointer<vtkImageShiftScale> shiftScaleFilter = vtkSmartPointer<vtkImageShiftScale>::New();
    shiftScaleFilter->SetInputData(image);
    // 设置缩放参数，使像素值范围在 [0, 1] 之间
    shiftScaleFilter->SetScale(1.0 / (max - min));  // 缩放参数根据原始范围调整
    shiftScaleFilter->SetShift(-min / (max - min));  // 偏移参数，用于将范围映射到 [0, 1]
    shiftScaleFilter->Update();
    image->DeepCopy(shiftScaleFilter->GetOutput());
    image->Modified();
}

vtkSmartPointer<vtkImageData> ImageProcessing::setImageVecMIP(std::vector<vtkSmartPointer<vtkImageData>> imageVec){
    // 创建一个新的图像，用于存储结果
    vtkSmartPointer<vtkImageData> outputImage = vtkSmartPointer<vtkImageData>::New();
    outputImage->DeepCopy(imageVec[0]); // 复制第一个图像的结构（大小、类型等）

    // 获取图像的范围
    int extent[6];
    outputImage->GetExtent(extent);

    // 根据范围遍历所有的像素
    for (int z = extent[4]; z <= extent[5]; z++) {
        for (int y = extent[2]; y <= extent[3]; y++) {
            for (int x = extent[0]; x <= extent[1]; x++) {
                // 对于每个像素，选择所有输入图像中的最大值
                double maxVal = imageVec[0]->GetScalarComponentAsDouble(x, y, z, 0);
                for (int i = 1; i < imageVec.size(); i++) {
                    double val = imageVec[i]->GetScalarComponentAsDouble(x, y, z, 0);
                    if (val > maxVal) {
                        maxVal = val;
                    }
                }
                // 将最大值设置为输出图像的值
                outputImage->SetScalarComponentFromDouble(x, y, z, 0, maxVal);
            }
        }
    }

    return outputImage;
}

void ImageProcessing::roundFloatImage(vtkSmartPointer<vtkImageData> image){
    // 获取图像的维度
    int* dims = image->GetDimensions();
    // 获取图像的数据
    float* pixelData = static_cast<float*>(image->GetScalarPointer());
    // 遍历图像的所有像素
    for (int z = 0; z < dims[2]; z++){
        for (int y = 0; y < dims[1]; y++){
            for (int x = 0; x < dims[0]; x++){
                // 获取当前像素的索引
                int index = x + y * dims[0] + z * dims[0] * dims[1];
                // 对当前像素进行四舍五入
                pixelData[index] = round(pixelData[index]);
            }
        }
    }
}

vtkSmartPointer<vtkImageData> ImageProcessing::extractImageVOI(vtkSmartPointer<vtkImageData> image, std::vector<int> extent){
    vtkSmartPointer<vtkExtractVOI> cropVOI = vtkSmartPointer<vtkExtractVOI>::New();
    //根据cube的bounds裁剪图像
    cropVOI->SetInputData(image);
    cropVOI->SetVOI(extent[0],extent[1],extent[2],extent[3],extent[4],extent[5]);
    cropVOI->Update();

    return cropVOI->GetOutput();
}

vtkSmartPointer<vtkImageData> ImageProcessing::fillRestoreImage(vtkSmartPointer<vtkImageData> inImage, vtkSmartPointer<vtkImageData> outImage){
    vtkSmartPointer<vtkImageData> result = vtkSmartPointer<vtkImageData>::New();
    result->DeepCopy(outImage);
    result->Modified();

    //将inImage区域中像素值为1的像素复制到outputMask中
    int *extent = inImage->GetExtent();
    for(int i=extent[0]; i<=extent[1]; i++){
        for(int j=extent[2]; j<=extent[3]; j++){
            for(int k=extent[4]; k<=extent[5]; k++){
                if(inImage->GetScalarComponentAsDouble(i,j,k,0) >= 0){
                    double val = inImage->GetScalarComponentAsDouble(i,j,k,0);
                    result->SetScalarComponentFromDouble(i,j,k,0,val);
                }
            }
        }
    }
    result->Modified();
    return result;
}

vtkSmartPointer<vtkImageData> ImageProcessing::addSamplingGuidedImageMask(vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkImageData> mask){
    //将image所有像素值都加1
    vtkSmartPointer<vtkImageMathematics> addConstant = vtkSmartPointer<vtkImageMathematics>::New();
    addConstant->SetInput1Data(image);
    addConstant->SetConstantC(1);
    addConstant->SetOperationToAddConstant();
    addConstant->Update();

    vtkSmartPointer<vtkImageMathematics> imageMath = vtkSmartPointer<vtkImageMathematics>::New();
    imageMath->SetInput1Data(addConstant->GetOutput());
    imageMath->SetInput2Data(mask);
    imageMath->SetOperationToMultiply();
    imageMath->Update();

    // 将image所有像素值都减1
    addConstant->SetInput1Data(imageMath->GetOutput());
    addConstant->SetConstantC(-1);
    addConstant->SetOperationToAddConstant();
    addConstant->Update();

    return addConstant->GetOutput();
}

vtkSmartPointer<vtkImageData> ImageProcessing::extractImageRegionByMaskValue(vtkSmartPointer<vtkImageData> image, vtkSmartPointer<vtkImageData> mask, int value){
    // 创建一个新的图像，用于存储结果
    vtkSmartPointer<vtkImageData> result = vtkSmartPointer<vtkImageData>::New();
    result->DeepCopy(image);
    // 获取图像的维度
    int* dims = image->GetDimensions();
    // 获取图像和掩码的数据
    float* imageData = static_cast<float*>(result->GetScalarPointer());
    float* maskData = static_cast<float*>(mask->GetScalarPointer());

    // 遍历图像的所有像素
    for (int z = 0; z < dims[2]; z++){
        for (int y = 0; y < dims[1]; y++){
            for (int x = 0; x < dims[0]; x++){
                // 获取当前像素的索引
                int index = x + y * dims[0] + z * dims[0] * dims[1];
                // 如果掩码的像素值等于value，保留图像的像素值，否则设置为-1
                if (maskData[index] == value){
                    imageData[index] = round(imageData[index]);
                }else{
                    imageData[index] = -1;
                }
            }
        }
    }
    return result;
}

maxValuePointInf ImageProcessing::getMaxPointInf(vtkSmartPointer<vtkImageData> image){
    //暴力遍历所有点，保证找到最大值点
    maxValuePointInf maxVal = {std::numeric_limits<int>::lowest(),
                              std::numeric_limits<int>::lowest(),
                              std::numeric_limits<int>::lowest(),
                              std::numeric_limits<double>::lowest()};

    bool allPixelsSame = true; // 标记所有像素点是否相同
    double epsilon = 1e-5;
    double firstPixel = image->GetScalarComponentAsDouble(image->GetExtent()[0], image->GetExtent()[2], image->GetExtent()[4], 0);
    for (int x = image->GetExtent()[0]; x <= image->GetExtent()[1]; x++){
        for (int y = image->GetExtent()[2]; y <= image->GetExtent()[3]; y++){
            for (int z = image->GetExtent()[4]; z <= image->GetExtent()[5]; z++){
                double pixel = image->GetScalarComponentAsDouble(x, y, z, 0);
                if (pixel>maxVal.maxValue){
                    maxVal.x = x;
                    maxVal.y = y;
                    maxVal.z = z;
                    maxVal.maxValue = pixel;  // 更新最大值
                }
                if (std::abs(pixel - firstPixel) > epsilon) {
                    allPixelsSame = false; // 存在不相同的像素点
                }
            }
        }
    }

    if (allPixelsSame) {
        maxVal = {std::numeric_limits<int>::lowest(),
                  std::numeric_limits<int>::lowest(),
                  std::numeric_limits<int>::lowest(),
                  std::numeric_limits<double>::lowest()};
        std::cout << "All pixels in the image are the same." << std::endl;
    }

    return maxVal;
}

std::vector<double> ImageProcessing::calculateLeftVentricularVolume(std::vector<std::vector<maxValuePointInf>> maxValueVecVec){
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkDelaunay3D> delaunay = vtkSmartPointer<vtkDelaunay3D>::New();
    vtkSmartPointer<vtkDataSetSurfaceFilter> surfaceFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();
    vtkSmartPointer<vtkMassProperties> massProperties = vtkSmartPointer<vtkMassProperties>::New();

    std::vector<double> volumeVec;
    for (int i = 0;i<maxValueVecVec.size();i++) {
        points->Reset();
        polydata->Initialize();
        for (int j = 0;j<maxValueVecVec[i].size();j++) {
            points->InsertNextPoint(maxValueVecVec[i][j].x, maxValueVecVec[i][j].y, maxValueVecVec[i][j].z);
        }
        polydata->SetPoints(points);
        delaunay->SetInputData(polydata);
        delaunay->Update();
        surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
        surfaceFilter->Update();
        massProperties->SetInputData(surfaceFilter->GetOutput());
        massProperties->Update();
        double volume = massProperties->GetVolume();
        volumeVec.push_back(volume);
    }
    return volumeVec;
}

std::vector<double> ImageProcessing::addAndSortIfNotPresent(std::vector<double> container, double value) {
    if (std::find(container.begin(), container.end(), value) == container.end()) {
        container.push_back(value);
        std::sort(container.begin(), container.end());
    }
    return container;
}

int ImageProcessing::findIndexIfPresent(const std::vector<double>& container, double value) {
    auto it = std::find(container.begin(), container.end(), value);
    if (it != container.end()) {
        return std::distance(container.begin(), it);
    }
    std::cout << "ERROR!!! The specified value does not exist in the container!" << std::endl;
    return -1;
}

std::vector<cv::Mat> ImageProcessing::GetSlices(vtkSmartPointer<vtkImageData> image, int axis)
{
    //判断image3D是否为空
    if (image == nullptr) {
        std::cout << "GetSlices image is nullptr" << std::endl;
        return std::vector<cv::Mat>();
    }

    //获取转化后图像的分辨率
    int *dims = image->GetDimensions();
    int *extent = image->GetExtent();

    //判断切片顺序
    int sliceCount;
    switch (axis) {
     case 2:
         sliceCount = dims[2];
         break;
     case 1:
         sliceCount = dims[1];
         break;
     case 0:
         sliceCount = dims[0];
         break;
    }

    std::vector<cv::Mat> mats2D;
    for (int i = 0; i < sliceCount; ++i) {
    cv::Mat mat2D;
    switch (axis) {
        case 2:
            mat2D = cv::Mat(dims[1], dims[0], CV_32SC1);
            for (int y = 0; y < dims[1]; ++y) {
                for (int x = 0; x < dims[0]; ++x) {
                    unsigned int* pixel = static_cast<unsigned int*>(image->GetScalarPointer(x+extent[0], y+extent[2], i+extent[4]));
                    mat2D.at<int>(y, x) = *pixel;
                }
            }
            break;
        case 1:
            mat2D = cv::Mat(dims[2], dims[0], CV_32SC1);
            for (int z = 0; z < dims[2]; ++z) {
                for (int x = 0; x < dims[0]; ++x) {
                    unsigned int* pixel = static_cast<unsigned int*>(image->GetScalarPointer(x+extent[0], i+extent[2], z+extent[4]));
                    mat2D.at<int>(z, x) = *pixel;
                }
            }
            break;
        case 0:
            mat2D = cv::Mat(dims[2], dims[1], CV_32SC1);
            for (int z = 0; z < dims[2]; ++z) {
                for (int y = 0; y < dims[1]; ++y) {
                    unsigned int* pixel = static_cast<unsigned int*>(image->GetScalarPointer(i+extent[0], y+extent[2], z+extent[4]));
                    mat2D.at<int>(z, y) = *pixel;
                }
            }
            break;
        }
    mats2D.push_back(mat2D);
    }
//    // 遍历mats2D，输出每个切片的图像的size
//    for (int i = 0; i < sliceCount; ++i) {
//        std::cout << "mats2D[" << i << "].size():" << mats2D[i].size() << std::endl;
//    }

    return mats2D;
}
