#pragma once

#include <list>

#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {

    class DLL_API FaceDetect {
    public:

        static FaceDetect *createInstance(enum Mode mode = TERMINAL);

        static void deleteInstance(FaceDetect *&ins);

    public:

        virtual int setPictureParameter(DetectParameter &parameter) = 0;

        virtual DetectParameter &getPictureParameter() = 0;

        virtual int detectPicture(Image &image, list<Face> &faceList) = 0;


        virtual int setVideoParameter(DetectParameter parameter) = 0;

        virtual DetectParameter getVideoParameter() = 0;

        virtual int detectVideo(Image &image, list<Face> &faceList) = 0;

    protected:
        FaceDetect();

        virtual ~FaceDetect();
    };
}
