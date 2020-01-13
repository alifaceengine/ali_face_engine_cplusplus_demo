#pragma once

#include <vector>

#ifdef OPENCV
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#endif

#ifdef WIN32
#else

#include <sys/time.h>
#include <dirent.h>

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
    1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec,\
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

int getFileList(string &path, list<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(path.c_str())) == NULL) {
        return -1;
    }
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(dirp->d_name);
    }
    closedir(dp);
    return 0;
}

bool comp(string p1, string p2) {
    int ymd = 0;
    int hms = 0;
    int mics = 0;
    int fn = 0;
    sscanf(p1.c_str(), "fd_%d-%d.%d_f%d.bmp", &ymd, &hms, &mics, &fn);

    int ymd2 = 0;
    int hms2 = 0;
    int mics2 = 0;
    int fn2 = 0;
    sscanf(p2.c_str(), "fd_%d-%d.%d_f%d.bmp", &ymd2, &hms2, &mics2, &fn2);

    bool bigger = false;
    if (ymd > ymd2) {
        return false;
    } else if (ymd < ymd2) {
        return true;
    } else {
        if (hms > hms2) {
            return false;
        } else if (hms < hms2) {
            return true;
        } else {
            if (mics > mics2) {
                return false;
            } else if (mics < mics2) {
                return true;
            } else {
                return false;
            }
        }
    }
}

int getPictureList(string &path, list<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(path.c_str())) == NULL) {
        printf("Can't open %s\n", path.c_str());
        return -1;
    }

    int index = 0;
    int ymd = 0;
    int hms = 0;
    int mics = 0;
    int fn = 0;

    while ((dirp = readdir(dp)) != NULL) {
        int num = sscanf(dirp->d_name, "fd_%d_%d-%d.%d_f%d.bmp", &index, &ymd, &hms, &mics, &fn);
        if (num != 5) {
            continue;
        }
        string filename(dirp->d_name);
        if (filename.c_str()[filename.size() - 1] != 'p' || filename.c_str()[filename.size() - 2] != 'm' ||
            filename.c_str()[filename.size() - 3] != 'b') {
            continue;
        }
        files.push_back(filename);
    }

    files.sort(comp);
    closedir(dp);
    return 0;
}