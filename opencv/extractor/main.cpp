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
    for(size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(255);
//        drawContours(drawing, contours_poly, (int)i, color);
        if(boundRect[i].width > 300 && boundRect[i].height > 300) {
            Scalar mean_color = mean( img3( boundRect[i] ) );
            if(mean_color.val[0] > 100) {
                rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);
                if(boundRect[i].width > target.width && boundRect[i].height > target.height) {
                    target = boundRect[i];
                }
            }
        }
    }

    Mat img4 = imread(image_path, IMREAD_COLOR);
    Mat dst;
    double scale = target.height / 400.;
    double w = target.width / scale;
    resize(img4(target), dst, Size(w,400), 0, 0, INTER_AREA);

    std::string out = make_name(argv[1], "_ext.jpg");
    imwrite(out, dst);
//    imshow("Display window", drawing);
//    waitKey(0);
    return 0;
}


/*


//https://www.tutorialspoint.com/opencv/opencv_scaling.htm
int scale {

// Reading the Image from the file and storing it in to a Matrix object
      String file ="E:/OpenCV/chap24/transform_input.jpg";
      Mat src = Imgcodecs.imread(file);

      // Creating an empty matrix to store the result
      Mat dst = new Mat();

      // Creating the Size object
      Size size = new Size(src.rows()*2, src.rows()*2);

      // Scaling the Image
      Imgproc.resize(src, dst, size, 0, 0, Imgproc.INTER_AREA);

      // Writing the image
      Imgcodecs.imwrite("E:/OpenCV/chap24/scale_output.jpg", dst);

}


//https://stackoverflow.com/questions/40461143/c-opencv-rotating-image-by-an-arbitrary-number-degree

int rotate()
{
Mat image1 = imread("Balloon.jpg", CV_LOAD_IMAGE_ANYCOLOR);
   Mat rotC(image1.cols, image1.rows, image1.type());
 
  #define PI 3.14156
void rotation(Mat image1)
{
    int hwidth = image1.rows / 2;
    int hheight = image1.cols / 2;
    double angle = 45.00 * PI / 180.0;

    for (int x = 0;x < image1.rows;x++) {

        for (int y = 0; y < image1.cols;y++) {


        int xt = x - hwidth;
        int yt = y - hheight;


        int xs = (int)round((cos(angle) * xt - sin(angle) * yt) + hwidth);
        int ys = (int)round((sin(angle) * xt + cos(angle) * yt) + hheight); 

         rotC.at<Vec3b>(x,y) = image1.at<Vec3b>(xs, ys);

      }
    }
 }



  int main()
{
 rotation(image1);
 imshow("color", rotC);
 waitKey(0);
}
}
*/