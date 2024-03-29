#ifndef TAGLIB_UTILS_H
#define TAGLIB_UTILS_H

#include <android/log.h>
#include "tdebuglistener.h"
#include "tpropertymap.h"

class DebugListener : public TagLib::DebugListener {
    void printMessage(const TagLib::String &msg) override {
        __android_log_print(ANDROID_LOG_VERBOSE, "TagLib", "%s", msg.toCString(true));
    }
};

DebugListener listener;

jclass stringClass = nullptr;

jclass hashMapClass = nullptr;
jmethodID hashMapInit = nullptr;
jmethodID hashMapPut = nullptr;

jclass metadataClass = nullptr;
jmethodID metadataConstructor = nullptr;

jclass audioPropertiesClass = nullptr;
jmethodID audioPropertiesConstructor = nullptr;

jclass pictureClass = nullptr;
jmethodID pictureConstructor = nullptr;

jclass entrySetClass = nullptr;
jmethodID iteratorMethod = nullptr;
jmethodID entrySetMethod = nullptr;

jclass iteratorClass = nullptr;
jmethodID hasNextMethod = nullptr;
jmethodID nextMethod = nullptr;

jclass mapEntryClass = nullptr;
jmethodID getKeyMethod = nullptr;
jmethodID getValueMethod = nullptr;

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass _stringClass = env->FindClass("java/lang/String");
    stringClass = reinterpret_cast<jclass>(env->NewGlobalRef(_stringClass));
    env->DeleteLocalRef(_stringClass);

    jclass _hashMapClass = env->FindClass("java/util/HashMap");
    hashMapClass = reinterpret_cast<jclass>(env->NewGlobalRef(_hashMapClass));
    env->DeleteLocalRef(_hashMapClass);
    hashMapInit = env->GetMethodID(hashMapClass, "<init>", "(I)V");
    hashMapPut = env->GetMethodID(hashMapClass, "put",
                                  "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jclass _metadataClass = env->FindClass("com/kyant/taglib/Metadata");
    metadataClass = reinterpret_cast<jclass>(env->NewGlobalRef(_metadataClass));
    env->DeleteLocalRef(_metadataClass);
    metadataConstructor = env->GetMethodID(metadataClass, "<init>",
                                           "(Lcom/kyant/taglib/AudioProperties;Ljava/util/HashMap;)V");

    jclass _audioPropertiesClass = env->FindClass("com/kyant/taglib/AudioProperties");
    audioPropertiesClass = reinterpret_cast<jclass>(env->NewGlobalRef(_audioPropertiesClass));
    env->DeleteLocalRef(_audioPropertiesClass);
    audioPropertiesConstructor = env->GetMethodID(audioPropertiesClass, "<init>", "(IIII)V");

    jclass _pictureClass = env->FindClass("com/kyant/taglib/Picture");
    pictureClass = reinterpret_cast<jclass>(env->NewGlobalRef(_pictureClass));
    env->DeleteLocalRef(_pictureClass);
    pictureConstructor = env->GetMethodID(pictureClass, "<init>",
                                          "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    jclass _entrySetClass = env->FindClass("java/util/Set");
    entrySetClass = reinterpret_cast<jclass>(env->NewGlobalRef(_entrySetClass));
    env->DeleteLocalRef(_entrySetClass);
    iteratorMethod = env->GetMethodID(entrySetClass, "iterator", "()Ljava/util/Iterator;");
    entrySetMethod = env->GetMethodID(hashMapClass, "entrySet", "()Ljava/util/Set;");

    jclass _iteratorClass = env->FindClass("java/util/Iterator");
    iteratorClass = reinterpret_cast<jclass>(env->NewGlobalRef(_iteratorClass));
    env->DeleteLocalRef(_iteratorClass);
    hasNextMethod = env->GetMethodID(iteratorClass, "hasNext", "()Z");
    nextMethod = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");

    jclass _mapEntryClass = env->FindClass("java/util/Map$Entry");
    mapEntryClass = reinterpret_cast<jclass>(env->NewGlobalRef(_mapEntryClass));
    env->DeleteLocalRef(_mapEntryClass);
    getKeyMethod = env->GetMethodID(mapEntryClass, "getKey", "()Ljava/lang/Object;");
    getValueMethod = env->GetMethodID(mapEntryClass, "getValue", "()Ljava/lang/Object;");

    TagLib::setDebugListener(&listener);

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    env->DeleteGlobalRef(stringClass);
    env->DeleteGlobalRef(hashMapClass);
    env->DeleteGlobalRef(metadataClass);
    env->DeleteGlobalRef(audioPropertiesClass);
    env->DeleteGlobalRef(pictureClass);
    env->DeleteGlobalRef(entrySetClass);
    env->DeleteGlobalRef(iteratorClass);
    env->DeleteGlobalRef(mapEntryClass);

    stringClass = nullptr;
    hashMapClass = nullptr;
    hashMapInit = nullptr;
    hashMapPut = nullptr;
    metadataClass = nullptr;
    metadataConstructor = nullptr;
    audioPropertiesClass = nullptr;
    audioPropertiesConstructor = nullptr;
    pictureClass = nullptr;
    pictureConstructor = nullptr;
    entrySetClass = nullptr;
    iteratorMethod = nullptr;
    entrySetMethod = nullptr;
    iteratorClass = nullptr;
    hasNextMethod = nullptr;
    nextMethod = nullptr;
    mapEntryClass = nullptr;
    getKeyMethod = nullptr;
    getValueMethod = nullptr;

    TagLib::setDebugListener(nullptr);
}

// Helper function to convert C++ StringList to JNI String array
jobjectArray StringListToJniStringArray(JNIEnv *env, const TagLib::StringList &stringList) {
    jobjectArray array = env->NewObjectArray(stringList.size(), stringClass, nullptr);
    int i = 0;
    for (const auto &str: stringList) {
        jstring jStr = env->NewStringUTF(str.toCString(true));
        env->SetObjectArrayElement(array, i, jStr);
        env->DeleteLocalRef(jStr);
        i++;
    }
    return array;
}

// Helper function to convert C++ PropertyMap to JNI HashMap
jobject PropertyMapToJniHashMap(JNIEnv *env, const TagLib::PropertyMap &propertyMap) {
    jobject hashMap = env->NewObject(hashMapClass, hashMapInit, static_cast<jint>(propertyMap.size()));

    for (const auto &it: propertyMap) {
        const char *key = it.first.toCString(true);
        const TagLib::StringList &valueList = it.second;

        jobjectArray valueArray = StringListToJniStringArray(env, valueList);

        jstring jKey = env->NewStringUTF(key);
        env->CallObjectMethod(hashMap, hashMapPut, jKey, valueArray);

        env->DeleteLocalRef(jKey);
        env->DeleteLocalRef(valueArray);
    }

    return hashMap;
}

// Helper function to convert JNI String array to C++ StringList
TagLib::StringList JniStringArrayToStringList(JNIEnv *env, jobjectArray stringArray) {
    TagLib::StringList stringList;

    jsize arrayLength = env->GetArrayLength(stringArray);
    for (jsize i = 0; i < arrayLength; ++i) {
        jstring jStr = static_cast<jstring>(env->GetObjectArrayElement(stringArray, i));
        const char *cStr = env->GetStringUTFChars(jStr, nullptr);
        stringList.append(TagLib::String(cStr, TagLib::String::UTF8));
        env->ReleaseStringUTFChars(jStr, cStr);
        env->DeleteLocalRef(jStr);
    }

    return stringList;
}

// Helper function to convert JNI HashMap to C++ PropertyMap
TagLib::PropertyMap JniHashMapToPropertyMap(JNIEnv *env, jobject hashMap) {
    TagLib::PropertyMap propertyMap;

    jobject entrySet = env->CallObjectMethod(hashMap, entrySetMethod);
    jobject iterator = env->CallObjectMethod(entrySet, iteratorMethod);

    while (env->CallBooleanMethod(iterator, hasNextMethod)) {
        jobject entry = env->CallObjectMethod(iterator, nextMethod);
        jobject key = env->CallObjectMethod(entry, getKeyMethod);
        jobject value = env->CallObjectMethod(entry, getValueMethod);

        const char *keyStr = env->GetStringUTFChars(static_cast<jstring>(key), nullptr);
        const auto valueList = JniStringArrayToStringList(env, static_cast<jobjectArray>(value));

        propertyMap[TagLib::String(keyStr, TagLib::String::UTF8)] = valueList;

        env->ReleaseStringUTFChars(static_cast<jstring>(key), keyStr);
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    return propertyMap;
}

void throwJavaException(JNIEnv *env, const char *message) {
    jclass exClass = env->FindClass("com/kyant/taglib/TagLibException");
    if (exClass != nullptr) {
        env->ThrowNew(exClass, message);
    }
    env->DeleteLocalRef(exClass);
}

#endif //TAGLIB_UTILS_H
