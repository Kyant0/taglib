package com.kyant.taglib

/**
 * An object that provides access to the native TagLib library.
 */
public object TagLib {

    private external fun getMetadata(
        fd: Int,
        readStyle: Int,
        withLyrics: Boolean,
    ): Metadata?

    /**
     * Get metadata from file descriptor.
     *
     * @param fd File descriptor
     * @param readStyle Read style for audio properties to balance speed and accuracy
     * @param withLyrics Whether to read lyrics. Note if you want to save the property map later,
     * you must set this to `true` in case the lyrics are erased
     */
    public fun getMetadata(
        fd: Int,
        readStyle: AudioPropertiesReadStyle = AudioPropertiesReadStyle.Average,
        withLyrics: Boolean = false,
    ): Metadata? = getMetadata(fd, readStyle.ordinal, withLyrics)

    /**
     * Save metadata by file descriptor.
     *
     * @param fd File descriptor
     * @param propertyMap Property map to save
     *
     * @return Whether the operation was successful
     */
    public external fun savePropertyMap(
        fd: Int,
        propertyMap: PropertyMap,
    ): Boolean

    /**
     * Get lyrics from file descriptor. This method is equivalent to
     * `getMetadata(fd, withLyrics = true)?.propertyMap["LYRICS"]?.getOrNull(0)`
     */
    public external fun getLyrics(fd: Int): String?

    /**
     * Get pictures from file descriptor. There may be multiple pictures with different types.
     */
    public external fun getPictures(fd: Int): Array<Picture>?

    /**
     * Get front cover from file descriptor.
     */
    public fun getFrontCover(fd: Int): Picture? = getPictures(fd)?.let { pictures ->
        pictures.find { picture -> picture.pictureType == "Front Cover" } ?: pictures.firstOrNull()
    }

    init {
        System.loadLibrary("taglib")
    }
}
