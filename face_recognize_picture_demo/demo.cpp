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

static int recognizePicture(char *personName, const char *filePath);

static int recognizePictures();

static const ModelType MODEL_TYPE = MODEL_3K;//MODEL_100K
static const Mode MODE = CLOUD;//TERMINAL
static const char *GROUP_NAME = "SMALL_xxx";
static Group sGroup;

int main() {
    //step 1: authorize or enable debug
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

    sFaceRecognize = FaceRecognize::createInstance(GROUP_NAME, MODE);
    LOG(TAG, "sFaceRecognize(%p)", sFaceRecognize);
    if (!sFaceRecognize) {
        LOG(TAG, "FaceRecognize::createInstance error");
        return 0;
    }

    //step 4: create group
    sGroup.name = GROUP_NAME;
    sGroup.modelType = MODEL_TYPE;
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
        unsigned char *data = 0;
        int dataLen = 0;
        loadFile(data, dataLen, (char *) (PICTURE_ROOT + BASE_PERSONS[i]).c_str());
        if (!data) {
            LOG(TAG, "");
            exit(-1);
        }

        Image image;
        image.data = data;
        image.format = COMPRESSED;
        image.dataLen = dataLen;

        memset(personName, 0, sizeof(personName));
        memset(featureName, 0, sizeof(featureName));
        sscanf(BASE_PERSONS[i], "%[^_]", personName);
        sscanf(BASE_PERSONS[i], "%*[^_]_%[^.]", featureName);

        Person person;
        person.name = personName;
        int status = sFaceRegister->registerPicture2(GROUP_NAME, image, person, featureName);
        if (image.data) {
            free(image.data);
        }

        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG(TAG, "addPerson[%d] %s, error(%d)", i, personName, status);
            return status;
        }
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

    if (!data) {
        LOG(TAG, "recognizePicture error, load %s fail", filePath);
        exit(-1);
    }
    Image image;
    image.data = data;
    image.format = COMPRESSED;
    image.dataLen = dataLen;

    list <RecognizeResult> recognizeResults;

    int status = sFaceRecognize->recognizePicture(image, recognizeResults);
    if (image.data) {
        free(image.data);
        image.data = 0;
    }

    if (status != OK) {
        LOG(TAG, "recognizePicture error(%d) resultNum(%d)", status, recognizeResults.size());
        return status;
    }

    for (auto result : recognizeResults) {
        if (strcmp(personName, result.personName.c_str()) != 0) {
            LOG(TAG, "recognizePicture FAIL: TestName(%s) BaseName(%s) similarity(%f)", personName,
                result.personName.c_str(), result.similarity);
            return FAILED;
        } else {
            LOG(TAG, "recognizePicture OK: TestName(%s) BaseName(%s) similarity(%f)", personName,
                result.personName.c_str(), result.similarity);
            return OK;
        }
    }
}
