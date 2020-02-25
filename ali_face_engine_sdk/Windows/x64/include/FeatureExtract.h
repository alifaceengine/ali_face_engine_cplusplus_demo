#pragma once

#include <vector>

#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {

    class DLL_API FeatureExtract {

    public:
        static FeatureExtract *createInstance(enum ModelType modelType, enum Mode mode = TERMINAL);

        static void deleteInstance(FeatureExtract *&ins);

    public:

        virtual int extractFeature(Image &image, Face &face, string &feature) = 0;

        virtual int extractFeature(Image &image, vector<FacePoint> &kps, string &feature) = 0;

    protected:
        FeatureExtract();

        virtual ~FeatureExtract();
    };
}