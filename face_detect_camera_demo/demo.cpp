#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceDetectDemo"

using namespace ali_face_engine;
using namespace std;

static int detectVideo();

FaceDetect *sFaceDetect;

int main() {
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "setPersistencePath error(%d)", status);
        return 0;
    } else {
        LOG(TAG, "setPersistencePath ok");
    }

    //step 3: create FaceVerify Instance (TERMINAL or CLOUD)
    sFaceDetect = FaceDetect::createInstance(TERMINAL);
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
    parameter.checkAge = false;
    parameter.checkGender = false;
    parameter.checkExpression = false;
    parameter.checkGlass = false;

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

        list <Face> faceList;
        sFaceDetect->detectVideo(image, faceList);

        struct timeval tv2;
        gettimeofday(&tv2, 0);
        int cost = ((tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec) / 1000);
        LOG(TAG, "detectVideo cost(%d)", cost);

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            LOG(TAG, "detectVideo trackId(%d) rect(%d,%d,%d,%d)\n",
                it->trackId,
                it->rect.left,
                it->rect.top,
                it->rect.right,
                it->rect.bottom);
            Tools::drawFaceRect(image, *it, 0xFF0000);
            Tools::drawFacePoint(image, *it, 0x00FF00);

            char display[1000] = {0};
            sprintf(display, "trackId : %d", it->trackId);
            sprintf(display, "%s, liveness : %d", display, it->attribute.liveness.score);
            printTextToWindow(&previewFrame, display, it->rect.left, it->rect.top + 20, 0x00FF00);
        }

        imshow("video", previewFrame);
        waitKey(1);
    }
}
