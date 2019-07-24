#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "FaceEngine.h"
#include "../common.h"

#define TAG "ToolCompareFeaturesDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

int main() {
    //step 1: authorize or enable debug
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "setPersistencePath error(%d) key(%s)", status);
        return -1;
    } else {
        LOG(TAG, "setPersistencePath ok key(%s)");
    }

    FaceRegister *sFaceRegister = FaceRegister::createInstance();
    if (!sFaceRegister) {
        LOG(TAG, "FaceRegister::createInstance error");
        return -1;
    }

    FaceDetect *sFaceDetect = FaceDetect::createInstance(TERMINAL);
    if (!sFaceDetect) {
        LOG(TAG, "FaceDetect::createInstance error");
        return -1;
    }

    unsigned char *jpeg1 = 0;
    int jpeg1Len = 0;
    unsigned char *jpeg2 = 0;
    int jpeg2Len = 0;

    loadFile(jpeg1, jpeg1Len, (char *) (PICTURE_ROOT + "face03.jpg").c_str());
    loadFile(jpeg2, jpeg2Len, (char *) (PICTURE_ROOT + "face04.jpg").c_str());

    Image image1;
    image1.data = jpeg1;
    image1.format = ImageFormat_UNKNOWN;
    image1.dataLen = jpeg1Len;

    Image image2;
    image2.data = jpeg2;
    image2.format = ImageFormat_UNKNOWN;
    image2.dataLen = jpeg2Len;

    list <Face> faces1;
    sFaceDetect->detectPicture(image1, faces1);

    list <Face> faces2;
    sFaceDetect->detectPicture(image2, faces2);

    if (faces1.size() == 0) {
        LOG(TAG, "image1 no face");
        return -1;
    }

    if (faces2.size() == 0) {
        LOG(TAG, "image2 no face");
        return -1;
    }

    string feature1;
    sFaceRegister->extractFeature(image1, *faces1.begin(), MODEL_SMALL, feature1);

    string feature2;
    sFaceRegister->extractFeature(image2, *faces2.begin(), MODEL_SMALL, feature2);

    if (feature1.size() == 0) {
        LOG(TAG, "feature1 is empty");
        return -1;
    }

    if (feature2.size() == 0) {
        LOG(TAG, "feature2 is empty");
        return -1;
    }

    float score = Tools::compareFeatures(feature1, feature2);

    LOG(TAG, "image1 vs image2 similarity = %f", score);

    return 0;
}