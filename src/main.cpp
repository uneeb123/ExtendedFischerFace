
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <cmath>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <vector>


#define PI 3.14159265


void apply_Low(cv::Mat& input,double sigma);
void apply_High(cv::Mat& input,double sigma);
void rotation_matrix(cv::Mat& output, double alpha, double x, double y, double z);
void apply_transformation(std::string name_result,cv::Mat& input, cv::Mat& output,cv::Mat& homography);


int getfiles (std::string dir, std::vector<std::string> &files , std::vector<std::string> &names)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        std::cout << "Error opening directory" << dir << std::endl;
        return -1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        std::string s(dirp->d_name);
        if(s != "." && s!= "..")
        {
            files.push_back(dir + "/" + s);
            names.push_back(s);
            //std::cout<< dir + "/" + std::string(dirp->d_name) << std::endl;
        }



    }
    closedir(dp);
    return 0;
}

int main()
{

    std::vector<std::string> images_names,test_names,files,files_test;
    std::vector<int> label;
    std::vector<cv::Mat> images;

    int class_1[] = {5,22,27,28,32,37};
    int size_class = 6;

    std::string trainSamples("./simple_samples"), testSamples("./test_samples");

    getfiles(trainSamples,images_names,files);

    int i,j;
    bool found;

    for(i = 0; i < images_names.size(); ++i)
    {

        cv::Mat orig_image;
        orig_image = cv::imread( images_names[i], CV_LOAD_IMAGE_GRAYSCALE );

        if( !orig_image.data )
        {
            std::cout<< "No image data " << images_names[i] << std::endl;
            return -1;
        }

        images.push_back(orig_image);
        int a = (files[i][5] - '0') * 10 + (files[i][6] - '0');

        //std::cout<<a << " " <<images_names[i][5] << images_names[i][6]<<std::endl;

        found = false;

        for(j = 0; j < size_class ; ++j)
        {
            if(a == class_1[j])
            {
                found = true;
                break;

            }

        }

        if(found)
            label.push_back(0);
        else
            label.push_back(1);
    }


    cv::Ptr<cv::FaceRecognizer> fisher = cv::createFisherFaceRecognizer();
    fisher->train(images, label);

    getfiles(testSamples,test_names,files_test);

    double positive = 0.0, negative = 0.0;

    for(i = 0; i < test_names.size(); ++i)
    {
        cv::Mat orig_image;
        orig_image = cv::imread( test_names[i], CV_LOAD_IMAGE_GRAYSCALE );

        if( !orig_image.data )
        {
            std::cout<< "No image data \n";
            return -1;
        }

        int b,PredictedClass = (files_test[i][5] - '0') * 10 + (files_test[i][6] - '0');


        found = false;

        for(j = 0; j < size_class ; ++j)
        {
            if(PredictedClass == class_1[j])
            {
                found = true;
                break;

            }

        }

        if(found)
            b = 0;
        else
            b = 1;

        if(b == fisher->predict(orig_image))
        {
            positive = positive + 1.0;
        }

        else
        {
            negative = negative + 1.0;
            std::cout<<"Error " <<PredictedClass << std::endl;
        }


    }

    std::cout<<"Succes Rate Simple Fisher " << positive / (positive + negative) * 100.0 <<" %\n";



/*

    cv::Mat orig_image;
    orig_image = cv::imread( "test.pgm", CV_LOAD_IMAGE_GRAYSCALE );

    if( !orig_image.data )
    {
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
*/
    return 0;
}
