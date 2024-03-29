import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.ParcelFileDescriptor
import androidx.test.platform.app.InstrumentationRegistry
import com.kyant.taglib.Picture
import com.kyant.taglib.TagLib
import org.junit.Assert
import org.junit.Test
import java.io.ByteArrayOutputStream
import java.io.File

class Tests {
    private val context: Context = InstrumentationRegistry.getInstrumentation().targetContext

    @Test
    fun test_all() {
        read_and_write_m4a()
        read_and_write_pictures_flac()
        read_flac_multiple_pictures()
        ensure_utf8()
    }

    private fun read_and_write_m4a() {
        getFdFromAssets(context, "Sample_BeeMoved_48kHz16bit.m4a").use { fd ->

            // Read metadata

            val metadata = TagLib.getMetadata(fd.dup().detachFd(), withLyrics = true)!!
            Assert.assertEquals(39936, metadata.audioProperties.length)
            Assert.assertEquals("Bee Moved", metadata.propertyMap["TITLE"]!!.single())

            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(58336, pictures.single().data.size)

            val lyrics = TagLib.getLyrics(fd.dup().detachFd())
            Assert.assertNull(lyrics)

            // Save metadata

            val newTitle = "Bee Moved (Remix)"
            val newPropertyMap =
                metadata.propertyMap.apply {
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

            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            val originalPicture = pictures.single()
            Assert.assertEquals(42716, originalPicture.data.size)

            // Save original pictures

            val saved = TagLib.savePictures(fd.dup().detachFd(), arrayOf(originalPicture))
            Assert.assertTrue(saved)
            Assert.assertEquals(originalPicture, TagLib.getPictures(fd.dup().detachFd())!!.single())

            // Save new pictures

            val outputStream1 = ByteArrayOutputStream()
            BitmapFactory
                .decodeStream(context.assets.open("699_200x200.jpg"))
                .compress(Bitmap.CompressFormat.JPEG, 100, outputStream1)
            Assert.assertEquals(22266, outputStream1.size())
            val newPicture1 =
                Picture(
                    data = outputStream1.toByteArray(),
                    description = "Front Cover",
                    pictureType = "Front Cover",
                    mimeType = "image/jpeg",
                )

            val outputStream2 = ByteArrayOutputStream()
            BitmapFactory
                .decodeStream(context.assets.open("947_200x200.jpg"))
                .compress(Bitmap.CompressFormat.JPEG, 100, outputStream2)
            Assert.assertEquals(15297, outputStream2.size())
            val newPicture2 =
                Picture(
                    data = outputStream2.toByteArray(),
                    description = "Back Cover",
                    pictureType = "Back Cover",
                    mimeType = "image/jpeg",
                )

            TagLib.savePictures(fd.dup().detachFd(), arrayOf(newPicture1, newPicture2))
            val newPictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(2, newPictures.size)
            Assert.assertEquals(newPicture1, newPictures[0])
            Assert.assertEquals(newPicture2, newPictures[1])
        }
    }

    private fun read_flac_multiple_pictures() {
        getFdFromAssets(context, "multiple_album_art.flac").use { fd ->
            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(3, pictures.size)
            Assert.assertEquals(29766, pictures[2].data.size)
        }
    }

    private fun ensure_utf8() {
        getFdFromAssets(context, "bladeenc.mp3").use { fd ->

            // Read metadata

            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals("Test", metadata.propertyMap["TITLE"]!!.single())

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

    private fun getFdFromAssets(
        context: Context,
        fileName: String,
    ): ParcelFileDescriptor {
        val file = getFileFromAssets(context, fileName)
        return ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_WRITE)
    }

    private fun getFileFromAssets(
        context: Context,
        fileName: String,
    ): File {
        return File(context.cacheDir, fileName).also {
            it.outputStream().use { cache ->
                context.assets.open(fileName).use { inputStream ->
                    inputStream.copyTo(cache)
                }
            }
        }
    }
}
