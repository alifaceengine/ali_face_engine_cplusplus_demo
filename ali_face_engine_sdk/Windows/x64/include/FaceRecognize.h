#pragma once

#include <list>
#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {

    class DLL_API FaceRecognize {

    public:
        class RecognizeVideoListener {
        public:
            virtual ~RecognizeVideoListener() {

            }

            virtual void onRecognized(Image &image, list<RecognizeResult> resultList) = 0;

            virtual void onVerified(Image &image, list<VerifyResult> resultList) = 0;
        };

    public:
        static FaceRecognize *createInstance(enum Mode mode = TERMINAL);

        static void deleteInstance(FaceRecognize *&ins);

    public:

        virtual int setGroupId(const string &groupId) = 0;

        virtual int reloadDB() = 0;

        virtual void setRecognizeVideoListener(RecognizeVideoListener *listener) = 0;

        virtual RecognizeVideoListener *getRecognizeVideoListener() = 0;

        virtual int recognizeVideo(Image &image, list<Face> &faceList) = 0;

        virtual int
        recognizePicture(Image &image, list<Face> &faceList, list<RecognizeResult> &recognizeResultList) = 0;

        virtual int recognizeFeature(string &feature, RecognizeResult &recognizeResult) = 0;

        virtual int verifyVideo(const string &personId, Image &image, list<Face> &faceList) = 0;

        virtual int verifyPicture(const string &personId, Image &image, list<Face> &faceList,
                                  list<VerifyResult> &verifyResultList) = 0;

    protected:
        FaceRecognize();

        virtual ~FaceRecognize();
    };
}