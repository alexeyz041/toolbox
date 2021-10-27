#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/features2d.hpp>


using namespace cv;


int main( int argc, char** argv )
{
  // Read image
  Mat im = imread("blob.jpg", IMREAD_GRAYSCALE);

  cv::SimpleBlobDetector::Params params;
  params.minDistBetweenBlobs = 50.0f;
  params.filterByInertia = false;
  params.filterByConvexity = false;
  params.filterByColor = true;
  params.filterByCircularity = false;
  params.filterByArea = true;
  params.minArea = 300.0f;
  params.maxArea = 600.0f;

  // Set up the detector with default parameters.
  cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params); 

  // Detect blobs.
  std::vector<KeyPoint> keypoints;
  detector->detect( im, keypoints );

  // Draw detected blobs as red circles.
  // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
  Mat im_with_keypoints;
  drawKeypoints( im, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

  // Show blobs
  imshow("keypoints", im_with_keypoints );
  waitKey(0);
}
