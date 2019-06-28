#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceDetectPictureDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

static int detectPicture();

FaceDetect *sFaceDetect;

int main() {
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "authorize error(%d) key(%s)", status, KEY);
        return 0;
    } else {
        LOG(TAG, "authorize ok key(%s)", KEY);
    }

    setConfigInt("CFG_FD_NORM_SIZE", 320);
    //step 2: create FaceVerify Instance (TERMINAL or CLOUD)
    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    //sFaceDetect = FaceDetect::createInstance(CLOUD);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);

    //step 3: detectPicture
    detectPicture();

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

    DetectParameter parameter = sFaceDetect->getPictureParameter();
    parameter.checkQuality = true;
    parameter.checkLiveness = true;
    parameter.checkAge = true;
    parameter.checkGender = true;
    parameter.checkExpression = true;
    parameter.checkGlass = true;

    sFaceDetect->setPictureParameter(parameter);

    list <Face> faceList;
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
        LOG(TAG,
            "detectPicture faces[%d] rect(%d,%d,%d,%d) quality(%d) liveness(%d) age(%d) gender(%d) expression(%d) glass(%d)",
            it->trackId, it->rect.left, it->rect.top, it->rect.right, it->rect.bottom, it->attribute.quality.score,
            it->attribute.liveness.score, it->attribute.age, it->attribute.gender, it->attribute.expression,
            it->attribute.glass);
    }

#ifdef OPENCV
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
