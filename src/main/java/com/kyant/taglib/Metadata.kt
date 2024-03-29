package com.kyant.taglib

/**
 * Metadata wraps the audio properties and the property map of a file.
 */
public data class Metadata(
    val audioProperties: AudioProperties,
    val propertyMap: PropertyMap,
) {
    override fun toString(): String {
        return "Metadata(audioProperties=$audioProperties, " +
            "propertyMap=${propertyMap.mapValues { it.value.contentToString() }})"
    }
}
