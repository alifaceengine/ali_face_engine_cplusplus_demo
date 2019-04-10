#pragma once

#include <list>
#include "type.h"

#ifdef WIN32
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

namespace ali_face_engine {
    class DLL_API FaceRegister {

    public:
        static FaceRegister *createInstance();

        static void deleteInstance(FaceRegister *&ins);

    public:

        virtual int createGroup(Group &group) = 0;

        virtual int deleteGroup(const string &groupId) = 0;

        virtual bool isGroupExist(const string &groupName) = 0;

        virtual int updateGroup(const string &groupId, Group &group) = 0;

        virtual int getGroup(const string &groupId, Group &group) = 0;

        virtual int getAllGroups(list<Group> &groupList) = 0;

        virtual int addPerson(const string &groupId, Person &person) = 0;

        virtual int deletePerson(const string &personId) = 0;

        virtual int deleteAllPersons(const string &groupId) = 0;

        virtual int updatePerson(const string &personId, Person &person) = 0;

        virtual int getPerson(const string &personId, Person &person) = 0;

        virtual int getAllPersons(const string &groupId, list<Person> &personList) = 0;

        virtual int getPersonNum(const string &groupId, int &num) = 0;

        virtual bool isPersonExist(const string &groupId, const string &personName) = 0;

        virtual int addFeature(const string &personId, Feature &feature) = 0;

        virtual int deleteFeature(const string &featureId) = 0;

        virtual int deleteAllFeatures(const string &personId) = 0;

        virtual int updateFeature(const string &featureId, Feature &feature) = 0;

        virtual int getFeatureNum(const string &personId, int &num) = 0;

        virtual int extractFeature(Image &image, Face &face, enum ModelType modelType, string &feature) = 0;

    protected:
        FaceRegister();

        virtual ~FaceRegister();
    };
}