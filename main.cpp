#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// Helper function to create a masked (region-of-interest) image.
Mat regionOfInterest(const Mat& img, const vector<Point>& vertices)
{
    Mat mask = Mat::zeros(img.size(), img.type());
    
    // Create a polygon from vertices and fill it
    fillConvexPoly(mask, vertices, Scalar(255, 255, 255));
    
    // Perform bitwise AND to keep only the region inside the polygon
    Mat masked;
    bitwise_and(img, mask, masked);
    return masked;
}

int main(int argc, char** argv)
{
    // ------------------------------------------------------------
    // 1. Read input image (or video frame).
    // ------------------------------------------------------------
    // Usage: ./LaneDetection path_to_image
    // e.g., ./LaneDetection road.jpg
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <path_to_image>" << endl;
        return -1;
    }
    
    // Load image
    Mat frame = imread(argv[1]);
    if (frame.empty()) {
        cerr << "Error: Unable to open the image file: " << argv[1] << endl;
        return -1;
    }

    // Resize image for demonstration (optional).
    // resize(frame, frame, Size(800, 600));

    // ------------------------------------------------------------
    // 2. Preprocessing
    //    - Convert to grayscale
    //    - Apply Gaussian blur
    //    - Canny edge detection
    // ------------------------------------------------------------
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Apply Gaussian blur to reduce noise and improve edge detection.
    Mat blurred;
    GaussianBlur(gray, blurred, Size(5, 5), 0);

    // Canny edge detector
    Mat edges;
    Canny(blurred, edges, 50, 150);

    // ------------------------------------------------------------
    // 3. Mask the region of interest (ROI).
    //    Assuming the bottom half of the image is the probable location of lanes.
    //    In practice, you'll adjust the polygon vertices for your camera setup.
    // ------------------------------------------------------------
    // Define polygon vertices. Here, we pick a trapezoid covering bottom portion.
    vector<Point> roiVertices;
    roiVertices.push_back(Point(0, frame.rows));
    roiVertices.push_back(Point(frame.cols / 2 - 50, frame.rows / 2 + 50));
    roiVertices.push_back(Point(frame.cols / 2 + 50, frame.rows / 2 + 50));
    roiVertices.push_back(Point(frame.cols, frame.rows));

    Mat maskedEdges = regionOfInterest(edges, roiVertices);

    // ------------------------------------------------------------
    // 4. Use Hough Transform to find lanes (lines).
    // ------------------------------------------------------------
    vector<Vec4i> lines;
    // Adjust parameters (rho, theta, threshold, minLineLength, maxLineGap) as needed.
    HoughLinesP(maskedEdges, lines, 1, CV_PI / 180, 50, 50, 10);

    // ------------------------------------------------------------
    // 5. Draw the lines on a copy of the original frame.
    // ------------------------------------------------------------
    Mat lineImage = frame.clone(); // for visualization

    // Loop through all detected lines
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        // (x1, y1) and (x2, y2)
        line(lineImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
    }

    // ------------------------------------------------------------
    // 6. Display results.
    // ------------------------------------------------------------
    imshow("Original Frame", frame);
    imshow("Edges", edges);
    imshow("Masked Edges", maskedEdges);
    imshow("Lane Detection", lineImage);

    waitKey(0);
    return 0;
}
