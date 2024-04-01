package com.kyant.taglib

/**
 * Metadata contains audio properties, property map and pictures of an audio file.
 */
public data class Metadata(
    val audioProperties: AudioProperties,
    val propertyMap: PropertyMap,
    val pictures: Array<Picture>,
) {
    override fun toString(): String {
        return "Metadata(audioProperties=$audioProperties, " +
            "propertyMap=${propertyMap.mapValues { it.value.contentToString() }}, " +
            "pictures=${pictures.contentToString()})"
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is Metadata) return false

        if (audioProperties != other.audioProperties) return false
        if (propertyMap != other.propertyMap) return false

        return pictures.contentEquals(other.pictures)
    }

    override fun hashCode(): Int {
        var result = audioProperties.hashCode()
        result = 31 * result + propertyMap.hashCode()
        result = 31 * result + (pictures?.contentHashCode() ?: 0)
        return result
    }
}
