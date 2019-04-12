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
    //step 1: authorize or enable debug
    enableDebug(true);
    int status = authorize(KEY);
    if (status != OK) {
        LOG(TAG, "authorize error(%d) key(%s)", status, KEY);
        return 0;
    } else {
        LOG(TAG, "authorize ok key(%s)", KEY);
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
    parameter.checkQuality = true;
    parameter.checkLiveness = true;
    parameter.checkAge = true;
    parameter.checkGender = true;
    parameter.checkExpression = true;
    parameter.checkGlass = true;

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
        LOG(TAG, "detectVideo cost(%d)", ((tv2.tv_sec - tv.tv_sec) * 1000 + (tv2.tv_usec - tv.tv_usec) / 1000));

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            printf("detectVideo trackId(%d) rect(%d,%d,%d,%d)\n",
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
            printTextToWindow(&previewFrame, display, it->rect.left, it->rect.top + 20);
        }

        imshow("video", previewFrame);
        waitKey(1);
    }
}
