#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceVerifyPictureDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

FaceVerify *sFaceVerify;

static int verifySamePerson();

static int verifyDifferentPerson();

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

    setCloudAddr("127.0.0.1", 8090);
    setCloudLoginAccount("admin", "666666");

    //step 3: create FaceVerify Instance (TERMINAL or CLOUD)
    sFaceVerify = FaceVerify::createInstance(TERMINAL);
    LOG(TAG, "sFaceVerify(%p)", sFaceVerify);

    //step 4: verifySamePerson
    verifySamePerson();

    //step 5: verifyDifferentPerson
    verifyDifferentPerson();

    return 0;
}

int verifySamePerson() {
    unsigned char *jpeg1 = 0;
    int jpeg1Len = 0;
    unsigned char *jpeg2 = 0;
    int jpeg2Len = 0;

    loadFile(jpeg1, jpeg1Len, (char *) (PICTURE_ROOT + "liudehua_feature1.jpg").c_str());
    loadFile(jpeg2, jpeg2Len, (char *) (PICTURE_ROOT + "liudehua_feature2.jpg").c_str());

    Image image1;
    image1.data = jpeg1;
    image1.format = ImageFormat_UNKNOWN;
    image1.dataLen = jpeg1Len;

    Image image2;
    image2.data = jpeg2;
    image2.format = ImageFormat_UNKNOWN;
    image2.dataLen = jpeg2Len;

    Face image1Face;
    list <Face> image2Faces;
    list <VerifyResult> verifyResultList;

    int status = sFaceVerify->verifyPicture(image1, image1Face, image2, image2Faces, verifyResultList);
    if (status != OK) {
        LOG(TAG, "verifyPicture error(%d)", status);
        return status;
    }

    for (list<VerifyResult>::iterator it = verifyResultList.begin(); it != verifyResultList.end(); ++it) {
        if (it->similarity >= 70) {
            LOG(TAG, "verifySamePerson OK similarity(%f)", it->similarity);
        } else {
            LOG(TAG, "verifySamePerson 000FAIL similarity(%f)", it->similarity);
        }
    }

    return OK;
}

int verifyDifferentPerson() {
    unsigned char *jpeg1 = 0;
    int jpeg1Len = 0;
    unsigned char *jpeg2 = 0;
    int jpeg2Len = 0;

    loadFile(jpeg1, jpeg1Len, (char *) (PICTURE_ROOT + "liudehua_feature1.jpg").c_str());
    loadFile(jpeg2, jpeg2Len, (char *) (PICTURE_ROOT + "zhangxueyou_feature1.jpg").c_str());

    Image image1;
    image1.data = jpeg1;
    image1.format = JPEG;
    image1.dataLen = jpeg1Len;

    Image image2;
    image2.data = jpeg2;
    image2.format = JPEG;
    image2.dataLen = jpeg2Len;

    Face image1Face;
    list <Face> image2Faces;
    list <VerifyResult> verifyResultList;

    int status = sFaceVerify->verifyPicture(image1, image1Face, image2, image2Faces, verifyResultList);
    if (status != OK || verifyResultList.size() == 0) {
        LOG(TAG, "verifyPicture error(%d)", status);
        return status;
    }

    for (list<VerifyResult>::iterator it = verifyResultList.begin(); it != verifyResultList.end(); ++it) {
        if (it->similarity < 70) {
            LOG(TAG, "verifyDifferentPerson OK similarity(%f)", it->similarity);
        } else {
            LOG(TAG, "verifyDifferentPerson FAIL similarity(%f)", it->similarity);
        }
    }

    return OK;
}