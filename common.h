#pragma once

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

static char *KEY = "eyJ2ZW5kb3JJZCI6ImNlc2hpX3ZlbmRvciIsInJvbGUiOjEsImNvZGUiOiJFOEUyNzE1NEY3QjYxMDQ3QjQ0RUNDN0IyOUJFM0ZFQiIsImV4cGlyZSI6IjIwMTkwNjMwIiwidHlwZSI6MX0=";

#ifdef WIN32
#define LOG(TAG, fmt, args) printf("[%s] : " fmt "\n", TAG, ##args)
#else
#define LOG(TAG, fmt, args...) printf("[%s] : " fmt "\n", TAG, ##args)
#endif

using namespace cv;

void printTextToWindow(IplImage *frame, char *display_text, int x, int y, int color) {
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

    cv::putText(previewFrame, text, origin, font_face, font_scale,
                cv::Scalar((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF), thickness, 8, 0);
}

void printTextToWindow(Mat *previewFrame, char *display_text, int x, int y, int color) {
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
    cv::putText(*previewFrame, text, origin, font_face, font_scale,
                cv::Scalar((color) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF), thickness, 8, 0);
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