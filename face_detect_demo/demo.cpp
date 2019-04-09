#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "FaceEngine.h"

#ifdef WIN32
#define LOG(TAG, fmt, args) printf("[%s] : " fmt "\n", TAG, ##args)
#else
#define LOG(TAG, fmt, args...) printf("[%s] : " fmt "\n", TAG, ##args)
#endif
#define TAG "FaceDetectDemo"

using namespace cv;
using namespace ali_face_engine;
using namespace std;

static int detectVideo();

static void printTextToWindow(IplImage *frame, char *display_text, int x, int y);

static int loadFile(unsigned char *&buf, int &len, char *path);

static int saveFile(char *buf, int len, char *path);

FaceDetect *sFaceDetect;

int main() {
    enableDebug(true);
    //step 1: authorize or enable debug
    LOG(TAG, "version(%s)", getVersion());
    int status = authorize(
            "eyJ2ZW5kb3JJZCI6ImNlc2hpX3ZlbmRvciIsInJvbGUiOjIsImNvZGUiOiIzRDE5RUIwNjY1OEE5MUExQzlCNDY0MzhDN0QwNDFGMyIsImV4cGlyZSI6IjIwMTkwMzMxIiwidHlwZSI6MX0=");
    LOG(TAG, "authorize(%d)", status);

    //step 2: set Cloud addr and account if you using CloudServer
    //setCloudAddr("127.0.0.1", 8080);
    setCloudAddr("101.132.89.177", 15000);
    setCloudLoginAccount("admin", "admin");

    //step 3: create FaceVerify Instance (TERMINAL or CLOUD)
    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    //sFaceDetect = FaceDetect::createInstance(CLOUD);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);

    //step 4: detectVideo
    detectVideo();

    FaceDetect::deleteInstance(sFaceDetect);
    return 0;
}

static int detectVideo() {
    VideoCapture capture(0);
    DetectParameter parameter = sFaceDetect->getVideoParameter();
    parameter.checkQuality = false;
    parameter.checkLiveness = false;
    sFaceDetect->setVideoParameter(parameter);

    while (1) {
        Mat previewFrame;
        capture >> previewFrame;

        Image image;
        image.data = previewFrame.data;
        image.width = previewFrame.cols;
        image.height = previewFrame.rows;
        image.format = BGR888;

        struct timeval tv;
        gettimeofday(&tv, 0);

        list<Face> faceList;
        sFaceDetect->detectVideo(image, faceList);

        struct timeval tv2;
        gettimeofday(&tv2, 0);
        LOG(TAG, "detectVideo cost(%d)", ((tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec) / 1000));

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            printf("detectVideo trackId(%d) rect(%d,%d,%d,%d)\n",
                   it->trackId,
                   it->rect.left,
                   it->rect.top,
                   it->rect.right,
                   it->rect.bottom);
            Tools::drawFaceRect(image, *it, 0xFF0000);
            Tools::drawFacePoint(image, *it, 0x00FF00);
        }

        imshow("video", previewFrame);
        waitKey(1);
    }
}

void printTextToWindow(IplImage *frame, char *display_text, int x, int y) {
    Mat previewFrame = cvarrToMat(frame);
    std::string text = display_text;
    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 0.8;
    int thickness = 1;
    int baseline;
    //获取文本框的长宽
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    //将文本框居中绘制
    cv::Point origin;
    origin.x = x;
    origin.y = y;
    cv::putText(previewFrame, text, origin, font_face, font_scale, cv::Scalar(0, 255, 0), thickness, 8, 0);
}

int loadFile(unsigned char *&buf, int &len, char *path) {
    FILE *pf = fopen(path, "rb");
    if (pf != NULL) {
        fseek(pf, 0, SEEK_END);
        len = ftell(pf);
        buf = (unsigned char *) malloc(len);
        fseek(pf, 0, SEEK_SET);
        fread(buf, 1, len, pf);
        fclose(pf);

        return 0;
    } else {
        LOG(TAG, "loadFile %s error", path);
        buf = NULL;
        len = 0;
        return -1;
    }
}

int saveFile(char *buf, int len, char *path) {
    FILE *pf = fopen(path, "wb");
    if (pf != NULL) {
        fwrite(buf, sizeof(char), len, pf);
        fclose(pf);
        return 0;
    }
    //LOGE(TAG, "saveFile %s error\n", path);
    return -1;
}