#pragma once

#include <string>

#include "type.h"

using namespace std;

namespace ali_face_engine {

    //[3.2.0]
    class FaceEngineConfig {
    public:
        static void changeSavePath(string &path);

        static bool isEnableDebug();

        static int FACE_FEATURE_EXTRACT_THREAD_NUM();

        static int LIVENESS_THREAD_NUM();

        static int QUALITY_THREAD_NUM();

        static int AGE_THREAD_NUM();

        static int GENDER_THREAD_NUM();

        static int EXPRESSION_THREAD_NUM();

        static int FR_SMALL_SIMILARITY_THRE();

        static int FR_CACHE_RESULT();

        static float FR_SMALL_ALGO_SIMILARITY_THRE();

        static int FR_QUALITY_THRE();

        static int FR_SYNC_INTERVAL();

        static int FD_TRACK_DETECT_INTERVAL();

        static int FD_TRACK_MAX_NUM();

        static int FD_NORM_SIZE();

        static int FD_START_SCALE();

        static int FD_MAX_SCALE();

        static int FD_DIFF_SCALE();

        static void FD_ROI(Rect &rect);

        static int FD_CONFIDENCE_THRE();
    };
}