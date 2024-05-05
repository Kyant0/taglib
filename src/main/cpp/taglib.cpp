#include "tfilestream.h"
#include "utils.h"

extern "C" {
JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getAudioProperties(
        JNIEnv *env,
        jobject,
        jint fd,
        jint read_style
) {
    char *path = getRealPathFromFd(fd);
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    auto style = static_cast<TagLib::AudioProperties::ReadStyle>(read_style);
    TagLibExt::FileRef f(path, stream.get(), true, style);

    if (f.isNull()) {
        free(path);
        return nullptr;
    }

    jobject audioProperties = getAudioProperties(env, f);
    free(path);
    return audioProperties;
}

JNIEXPORT jobject JNICALL
Java_com_kyant_taglib_TagLib_getMetadata(
        JNIEnv *env,
        jobject,
        jint fd,
        jboolean read_pictures
) {
    char *path = getRealPathFromFd(fd);
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    TagLibExt::FileRef f(path, stream.get(), false);

    if (f.isNull()) {
        free(path);
        return nullptr;
    }

    jobject propertiesMap = getPropertyMap(env, f);
    jobjectArray pictures;
    if (read_pictures) {
        pictures = getPictures(env, f);
    } else {
        pictures = emptyPictureArray(env);
    }

    jobject metadata = env->NewObject(
            metadataClass, metadataConstructor,
            propertiesMap, pictures
    );
    free(path);
    return metadata;
}

JNIEXPORT jobjectArray JNICALL
Java_com_kyant_taglib_TagLib_getPictures(
        JNIEnv *env,
        jobject,
        jint fd
) {
    char *path = getRealPathFromFd(fd);
    auto stream = std::make_unique<TagLib::FileStream>(fd, true);
    TagLibExt::FileRef f(path, stream.get(), false);

    if (f.isNull()) {
        free(path);
        return emptyPictureArray(env);
    }

    jobjectArray pictures = getPictures(env, f);
    free(path);
    return pictures;
}

JNIEXPORT jboolean JNICALL
Java_com_kyant_taglib_TagLib_savePropertyMap(
        JNIEnv *env,
        jobject,
        jint fd,
        jobject property_map
) {
    char *path = getRealPathFromFd(fd);
    auto stream = std::make_unique<TagLib::FileStream>(fd, false);
    TagLibExt::FileRef f(path, stream.get(), false);

    if (f.isNull()) {
        free(path);
        return false;
    }

    auto propertyMap = JniHashMapToPropertyMap(env, property_map);
    f.setProperties(propertyMap);
    bool success = f.save();
    free(path);
    return success;
}

JNIEXPORT jboolean JNICALL
Java_com_kyant_taglib_TagLib_savePictures(
        JNIEnv *env,
        jobject,
        jint fd,
        jobjectArray pictures
) {
    char *path = getRealPathFromFd(fd);
    auto stream = std::make_unique<TagLib::FileStream>(fd, false);
    TagLibExt::FileRef f(path, stream.get(), false);

    if (f.isNull()) {
        free(path);
        return false;
    }

    auto pictureList = JniPictureArrayToPictureList(env, pictures);
    f.setComplexProperties("PICTURE", pictureList);
    bool success = f.save();
    free(path);
    return success;
}
}
