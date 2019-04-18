#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceExtracFeatureDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

FaceRegister *sFaceRegister;
FaceDetect *sFaceDetect;

static int extractFeature();

int main() {
    //step 1: authorize or enable debug
    enableDebug(true);
    int status = authorize(KEY);
    if (status != OK) {
        LOG(TAG, "authorize error(%d) key(%s)", status, KEY);
        return 0;
    } else {
        LOG(TAG, "authorize ok key(%s)", KEY);
    }

    //step 2: set Cloud addr and account if you using CloudServer
    setCloudAddr("101.132.89.177", 15000);
    setCloudLoginAccount("admin", "admin");

    //step 3: create FaceRegister Instance
    sFaceRegister = FaceRegister::createInstance();
    LOG(TAG, "sFaceRegister(%p)", sFaceRegister);
    if (!sFaceRegister) {
        LOG(TAG, "FaceRegister::createInstance error");
        return 0;
    }

    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);
    if (!sFaceDetect) {
        LOG(TAG, "FaceDetect::createInstance error");
        return 0;
    }

    status = extractFeature();
    if (status != OK) {
        LOG(TAG, "extractFeature error(%d)", status);
        return 0;
    }
    LOG(TAG, "extractFeature OK");
    return 0;
}

int extractFeature() {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, (char *) (PICTURE_ROOT + "fanbingbing_with_glass.jpg").c_str());

    Image image;
    image.data = data;
    image.format = ImageFormat_UNKNOWN;
    image.dataLen = dataLen;

    list <Face> faces;
    int status = sFaceDetect->detectPicture(image, faces);

    if (status != OK || faces.size() == 0) {
        LOG(TAG, "detectPicture error(%d)", status);
        return status;
    }

    string feature;
    status = sFaceRegister->extractFeature(image, *faces.begin(), MODEL_SMALL, feature);
    if (status != OK) {
        LOG(TAG, "extractFeature error(%d)", status);
        return status;
    }

    LOG(TAG, "extractFeature feature:\n%s", feature.c_str());
    return OK;
}
