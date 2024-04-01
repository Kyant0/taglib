package com.kyant.taglib

/**
 * An object that provides access to the native TagLib library.
 */
public object TagLib {
    private external fun getMetadata(
        fd: Int,
        readStyle: Int,
        readPictures: Boolean,
    ): Metadata?

    /**
     * Get metadata from file descriptor.
     *
     * @param fd File descriptor
     * @param readStyle Read style for audio properties to balance speed and accuracy
     * @param readPictures Whether to read pictures
     */
    public fun getMetadata(
        fd: Int,
        readStyle: AudioPropertiesReadStyle = AudioPropertiesReadStyle.Average,
        readPictures: Boolean = true,
    ): Metadata? = getMetadata(fd, readStyle.ordinal, readPictures)

    /**
     * Get lyrics from file descriptor. This method is equivalent to
     * `getMetadata(fd, withLyrics = true)?.propertyMap["LYRICS"]?.getOrNull(0)`
     */
    @Deprecated(
        "Use getMetadata instead",
        ReplaceWith(
            "getMetadata(fd)?.propertyMap?.get(\"LYRICS\")?.firstOrNull()",
            "com.kyant.taglib.TagLib.getMetadata",
        ),
    )
    public fun getLyrics(fd: Int): String? = getMetadata(fd)?.propertyMap?.get("LYRICS")?.firstOrNull()

    /**
     * Get pictures from file descriptor. There may be multiple pictures with different types.
     */
    public external fun getPictures(fd: Int): Array<Picture>

    /**
     * Get front cover from file descriptor.
     */
    public fun getFrontCover(fd: Int): Picture? {
        val pictures = getPictures(fd)
        return pictures.find { picture -> picture.pictureType == "Front Cover" }
            ?: pictures.firstOrNull()
    }

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
     * Save pictures by file descriptor.
     *
     * @param fd File descriptor
     * @param pictures Pictures to save
     *
     * @return Whether the operation was successful
     */
    public external fun savePictures(
        fd: Int,
        pictures: Array<Picture>,
    ): Boolean

    init {
        System.loadLibrary("taglib")
    }
}
