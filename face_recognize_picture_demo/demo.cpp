#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

#define TAG "FaceRecognizePictureDemo"

using namespace ali_face_engine;
using namespace std;

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

static int recognizePicture(char *personName, const char *filePath);

static int recognizePictures();

static const char *GROUP_NAME = "group_test";
static Group sGroup;

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

    sFaceRecognize = FaceRecognize::createInstance(TERMINAL);
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
    sGroup.modelType = MODEL_SMALL;
    //sGroup.modelType = MODEL_BIG;
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

    sFaceRecognize->setGroupId(sGroup.id);

    //step 6: recognizePictures
    status = recognizePictures();
    if (status != OK) {
        LOG(TAG, "recognizePictures error(%d)", status);
        return 0;
    }
    LOG(TAG, "recognizePictures OK");
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
    image.format = ImageFormat_UNKNOWN;
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


int recognizePictures() {
    char personName[PERSON_NAME_MAX_SIZE] = {0};

    for (int i = 0; i < TEST_PERSONS_NUM; i++) {
        memset(personName, 0, sizeof(personName));
        sscanf(TEST_PERSONS[i], "%[^.]", personName);

        int status = recognizePicture(personName, (PICTURE_ROOT + TEST_PERSONS[i]).c_str());
        if (status != OK) {
            LOG(TAG, "recognizePicture[%d] %s, error(%d)", i, personName, status);
            return status;
        }
    }
    return OK;
}

int recognizePicture(char *personName, const char *filePath) {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, (char *) filePath);

    Image image;
    image.data = data;
    image.format = ImageFormat_UNKNOWN;
    image.dataLen = dataLen;

    list <Face> faces;
    list <RecognizeResult> recognizeResults;

    int status = sFaceRecognize->recognizePicture(image, faces, recognizeResults);
    if (status != OK) {
        LOG(TAG, "recognizePicture error(%d) resultNum(%d)", status, recognizeResults.size());
        return status;
    }

    for (list<RecognizeResult>::iterator it = recognizeResults.begin(); it != recognizeResults.end(); ++it) {
        if (strcmp(personName, it->personName.c_str()) != 0) {
            LOG(TAG, "recognizePicture FAIL: TestName(%s) BaseName(%s) similarity(%f)", personName,
                it->personName.c_str(), it->similarity);
            return FAILED;
        } else {
            LOG(TAG, "recognizePicture OK: TestName(%s) BaseName(%s) similarity(%f)", personName,
                it->personName.c_str(), it->similarity);
            return OK;
        }
    }

}
