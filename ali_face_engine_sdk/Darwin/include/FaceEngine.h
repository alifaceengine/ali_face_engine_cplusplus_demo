#pragma once

#ifdef __ANDROID__
#include <jni.h>
#endif

#include "type.h"
#include "FaceDetect.h"
#include "FaceAttributeAnalyze.h"
#include "FaceRecognize.h"
#include "FaceVerify.h"
#include "FaceRegister.h"
#include "Tools.h"
#include "Codec.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {

    DLL_API char *getVersion();

    DLL_API const char *getVendorId();

    DLL_API bool isAuthorized();

#ifdef __ANDROID__
    DLL_API void setJavaVM(JavaVM *vm);
#endif

    DLL_API int authorize(char *key);

    DLL_API void setCloudAddr(const char *ip, const int port);

    DLL_API void setCloudLoginAccount(const char *cId, const char *cSecret);

    DLL_API void enableDebug(const bool enable);

    DLL_API void setPersistencePath(const char *path);

    //version>=3.3.0
    DLL_API void setConfigString(const char *key, const char *value);

    //version>=3.3.0
    DLL_API void setConfigInt(const char *key, const int value);

    //version>=3.3.0
    DLL_API void setConfigFloat(const char *key, const float value);

    //face
    DLL_API Face *newFace();

    DLL_API Face *newFaces(int size);

    DLL_API Face *getFace(Face *faces, int index);

    DLL_API void deleteFace(Face *&face);

    DLL_API void deleteFaces(Face *&faces);

    DLL_API FacePoint *getFacePoint(FacePoint *facePoints, int index);


    //group
    DLL_API void deleteGroup(Group *&group);

    DLL_API void deleteGroups(Group *&groups);

    DLL_API Group *getGroup(Group *groups, int index);


    //person
    DLL_API Person *newPerson();

    DLL_API void deletePerson(Person *&person);

    DLL_API void deletePersons(Person *&persons);

    DLL_API Person *getPerson(Person *persons, int index);


    //RecognizeResult
    DLL_API RecognizeResult *getRecognizeResult(RecognizeResult *recognizeResults, int index);

    DLL_API void deleteRecognizeResult(RecognizeResult *&recognizeResult);

    DLL_API void deleteRecognizeResults(RecognizeResult *&recognizeResults);


    //VerifyResult
    DLL_API VerifyResult *getVerifyResult(VerifyResult *verifyResults, int index);

    DLL_API void deleteVerifyResult(VerifyResult *&verifyResult);

    DLL_API void deleteVerifyResults(VerifyResult *&verifyResults);

}
