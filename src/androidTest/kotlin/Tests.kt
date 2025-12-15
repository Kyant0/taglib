import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.ParcelFileDescriptor
import androidx.test.platform.app.InstrumentationRegistry
import com.kyant.taglib.Constants
import com.kyant.taglib.Picture
import com.kyant.taglib.TagLib
import org.junit.Assert
import org.junit.Test
import java.io.ByteArrayOutputStream
import java.io.File
import java.nio.charset.Charset

class Tests {
    private val context: Context = InstrumentationRegistry.getInstrumentation().targetContext

    @Test
    fun test_all() {
        read_and_write_m4a()
        read_and_write_pictures_flac()
        read_flac_multiple_pictures()
        ensure_utf8()
        bad_encoding()
        testConstants()
    }

    private fun read_and_write_m4a() {
        getFdFromAssets(context, "Sample_BeeMoved_48kHz16bit.m4a").use { fd ->

            // Read audio properties

            val audioProperties = TagLib.getAudioProperties(fd.dup().detachFd())!!
            Assert.assertEquals(39936, audioProperties.length)

            // Read metadata

            val metadata = TagLib.getMetadata(fd = fd.dup().detachFd(), readPictures = true)!!
            Assert.assertEquals("Bee Moved", metadata.propertyMap["TITLE"]!!.single())
            Assert.assertEquals(58336, metadata.pictures.single().data.size)

            // Read single metadata

            val artists = TagLib.getMetadataPropertyValues(fd.dup().detachFd(), "ARTIST")!!
            Assert.assertEquals("Blue Monday FM", artists.single())

            // Save metadata

            val newTitle = "Bee Moved (Remix)"
            val newPropertyMap = metadata.propertyMap.apply {
                this["TITLE"] = arrayOf(newTitle)
            }
            val saved = TagLib.savePropertyMap(fd.dup().detachFd(), newPropertyMap)
            Assert.assertTrue(saved)

            val newMetadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals(newTitle, newMetadata.propertyMap["TITLE"]!!.single())
        }
    }

    private fun read_and_write_pictures_flac() {
        getFdFromAssets(context, "是什么让我遇见这样的你 - 白安.flac").use { fd ->
            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals("是什么让我遇见这样的你", metadata.propertyMap["TITLE"]!!.single())

            val pictures = TagLib.getPictures(fd.dup().detachFd())
            val originalPicture = pictures.single()
            Assert.assertEquals(42716, originalPicture.data.size)

            // Save the original picture

            val saved = TagLib.savePictures(fd.dup().detachFd(), arrayOf(originalPicture))
            Assert.assertTrue(saved)
            Assert.assertEquals(originalPicture, TagLib.getPictures(fd.dup().detachFd()).single())

            // Save new multiple pictures

            val newPicture1 = ByteArrayOutputStream().use {
                BitmapFactory
                    .decodeStream(context.assets.open("699_200x200.jpg"))
                    .compress(Bitmap.CompressFormat.JPEG, 100, it)
                Picture(
                    data = it.toByteArray(),
                    description = "Front Cover",
                    pictureType = "Front Cover",
                    mimeType = "image/jpeg",
                )
            }

            val newPicture2 = ByteArrayOutputStream().use {
                BitmapFactory
                    .decodeStream(context.assets.open("947_200x200.jpg"))
                    .compress(Bitmap.CompressFormat.JPEG, 100, it)
                Picture(
                    data = it.toByteArray(),
                    description = "Back Cover",
                    pictureType = "Back Cover",
                    mimeType = "image/jpeg",
                )
            }

            TagLib.savePictures(fd.dup().detachFd(), arrayOf(newPicture1, newPicture2))
            val newPictures = TagLib.getPictures(fd.dup().detachFd())
            Assert.assertEquals(2, newPictures.size)
            Assert.assertEquals(newPicture1, newPictures[0])
            Assert.assertEquals(newPicture2, newPictures[1])
        }
    }

    private fun read_flac_multiple_pictures() {
        getFdFromAssets(context, "multiple_album_art.flac").use { fd ->
            val pictures = TagLib.getPictures(fd.dup().detachFd())
            Assert.assertEquals(3, pictures.size)
            Assert.assertEquals(29766, pictures[2].data.size)
        }
    }

    private fun ensure_utf8() {
        getFdFromAssets(context, "bladeenc.mp3").use { fd ->

            // Read metadata

            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals("Test", metadata.propertyMap["TITLE"]!!.single())

            val pictures = TagLib.getPictures(fd.dup().detachFd())
            Assert.assertEquals(0, pictures.size)

            // Save metadata

            val newTitleKey = "TITLE ú"
            val newTitles = arrayOf("Test ú", "Test")
            val newPropertyMap =
                metadata.propertyMap.apply {
                    this[newTitleKey] = newTitles
                }
            val saved = TagLib.savePropertyMap(fd.dup().detachFd(), newPropertyMap)
            Assert.assertTrue(saved)

            val newMetadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertArrayEquals(newTitles, newMetadata.propertyMap[newTitleKey])
        }
    }

    private fun bad_encoding() {
        getFdFromAssets(context, "Honor.mp3").use { fd ->
            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals(
                "荣耀".toByteArray(Charset.forName("GB2312")).toString(Charsets.ISO_8859_1),
                metadata.propertyMap["TITLE"]!!.single()
            )
        }
    }

    private fun getFdFromAssets(context: Context, fileName: String): ParcelFileDescriptor {
        val file = getFileFromAssets(context, fileName)
        return ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_WRITE)
    }

    private fun getFileFromAssets(context: Context, fileName: String): File {
        return File(context.cacheDir, fileName).apply {
            outputStream().use { cache ->
                context.assets.open(fileName).use { inputStream ->
                    inputStream.copyTo(cache)
                }
            }
        }
    }

    fun testConstants() {
        Assert.assertEquals("ALBUM", Constants.ALBUM)
        Assert.assertEquals("BPM", Constants.BPM)
        Assert.assertEquals("COMPOSER", Constants.COMPOSER)
        Assert.assertEquals("GENRE", Constants.GENRE)
        Assert.assertEquals("COPYRIGHT", Constants.COPYRIGHT)
        Assert.assertEquals("ENCODINGTIME", Constants.ENCODINGTIME)
        Assert.assertEquals("PLAYLISTDELAY", Constants.PLAYLISTDELAY)
        Assert.assertEquals("ORIGINALDATE", Constants.ORIGINALDATE)
        Assert.assertEquals("DATE", Constants.DATE)
        Assert.assertEquals("RELEASEDATE", Constants.RELEASEDATE)
        Assert.assertEquals("TAGGINGDATE", Constants.TAGGINGDATE)
        Assert.assertEquals("ENCODEDBY", Constants.ENCODEDBY)
        Assert.assertEquals("LYRICIST", Constants.LYRICIST)
        Assert.assertEquals("FILETYPE", Constants.FILETYPE)
        Assert.assertEquals("WORK", Constants.WORK)
        Assert.assertEquals("Work", Constants.ITUNES_WORK)
        Assert.assertEquals("TITLE", Constants.TITLE)
        Assert.assertEquals("SUBTITLE", Constants.SUBTITLE)
        Assert.assertEquals("INITIALKEY", Constants.INITIALKEY)
        Assert.assertEquals("LANGUAGE", Constants.LANGUAGE)
        Assert.assertEquals("LENGTH", Constants.LENGTH)
        Assert.assertEquals("MEDIA", Constants.MEDIA)
        Assert.assertEquals("MOOD", Constants.MOOD)
        Assert.assertEquals("ORIGINALALBUM", Constants.ORIGINALALBUM)
        Assert.assertEquals("ORIGINALFILENAME", Constants.ORIGINALFILENAME)
        Assert.assertEquals("ORIGINALLYRICIST", Constants.ORIGINALLYRICIST)
        Assert.assertEquals("ORIGINALARTIST", Constants.ORIGINALARTIST)
        Assert.assertEquals("OWNER", Constants.OWNER)
        Assert.assertEquals("ARTIST", Constants.ARTIST)
        Assert.assertEquals("ALBUMARTIST", Constants.ALBUMARTIST)
        Assert.assertEquals("PERFORMER", Constants.PERFORMER)
        Assert.assertEquals("CONDUCTOR", Constants.CONDUCTOR)
        Assert.assertEquals("REMIXER", Constants.REMIXER)
        Assert.assertEquals("ARRANGER", Constants.ARRANGER)
        Assert.assertEquals("DISCNUMBER", Constants.DISCNUMBER)
        Assert.assertEquals("PRODUCEDNOTICE", Constants.PRODUCEDNOTICE)
        Assert.assertEquals("LABEL", Constants.LABEL)
        Assert.assertEquals("TRACKNUMBER", Constants.TRACKNUMBER)
        Assert.assertEquals("RADIOSTATION", Constants.RADIOSTATION)
        Assert.assertEquals("RADIOSTATIONOWNER", Constants.RADIOSTATIONOWNER)
        Assert.assertEquals("ALBUMSORT", Constants.ALBUMSORT)
        Assert.assertEquals("COMPOSERSORT", Constants.COMPOSERSORT)
        Assert.assertEquals("ARTISTSORT", Constants.ARTISTSORT)
        Assert.assertEquals("TITLESORT", Constants.TITLESORT)
        Assert.assertEquals("ALBUMARTISTSORT", Constants.ALBUMARTISTSORT)
        Assert.assertEquals("ISRC", Constants.ISRC)
        Assert.assertEquals("ENCODING", Constants.ENCODING)
        Assert.assertEquals("DISCSUBTITLE", Constants.DISCSUBTITLE)
        Assert.assertEquals("COPYRIGHTURL", Constants.COPYRIGHTURL)
        Assert.assertEquals("FILEWEBPAGE", Constants.FILEWEBPAGE)
        Assert.assertEquals("ARTISTWEBPAGE", Constants.ARTISTWEBPAGE)
        Assert.assertEquals("AUDIOSOURCEWEBPAGE", Constants.AUDIOSOURCEWEBPAGE)
        Assert.assertEquals("RADIOSTATIONWEBPAGE", Constants.RADIOSTATIONWEBPAGE)
        Assert.assertEquals("PAYMENTWEBPAGE", Constants.PAYMENTWEBPAGE)
        Assert.assertEquals("PUBLISHERWEBPAGE", Constants.PUBLISHERWEBPAGE)
        Assert.assertEquals("COMMENT", Constants.COMMENT)
        Assert.assertEquals("PODCAST", Constants.PODCAST)
        Assert.assertEquals("PODCASTCATEGORY", Constants.PODCASTCATEGORY)
        Assert.assertEquals("PODCASTDESC", Constants.PODCASTDESC)
        Assert.assertEquals("PODCASTID", Constants.PODCASTID)
        Assert.assertEquals("PODCASTURL", Constants.PODCASTURL)
        Assert.assertEquals("MOVEMENTNAME", Constants.MOVEMENTNAME)
        Assert.assertEquals("MOVEMENTNUMBER", Constants.MOVEMENTNUMBER)
        Assert.assertEquals("GROUPING", Constants.GROUPING)
        Assert.assertEquals("COMPILATION", Constants.COMPILATION)
    }
}