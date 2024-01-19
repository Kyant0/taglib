package com.kyant.taglib

/**
 * An object that provides access to the native TagLib library.
 */
public object TagLib {
    private external fun getMetadata(
        fd: Int,
        fileName: String,
        readStyle: Int,
        withLyrics: Boolean,
    ): Metadata?

    /**
     * Get metadata from file descriptor.
     *
     * @param fd File descriptor
     * @param fileName File name. This is used to determine the file type, leave it empty if unknown
     * @param readStyle Read style for audio properties to balance speed and accuracy
     * @param readLyrics Whether to read lyrics. Note if you want to save the property map later,
     * you must set this to `true` in case the lyrics are erased
     */
    public fun getMetadata(
        fd: Int,
        fileName: String = "",
        readStyle: AudioPropertiesReadStyle = AudioPropertiesReadStyle.Average,
        readLyrics: Boolean = false,
    ): Metadata? = getMetadata(fd, fileName, readStyle.ordinal, readLyrics)

    /**
     * Save metadata by file descriptor.
     *
     * @param fd File descriptor
     * @param fileName File name. This is used to determine the file type, leave it empty if unknown
     * @param propertyMap Property map to save
     *
     * @return Whether the operation was successful
     */
    public external fun savePropertyMap(
        fd: Int,
        fileName: String = "",
        propertyMap: PropertyMap,
    ): Boolean

    /**
     * Get lyrics from file descriptor. This method is equivalent to
     * `getMetadata(fd, withLyrics = true)?.propertyMap["LYRICS"]?.getOrNull(0)`
     */
    public external fun getLyrics(
        fd: Int,
        fileName: String = "",
    ): String?

    /**
     * Get pictures from file descriptor. There may be multiple pictures with different types.
     */
    public external fun getPictures(
        fd: Int,
        fileName: String = "",
    ): Array<Picture>?

    /**
     * Get front cover from file descriptor.
     */
    public fun getFrontCover(
        fd: Int,
        fileName: String = "",
    ): Picture? =
        getPictures(fd, fileName)?.let { pictures ->
            pictures.find { picture -> picture.pictureType == "Front Cover" } ?: pictures.firstOrNull()
        }

    init {
        System.loadLibrary("taglib")
    }
}
