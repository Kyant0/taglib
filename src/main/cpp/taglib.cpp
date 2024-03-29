#include <jni.h>
#include "utils.h"
#include "fileref.h"
#include "tfilestream.h"

extern "C" JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getMetadata(
        JNIEnv *env,
        jobject /* this */,
        jint fd,
        jint read_style,
        jboolean with_lyrics
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
        if (!with_lyrics && properties.contains("LYRICS")) {
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

            jbyteArray bytes = env->NewByteArray(static_cast<jint>(pictureData.size()));
            auto description = picture.value("description").toString();
            jstring jDescription = env->NewStringUTF(description.toCString(true));
            auto pictureType = picture.value("pictureType").toString();
            jstring jPictureType = env->NewStringUTF(pictureType.toCString(true));
            auto mimeType = picture.value("mimeType").toString();
            jstring jMimeType = env->NewStringUTF(mimeType.toCString(true));

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

extern "C" JNIEXPORT jboolean JNICALL
Java_com_kyant_taglib_TagLib_savePictures(
        JNIEnv *env,
        jobject /* this */,
        jint fd,
        jobjectArray pictures
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, false);
        TagLib::FileRef f(stream.get(), false);

        if (f.isNull()) {
            return false;
        }

        TagLib::List<TagLib::Map<TagLib::String, TagLib::Variant>> properties;
        auto pictureCount = env->GetArrayLength(pictures);
        for (auto i = 0; i < pictureCount; i++) {
            auto pictureObject = env->GetObjectArrayElement(pictures, i);
            auto bytes = reinterpret_cast<jbyteArray>(env->CallObjectMethod(pictureObject, pictureGetData));
            auto description = reinterpret_cast<jstring>(env->CallObjectMethod(pictureObject, pictureGetDescription));
            auto pictureType = reinterpret_cast<jstring>(env->CallObjectMethod(pictureObject, pictureGetPictureType));
            auto mimeType = reinterpret_cast<jstring>(env->CallObjectMethod(pictureObject, pictureGetMimeType));

            auto pictureData = env->GetByteArrayElements(bytes, nullptr);
            auto pictureDataSize = env->GetArrayLength(bytes);
            TagLib::ByteVector pictureDataVector(
                    reinterpret_cast<const char *>(pictureData),
                    static_cast<uint>(pictureDataSize)
            );
            env->ReleaseByteArrayElements(bytes, pictureData, JNI_ABORT);

            TagLib::Map<TagLib::String, TagLib::Variant> pictureProperties;
            pictureProperties["data"] = pictureDataVector;
            pictureProperties["description"] = TagLib::String(env->GetStringUTFChars(description, nullptr));
            pictureProperties["pictureType"] = TagLib::String(env->GetStringUTFChars(pictureType, nullptr));
            pictureProperties["mimeType"] = TagLib::String(env->GetStringUTFChars(mimeType, nullptr));
            properties.append(pictureProperties);
        }

        f.setComplexProperties("PICTURE", properties);
        bool success = f.save();
        return success;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return false;
    }
}
