package com.kyant.taglib

/**
 * Picture contains information and data of a picture.
 *
 * @param data ByteVector with picture data
 * @param description String with description
 * @param pictureType String with type as specified for ID3v2, e.g. "Front Cover", "Back Cover", "Band"
 * @param mimeType String with image format, e.g. "image/jpeg"
 */
public data class Picture(
    val data: ByteArray,
    val description: String,
    val pictureType: String,
    val mimeType: String,
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as Picture

        if (!data.contentEquals(other.data)) return false
        if (description != other.description) return false
        if (pictureType != other.pictureType) return false
        if (mimeType != other.mimeType) return false

        return true
    }

    override fun hashCode(): Int {
        var result = data.contentHashCode()
        result = 31 * result + description.hashCode()
        result = 31 * result + pictureType.hashCode()
        result = 31 * result + mimeType.hashCode()
        return result
    }
}
