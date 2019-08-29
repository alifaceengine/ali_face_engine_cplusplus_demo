#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceVerifyVideoDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

FaceVerify *sFaceVerify;
FaceDetect *sFaceDetect;

static int verifyVideo();

static VerifyResult *getVerifyResult(int trackId);

int main() {
    //step 1: authorize or enable debug
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "setPersistencePath error(%d)", status);
        return 0;
    } else {
        LOG(TAG, "setPersistencePath ok");
    }

    sFaceVerify = FaceVerify::createInstance(TERMINAL);
    LOG(TAG, "sFaceVerify(%p)", sFaceVerify);
    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);

    verifyVideo();

    return 0;
}

static list <VerifyResult> sResultList;

class VerifyVideoListenerImp : public FaceVerify::VerifyVideoListener {
public:
    VerifyVideoListenerImp() {

    }

    virtual ~VerifyVideoListenerImp() {

    }

    virtual void onVerified(Image &image, list <VerifyResult> resultList) {
        LOG(TAG, "%d", resultList.size());
        sResultList = resultList;
        int i = 0;
        for (list<VerifyResult>::iterator it = resultList.begin(); it != resultList.end(); ++it) {
            LOG(TAG, "onVerified[%d] similarity(%f)", i, it->similarity);
            i++;
        }
    }
};


static int verifyVideo() {
    FaceVerify::VerifyVideoListener *listener = new VerifyVideoListenerImp();
    sFaceVerify->setVerifyVideoListener(listener);

    Mat mat = imread((PICTURE_ROOT + "liudehua.jpg").c_str(), IMREAD_COLOR);
    if (mat.empty()) {
        LOG(TAG, "verifyVideo error, because imread liudehua.jpg");
        return FAILED;
    }

    Image imageMe;
    imageMe.data = mat.data;
    imageMe.width = mat.cols;
    imageMe.height = mat.rows;
    imageMe.format = BGR888;

    list <Face> facesMeList;
    int status = sFaceDetect->detectPicture(imageMe, facesMeList);
    if (status != OK || facesMeList.size() == 0) {
        LOG(TAG, "verifyVideo error(%d), because detectPicture me", status);
        return status;
    }

    status = sFaceVerify->registerFace(imageMe, *facesMeList.begin());
    if (status != OK) {
        LOG(TAG, "registerFace error(%d)", status);
        return status;
    }

    VideoCapture capture(0);
    while (1) {
        Mat previewFrame;
        capture >> previewFrame;

        Image image;
        image.data = previewFrame.data;
        image.width = previewFrame.cols;
        image.height = previewFrame.rows;
        image.format = BGR888;

        list <Face> faces;
        sFaceVerify->verifyVideo(image, faces);

        for (list<Face>::iterator it = faces.begin(); it != faces.end(); ++it) {
            VerifyResult *result = getVerifyResult(it->trackId);

            float similarity = -1;
            if (result) {
                similarity = result->similarity;
            }
            char display_text[100] = {0};
            sprintf(display_text, "%f", similarity);

            if (similarity >= 70) {
                Tools::drawFaceRect(image, *it, 0x0000FF);
                printTextToWindow(&previewFrame, display_text, it->rect.left, it->rect.top, 0x00FF00);
            } else {
                Tools::drawFaceRect(image, *it, 0xFF0000);
                printTextToWindow(&previewFrame, display_text, it->rect.left, it->rect.top, 0xFF0000);
            }
        }

        imshow("video", previewFrame);

        waitKey(1);
    }
}

VerifyResult *getVerifyResult(int trackId) {
    if (sResultList.size() == 0) {
        return 0;
    }

    for (list<VerifyResult>::iterator it = sResultList.begin(); it != sResultList.end(); ++it) {
        if (it->trackId == trackId) {
            VerifyResult *result = new VerifyResult();
            *result = *it;
            return result;
        }
    }

    return 0;
}
