#pragma once

#include <string>
#include <string.h>
#include <list>

using namespace std;

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {
    enum Error {
        OK = 0,
        FAILED = -1,
        ERROR_EXPIRE = -2,
        ERROR_AUTH_FAIL = -3,
        ERROR_INVALID_ARGUMENT = -4,
        ERROR_DB_EXEC = -5,
        ERROR_EXISTED = -6,
        ERROR_NOT_EXIST = -7,
        ERROR_NETWORK_FAIL = -8,
        ERROR_NETWORK_RECV_JSON_WRONG = -9,
        ERROR_NO_FACE = -10,
        ERROR_FORMAT_NOT_SUPPORT = -11,
        ERROR_NO_ID = -12,

        ERROR_CLOUD_OK = OK,
        ERROR_CLOUD_ACCOUT_WRONG = -20,
        ERROR_CLOUD_REQUEST_DATA_ERROR = -21,
        ERROR_CLOUD_DB_EXEC_ERROR = -22,
        ERROR_CLOUD_EXISTED_ERROR = -23,
        ERROR_CLOUD_NOT_EXIST_ERROR = -24,
        ERROR_CLOUD_NO_AUTHORIZE = -25,
        ERROR_CLOUD_ALGORITHOM_ERROR = -26,
        ERROR_CLOUD_NO_FACE = -27,
        ERROR_CLOUD_FAILED = -28,
        ERROR_CLOUD_NOT_SUPPORT = -29
    };

    enum Mode {
        TERMINAL = 1,
        CLOUD = 2
    };

    enum ModelType {
        MODEL_SMALL = 1,//same as MODEL_3K
        MODEL_3K = 1,//for 3000 persons
        MODEL_BIG = 2,//same as MODEL_100K
        MODEL_100K = 2//for 100000 persons
    };

    enum ImageFormat {
        ImageFormat_UNKNOWN = -1,//same as COMPRESSED
        COMPRESSED = -1,//JPEG, PNG, BMP
        RGB888 = 0,
        BGR888 = 1,
        NV21 = 10,
        NV12 = 11,
        YV12 = 12,
        I420 = 13,
    };

    enum ImageRotation {
        ANGLE_0 = 0,
        ANGLE_90 = 90,
        ANGLE_180 = 180,
        ANGLE_270 = 270
    };

    class DLL_API Image {
    public:
        unsigned char *data;
        enum ImageFormat format;
        enum ImageRotation rotation;
        int width;
        int stride;
        int height;
        int dataLen;
        uint64_t timestamp;
        unsigned int customerIndex;

    public:
        Image();

        int getDataLen();

        Image clone();
    };

    class DLL_API Rect {
    public:
        int left;
        int top;
        int right;
        int bottom;

        Rect();
    };

    class DLL_API Pose {
    public:
        float pitch;
        float yaw;
        float roll;

        Pose();
    };

    class DLL_API FacePoint {
    public:
        float x;
        float y;

        FacePoint();
    };

    class DLL_API Quality {
    public:
        int score;

        Quality();
    };

    class DLL_API Liveness {
    public:
        int score;

        Liveness();
    };

    enum Gender {
        GENGER_UNKNOWN = -1,
        GENGER_MALE = 0,
        GENGER_FEMALE
    };

    enum Expression {
        EXPRESSION_UNKOWN = -1,
        EXPRESSION_DISLIKE = 0,
        EXPRESSION_LIKE,
        EXPRESSION_NETRUAL,
        EXPRESSION_SURPRISE
    };

    enum Glass {
        GLASS_UNKOWN = -1,
        GLASS_NO = 0,
        GLASS_YES
    };

    class DLL_API DetectParameter {
    public:
        bool checkQuality;
        bool checkLiveness;
        bool checkAge;
        bool checkGender;
        bool checkExpression;
        bool checkGlass;

    public:
        DetectParameter();
    };

    const int GROUP_NAME_MAX_SIZE = 255;
    const int PERSON_NAME_MAX_SIZE = 1024;
    const int FEATURE_NAME_MAX_SIZE = 255;
    const int TAG_MAX_SIZE = 1024;

    class DLL_API Attribute {
    public:
        Quality quality;
        Liveness liveness;
        int age;
        enum Gender gender;
        enum Expression expression;
        enum Glass glass;

        Attribute();
    };

    const int FACE_POINT_NUM = 51;

    class DLL_API Face {
    public:
        int trackId;
        int confidence;
        Rect rect;
        Pose pose;
        FacePoint facePoints[FACE_POINT_NUM];
        Attribute attribute;

        Face();
    };

    class DLL_API Feature {
    public:
        string id;
        string name;
        string feature;

        Feature();
    };

    class DLL_API Person {
    public:
        string id;
        string name;
        string tag;
        list<Feature> features;

        Person();
    };

    class DLL_API Group {
    public:
        string id;
        string name;
        enum ModelType modelType;
        string tag;

        Group();
    };

    class DLL_API RecognizeResult {
    public:
        int trackId;
        string personId;
        string personName;
        string personTag;
        float similarity;
        string feature;
        Face face;

        RecognizeResult();
    };

    class DLL_API VerifyResult {
    public:
        int trackId;
        float similarity;
        string feature;
        Face face;

        VerifyResult();
    };
}