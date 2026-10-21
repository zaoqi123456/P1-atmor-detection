#include <opencv2/opencv.hpp>
#include <iostream>

int main(){
    cv::VideoCapture cap("video.avi");
    cv::Mat frame;
    
    while(true){

        cv::namedWindow("test", cv::WINDOW_NORMAL); 
        cv::resizeWindow("test", 1980, 1480); 

        cap >> frame;
        if(frame.empty()) break;
        cv::Mat hsv;
        cv::cvtColor(frame,hsv,cv::COLOR_BGR2HSV);
        cv::Mat mask1,mask2;
        cv::inRange(hsv,cv::Scalar(0,100,150),cv::Scalar(20,255,255),mask1);
        cv::inRange(hsv,cv::Scalar(170,100,150),cv::Scalar(180,255,255),mask2);
        cv::Mat mask;
        cv::bitwise_or(mask1,mask2,mask);     //掩膜完成

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
        std::vector<cv::Point2f> contour;
        for(size_t i = 0;i < contours.size();i++){
            double area = cv::contourArea(contours[i]);
            if(area < 10) continue;
            cv::Rect test = cv::boundingRect(contours[i]);
            double wid = test.width,hei = test.height;
            double hei_wid = hei/wid , wid_hei = wid/hei;
            if(hei_wid < 2 && wid_hei < 2) continue;   //筛选完毕

            contour.insert(contour.end(),contours[i].begin(),contours[i].end());
        }
        cv::RotatedRect rorated = cv::minAreaRect(contour);
        cv::Point2f ver[4];
        rorated.points(ver);
        std::sort(ver,ver+4,[](const cv::Point2f &a,const cv::Point2f &b){return a.y < b.y;});
        std::vector<cv::Point2f> sorted;
        cv::Point2f left_top = (ver[0].x < ver[1].x) ? ver[0] : ver[1];sorted.push_back(left_top);
        cv::Point2f right_top = (ver[0].x > ver[1].x) ? ver[0] : ver[1];sorted.push_back(right_top);
        cv::Point2f right_bot = (ver[2].x > ver[3].x) ? ver[2] : ver[3];sorted.push_back(right_bot);
        cv::Point2f left_bot = (ver[2].x < ver[3].x) ? ver[2] : ver[3];sorted.push_back(left_bot);

        for(int i = 0;i < 4;i++){
            cv::line(frame,sorted[i],sorted[( i + 1 ) % 4],cv::Scalar(0,255,0),1,cv::LINE_AA);
        }

        std::vector<cv::Point3f> object;
        object.push_back(cv::Point3f(-67.5f,-27.5f,0.0f));
        object.push_back(cv::Point3f(-67.5f,27.5f,0.0f));
        object.push_back(cv::Point3f(67.5f,27.5f,0.0f));
        object.push_back(cv::Point3f(67.5f,-27.5f,0.0f));

        cv::Mat camera = (cv::Mat_<double>(3,3) << 9.28130989e+02,0,3.77572945e+02,0,
                                                9.30138391e+02,2.83892859e+02,0,
                                                0,1.0000);
        cv::Mat dist = (cv::Mat_<double>(1,5) << -2.54433647e-01, 5.69431382e-01, 
                                                3.65405229e-03, -1.09433818e-03, 
                                                -1.33846840e+00);
        cv::Mat rvec,tvec;
        static cv::Mat rvec_pre,tvec_pre;
        static bool has_pre = false;
        if(has_pre){
            rvec = rvec_pre;
            tvec = tvec_pre;
            cv::solvePnP(object,sorted,camera,dist,rvec,tvec,true,cv::SOLVEPNP_ITERATIVE);
        }
        else{
            cv::solvePnP(object,sorted,camera,dist,rvec,tvec,false,cv::SOLVEPNP_IPPE);
        }
        rvec_pre = rvec.clone();
        tvec_pre = tvec.clone();
        has_pre = true;

        std::vector<cv::Point3f> flu;
        flu.push_back(cv::Point3f(0.0f,0.0f,0.0f));
        flu.push_back(cv::Point3f(50.0f,0.0f,0.0f));
        flu.push_back(cv::Point3f(0.0f,50.0f,0.0f));
        flu.push_back(cv::Point3f(0.0f,0.0f,50.0f));
        std::vector<cv::Point2f> im;
        cv::projectPoints(flu,rvec,tvec,camera,dist,im);
        
        cv::arrowedLine(frame,im[0],im[1],cv::Scalar(255,0,0),1,cv::LINE_AA,0,0.15);
        cv::arrowedLine(frame,im[0],im[2],cv::Scalar(0,255,0),1,cv::LINE_AA,0,0.15);
        cv::arrowedLine(frame,im[0],im[3],cv::Scalar(0,0,255),1,cv::LINE_AA,0,0.15);
        cv::imshow("test",frame);
        if(cv::waitKey(60) == 31) break;
    }
    return 0;
}
    