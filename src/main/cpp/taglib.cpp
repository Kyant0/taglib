#include <jni.h>
#include "fileref.h"
#include "tfilestream.h"
#include "tpropertymap.h"

jmethodID metadataConstructor = nullptr;
jmethodID audioPropertiesConstructor = nullptr;
jmethodID pictureConstructor = nullptr;

// Helper function to convert C++ StringList to JNI String array
jobjectArray StringListToJniStringArray(JNIEnv *env, const TagLib::StringList &stringList) {
    jclass stringCls = env->FindClass("java/lang/String");
    jobjectArray array = env->NewObjectArray(stringList.size(), stringCls, nullptr);
    int i = 0;
    for (const auto &str: stringList) {
        env->SetObjectArrayElement(array, i, env->NewStringUTF(str.toCString(true)));
        i++;
    }
    return array;
}

// Helper function to convert C++ PropertyMap to JNI HashMap
jobject PropertyMapToJniHashMap(JNIEnv *env, const TagLib::PropertyMap &propertyMap) {
    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID hashMapInit = env->GetMethodID(hashMapClass, "<init>", "(I)V");
    jmethodID hashMapPut = env->GetMethodID(hashMapClass, "put",
                                            "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

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

    env->DeleteLocalRef(hashMapClass);

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

    jclass hashMapClass = env->FindClass("java/util/HashMap");
    jmethodID entrySetMethod = env->GetMethodID(hashMapClass, "entrySet", "()Ljava/util/Set;");

    jclass entrySetClass = env->FindClass("java/util/Set");
    jmethodID iteratorMethod = env->GetMethodID(entrySetClass, "iterator", "()Ljava/util/Iterator;");

    jclass iteratorClass = env->FindClass("java/util/Iterator");
    jmethodID hasNextMethod = env->GetMethodID(iteratorClass, "hasNext", "()Z");
    jmethodID nextMethod = env->GetMethodID(iteratorClass, "next", "()Ljava/lang/Object;");
    jclass mapEntryClass = env->FindClass("java/util/Map$Entry");
    jmethodID getKeyMethod = env->GetMethodID(mapEntryClass, "getKey", "()Ljava/lang/Object;");
    jmethodID getValueMethod = env->GetMethodID(mapEntryClass, "getValue", "()Ljava/lang/Object;");

    jobject entrySet = env->CallObjectMethod(hashMap, entrySetMethod);
    jobject iterator = env->CallObjectMethod(entrySet, iteratorMethod);

    while (env->CallBooleanMethod(iterator, hasNextMethod)) {
        jobject entry = env->CallObjectMethod(iterator, nextMethod);
        jobject key = env->CallObjectMethod(entry, getKeyMethod);
        jobject value = env->CallObjectMethod(entry, getValueMethod);

        const char *keyStr = env->GetStringUTFChars(static_cast<jstring>(key), nullptr);
        const TagLib::StringList valueList = JniStringArrayToStringList(env, static_cast<jobjectArray>(value));

        propertyMap[TagLib::String(keyStr)] = valueList;

        env->ReleaseStringUTFChars(static_cast<jstring>(key), keyStr);
        env->DeleteLocalRef(entry);
        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(hashMapClass);
    env->DeleteLocalRef(iteratorClass);
    env->DeleteLocalRef(mapEntryClass);

    return propertyMap;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getMetadata(JNIEnv *env,
                                         jobject /* this */,
                                         jint fd,
                                         jint read_style,
                                         jboolean with_lyrics) {
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    auto style = static_cast<TagLib::AudioProperties::ReadStyle>(read_style);
    TagLib::FileRef fileRef(stream.get(), true, style);

    if (fileRef.isNull()) {
        return nullptr;
    }

    jclass metadataClass = env->FindClass("com/kyant/taglib/Metadata");
    if (metadataConstructor == nullptr) {
        metadataConstructor = env->GetMethodID(metadataClass, "<init>",
                                               "(Lcom/kyant/taglib/AudioProperties;Ljava/util/Map;)V");
    }

    jclass audioPropertiesClass = env->FindClass("com/kyant/taglib/AudioProperties");
    if (audioPropertiesConstructor == nullptr) {
        audioPropertiesConstructor = env->GetMethodID(audioPropertiesClass, "<init>", "(IIII)V");
    }

    auto audioProperties = fileRef.audioProperties();
    jint duration = static_cast<jint>(audioProperties->lengthInMilliseconds());
    jint bitrate = static_cast<jint>(audioProperties->bitrate());
    jint sampleRate = static_cast<jint>(audioProperties->sampleRate());
    jint channels = static_cast<jint>(audioProperties->channels());
    jobject audioPropertiesObject = env->NewObject(
            audioPropertiesClass, audioPropertiesConstructor,
            duration, bitrate, sampleRate, channels);

    TagLib::PropertyMap properties = fileRef.tag()->properties();
    if (!with_lyrics) {
        properties.erase("LYRICS");
    }
    jobject propertiesMap = PropertyMapToJniHashMap(env, properties);

    jobject metadata = env->NewObject(
            metadataClass, metadataConstructor,
            audioPropertiesObject, propertiesMap
    );
    env->DeleteLocalRef(metadataClass);
    env->DeleteLocalRef(audioPropertiesClass);
    env->DeleteLocalRef(propertiesMap);

    return metadata;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_kyant_taglib_TagLib_savePropertyMap(JNIEnv *env,
                                             jobject /* this */,
                                             jint fd,
                                             jobject property_map) {
    auto stream = std::make_unique<TagLib::FileStream>(fd, false);
    TagLib::FileRef fileRef(stream.get(), false);

    if (fileRef.isNull()) {
        return false;
    }

    jclass metadataClass = env->FindClass("com/kyant/taglib/Metadata");
    if (metadataConstructor == nullptr) {
        metadataConstructor = env->GetMethodID(metadataClass, "<init>",
                                               "(Lcom/kyant/taglib/AudioProperties;Ljava/util/Map;)V");
    }

    auto propertiesMap = JniHashMapToPropertyMap(env, property_map);
    fileRef.setProperties(propertiesMap);
    return fileRef.save();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_kyant_taglib_TagLib_getLyrics(JNIEnv *env, jobject /* this */, jint fd) {
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    TagLib::FileRef fileRef(stream.get(), false);

    if (fileRef.isNull()) {
        return nullptr;
    }

    TagLib::PropertyMap properties = fileRef.tag()->properties();
    if (!properties.contains("LYRICS")) {
        return nullptr;
    }

    auto lyrics = properties.find("LYRICS")->second.front().toCString(true);
    return env->NewStringUTF(lyrics);
}

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_kyant_taglib_TagLib_getPictures(JNIEnv *env,
                                         jobject /* this */,
                                         jint fd) {
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    TagLib::FileRef fileRef(stream.get(), false);

    jclass pictureClass = env->FindClass("com/kyant/taglib/Picture");
    if (pictureConstructor == nullptr) {
        pictureConstructor = env->GetMethodID(pictureClass, "<init>",
                                              "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    }

    auto file = fileRef.file();
    if (file == nullptr) {
        return nullptr;
    }

    auto pictures = file->complexProperties("PICTURE");
    if (pictures.isEmpty()) {
        return nullptr;
    }

    jobjectArray pictureArray = env->NewObjectArray(pictures.size(), pictureClass, nullptr);
    for (const auto &picture: pictures) {
        TagLib::ByteVector pictureData = picture.value("data").toByteVector();
        if (pictureData.isEmpty()) {
            continue;
        }

        TagLib::String description = picture.value("description").toString();
        jstring jDescription = env->NewStringUTF(description.toCString(true));
        TagLib::String pictureType = picture.value("pictureType").toString();
        jstring jPictureType = env->NewStringUTF(pictureType.toCString(true));
        TagLib::String mimeType = picture.value("mimeType").toString();
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
        env->SetObjectArrayElement(pictureArray, static_cast<jsize>(&picture - &pictures[0]), pictureObject);
        env->DeleteLocalRef(pictureObject);
    }

    env->DeleteLocalRef(pictureClass);

    return pictureArray;
}
