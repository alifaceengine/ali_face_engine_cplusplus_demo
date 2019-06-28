#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FaceEngine.h"
#include "../common.h"

using namespace ali_face_engine;
using namespace std;

#define TAG "FaceRecognizeMP4Demo"

static string PICTURE_ROOT = "../pictures/";

FaceRegister *sFaceRegister;
FaceRecognize *sFaceRecognize;
FaceDetect *sFaceDetect;

//persons, jpgfile : personname_featureid.jpg
static const char *BASE_PERSONS[] = {
        "liudehua_feature1.jpg", "liudehua_feature2.jpg",
        "zhangxueyou_feature1.jpg", "zhangxueyou_feature2.jpg"};
static const int BASE_PERSONS_NUM = sizeof(BASE_PERSONS) / sizeof(char *);

static int addPersonsAndFeatures();

static int addFeature(char *personName, char *featureId, const char *filePath);

static int recognizeMP4();

static const char *GROUP_NAME = "SMALL_2000";
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

    sFaceRecognize = FaceRecognize::createInstance(GROUP_NAME, TERMINAL);
    LOG(TAG, "sFaceRecognize(%p)", sFaceRecognize);
    if (!sFaceRecognize) {
        LOG(TAG, "FaceRecognize::createInstance error");
        return 0;
    }

    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);
    if (!sFaceDetect) {
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

    //step 9
    status = recognizeMP4();
    if (status != OK) {
        LOG(TAG, "recognizeMP4 error(%d)", status);
        return 0;
    }
    LOG(TAG, "recognizeMP4 OK");

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

static int recognizeMP4() {
    LOG(TAG, "recognizeMP4");
    FaceRecognize::RecognizeVideoListener *listener = new RecognizeVideoListenerImp();
    sFaceRecognize->setRecognizeVideoListener(listener);


    IplImage *frame = NULL;
    CvCapture *capture = NULL;
    capture = cvCreateFileCapture("/Users/junyuan.hjy/Downloads/2019-04-16/10.19.7.101_01_2019041616453477.mp4");
    frame = cvQueryFrame(capture);


    //save result to mp4
    CvVideoWriter *writer = 0;


    //draw roi
    ali_face_engine::Rect roi;
    //FaceEngineConfig::FD_ROI(roi);


    cvNamedWindow("frame");
    int frameIndex = 0;
    int dropFrameCount = 0;
    while (frame) {
        LOG(TAG, "frame(%d): size(%dx%d) frame->imageSize=%d depth=%d widthStep=%d nChannels=%d nSize=%d", frameIndex,
            frame->width, frame->height, frame->imageSize, frame->depth, frame->widthStep, frame->nChannels,
            frame->nSize);

        if (frameIndex < dropFrameCount) {
            frame = cvQueryFrame(capture);
            frameIndex++;
            continue;
        }

        Image image;
        image.data = (unsigned char *) frame->imageData;
        image.width = frame->widthStep / 3;
        image.height = frame->height;
        image.format = BGR888;

        list <Face> faceList;
        int error = sFaceDetect->detectVideo(image, faceList);
        if (faceList.size()) {
            error = sFaceRecognize->recognizeVideo(image, faceList);
        }

        LOG(TAG, "faceList:%d error=%d", faceList.size(), error);

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            RecognizeResult *result = getRecognizeResult(it->trackId);
            if (result) {
                char display_text[100] = {0};
                sprintf(display_text, "trackId:%d, %s:%f", it->trackId, result->personName.c_str(),
                        result->similarity);

                if (result->similarity >= 70) {
                    printTextToWindow(frame, display_text, it->rect.left, it->rect.top + 10, 0x00FF00);
                    Tools::drawFaceRect(image, *it, 0x00FF00);
                } else {
                    printTextToWindow(frame, display_text, it->rect.left, it->rect.top + 10, 0xFF0000);
                    Tools::drawFaceRect(image, *it, 0xFF0000);
                }
            } else {
                char display_text[100] = {0};
                sprintf(display_text, "trackId:%d, not recognized", it->trackId);
                printTextToWindow(frame, display_text, it->rect.left, it->rect.top + 10, 0xFF0000);
                Tools::drawFaceRect(image, *it, 0xFF0000);
            }
        }


        //draw roi
        //Tools::drawFaceRect(image, roi, 0xFFFF00);


        //display
        IplImage *display = frame;
        float scale = 1.0;
        if (scale != 1.0) {
            CvSize sz;
            sz.width = frame->width * scale;
            sz.height = frame->height * scale;
            display = cvCreateImage(sz, frame->depth, frame->nChannels);
            cvResize(frame, display, CV_INTER_CUBIC);
        }


        //save to mp4
        if (false) {
            if (!writer) {
                char *writePath = "./result.avi";
                writer = cvCreateVideoWriter(writePath, CV_FOURCC('D', 'I', 'V', '3'), 24, cvGetSize(frame)
                );
            }
            cvWriteFrame(writer, frame);
        }

        cvShowImage("frame", display);
        waitKey(1);

        frame = cvQueryFrame(capture);
        frameIndex++;
    }

    if (writer) {
        cvReleaseVideoWriter(&writer);
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
