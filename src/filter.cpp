
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <fstream>
#include <string>

using namespace std;

cv::Mat normalized_gray_img(cv::Mat& m)
{
    cv::Mat mn= m.clone();

    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::minMaxLoc(mn, &minVal, &maxVal, &minLoc, &maxLoc );

    mn -= minVal;
    if ((maxVal - minVal) > 1e-7){
        mn *= 1.0/(maxVal - minVal);
    }
    return mn;
}

void save_double_gray_img(const string& name, cv::Mat& m)
{
    cv::Mat mn= normalized_gray_img(m);
    cv::Mat mf;
    mn.convertTo(mf, cv::DataType<float>::type, 255.0);
    cv::Mat cm;
    cvtColor(mf, cm, CV_GRAY2BGR);
    cv::imwrite(name, cm);
}

void gaussian (cv::Mat& kernel, double sigma)
{
    int n,m,i,j;
    m = kernel.rows;
    n = kernel.cols;

    double sum = 0.0;
    double k1,k2;

    k2 = static_cast<double>(-(m-1) / 2);


    for(i=0; i < m; ++i)
    {
        k1 = static_cast<double>(-(n-1) / 2);
        for(j = 0; j < n; ++j)
        {

            kernel.at<double>(i,j) = exp( - (k1 * k1 + k2 * k2) / (2 * sigma * sigma));

            sum = sum + kernel.at<double>(i,j);
            k1 = k1 + 1.0;
        }

        k2 = k2 + 1.0;

    }

    for(i = 0; i < m; ++i)
        for(j = 0; j < n; ++j)
            kernel.at<double>(i,j) = kernel.at<double>(i,j) / sum;

    std::ofstream ofs("kernel.txt");

    for(i = 0 ; i < m ; ++i)
    {
        for(j = 0; j < n ; ++j)
            ofs << kernel.at<double>(i,j)<<" ";

        ofs<<"\n";

    }

    ofs.close();





}


void shift(cv::Mat& input)
{
    int m,n,i,j;

    double aux1,aux2;

    m = input.rows / 2;
    n = input.cols / 2;
    for (i = 0; i < m; ++i)
    {
         for (j = 0; j < n; ++j)
         {
              aux1 = input.at<double>(i,j);
              input.at<double>(i,j) = input.at<double>(i+m,j+n);
              input.at<double>(i+m,j+n) = aux1;

              aux2 = input.at<double>(i+m,j);
              input.at<double>(i+m,j) = input.at<double>(i,j+n);
              input.at<double>(i,j+n) = aux2;
         }
    }

}

void apply_Low(cv::Mat& input,double sigma)
{

    int i,j;
    cv::Mat gaus(input.rows,input.cols,CV_64F),input_fft,result(input.rows,input.cols,CV_64F),output;

    gaussian(gaus,sigma);



    shift(gaus);


    std::ofstream ofs("shift.txt");

    for(i = 0 ; i < gaus.rows ; ++i)
    {
        for(j = 0; j < gaus.cols ; ++j)
            ofs << gaus.at<double>(i,j)<<" ";

        ofs<<"\n";

    }

    ofs.close();

    cv::dft(input,input_fft);




    for(i = 0; i < input_fft.rows; ++i)
        for(j = 0; j < input_fft.cols; ++j)
            result.at<double>(i,j) = gaus.at<double>(i,j) * input_fft.at<double>(i,j);

    cv::dft(result,output,cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    cv::normalize(output, output, 0, 1, CV_MINMAX);


    save_double_gray_img("image.jpg",output);


}


void apply_High(cv::Mat& input,double sigma)
{

    int i,j;
    double maximum,minimum;
    cv::Point minLoc,maxLoc;

    cv::Mat gaus(input.rows,input.cols,CV_64F),input_fft,result(input.rows,input.cols,CV_64F),output;


    gaussian(gaus,sigma);

    cv::minMaxLoc(gaus, &minimum, &maximum, &minLoc, &maxLoc );

    cv::Mat High_Filter = (gaus - maximum) * (-1.0);


    shift(High_Filter);

    cv::dft(input,input_fft);




    for(i = 0; i < input_fft.rows; ++i)
        for(j = 0; j < input_fft.cols; ++j)
            result.at<double>(i,j) = High_Filter.at<double>(i,j) * input_fft.at<double>(i,j);

    cv::dft(result,output,cv::DFT_INVERSE|cv::DFT_REAL_OUTPUT);
    cv::normalize(output, output, 0, 1, CV_MINMAX);


    save_double_gray_img("image_high.jpg",output);


}

void rotation_matrix(cv::Mat& output, double alpha, double x, double y, double z)
{
    output = cv::Mat::zeros(3, 3, CV_64F);
    output += cv::Mat::eye(3, 3, CV_64F) * cos(alpha);


    std::cout<<"Before AX\n";





    cv::Mat aX = cv::Mat::zeros(3, 3, CV_64F);
    aX.at<double>(0,1) = -z;
    aX.at<double>(0,2) = y;
    aX.at<double>(1,0) = z;
    aX.at<double>(1,2) = -x;
    aX.at<double>(2,0) = -y;
    aX.at<double>(2,1) = x;

    output += (aX * sin(alpha));

    std::cout<<"After AX\n";

    cv::Mat aS = cv::Mat::zeros(3, 1, CV_64F);
    aS.at<double>(0,0) = x;
    aS.at<double>(1,0) = y;
    aS.at<double>(2,0) = z;

    std::cout<<"Before ass\n";

    cv::Mat aSS = aS * aS.t();

    output += (aSS * (1-cos(alpha)));





}


void apply_transformation(std::string name_result,cv::Mat& input, cv::Mat& output,cv::Mat& homography)
{
    int i,j,x,y;
    double new_i,new_j;
    cv::Mat coord_a = cv::Mat::zeros(3, 1, CV_64F);
    cv::Mat coord_b = cv::Mat::zeros(3, 1, CV_64F);
    cv::Mat result = cv::Mat::zeros(input.rows, input.cols, CV_32F);
    coord_a.at<double>(2,0) = 1.0;

    for(i=0;i<input.rows;++i)
    {
        new_i = i;
        coord_a.at<double>(0,0) = new_i;
        for(j=0;j<input.cols;++j)
        {
            new_j = j;
            coord_a.at<double>(1,0) = new_j;
            coord_b = homography * coord_a;
            x = floor(coord_b.at<double>(0,0)/coord_b.at<double>(2,0));
            y = floor(coord_b.at<double>(1,0)/coord_b.at<double>(2,0));

            if(x >= 0 && x<input.rows && y >= 0 && y<input.cols)
            {
                result.at<float>(x,y) = input.at<float>(x,y);

            }


        }

    }

    output = result;

    save_double_gray_img(name_result,result);


}
