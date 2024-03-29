#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;


std::string make_name(std::string path, std::string ending)
{
    path.erase(path.find_last_of('.'), std::string::npos);
    path = "out/" + path + ending;
    return path;
}


int main(int argc, char **argv)
{
    if(argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "  ext image.jpg" << std::endl;
        return 1;
    }
    std::string image_path = samples::findFile(argv[1]);
    Mat img = imread(image_path, IMREAD_GRAYSCALE);
    if(img.empty()) {
        std::cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    std::cout << "channels = " << img.channels() << std::endl;
    std::cout << "width = " << img.cols << std::endl;
    std::cout << "height = " << img.rows << std::endl;
    std::cout << "depth = " << img.depth() << std::endl;

    Mat img2 = Mat::zeros(img.size(), CV_8U);
    bitwise_not(img, img2);

    Mat img3 = Mat::zeros(img.size(), CV_8U);
    threshold(img2, img3, 128, 0, /*threshold_type*/ 3);

    Mat canny_output;
    int thresh = 0;
    Canny(img3, canny_output, thresh, thresh*2);

    std::vector<std::vector<Point>> contours;
    findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<Point> > contours_poly( contours.size() );
    std::vector<Rect> boundRect( contours.size() );

    for(size_t i = 0; i < contours.size(); i++) {
        approxPolyDP( contours[i], contours_poly[i], 3, true );
        boundRect[i] = boundingRect( contours_poly[i] );
    }

    Mat drawing = Mat::zeros( canny_output.size(), CV_8U );
    
    Rect target;
    bool found = false;
    for(size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(255);
//        drawContours(drawing, contours_poly, (int)i, color);
        if(boundRect[i].width > 300 && boundRect[i].height > 300) {
            Scalar mean_color = mean( img3( boundRect[i] ) );
            if(mean_color.val[0] > 100) {
                rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
                if(boundRect[i].width > target.width && boundRect[i].height > target.height) {
                    target = boundRect[i];
                    found = true;
                }
            }
        }
    }

    if(!found) {
        std::cout << "image not found" << std::endl;
        return 1;
    }

    Mat img4 = imread(image_path, IMREAD_COLOR);
    Mat dst;
    double scale = target.height / 400.;
    std::cout << "scale = " << scale << std::endl;
    double w = target.width / scale;
    resize(img4(target), dst, Size(w, 400), 0, 0, INTER_AREA);

    std::string out = make_name(argv[1], "_ext.jpg");
    imwrite(out, dst);
//    imshow("Display window", drawing);
//    waitKey(0);
    return 0;
}


void rotate(Mat &src, Mat &dst)
{
    int hwidth = src.rows / 2;
    int hheight = src.cols / 2;
    double angle = 5.00 * M_PI / 180.0;

    for (int x = 0; x < src.rows; x++) {
        for (int y = 0; y < src.cols; y++) {
          int xt = x - hwidth;
          int yt = y - hheight;
          int xs = (int)round((cos(angle) * xt - sin(angle) * yt) + hwidth);
          int ys = (int)round((sin(angle) * xt + cos(angle) * yt) + hheight); 
          dst.at<Vec3b>(x,y) = src.at<Vec3b>(xs, ys);
        }
    }
}
