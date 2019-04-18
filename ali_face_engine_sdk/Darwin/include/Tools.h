#pragma once

#include <string>
#include "type.h"

using namespace std;

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {
    class DLL_API Tools {
    public:
        static float compareFeatures(string &feature1, string &feature2);

        static void drawFaceRect(Image &image, Face &face, int color);

        static void drawFaceRect(Image &image, list<Face> faceList, int color);

        static void drawFacePoint(Image &image, Face &face, int color);

        static void drawFacePoint(Image &image, list<Face> faceList, int color);


        static void drawFaceRect(Image *image, Face *face, int color);

        static void drawFaceRect(Image &image, Rect &rect, int color);

        static void drawFaceRect(Image *image, Face *faces, int faceNum, int color);

        static void drawFacePoint(Image *image, Face *face, int color);

        static void drawFacePoint(Image *image, Face *faces, int faceNum, int color);


        static string httpPost(string &url, string &postData);

        static string httpPost(string &url, char *postDataFile);

        static char *getExpressionStr(enum Expression expression);

        static char *getGenderStr(enum Gender gender);

        static char *getGlassStr(enum Glass glass);
    };
}