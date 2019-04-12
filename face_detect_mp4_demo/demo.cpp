#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceDetectMP4Demo"

using namespace ali_face_engine;
using namespace std;

static int detectMP4();

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

    detectMP4();

    FaceDetect::deleteInstance(sFaceDetect);
    return 0;
}

int detectMP4() {
    DetectParameter parameter = sFaceDetect->getVideoParameter();
    parameter.checkQuality = false;
    parameter.checkLiveness = false;
    sFaceDetect->setVideoParameter(parameter);

    IplImage *frame = NULL;
    CvCapture *capture = NULL;
    capture = cvCreateFileCapture("/Users/junyuan.hjy/Downloads/VID-20190412-WA0014.mp4");
    frame = cvQueryFrame(capture);

    cvNamedWindow("frame");
    int frameIndex = 0;
    while (frame) {
        LOG(TAG, "frame(%d): size(%dx%d) frame->imageSize=%d depth=%d widthStep=%d nChannels=%d nSize=%d", frameIndex,
            frame->width, frame->height, frame->imageSize, frame->depth, frame->widthStep, frame->nChannels,
            frame->nSize);

        Image image;
        image.data = (unsigned char *) frame->imageData;
        image.width = frame->widthStep / 3;
        image.height = frame->height;
        image.format = BGR888;

        list <Face> faceList;
        int status = sFaceDetect->detectVideo(image, faceList);

        int i = 0;
        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            LOG(TAG, "detectVideo faces[%d] confidence(%d) quality(%d) liveness(%d) image(%p) yaw=%f", i,
                it->confidence, it->attribute.quality.score, it->attribute.liveness.score, image.data, it->pose.yaw);
            Tools::drawFaceRect(image, *it, 0xFF0000);

            char display_text[100] = {0};
            sprintf(display_text, "conf:%d, quality:%d", it->confidence, it->attribute.quality.score);

            printTextToWindow(frame, display_text, it->rect.left, it->rect.top + 10);
            i++;
        }

        cvShowImage("frame", frame);
        cvWaitKey(1);

        frame = cvQueryFrame(capture);
        frameIndex++;
    }

    return 0;
}