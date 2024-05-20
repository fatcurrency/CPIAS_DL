#include "imageLoadingBox.h"

ImageLoading::ImageLoading() {
    iniResetdir = false;
    image4DStatus = false;
    FourDImageType::Pointer originFourDImage = FourDImageType::New();
    ThreeDImageType::Pointer originThreeDImage = ThreeDImageType::New();
    VTKOriginThreeDImage = vtkSmartPointer<vtkImageData>::New();

}

ImageLoading::~ImageLoading(){

}

void ImageLoading::loadDicomSeriesImage(std::string Pathstring){
    std::experimental::filesystem::path filePath(Pathstring);//选中的dicom文件路径
    std::experimental::filesystem::path parentPath = filePath.parent_path();//当前dicom所在文件夹路径

    auto fileDicomIO = itk::GDCMImageIO::New();
    fileDicomIO->SetFileName(Pathstring);
    fileDicomIO->ReadImageInformation();
    fileDicomIO->UpdateOutputData();
    std::string selectedSeriesUID = fileDicomIO->GetUIDPrefix();//读取选中的dicom文件的uid号
    std::cout << "selectedSeriesUID of the selected file:" << selectedSeriesUID << std::endl;

    // 判断这个图象序列是不是带有时间维度
    std::string triggerTimeString = ""; //触发时间参数
    std::string frameReferenceTimeString = ""; // 每个帧参考时间（时间轴）
    fileDicomIO->GetValueFromTag( "0018|1060", triggerTimeString);
    fileDicomIO->GetValueFromTag( "0054|1300", frameReferenceTimeString);
    std::cout << "trigger Time:" << triggerTimeString << std::endl;
    std::cout << "frame Reference Time:" << frameReferenceTimeString << std::endl;

    //读取当前文件夹中所有文件
    auto nameGenerator = itk::GDCMSeriesFileNames::New();
    nameGenerator->SetUseSeriesDetails(true);
    nameGenerator->AddSeriesRestriction("0008|0021");
    nameGenerator->SetGlobalWarningDisplay(false);
    nameGenerator->SetDirectory(parentPath.string());

    //当前文件夹中可能包含有多个不同series的图像文件（可能存在不同的SeriesUID）
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    auto                      seriesItr = seriesUID.begin();
    auto                      seriesEnd = seriesUID.end();
    if (seriesItr != seriesEnd){
        std::cout << "The directory: ";
        std::cout << parentPath.string() << std::endl;
        std::cout << "Contains the following DICOM Series: ";
        std::cout << std::endl;
    }else{
        std::cout << "No DICOMs in: " << parentPath.string() << std::endl;
        return;
    }

    //逐一读取不同的series的uid，并打印出来
    while (seriesItr != seriesEnd){
        std::cout << seriesItr->c_str() << std::endl;
        ++seriesItr;
    }

    //遍历文件夹中所有的series，找到与选中的文件相对应的序列
    seriesItr = seriesUID.begin();
    while(seriesItr != seriesUID.end()){
        std::string seriesIdentifier = seriesItr->c_str();
        seriesItr++;
        // 根据当前的SeriesUID号，读取对应的文件列表，判断是否存在与选中的dicom图像相同文件名的
        FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);
        for (const auto& fileName : fileNames){
            // 该序列中存在与所选文件名一样的字符串，则该序列号就是我们所需要的
            if (fileName == filePath){
                // 遍历文件名列表，获取各个文件的序列号
                std::vector<double> triggerTimeContainer;
                std::vector<double> frameReferenceTimeContainer;
                for (unsigned int i = 0; i < fileNames.size(); i++){
                    fileDicomIO->SetFileName(fileNames[i]);
                    fileDicomIO->ReadImageInformation();
                    fileDicomIO->UpdateOutputData();
                    fileDicomIO->GetValueFromTag("0018|1060", triggerTimeString );
                    fileDicomIO->GetValueFromTag("0054|1300", frameReferenceTimeString);
                    // 获取门控触发时间信息
                    if(triggerTimeString != ""){
                        double triggerTimeDouble = std::stod(triggerTimeString);
                        //std::cout << "File " << i << " => " << "triggerTime:" << triggerTimeDouble << std::endl;
                        triggerTimeContainer = ImageProcessing::addAndSortIfNotPresent(triggerTimeContainer,triggerTimeDouble);
                    }
                    // 获取动态时间帧信息
                    if(frameReferenceTimeString != ""){
                        double frameReferenceTimeDouble = std::stod(frameReferenceTimeString);
                        //std::cout << "File " << i << " => " << "frameReferenceTime:" << frameReferenceTimeDouble << std::endl;
                        frameReferenceTimeContainer = ImageProcessing::addAndSortIfNotPresent(frameReferenceTimeContainer,frameReferenceTimeDouble);
                    }
                }
                std::cout << "trigger Time Num:" << triggerTimeContainer.size() <<
                             ",frame Reference Time Num:" << frameReferenceTimeContainer.size() << std::endl;


                JoinSeriesFilterType::Pointer joinSeriesFilter = JoinSeriesFilterType::New();

                // 这是静态单帧图像
                if(triggerTimeContainer.size() == 0 && frameReferenceTimeContainer.size() == 1){
                    std::cout << "This is a static single frame image" << std::endl;
                    // 读取图像
                    auto reader = itk::ImageSeriesReader<ThreeDImageType>::New();
                    auto dicomIO = itk::GDCMImageIO::New();
                    reader->SetFileNames(fileNames);
                    reader->MetaDataDictionaryArrayUpdateOn();
                    reader->SetImageIO(dicomIO);
                    reader->ForceOrthogonalDirectionOff();
                    try{
                        reader->Update();
                    }catch (itk::ExceptionObject & ex){
                        std::cout << ex << std::endl;
                        return;
                    }
                    joinSeriesFilter->SetInput(0, reader->GetOutput());

                // 这是门控多帧图像
                }else if(triggerTimeContainer.size() > 0 && frameReferenceTimeContainer.size() == 1){
                    std::cout << "This is the gated image" << std::endl;

                    std::vector<FileNamesContainer> timeSeriesFileNames;
                    timeSeriesFileNames.resize(triggerTimeContainer.size());
                    for (unsigned int i = 0; i < fileNames.size(); i++){
                        fileDicomIO->SetFileName(fileNames[i]);
                        fileDicomIO->ReadImageInformation();
                        fileDicomIO->UpdateOutputData();
                        fileDicomIO->GetValueFromTag("0018|1060", triggerTimeString );
                        double triggerTimeDouble = std::stod(triggerTimeString);
                        int index = ImageProcessing::findIndexIfPresent(triggerTimeContainer,triggerTimeDouble);
                        if(index == -1){
                            continue;
                        }
                        timeSeriesFileNames.at(index).push_back(fileNames[i]);
                    }
                    for (unsigned int i = 0; i < timeSeriesFileNames.size(); i++){
                        // 读取图像
                        auto reader = itk::ImageSeriesReader<ThreeDImageType>::New();
                        auto dicomIO = itk::GDCMImageIO::New();
                        reader->SetFileNames(timeSeriesFileNames[i]);
                        reader->MetaDataDictionaryArrayUpdateOn();
                        reader->SetImageIO(dicomIO);
                        reader->ForceOrthogonalDirectionOff();
                        try{
                            reader->Update();
                        }catch (itk::ExceptionObject & ex){
                            std::cout << ex << std::endl;
                            return;
                        }
                        joinSeriesFilter->SetInput(i, reader->GetOutput());
                    }
                // 这是动态多帧图像（没有触发时间数据,但是带有时间帧信息）
                }else if(triggerTimeContainer.size() == 0 && frameReferenceTimeContainer.size() > 1){
                    std::cout << "This is a dynamic image" << std::endl;

                    std::vector<FileNamesContainer> timeSeriesFileNames;
                    timeSeriesFileNames.resize(frameReferenceTimeContainer.size());
                    for (unsigned int i = 0; i < fileNames.size(); i++){
                        fileDicomIO->SetFileName(fileNames[i]);
                        fileDicomIO->ReadImageInformation();
                        fileDicomIO->UpdateOutputData();
                        fileDicomIO->GetValueFromTag("0054|1300", frameReferenceTimeString);
                        double frameReferenceTimeDouble = std::stod(frameReferenceTimeString);
                        int index = ImageProcessing::findIndexIfPresent(frameReferenceTimeContainer,frameReferenceTimeDouble);
                        if(index == -1){
                            continue;
                        }
                        timeSeriesFileNames.at(index).push_back(fileNames[i]);
                    }
                    for (unsigned int i = 0; i < timeSeriesFileNames.size(); i++){
                        // 读取图像
                        auto reader = itk::ImageSeriesReader<ThreeDImageType>::New();
                        auto dicomIO = itk::GDCMImageIO::New();
                        reader->SetFileNames(timeSeriesFileNames[i]);
                        reader->MetaDataDictionaryArrayUpdateOn();
                        reader->SetImageIO(dicomIO);
                        reader->ForceOrthogonalDirectionOff();
                        try{
                            reader->Update();
                        }catch (itk::ExceptionObject & ex){
                            std::cout << ex << std::endl;
                            return;
                        }
                        joinSeriesFilter->SetInput(i, reader->GetOutput());
                    }
                }else {
                    std::cout << "What happened?" << std::endl;
                }
                joinSeriesFilter->Update();

                //打印triggerTimeContainer
                for (unsigned int i = 0; i < triggerTimeContainer.size(); i++){
                    std::cout << "triggerTimeContainer[" << i << "]:" << triggerTimeContainer[i] << std::endl;
                }
                //打印frameReferenceTimeContainer
                for (unsigned int i = 0; i < frameReferenceTimeContainer.size(); i++){
                    std::cout << "frameReferenceTimeContainer[" << i << "]:" << frameReferenceTimeContainer[i] << std::endl;
                }

                originFourDImage = joinSeriesFilter->GetOutput();
                // 检查是否包含时间序列
                originFourDImageRegion = originFourDImage->GetLargestPossibleRegion();
                imageTimeSize = static_cast<unsigned int>(originFourDImageRegion.GetSize()[3]);
                if (imageTimeSize > 1) {
                    image4DStatus = true;
                    std::cout << "The image contains a time series." << std::endl;
                    std::cout << "Number of time frames:" << imageTimeSize << std::endl;
                } else {
                    image4DStatus = false;
                    std::cout << "The image does not contain a time series." << std::endl;
                    std::cout << "Number of time frames:" << imageTimeSize << std::endl;
                }
                // 默认设置四维图像的第一个时间帧为三维的originThreeDImage变量值
                originThreeDImage = extractSpecifiedTimeFrame(originFourDImage,0);
                this->setIdentityDirection(iniResetdir);
                this->itkTOvtkOriginImage(originThreeDImage);

                if (originThreeDImage.IsNull()){
                    std::cerr << "Failed to read image: " << Pathstring << std::endl;
                    return;
                }

                this->VTKOriginThreeDImageVec = this->itkTOvtkOriginFourDImage(this->originFourDImage);
            }
        }
    }
}

void ImageLoading::loadNiftiImage(std::string Pathstring){
    // 加载动态或门控数据
    originFourDImage = itk::ReadImage<FourDImageType>(Pathstring);

    // 检查是否包含时间序列
    originFourDImageRegion = originFourDImage->GetLargestPossibleRegion();
    imageTimeSize = static_cast<unsigned int>(originFourDImageRegion.GetSize()[3]);
    if (imageTimeSize > 1) {
        image4DStatus = true;
        std::cout << "The image contains a time series." << std::endl;
        std::cout << "Number of time frames:" << imageTimeSize << std::endl;
    } else {
        image4DStatus = false;
        std::cout << "The image does not contain a time series." << std::endl;
        std::cout << "Number of time frames:" << imageTimeSize << std::endl;
    }
    // 默认设置四维图像的第一个时间帧为三维的originThreeDImage变量值
    originThreeDImage = extractSpecifiedTimeFrame(originFourDImage,0);
    this->setIdentityDirection(iniResetdir);
    this->itkTOvtkOriginImage(originThreeDImage);

    if (originThreeDImage.IsNull()){
        std::cerr << "Failed to read image: " << Pathstring << std::endl;
        return;
    }

    this->VTKOriginThreeDImageVec = this->itkTOvtkOriginFourDImage(this->originFourDImage);
    this->VTKOriginThreeDImage->DeepCopy(this->VTKOriginThreeDImageVec[0]);
}

ThreeDImageType::Pointer ImageLoading::setIdentityDirection(bool identity){
    if(identity == false){
        return originThreeDImage;
    }
    ThreeDImageType::DirectionType resetDir;
    resetDir.SetIdentity();//单位矩阵
    originThreeDImage->SetDirection(resetDir);

    return originThreeDImage;
}

vtkSmartPointer<vtkImageData> ImageLoading::itkTOvtkOriginImage(ThreeDImageType::Pointer itkImage){
    auto vtkitkfilter = itk::ImageToVTKImageFilter<ThreeDImageType>::New();
    vtkitkfilter->SetInput(itkImage);
    vtkitkfilter->Update();

    VTKOriginThreeDImage->DeepCopy(vtkitkfilter->GetOutput());
    VTKOriginThreeDImage->AllocateScalars(VTK_FLOAT, 1);//注意，这里存在和图像像素类型匹配的参数设置
    VTKOriginThreeDImage->Modified();

//    VTKOriginThreeDImage->PrintSelf(std::cout,vtkIndent());
//    VTKOriginThreeDImage->Print(std::cout);

    return VTKOriginThreeDImage;
}

std::vector<vtkSmartPointer<vtkImageData>> ImageLoading::itkTOvtkOriginFourDImage(FourDImageType::Pointer itkImage){
    std::vector<vtkSmartPointer<vtkImageData>> vtkThreeDImageVec;
    unsigned int timeSize = static_cast<unsigned>(itkImage->GetLargestPossibleRegion().GetSize()[3]);

    auto vtkitkfilter = itk::ImageToVTKImageFilter<ThreeDImageType>::New();
    for(unsigned int i = 0; i < timeSize; i++){
        originThreeDImage = extractSpecifiedTimeFrame(itkImage,i);
        this->setIdentityDirection(iniResetdir);
        vtkitkfilter->SetInput(originThreeDImage);
        vtkitkfilter->Update();

        vtkSmartPointer<vtkImageData> vtkThreeDImage = vtkSmartPointer<vtkImageData>::New();
        vtkThreeDImage->DeepCopy(vtkitkfilter->GetOutput());
        vtkThreeDImage->AllocateScalars(VTK_FLOAT, 1);//注意，这里存在和图像像素类型匹配的参数设置
        vtkThreeDImage->Modified();

        vtkThreeDImageVec.push_back(vtkThreeDImage);
    }
    return vtkThreeDImageVec;
}

void ImageLoading::vtkTOitkImage(vtkSmartPointer<vtkImageData> vtkImage,ThreeDImageType::Pointer itkImage){
    VTKToITKFilterType::Pointer vtkToItkFilter = VTKToITKFilterType::New();

    vtkToItkFilter->SetInput(vtkImage);
    vtkToItkFilter->Update();

    itkImage = vtkToItkFilter->GetOutput();

    std::cout << "vtkToItkFilter" << itkImage << std::endl;
}

vtkSmartPointer<vtkImageData> ImageLoading::getVTKThreeDImage(){
    return this->VTKOriginThreeDImage;
}

std::vector<vtkSmartPointer<vtkImageData>> ImageLoading::getVTKThreeDImageVec(){
    return this->VTKOriginThreeDImageVec;
}

bool ImageLoading::getImage4DStatus(){
    return image4DStatus;
}

unsigned int ImageLoading::getOriginFourDImageTimeSize(){
    return imageTimeSize;
}

ThreeDImageType::Pointer ImageLoading::extractSpecifiedTimeFrame(FourDImageType::Pointer itkFourDImage,unsigned int frameIndex){
    FourDImageType::RegionType inputRegion = itkFourDImage->GetLargestPossibleRegion();
    if(frameIndex>=inputRegion.GetSize()[3]){
        std::cout << "There is no specified time frame:" << frameIndex << std::endl;
        return nullptr;
    }

    FourDImageType::SizeType size = inputRegion.GetSize();
    size[3] = 0;  // 设置时间维度的大小为0
    FourDImageType::IndexType start = inputRegion.GetIndex();
    start[3] = frameIndex;  // 设置要提取的时间帧
    FourDImageType::RegionType extractRegion(start, size);

    ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
    extractFilter->SetExtractionRegion(extractRegion);
    extractFilter->SetInput(itkFourDImage);
    extractFilter->SetDirectionCollapseToIdentity(); // 确保方向保持一致
    extractFilter->Update();

    // 返回提取到的三维图像
    return extractFilter->GetOutput();
}

void ImageLoading::getVTKimageInfPrint(vtkSmartPointer<vtkImageData> vtkImage){
//    vtkImage->PrintSelf(std::cout,vtkIndent());

    std::cout << "---------------------" << std::endl;
    std::cout<<"originImageData:"<<std::endl;
    std::cout<<"ScalarType:"<<vtkImage->GetScalarTypeAsString()<<std::endl;
    std::cout<<"NumberOfScalarComponents:"<<vtkImage->GetNumberOfScalarComponents()<<std::endl;
    std::cout<<"NumberOfPoints:"<<vtkImage->GetNumberOfPoints()<<std::endl;
    std::cout<<"NumberOfCells:"<<vtkImage->GetNumberOfCells()<<std::endl;
    std::cout<<"Dimensions:"<<vtkImage->GetDimensions()[0]<<" "<<vtkImage->GetDimensions()[1]<<" "<<vtkImage->GetDimensions()[2]<<std::endl;
    std::cout<<"Spacing:"<<vtkImage->GetSpacing()[0]<<" "<<vtkImage->GetSpacing()[1]<<" "<<vtkImage->GetSpacing()[2]<<std::endl;
    std::cout<<"Origin:"<<vtkImage->GetOrigin()[0]<<" "<<vtkImage->GetOrigin()[1]<<" "<<vtkImage->GetOrigin()[2]<<std::endl;
    std::cout<<"ScalarRange:"<<vtkImage->GetScalarRange()[0]<<" "<<vtkImage->GetScalarRange()[1]<<std::endl;
    std::cout<<"Extent:"<<vtkImage->GetExtent()[0]<<" "<<vtkImage->GetExtent()[1]<<" "<<vtkImage->GetExtent()[2]<<" "<<vtkImage->GetExtent()[3]<<" "<<vtkImage->GetExtent()[4]<<" "<<vtkImage->GetExtent()[5]<<std::endl;
    std::cout<<"Bounds:"<<vtkImage->GetBounds()[0]<<" "<<vtkImage->GetBounds()[1]<<" "<<vtkImage->GetBounds()[2]<<" "<<vtkImage->GetBounds()[3]<<" "<<vtkImage->GetBounds()[4]<<" "<<vtkImage->GetBounds()[5]<<std::endl;
    std::cout<<"Center:"<<vtkImage->GetCenter()[0]<<" "<<vtkImage->GetCenter()[1]<<" "<<vtkImage->GetCenter()[2]<<std::endl;
    std::cout << "---------------------" << std::endl;
}

void ImageLoading::fillVTKimageInftoStruct(vtkSmartPointer<vtkImageData> vtkImage,vtkImageInf* infstruct){
    vtkImage->GetDimensions(infstruct->dimension);
    vtkImage->GetSpacing(infstruct->spacing);
    vtkImage->GetOrigin(infstruct->origin);
    vtkImage->GetScalarRange(infstruct->scalarRange);
    vtkImage->GetExtent(infstruct->extent);
    vtkImage->GetBounds(infstruct->bound);
    vtkImage->GetCenter(infstruct->center);
}

//需要注意的是，vtkImage中不包含旋转转换矩阵，所以将vtk转化为itk时，统一初始化为单位矩阵了(后续考虑添加一个矩阵进来)
void ImageLoading::saveVTKimageToNII(vtkSmartPointer<vtkImageData> vtkImage,std::string filename){

    std::experimental::filesystem::path currentPath = std::experimental::filesystem::current_path();
    //在当前路径下创建一个文件夹
    std::experimental::filesystem::path savePath = currentPath / "TemporarySaving";
    if (!std::experimental::filesystem::exists(savePath)){
        std::experimental::filesystem::create_directory(savePath);
    }
    //在文件夹下创建一个文件
    std::experimental::filesystem::path saveFile = savePath / filename;
    //往saveFile后加入后缀
    saveFile += ".nii.gz";

    VTKToITKFilterType::Pointer vtkToItkFilter = VTKToITKFilterType::New();

    vtkToItkFilter->SetInput(vtkImage);
    vtkToItkFilter->Update();

    ThreeDImageType::Pointer itkImage = vtkToItkFilter->GetOutput();
//    itkImage->SetDirection()
    itkImage->Update();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(saveFile.string());
    writer->SetInput(itkImage);

    try {
        writer->Update();
        std::cout << "Image saved to " << saveFile << std::endl;
    } catch (itk::ExceptionObject &ex) {
        std::cerr << "Error: " << ex << std::endl;
    }
}

void ImageLoading::saveVTKimageToNIIinFilepath(vtkSmartPointer<vtkImageData> vtkImage,std::string filepath){
    std::experimental::filesystem::path saveFile = filepath;
    VTKToITKFilterType::Pointer vtkToItkFilter = VTKToITKFilterType::New();

    vtkToItkFilter->SetInput(vtkImage);
    vtkToItkFilter->Update();

    ThreeDImageType::Pointer itkImage = vtkToItkFilter->GetOutput();
    itkImage->Update();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(saveFile.string());
    writer->SetInput(itkImage);

    try {
        writer->Update();
        std::cout << "Image saved to " << saveFile << std::endl;
    } catch (itk::ExceptionObject &ex) {
        std::cerr << "Error: " << ex << std::endl;
    }
}

void ImageLoading::savePolarmapRowDatainFilepath(cardiacModelParameter CMP, std::vector<maxValuePointInf>& pointCloud, std::string& filepath){
    std::ofstream file(filepath);
    if (!file)
    {
        std::cout << "Failed to open file for writing." << std::endl;
        return;
    }

    // Write the total number of points as the first line
    file << pointCloud.size() << "," << CMP.sectorCount << "," << CMP.hemiRingCount << std::endl;

    // Write each point's data on a new line
    for (const auto& point : pointCloud)
    {
        file << point.x << "," << point.y << "," << point.z << "," << point.maxValue << std::endl;
    }

    file.close();
}

void ImageLoading::writeVectorToCSV(const std::vector<double>& data, const std::string& filename){
    std::experimental::filesystem::path currentPath = std::experimental::filesystem::current_path();
    //在当前路径下创建一个文件夹
    std::experimental::filesystem::path savePath = currentPath / "TemporarySaving";
    if (!std::experimental::filesystem::exists(savePath)){
        std::experimental::filesystem::create_directory(savePath);
    }
    //在文件夹下创建一个文件
    std::experimental::filesystem::path saveFile = savePath / filename;
    //往saveFile后加入后缀
    saveFile += ".csv";

    //如果文件已经存在，则删除
    if (std::experimental::filesystem::exists(saveFile)){
        std::experimental::filesystem::remove(saveFile);
    }

    std::ofstream file(saveFile);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    for (size_t i = 0; i < data.size(); ++i) {
        file << i + 1 << "," << data[i] << "\n";
    }

    file.close();
}

void ImageLoading::getPolarmapSegmentContourFromSRC(const QString& resourcePath,vtkSmartPointer<vtkImageData> image){
    // 使用ITK读取临时文件
    itk::ImageFileReader<itk::Image<unsigned char, 2>>::Pointer reader = itk::ImageFileReader<itk::Image<unsigned char, 2>>::New();
    reader->SetFileName(resourcePath.toStdString());
    reader->Update();

    // 将ITK图像转换为VTK图像
    typedef itk::ImageToVTKImageFilter<itk::Image<unsigned char, 2>> ConverterType;
    ConverterType::Pointer converter = ConverterType::New();
    converter->SetInput(reader->GetOutput());
    converter->Update();

    image->DeepCopy(converter->GetOutput());
    image->Modified();

//    // 打印image的信息
//    int* dims = image->GetDimensions();
//    std::cout << "Image dimensions: " << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;
//    std::cout << "Image scalar type: " << image->GetScalarTypeAsString() << std::endl;
//    std::cout << "Image number of scalar components: " << image->GetNumberOfScalarComponents() << std::endl;
//    std::cout << "Image number of points: " << image->GetNumberOfPoints() << std::endl;
}
