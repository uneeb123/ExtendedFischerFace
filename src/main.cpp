
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

void apply_Low(cv::Mat& input,double sigma);
void apply_High(cv::Mat& input,double sigma);

int main()
{

    cv::Mat orig_image;
    orig_image = cv::imread( "marilyn.bmp", CV_LOAD_IMAGE_GRAYSCALE );

    if( !orig_image.data ){
        std::cout<< "No image data \n";
        return -1;
    }

    cv::Mat m;
    orig_image.convertTo(m, cv::DataType<double>::type);
    std::cout<<"Before low\n";

    apply_Low(m,20.0);
    apply_High(m,20.0);
    return 0;
}
