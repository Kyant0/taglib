import android.content.Context
import android.os.ParcelFileDescriptor
import androidx.test.platform.app.InstrumentationRegistry
import com.kyant.taglib.TagLib
import org.junit.Assert
import org.junit.Test
import java.io.File

class Tests {
    @Test
    fun test() {
        val context = InstrumentationRegistry.getInstrumentation().targetContext

        // Asset is from https://helpguide.sony.net/high-res/sample1/v1/en/index.html
        getFdFromAssets(context, "Sample_BeeMoved_48kHz16bit.m4a").use { fd ->

            // Read metadata

            val metadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals(39936, metadata.audioProperties.length)
            Assert.assertEquals("Bee Moved", metadata.propertyMap["TITLE"]!![0])

            val pictures = TagLib.getPictures(fd.dup().detachFd())!!
            Assert.assertEquals(58336, pictures[0].data.size)

            val lyrics = TagLib.getLyrics(fd.dup().detachFd())
            Assert.assertNull(lyrics)

            // Save metadata

            val newPropertyMap = metadata.propertyMap.toMutableMap().apply {
                this["TITLE"] = arrayOf("Bee Moved (Remix)")
            }.toMap()
            val saved = TagLib.savePropertyMap(fd.dup().detachFd(), newPropertyMap)
            Assert.assertTrue(saved)

            val newMetadata = TagLib.getMetadata(fd.dup().detachFd())!!
            Assert.assertEquals("Bee Moved (Remix)", newMetadata.propertyMap["TITLE"]!![0])
        }
    }

    private fun getFdFromAssets(context: Context, fileName: String): ParcelFileDescriptor {
        val file = getFileFromAssets(context, fileName)
        return ParcelFileDescriptor.open(file, ParcelFileDescriptor.MODE_READ_WRITE)
    }

    private fun getFileFromAssets(context: Context, fileName: String): File = File(context.cacheDir, fileName).also {
        it.outputStream().use { cache ->
            context.assets.open(fileName).use { inputStream ->
                inputStream.copyTo(cache)
            }
        }
    }
}
