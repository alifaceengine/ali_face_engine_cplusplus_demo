#pragma once

#ifdef OPENCV
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#endif

#ifdef WIN32
#else

#include <sys/time.h>

#endif

static char *KEY = "eyJ2ZW5kb3JJZCI6ImNlc2hpX3ZlbmRvciIsInJvbGUiOjEsImNvZGUiOiJFOEUyNzE1NEY3QjYxMDQ3QjQ0RUNDN0IyOUJFM0ZFQiIsImV4cGlyZSI6IjIwMTkwNjMwIiwidHlwZSI6MX0=";

#ifdef WIN32
#define LOG(TAG, fmt, args) printf("[%s] : " fmt "\n", TAG, ##args)
#else
#define LOG(TAG, fmt, args...) do{ \
    struct timeval tv; \
gettimeofday(&tv, 0); \
struct tm *p = localtime(&tv.tv_sec); \
    printf("%04d-%02d-%02d %02d:%02d:%02d.%03d I [%s] : " fmt "\n", \
    1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec/1000,\
    TAG, ##args);\
}while(0)\

#endif


int saveFile(vector<unsigned char> &buf, char *path) {
    FILE *pf = fopen(path, "wb");
    unsigned char tmp[1];
    if (pf != NULL) {
        for (auto c : buf) {
            tmp[0] = c;
            fwrite(tmp, sizeof(char), 1, pf);
        }
        fclose(pf);
        return 0;
    }
    return -1;
}

#ifdef OPENCV
using namespace cv;

void printTextToWindow(IplImage *frame, char *display_text, int x, int y, int color) {
    Mat previewFrame = cvarrToMat(frame);
    std::string text = display_text;
    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 0.5;
    int thickness = 1;
    int baseline;
    //获取文本框的长宽
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    //将文本框居中绘制
    cv::Point origin;
    origin.x = x;
    origin.y = y;

    cv::putText(previewFrame, text, origin, font_face, font_scale,
                cv::Scalar((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF), thickness, 8, 0);
}

void printTextToWindow(Mat *previewFrame, char *display_text, int x, int y, int color) {
    std::string text = display_text;
    int font_face = cv::FONT_HERSHEY_COMPLEX;
    double font_scale = 0.5;
    int thickness = 1;
    int baseline;
    //获取文本框的长宽
    cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    //将文本框居中绘制
    cv::Point origin;
    origin.x = x;
    origin.y = y;
    cv::putText(*previewFrame, text, origin, font_face, font_scale,
                cv::Scalar((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF), thickness, 8, 0);
}

static int MatToJpeg(const cv::Mat mat, string &file) {
    if (mat.empty()) {
        return 0;
    }

    std::vector<unsigned char> buff;
    std::vector<int> param = std::vector<int>(2);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 95; // default(95) 0-100
    cv::imencode(".jpg", mat, buff, param);
    saveFile(buff, (char *) file.c_str());
    return 0;
}

static int IplImageToJpeg(const IplImage *iplImage, string &file) {
    if (!iplImage) {
        return 0;
    }

#if 1
    cv::Mat mat = cv::cvarrToMat(iplImage);
#else
    cv::Mat mat = iplImage;
#endif
    return MatToJpeg(mat, file);
}

#endif

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
    return -1;
}

int saveFile(unsigned char *buf, int len, char *path) {
    FILE *pf = fopen(path, "wb");
    if (pf != NULL) {
        fwrite(buf, sizeof(char), len, pf);
        fclose(pf);
        return 0;
    }
    return -1;
}