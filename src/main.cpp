
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

#define PI 3.14159265


void apply_Low(cv::Mat& input,double sigma);
void apply_High(cv::Mat& input,double sigma);
void rotation_matrix(cv::Mat& output, double alpha, double x, double y, double z);
void apply_transformation(std::string name_result,cv::Mat& input, cv::Mat& output,cv::Mat& homography);

int main()
{

    cv::Mat orig_image;
    orig_image = cv::imread( "marilyn.bmp", CV_LOAD_IMAGE_GRAYSCALE );

    if( !orig_image.data ){
        std::cout<< "No image data \n";
        return -1;
    }

    cv::Mat m,n;

    orig_image.convertTo(m, cv::DataType<double>::type);
    orig_image.convertTo(n, cv::DataType<float>::type);
    std::cout<<"Before low\n";

    apply_Low(m,20.0);
    apply_High(m,20.0);

    cv::Mat R;

    rotation_matrix(R,PI/180.0 * 0.0,1.0,0.0,0.0);

    std::cout<<R<<std::endl;

    cv::Mat out;

    std::cout<< "After rotate\n";

    apply_transformation("rotate.jpg",n,out,R);

    return 0;
}
