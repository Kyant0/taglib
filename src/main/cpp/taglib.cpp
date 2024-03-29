#include <android/log.h>
#include <jni.h>
#include <unistd.h>
#include "audioproperties.h"
#include "fileref.h"
#include "tdebuglistener.h"
#include "tfilestream.h"
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

    TagLib::setDebugListener(nullptr);
}

// Helper function to convert C++ StringList to JNI String array
jobjectArray StringListToJniStringArray(JNIEnv *env, const TagLib::StringList &stringList) {
    jobjectArray array = env->NewObjectArray(stringList.size(), stringClass, nullptr);
    int i = 0;
    for (const auto &str: stringList) {
        env->SetObjectArrayElement(array, i, env->NewStringUTF(str.toCString(true)));
        i++;
    }
    return array;
}

// Helper function to convert C++ PropertyMap to JNI HashMap
jobject PropertyMapToJniHashMap(JNIEnv *env, const TagLib::PropertyMap &propertyMap) {
    jobject hashMap = env->NewObject(hashMapClass, hashMapInit, static_cast<jint>(propertyMap.size()));

    for (const auto &it: propertyMap) {
        const std::string key = it.first.toCString(true);
        const TagLib::StringList &valueList = it.second;

        jobjectArray valueArray = StringListToJniStringArray(env, valueList);

        jstring jKey = env->NewStringUTF(key.c_str());
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

        stringList.append(cStr);

        env->ReleaseStringUTFChars(jStr, cStr);
        env->DeleteLocalRef(jStr);
    }

    return stringList;
}

// Helper function to convert JNI HashMap to C++ PropertyMap
TagLib::PropertyMap JniHashMapToPropertyMap(JNIEnv *env, jobject hashMap) {
    TagLib::PropertyMap propertyMap;

    jclass entrySetClass = env->FindClass("java/util/Set");
    jmethodID iteratorMethod = env->GetMethodID(entrySetClass, "iterator", "()Ljava/util/Iterator;");
    jmethodID entrySetMethod = env->GetMethodID(hashMapClass, "entrySet", "()Ljava/util/Set;");
    jobject entrySet = env->CallObjectMethod(hashMap, entrySetMethod);
    jobject iterator = env->CallObjectMethod(entrySet, iteratorMethod);

    jclass iteratorClass = env->FindClass("java/util/Iterator");
    jmethodID hasNextMethod = env->GetMethodID(iteratorClass, "hasNext", "()Z");
    jmethodID nextMethod = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");

    jclass mapEntryClass = env->FindClass("java/util/Map$Entry");
    jmethodID getKeyMethod = env->GetMethodID(mapEntryClass, "getKey", "()Ljava/lang/Object;");
    jmethodID getValueMethod = env->GetMethodID(mapEntryClass, "getValue", "()Ljava/lang/Object;");

    while (env->CallBooleanMethod(iterator, hasNextMethod)) {
        jobject entry = env->CallObjectMethod(iterator, nextMethod);
        jobject key = env->CallObjectMethod(entry, getKeyMethod);
        jobject value = env->CallObjectMethod(entry, getValueMethod);

        const char *keyStr = env->GetStringUTFChars(static_cast<jstring>(key), nullptr);
        const auto valueList = JniStringArrayToStringList(env, static_cast<jobjectArray>(value));

        propertyMap[TagLib::String(keyStr)] = valueList;

        env->ReleaseStringUTFChars(static_cast<jstring>(key), keyStr);
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(entrySetClass);
    env->DeleteLocalRef(iteratorClass);
    env->DeleteLocalRef(mapEntryClass);

    return propertyMap;
}

void throwJavaException(JNIEnv *env, const char *message) {
    jclass exClass = env->FindClass("com/kyant/taglib/TagLibException");
    if (exClass != nullptr) {
        env->ThrowNew(exClass, message);
    }
    env->DeleteLocalRef(exClass);
}


extern "C" JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getMetadata(
        JNIEnv *env,
        jobject /* this */,
        jint fd,
        jint read_style,
        jboolean read_lyrics
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, true);
        auto style = static_cast<TagLib::AudioProperties::ReadStyle>(read_style);
        TagLib::FileRef f(stream.get(), true, style);

        if (f.isNull()) {
            return nullptr;
        }

        auto audioProperties = f.audioProperties();
        jobject audioPropertiesObject;
        if (audioProperties) {
            jint duration = static_cast<jint>(audioProperties->lengthInMilliseconds());
            jint bitrate = static_cast<jint>(audioProperties->bitrate());
            jint sampleRate = static_cast<jint>(audioProperties->sampleRate());
            jint channels = static_cast<jint>(audioProperties->channels());
            audioPropertiesObject = env->NewObject(
                    audioPropertiesClass, audioPropertiesConstructor,
                    duration, bitrate, sampleRate, channels);
        } else {
            audioPropertiesObject = env->NewObject(
                    audioPropertiesClass, audioPropertiesConstructor,
                    0, 0, 0, 0);
        }

        auto properties = f.properties();
        if (!read_lyrics && properties.contains("LYRICS")) {
            properties.erase("LYRICS");
        }
        jobject propertiesMap = PropertyMapToJniHashMap(env, properties);

        jobject metadata = env->NewObject(
                metadataClass, metadataConstructor,
                audioPropertiesObject, propertiesMap
        );
        return metadata;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return nullptr;
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_kyant_taglib_TagLib_savePropertyMap(
        JNIEnv *env,
        jobject /* this */,
        jint fd,
        jobject property_map
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, false);
        TagLib::FileRef f(stream.get(), false);

        if (f.isNull()) {
            return false;
        }

        auto propertiesMap = JniHashMapToPropertyMap(env, property_map);
        f.setProperties(propertiesMap);
        bool success = f.save();
        return success;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return false;
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_kyant_taglib_TagLib_getLyrics(
        JNIEnv *env,
        jobject /* this */,
        jint fd
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, true);
        TagLib::FileRef f(stream.get(), false);

        if (f.isNull()) {
            return nullptr;
        }

        auto properties = f.properties();
        if (!properties.contains("LYRICS")) {
            return nullptr;
        }

        auto lyrics = properties.find("LYRICS")->second.front().toCString(true);
        return env->NewStringUTF(lyrics);
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return nullptr;
    }
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_kyant_taglib_TagLib_getPictures(
        JNIEnv *env,
        jobject /* this */,
        jint fd
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, true);
        TagLib::FileRef f(stream.get(), false);

        if (f.isNull()) {
            return nullptr;
        }

        auto pictures = f.complexProperties("PICTURE");
        if (pictures.isEmpty()) {
            return nullptr;
        }

        auto pictureIndex = 0;
        jobjectArray pictureArray = env->NewObjectArray(pictures.size(), pictureClass, nullptr);
        for (const auto &picture: pictures) {
            auto pictureData = picture.value("data").toByteVector();
            if (pictureData.isEmpty()) {
                continue;
            }

            auto description = picture.value("description").toString();
            jstring jDescription = env->NewStringUTF(description.toCString(true));
            auto pictureType = picture.value("pictureType").toString();
            jstring jPictureType = env->NewStringUTF(pictureType.toCString(true));
            auto mimeType = picture.value("mimeType").toString();
            jstring jMimeType = env->NewStringUTF(mimeType.toCString(true));
            jbyteArray bytes = env->NewByteArray(static_cast<jint>(pictureData.size()));

            env->SetByteArrayRegion(
                    bytes,
                    0,
                    static_cast<jint>(pictureData.size()),
                    reinterpret_cast<const jbyte *>(pictureData.data())
            );
            jobject pictureObject = env->NewObject(
                    pictureClass, pictureConstructor,
                    bytes, jDescription, jPictureType, jMimeType);
            env->DeleteLocalRef(bytes);
            env->DeleteLocalRef(jDescription);
            env->DeleteLocalRef(jPictureType);
            env->DeleteLocalRef(jMimeType);
            env->SetObjectArrayElement(pictureArray, pictureIndex, pictureObject);
            env->DeleteLocalRef(pictureObject);
            pictureIndex++;
        }

        return pictureArray;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return nullptr;
    }
}
