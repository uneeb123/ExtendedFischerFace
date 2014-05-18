
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
