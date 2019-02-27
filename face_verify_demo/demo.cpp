#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "FaceEngine.h"
#include "type.h"
#include "Tools.h"

#define LOG(TAG, fmt, args...) printf("[%s] : " fmt "\n", TAG, ##args)
#define TAG "FaceVerifyDemo"

using namespace cv;
using namespace ali_face_engine;
using namespace std;

static string PICTURE_ROOT = "../pictures/";

FaceVerify *sFaceVerify;
FaceDetect *sFaceDetect;

static int loadFile(unsigned char *&buf, int &len, char *path);

static int verifySamePerson();

static int verifyDifferentPerson();

static int verifyVideo();

static VerifyResult *getVerifyResult(int trackId);

int main() {
    //step 1: authorize or enable debug
    LOG(TAG, "version(%s)", getVersion());
    int status = authorize(
            "eyJ2ZW5kb3JJZCI6ImNlc2hpX3ZlbmRvciIsInJvbGUiOjIsImNvZGUiOiIzRDE5RUIwNjY1OEE5MUExQzlCNDY0MzhDN0QwNDFGMyIsImV4cGlyZSI6IjIwMTkwMzMxIiwidHlwZSI6MX0=");
    LOG(TAG, "authorize(%d)", status);
    enableDebug(false);

    //step 2: set Cloud addr and account if you using CloudServer
    setCloudAddr("101.132.89.177", 15000);
    //setCloudAddr("127.0.0.1", 8080);
    setCloudLoginAccount("admin", "admin");

    //step 3: create FaceVerify Instance (TERMINAL or CLOUD)
    //sFaceVerify = FaceVerify::createInstance(TERMINAL);
    sFaceVerify = FaceVerify::createInstance(CLOUD);
    LOG(TAG, "sFaceVerify(%p)", sFaceVerify);
    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);

    //step 4: verifySamePerson
    verifySamePerson();

    //step 5: verifyDifferentPerson
    verifyDifferentPerson();

    //step 6: verifyVideo
    verifyVideo();

    return 0;
}

int verifySamePerson() {
    LOG(TAG, "verifySamePerson ====");
    unsigned char *jpeg1 = 0;
    int jpeg1Len = 0;
    unsigned char *jpeg2 = 0;
    int jpeg2Len = 0;

    loadFile(jpeg1, jpeg1Len, (char *) (PICTURE_ROOT + "liudehua_feature1.jpg").c_str());
    loadFile(jpeg2, jpeg2Len, (char *) (PICTURE_ROOT + "liudehua_feature2.jpg").c_str());

    Image image1;
    image1.data = jpeg1;
    image1.format = ImageFormat_UNKNOWN;
    image1.dataLen = jpeg1Len;

    Image image2;
    image2.data = jpeg2;
    image2.format = ImageFormat_UNKNOWN;
    image2.dataLen = jpeg2Len;

    Face image1Face;
    list <Face> image2Faces;
    list <VerifyResult> verifyResultList;

    int status = sFaceVerify->verifyPicture(image1, image1Face, image2, image2Faces, verifyResultList);
    if (status != OK) {
        LOG(TAG, "verifyPicture error(%d)", status);
        return status;
    }

    LOG(TAG, "verifyPicture verifyResultList(%d)", verifyResultList.size());
    for (list<VerifyResult>::iterator it = verifyResultList.begin(); it != verifyResultList.end(); ++it) {
        if (it->similarity >= 70) {
            LOG(TAG, "verifySamePerson OK similarity(%f)", it->similarity);
        } else {
            LOG(TAG, "verifySamePerson 000FAIL similarity(%f)", it->similarity);
        }
    }

    return OK;
}

int verifyDifferentPerson() {
    unsigned char *jpeg1 = 0;
    int jpeg1Len = 0;
    unsigned char *jpeg2 = 0;
    int jpeg2Len = 0;

    loadFile(jpeg1, jpeg1Len, (char *) (PICTURE_ROOT + "liudehua_feature1.jpg").c_str());
    loadFile(jpeg2, jpeg2Len, (char *) (PICTURE_ROOT + "zhangxueyou_feature1.jpg").c_str());

    Image image1;
    image1.data = jpeg1;
    image1.format = JPEG;
    image1.dataLen = jpeg1Len;

    Image image2;
    image2.data = jpeg2;
    image2.format = JPEG;
    image2.dataLen = jpeg2Len;

    Face image1Face;
    list <Face> image2Faces;
    list <VerifyResult> verifyResultList;

    int status = sFaceVerify->verifyPicture(image1, image1Face, image2, image2Faces, verifyResultList);
    if (status != OK || verifyResultList.size() == 0) {
        LOG(TAG, "verifyPicture error(%d)", status);
        return status;
    }

    for (list<VerifyResult>::iterator it = verifyResultList.begin(); it != verifyResultList.end(); ++it) {
        if (it->similarity < 70) {
            LOG(TAG, "verifyDifferentPerson OK similarity(%f)", it->similarity);
        } else {
            LOG(TAG, "verifyDifferentPerson FAIL similarity(%f)", it->similarity);
        }
    }

    return OK;
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
            char similarity_text[100] = {0};
            sprintf(similarity_text, "%f", similarity);
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

            if (similarity >= 70) {
                Tools::drawFaceRect(image, *it, 0x00FF00);
                cv::putText(previewFrame, text, origin, font_face, font_scale, cv::Scalar(0, 255, 0), thickness,
                            8, 0);
            } else {
                Tools::drawFaceRect(image, *it, 0xFF0000);
                cv::putText(previewFrame, text, origin, font_face, font_scale, cv::Scalar(0, 0, 255), thickness,
                            8, 0);
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
        LOG(TAG, "loadFile %s error", path);
        buf = NULL;
        len = 0;
        return -1;
    }
}