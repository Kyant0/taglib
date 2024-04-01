#include <jni.h>
#include "utils.h"
#include "tfilestream.h"

extern "C" JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getMetadata(
        JNIEnv *env,
        jobject /* this */,
        jint fd,
        jint read_style,
        jboolean read_pictures
) {
    try {
        auto stream = std::make_unique<TagLib::FileStream>(fd, true);
        auto style = static_cast<TagLib::AudioProperties::ReadStyle>(read_style);
        TagLib::FileRef f(stream.get(), true, style);

        if (f.isNull()) {
            return nullptr;
        }

        jobject audioProperties = getAudioProperties(env, f);
        jobject propertiesMap = getPropertyMap(env, f);
        jobjectArray pictures;
        if (read_pictures) {
            pictures = getPictures(env, f);
        } else {
            pictures = emptyPictureArray(env);
        }

        jobject metadata = env->NewObject(
                metadataClass, metadataConstructor,
                audioProperties, propertiesMap, pictures
        );
        return metadata;
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
            return emptyPictureArray(env);
        }

        jobjectArray pictures = getPictures(env, f);
        return pictures;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return emptyPictureArray(env);
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

        auto propertyMap = JniHashMapToPropertyMap(env, property_map);
        f.setProperties(propertyMap);
        bool success = f.save();
        return success;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return false;
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

        auto pictureList = JniPictureArrayToPictureList(env, pictures);
        f.setComplexProperties("PICTURE", pictureList);
        bool success = f.save();
        return success;
    } catch (const std::exception &e) {
        throwJavaException(env, e.what());
        return false;
    }
}
