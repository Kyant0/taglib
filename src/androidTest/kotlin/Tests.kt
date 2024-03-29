import android.content.Context
import android.os.ParcelFileDescriptor
import androidx.test.platform.app.InstrumentationRegistry
import com.kyant.taglib.TagLib
import org.junit.Assert
import org.junit.Test
import java.io.File

class Tests {
    private val context: Context = InstrumentationRegistry.getInstrumentation().targetContext

    @Test
    fun test_all() {
        read_and_write_m4a()
        read_flac_multiple_pictures()
        ensure_utf8()
    }

    private fun read_and_write_m4a() {
        // Asset is from https://helpguide.sony.net/high-res/sample1/v1/en/index.html
        getFdFromAssets(context, "Sample_BeeMoved_48kHz16bit.m4a").use { fd ->

            // Read metadata

            val metadata = TagLib.getMetadata(fd.dup().detachFd(), withLyrics = true)!!
            Assert.assertEquals(39936, metadata.audioProperties.length)
            Assert.assertEquals("Bee Moved", metadata.propertyMap["TITLE"]!![0])

            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(58336, pictures[0].data.size)

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
            Assert.assertEquals(newTitle, newMetadata.propertyMap["TITLE"]!![0])
        }
    }

    private fun read_flac_multiple_pictures() {
        getFdFromAssets(context, "是什么让我遇见这样的你 - 白安.flac").use { fd ->
            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals("是什么让我遇见这样的你", metadata.propertyMap["TITLE"]!![0])

            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(42716, pictures[0].data.size)
        }

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
            Assert.assertEquals("Test", metadata.propertyMap["TITLE"]!![0])

            // Save metadata

            val newTitle = "Test ú"
            val newPropertyMap =
                metadata.propertyMap.apply {
                    this["TITLE"] = arrayOf(newTitle)
                }
            val saved = TagLib.savePropertyMap(fd.dup().detachFd(), newPropertyMap)
            Assert.assertTrue(saved)

            val newMetadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals(newTitle, newMetadata.propertyMap["TITLE"]!![0])
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
