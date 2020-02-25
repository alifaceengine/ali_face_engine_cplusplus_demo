#pragma once

#include <list>
#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {
    class DLL_API FaceRecognizeNoDB {

    public:
        class Result {
        public:
            string personId;
            string featureId;
            float similarity = 0;
        };

    public:
        static FaceRecognizeNoDB *createInstance(const string &groupId);

        static void deleteInstance(FaceRecognizeNoDB *&ins);

    public:
        virtual int addPerson(const string &personId) = 0;

        virtual int deletePerson(const string &personId) = 0;

        virtual int deleteManyPersons(const list<string> &personIds) = 0;

        virtual int deleteAllPersons() = 0;

        virtual int getPersonNum(int &num) = 0;

        virtual int addFeature(const string &personId, const string &featureId, const string &feature) = 0;

        virtual int deleteFeature(const string &personId, const string &featureId) = 0;

        virtual int deleteAllFeatures(const string &personId) = 0;

        virtual int getFeatureNum(const string &personId, int &num) = 0;

        virtual int getFeatureNumOfGroup(int &num) = 0;

        virtual int recognizeFeature(string feature, Result &result) = 0;

        virtual int recognizeFeature(string feature, int topN, list<Result> &results) = 0;

        virtual int recognizeFeatureOfPerson(string personId, string feature, Result &result) = 0;

    protected:
        FaceRecognizeNoDB();

        virtual ~FaceRecognizeNoDB();
    };
}