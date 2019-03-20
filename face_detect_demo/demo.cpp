#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static string PICTURE_ROOT = "../pictures/";

static int detectPicture();

static int detectVideo();

static int detectMP4();

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

    //step 4: detectPicture
    //detectPicture();

    //step 5: detectVideo
    //detectVideo();

    detectMP4();

    FaceDetect::deleteInstance(sFaceDetect);
    return 0;
}

int detectPicture() {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, (char *) (PICTURE_ROOT + "fanbingbing_with_glass.jpg").c_str());

    Image image;
    image.data = data;
    image.format = ImageFormat_UNKNOWN;
    image.dataLen = dataLen;

    list <Face> faceList;
    DetectParameter parameter = sFaceDetect->getPictureParameter();
    parameter.checkQuality = true;
    parameter.checkLiveness = true;
    parameter.checkAge = true;
    parameter.checkGender = true;
    parameter.checkExpression = true;
    parameter.checkGlass = true;

    sFaceDetect->setPictureParameter(parameter);
    int status = sFaceDetect->detectPicture(image, faceList);

    if (status != OK) {
        LOG(TAG, "detectPicture error(%d)", status);
        if (image.data) {
            free(image.data);
            image.data = 0;
        }
        return status;
    }

    LOG(TAG, "detectPicture faceNum(%d)", faceList.size());
    for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
        LOG(TAG, "detectPicture faces[%d] quality(%d) liveness(%d) age(%d) gender(%d) expression(%d) glass(%d)",
            it->trackId, it->attribute.quality.score, it->attribute.liveness.score, it->attribute.age,
            it->attribute.gender, it->attribute.expression, it->attribute.glass);
    }

#if 1
    Image rgbImage = Codec::toBGR888(image);
    Tools::drawFaceRect(rgbImage, faceList, 0xFF0000);
    Tools::drawFacePoint(rgbImage, faceList, 0x00FF00);

    cv::Mat yuvImg;
    yuvImg.create(rgbImage.height, rgbImage.width, CV_8UC3);
    memcpy(yuvImg.data, rgbImage.data, rgbImage.width * rgbImage.height * 3);

    float scale = 1.0;
    Size dsize = Size(yuvImg.cols * scale, yuvImg.rows * scale);
    Mat image2 = Mat(dsize, CV_32S);
    resize(yuvImg, image2, dsize);

    cv::imshow("yuv", image2);
    waitKey(0);
#endif

    if (image.data) {
        free(image.data);
        image.data = 0;
    }
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

        list <Face> faceList;
        sFaceDetect->detectVideo(image, faceList);

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

int detectMP4() {
    DetectParameter parameter = sFaceDetect->getVideoParameter();
    parameter.checkQuality = true;
    parameter.checkLiveness = false;
    sFaceDetect->setVideoParameter(parameter);

    IplImage *frame = NULL;
    CvCapture *capture = NULL;
    capture = cvCreateFileCapture("/Users/junyuan.hjy/Downloads/VID-20190318-WA0011.mp4");
    frame = cvQueryFrame(capture);

    cvNamedWindow("frame");
    int frameIndex = 0;
    while (frame) {
        LOG(TAG, "frame(%d): size(%dx%d) frame->imageSize=%d depth=%d widthStep=%d nChannels=%d nSize=%d", frameIndex,
            frame->width, frame->height, frame->imageSize, frame->depth, frame->widthStep, frame->nChannels,
            frame->nSize);

        if (frameIndex < 0) {
            frame = cvQueryFrame(capture);
            frameIndex++;
            continue;
        }

        if (frameIndex == 1000) {
            //break;
        }

        Image image;
        image.data = (unsigned char *) frame->imageData;
        image.width = frame->widthStep / 3;
        image.height = frame->height;
        image.format = BGR888;

        list <Face> faceList;
        int status = sFaceDetect->detectVideo(image, faceList);

        int i = 0;
        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            LOG(TAG, "detectVideo faces[%d] confidence(%d) quality(%d) liveness(%d) image(%p) yaw=%f", i,
                it->confidence, it->attribute.quality.score, it->attribute.liveness.score, image.data, it->pose.yaw);
            Tools::drawFaceRect(image, *it, 0xFF0000);

            char display_text[100] = {0};
            sprintf(display_text, "conf:%d, quality:%d", it->confidence, it->attribute.quality.score);

            printTextToWindow(frame, display_text, it->rect.left, it->rect.top + 10);
            i++;
        }

        cvShowImage("frame", frame);
        cvWaitKey(1);

        frame = cvQueryFrame(capture);
        frameIndex++;
    }

    return 0;
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