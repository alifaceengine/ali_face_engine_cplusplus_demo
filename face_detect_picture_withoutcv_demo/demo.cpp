#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"

#define TAG "FaceDetectPictureDemo"

using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

static int detectPicture();

static int loadFile(unsigned char *&buf, int &len, char *path);

FaceDetect *sFaceDetect;

int main() {
    int status = setPersistencePath("../");
    if (status != OK) {
        printf("setPersistencePath error(%d)\n", status);
        return 0;
    } else {
        printf("setPersistencePath ok\n");
    }

    //step 2: create FaceVerify Instance (TERMINAL or CLOUD)
    sFaceDetect = FaceDetect::createInstance(TERMINAL);

    printf("sFaceDetect(%p)\n", sFaceDetect);

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
    image.format = COMPRESSED;
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
        printf("detectPicture error(%d)\n", status);
        if (image.data) {
            free(image.data);
            image.data = 0;
        }
        return status;
    }

    printf("detectPicture faceNum(%d)\n", faceList.size());
    for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
        printf("detectPicture faces[%d] rect(%d,%d,%d,%d) quality(%d) liveness(%d) age(%d) gender(%d) expression(%d) glass(%d)\n",
               it->trackId, it->rect.left, it->rect.top, it->rect.right, it->rect.bottom, it->attribute.quality.score,
               it->attribute.liveness.score, it->attribute.age, it->attribute.gender, it->attribute.expression,
               it->attribute.glass);
    }

    if (image.data) {
        free(image.data);
        image.data = 0;
    }
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