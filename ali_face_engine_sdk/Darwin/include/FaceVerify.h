#pragma once

#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {
    class DLL_API FaceVerify {

    public:
        class VerifyVideoListener {
        public:
            virtual ~VerifyVideoListener() {

            }

            virtual void onVerified(Image &image, list<VerifyResult> resultList) = 0;
        };

    public:
        static FaceVerify *createInstance(enum Mode mode = TERMINAL);

        static void deleteInstance(FaceVerify *&ins);

    public:
        virtual void setVerifyVideoListener(VerifyVideoListener *listener) = 0;

        virtual VerifyVideoListener *getVerifyVideoListener() = 0;

        virtual int verifyVideo(Image &image, list<Face> &faceList) = 0;

        virtual int registerFace(Image &image, Face &face) = 0;

        virtual int registerFeature(string &feature) = 0;

        virtual int
        verifyPicture(Image &image1, Face &image1Face, Image &image2, list<Face> &image2FaceList,
                      list<VerifyResult> &verifyResultList) = 0;

    protected:
        FaceVerify();

        virtual ~FaceVerify();
    };
}