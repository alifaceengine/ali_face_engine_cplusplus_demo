#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "FaceEngine.h"

#ifdef WIN32
#define LOG(fmt, ...) printf(fmt "\n", __VA_ARGS__)
#else
#define LOG(fmt, args...) printf(fmt "\n", ##args)
#endif


using namespace cv;
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

static int recognizeVideo();

static int verifyVideo();

static int verifySamePerson();

static int verifyDifferentPerson();

static int loadFile(unsigned char *&buf, int &len, char *path);

int saveFile(char *buf, int len, char *path);

static const char *GROUP_NAME = "groupu1";
static Group sGroup;

class RecognizeVideoListenerImp2 : public FaceRecognize::RecognizeVideoListener {
public:
    RecognizeVideoListenerImp2() {

    }

    virtual ~RecognizeVideoListenerImp2() {

    }

    virtual void onRecognized(Image &image, list <RecognizeResult> results) {
        LOG("");
        int i = 0;
        for (list<RecognizeResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG("onRecognized[%d] BaseName(%s)  similarity(%f)", i,
                it->personName.c_str(), it->similarity);
            i++;
        }
    }

    virtual void onVerified(Image &image, list <VerifyResult> results) {
        LOG("");
        //sVerifyResults = results;
        int i = 0;
        for (list<VerifyResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG("onVerified[%d] similarity(%f)", i, it->similarity);
            i++;
        }
    }
};

int recognizeVideo2() {
    FaceRecognize::RecognizeVideoListener *listener = new RecognizeVideoListenerImp2();
    sFaceRecognize->setRecognizeVideoListener(listener);

    unsigned char *data = 0;
    int dataLen = 0;

#if 0
    //loadFile(data, dataLen, "20.bgr");
    loadFile(data, dataLen, "1280x720.bgr");
    Image image;
    image.data = data;
    image.width = 1200;
    image.height = 720;
    image.format = BGR888;
    image.dataLen = dataLen;
#else
    loadFile(data, dataLen, "liudehua_400x519.rgb24");
    Image image;
    image.data = data;
    image.width = 400;
    image.height = 519;
    image.format = RGB888;
    image.dataLen = dataLen;
#endif
    while (1) {
        list <Face> faces;
        LOG("recognizeVideo begin");
        sFaceRecognize->recognizeVideo(image, faces);
        LOG("recognizeVideo count=%d", faces.size());
    }
}

void extractFeature(bool loop) {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, "1280x720.bgr");

    Image image;
    image.data = data;
    image.width = 1280;
    image.height = 720;
    image.format = BGR888;
    image.dataLen = dataLen;

    list <Face> faces;
    int status = sFaceDetect->detectPicture(image, faces);
    if (status != OK || faces.size() == 0) {
        LOG("detectPicture error(%d)", status);
        if (image.data) {
            free(image.data);
            image.data = 0;
        }
        return;
    }

    int loopNum = 0;
    while (true) {
        string feature;
        status = sFaceRegister->extractFeature(image, *faces.begin(), MODEL_SMALL, feature);

        //sleep(1);
        if (loop) {
            continue;
        } else {
            if (loopNum == 2) {
                break;
            }
        }

        loopNum++;
    }

    if (image.data) {
        free(image.data);
        image.data = 0;
    }
}

int main() {
    //step 1: authorize or enable debug
    LOG("version(%s)", getVersion());
    int status = authorize(
            "eyJ2ZW5kb3JJZCI6ImNlc2hpX3ZlbmRvciIsInJvbGUiOjIsImNvZGUiOiIzRDE5RUIwNjY1OEE5MUExQzlCNDY0MzhDN0QwNDFGMyIsImV4cGlyZSI6IjIwMTkwMzMxIiwidHlwZSI6MX0=");
    LOG("authorize(%d)", status);
    setPersistencePath("./");
    enableDebug(false);

    //step 2: set Cloud addr and account if you using CloudServer
    //setCloudAddr("101.132.89.177", 15000);
    //setCloudAddr("127.0.0.1", 8080);
    setCloudLoginAccount("admin", "admin");

    //step 3: create FaceRecognize Instance (TERMINAL or CLOUD)
    sFaceRegister = FaceRegister::createInstance();
    LOG("sFaceRegister(%p)", sFaceRegister);
    if (!sFaceRegister) {
        LOG("FaceRegister::createInstance error");
        return 0;
    }

    sFaceRecognize = FaceRecognize::createInstance(TERMINAL);
    //sFaceRecognize = FaceRecognize::createInstance(CLOUD);
    LOG("sFaceRecognize(%p)", sFaceRecognize);
    if (!sFaceRecognize) {
        LOG("FaceRecognize::createInstance error");
        return 0;
    }

    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG("sFaceDetect(%p)", sFaceDetect);
    if (!sFaceRecognize) {
        LOG("FaceDetect::createInstance error");
        return 0;
    }

    //step 4: create group
    sGroup.name = GROUP_NAME;
    sGroup.modelType = MODEL_SMALL;
    //sGroup.modelType = MODEL_BIG;
    status = sFaceRegister->createGroup(sGroup);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG("createGroup error(%d)", status);
        return 0;
    }
    LOG("createGroup OK, id(%s) name(%s) modelType(%d)", sGroup.id.c_str(), sGroup.name.c_str(), sGroup.modelType);

    sFaceRecognize->setGroupId(sGroup.id);

    //step 5: addPersonsAndFeatures
    status = addPersonsAndFeatures();
    if (status != OK) {
        LOG("addPersonsAndFeatures error(%d)", status);
        return 0;
    }
    LOG("addPersonsAndFeatures OK");

    //step 6: recognizePictures
    status = recognizePictures();
    if (status != OK) {
        LOG("recognizePictures error(%d)", status);
        return 0;
    }
    LOG("recognizePictures OK");

    //step 9: recognizeVideo
    status = recognizeVideo();
    //status = verifyVideo();
    if (status != OK) {
        LOG("recognizeVideo error(%d)", status);
        return 0;
    }
    LOG("recognizeVideo OK");

    return 0;
}


int addPersonsAndFeatures() {
    char personName[PERSON_NAME_MAX_SIZE] = {0};
    char featureName[FEATURE_ID_MAX_SIZE] = {0};

    for (int i = 0; i < BASE_PERSONS_NUM; i++) {
        memset(personName, 0, sizeof(personName));
        memset(featureName, 0, sizeof(featureName));
        sscanf(BASE_PERSONS[i], "%[^_]", personName);
        sscanf(BASE_PERSONS[i], "%*[^_]_%[^.]", featureName);

        Person person;
        person.name = personName;
        int status = sFaceRegister->addPerson(sGroup.id, person);
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG("addPerson[%d] %s, error(%d)", i, personName, status);
            return status;
        }

        status = addFeature((char *) person.id.c_str(), featureName, (PICTURE_ROOT + BASE_PERSONS[i]).c_str());
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG("addFeature[%d] %s, status(%d)", i, featureName, status);
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
        LOG("detectPicture error(%d)", status);
        return status;
    }

    string feature;
    status = sFaceRegister->extractFeature(image, *faces.begin(), sGroup.modelType, feature);
    if (status != OK) {
        LOG("extractFeature error(%d)", status);
        return status;
    }

    Feature feature1;
    feature1.name = featureName;
    feature1.feature = feature;
    status = sFaceRegister->addFeature(personId, feature1);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG("addFeature error(%d)", status);
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
            LOG("recognizePicture[%d] %s, error(%d)", i, personName, status);
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
        LOG("recognizePicture error(%d) resultNum(%d)", status, recognizeResults.size());
        return status;
    }

    for (list<RecognizeResult>::iterator it = recognizeResults.begin(); it != recognizeResults.end(); ++it) {
        if (strcmp(personName, it->personName.c_str()) != 0) {
            LOG("recognizePicture FAIL: TestName(%s) BaseName(%s) similarity(%f)", personName,
                it->personName.c_str(), it->similarity);
            return FAILED;
        } else {
            LOG("recognizePicture OK: TestName(%s) BaseName(%s) similarity(%f)", personName,
                it->personName.c_str(), it->similarity);
            return OK;
        }
    }

}

static RecognizeResult *getRecognizeResult(int trackId);

static list <RecognizeResult> sResults;

static list <VerifyResult> sVerifyResults;

class RecognizeVideoListenerImp : public FaceRecognize::RecognizeVideoListener {
public:
    RecognizeVideoListenerImp() {

    }

    virtual ~RecognizeVideoListenerImp() {

    }

    virtual void onRecognized(Image &image, list <RecognizeResult> results) {
        LOG("");
        sResults = results;
        int i = 0;
        for (list<RecognizeResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG("onRecognized[%d] BaseName(%s)  similarity(%f)", i,
                it->personName.c_str(), it->similarity);
            i++;
        }
    }

    virtual void onVerified(Image &image, list <VerifyResult> results) {
        LOG("");
        sVerifyResults = results;
        int i = 0;
        for (list<VerifyResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG("onVerified[%d] similarity(%f)", i, it->similarity);
            i++;
        }
    }
};

static int recognizeVideo() {
    LOG("recognizeVideo");
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
        int statusa = sFaceRecognize->recognizeVideo(image, faceList);

        LOG("recognizeVideo faceList:%d statusa=%d", faceList.size(), statusa);

        for (list<Face>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
            RecognizeResult *result = getRecognizeResult(it->trackId);
            if (result) {
                char similarity_text[100] = {0};
                sprintf(similarity_text, "%s:%f", result->personName.c_str(), result->similarity);
                std::string text = similarity_text;
                int font_face = cv::FONT_HERSHEY_COMPLEX;
                double font_scale = 1;
                int thickness = 2;
                int baseline;
                //获取文本框的长宽
                cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

                //将文本框居中绘制
                cv::Point origin;
                origin.x = it->rect.left + 100;
                origin.y = it->rect.top + text_size.height + 10;
                if (result->similarity >= 70) {
                    Tools::drawFaceRect(image, *it, 0x00FF00);
                    cv::putText(previewFrame, text, origin, font_face, font_scale, cv::Scalar(0, 255, 0), thickness,
                                8, 0);
                } else {
                    Tools::drawFaceRect(image, *it, 0xFF0000);
                    cv::putText(previewFrame, text, origin, font_face, font_scale, cv::Scalar(0, 0, 255), thickness,
                                8, 0);
                }
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


int saveFile(char *buf, int len, char *path) {
    FILE *pf = fopen(path, "wb");
    if (pf != NULL) {
        fwrite(buf, sizeof(char), len, pf);
        fclose(pf);
        return 0;
    }

    return -1;
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