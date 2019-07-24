#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

using namespace ali_face_engine;
using namespace std;

#define TAG "FaceRecognizeCameraDemo"

static string PICTURE_ROOT = "../pictures/";

FaceRegister *sFaceRegister;
FaceRecognize *sFaceRecognize;
FaceDetect *sFaceDetect;

//persons, jpgfile : personname_featureid.jpg
static const char *BASE_PERSONS[] = {
        "liudehua_feature1.jpg", "liudehua_feature2.jpg",
        "zhangxueyou_feature1.jpg", "zhangxueyou_feature2.jpg"};
static const int BASE_PERSONS_NUM = sizeof(BASE_PERSONS) / sizeof(char *);

static const char *TEST_PERSONS[] = {
        "liudehua.jpg",
        "zhangxueyou.jpg"};

static const int TEST_PERSONS_NUM = sizeof(TEST_PERSONS) / sizeof(char *);

static int addPersonsAndFeatures();

static int addFeature(char *personName, char *featureId, const char *filePath);

static int recognizeVideo();

static const char *GROUP_NAME = "SMALL_2000";
static Group sGroup;

int main() {
    //step 1: set config path
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "setPersistencePath error(%d) key(%s)", status);
        return 0;
    } else {
        LOG(TAG, "setPersistencePath ok key(%s)");
    }

    //step 3: create FaceRegister Instance
    sFaceRegister = FaceRegister::createInstance();
    LOG(TAG, "sFaceRegister(%p)", sFaceRegister);
    if (!sFaceRegister) {
        LOG(TAG, "FaceRegister::createInstance error");
        return 0;
    }

    sFaceRecognize = FaceRecognize::createInstance(GROUP_NAME, TERMINAL);
    //sFaceRecognize = FaceRecognize::createInstance(CLOUD);
    LOG(TAG, "sFaceRecognize(%p)", sFaceRecognize);
    if (!sFaceRecognize) {
        LOG(TAG, "FaceRecognize::createInstance error");
        return 0;
    }

    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);
    if (!sFaceRecognize) {
        LOG(TAG, "FaceDetect::createInstance error");
        return 0;
    }

    //step 4: create group
    sGroup.name = GROUP_NAME;
    sGroup.modelType = MODEL_3K;
    //sGroup.modelType = MODEL_100K;
    status = sFaceRegister->createGroup(sGroup);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG(TAG, "createGroup error(%d)", status);
        return 0;
    }
    LOG(TAG, "createGroup OK, id(%s) name(%s) modelType(%d)", sGroup.id.c_str(), sGroup.name.c_str(), sGroup.modelType);

#if 1
    //step 5: addPersonsAndFeatures
    status = addPersonsAndFeatures();
    if (status != OK) {
        LOG(TAG, "addPersonsAndFeatures error(%d)", status);
        return 0;
    }
    LOG(TAG, "addPersonsAndFeatures OK");
#endif

    //step 9: recognizeVideo
    status = recognizeVideo();
    //status = verifyVideo();
    if (status != OK) {
        LOG(TAG, "recognizeVideo error(%d)", status);
        return 0;
    }
    LOG(TAG, "recognizeVideo OK");

    return 0;
}


int addPersonsAndFeatures() {
    char personName[PERSON_NAME_MAX_SIZE] = {0};
    char featureName[FEATURE_NAME_MAX_SIZE] = {0};

    for (int i = 0; i < BASE_PERSONS_NUM; i++) {
        memset(personName, 0, sizeof(personName));
        memset(featureName, 0, sizeof(featureName));
        sscanf(BASE_PERSONS[i], "%[^_]", personName);
        sscanf(BASE_PERSONS[i], "%*[^_]_%[^.]", featureName);

        Person person;
        person.name = personName;
        int status = sFaceRegister->addPerson(sGroup.id, person);
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG(TAG, "addPerson[%d] %s, error(%d)", i, personName, status);
            return status;
        }

        status = addFeature((char *) person.id.c_str(), featureName, (PICTURE_ROOT + BASE_PERSONS[i]).c_str());
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG(TAG, "addFeature[%d] %s, status(%d)", i, featureName, status);
            return status;
        }
    }
    return OK;
}

int addFeature(char *personId, char *featureName, const char *filePath) {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, (char *) filePath);

    Image image;
    image.data = data;
    image.format = COMPRESSED;
    image.dataLen = dataLen;

    list <Face> faces;
    int status = sFaceDetect->detectPicture(image, faces);

    if (status != OK || faces.size() == 0) {
        LOG(TAG, "detectPicture error(%d)", status);
        return status;
    }

    string feature;
    status = sFaceRegister->extractFeature(image, *faces.begin(), sGroup.modelType, feature);
    if (status != OK) {
        LOG(TAG, "extractFeature error(%d)", status);
        return status;
    }

    Feature feature1;
    feature1.name = featureName;
    feature1.feature = feature;
    status = sFaceRegister->addFeature(personId, feature1);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG(TAG, "addFeature error(%d)", status);
        return status;
    }

    return OK;
}

static RecognizeResult *getRecognizeResult(int trackId);

static list <RecognizeResult> sResults;

class RecognizeVideoListenerImp : public FaceRecognize::RecognizeVideoListener {
public:
    RecognizeVideoListenerImp() {

    }

    virtual ~RecognizeVideoListenerImp() {

    }

    virtual void onRecognized(Image &image, list <RecognizeResult> results) {
        LOG(TAG, "");
        sResults = results;
        int i = 0;
        for (list<RecognizeResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG(TAG, "onRecognized[%d] BaseName(%s)  similarity(%f)", i,
                it->personName.c_str(), it->similarity);
            i++;
        }
    }

    virtual void onVerified(Image &image, list <VerifyResult> results) {
        LOG(TAG, "");
    }
};

static int recognizeVideo() {
    LOG(TAG, "recognizeVideo");
    FaceRecognize::RecognizeVideoListener *listener = new RecognizeVideoListenerImp();
    sFaceRecognize->setRecognizeVideoListener(listener);

    VideoCapture capture(0);
    while (1) {
        Mat previewFrame;
        capture >> previewFrame;

        Image image;
        image.data = previewFrame.data;
        image.width = previewFrame.cols;
        image.height = previewFrame.rows;
        image.format = BGR888;
        image.rotation = ANGLE_0;

        list <Face> faceList;
        int error = sFaceRecognize->recognizeVideo(image, faceList);

        LOG(TAG, "recognizeVideo faceList:%d error=%d", faceList.size(), error);

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            RecognizeResult *result = getRecognizeResult(it->trackId);
            if (result) {
                char display_text[100] = {0};
                sprintf(display_text, "trackId:%d, %s:%f", it->trackId, result->personName.c_str(),
                        result->similarity);

                if (result->similarity >= 70) {
                    printTextToWindow(&previewFrame, display_text, it->rect.left, it->rect.top + 10, 0x00FF00);
                    Tools::drawFaceRect(image, *it, 0x00FF00);
                } else {
                    printTextToWindow(&previewFrame, display_text, it->rect.left, it->rect.top + 10, 0xFF0000);
                    Tools::drawFaceRect(image, *it, 0xFF0000);
                }
            } else {
                char display_text[100] = {0};
                sprintf(display_text, "trackId:%d, no body in db", it->trackId);
                printTextToWindow(&previewFrame, display_text, it->rect.left, it->rect.top + 10, 0xFF0000);
                Tools::drawFaceRect(image, *it, 0xFF0000);
            }
        }

        imshow("recognizeVideo", previewFrame);
        waitKey(1);
    }
}

RecognizeResult *getRecognizeResult(int trackId) {
    for (list<RecognizeResult>::iterator it = sResults.begin(); it != sResults.end(); ++it) {
        if (it->trackId == trackId) {
            RecognizeResult *result = new RecognizeResult();
            *result = *it;
            return result;
        }
    }
    return 0;
}
