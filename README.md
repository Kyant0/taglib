# TagLib for Android

An Android native library for reading and writing metadata tags in audio files — powered by TagLib.

# Add to Project

1. Download the prebuilt .aar from the build/outputs/aar/ directory after building the project manually.


2. Place the `.aar` file in your app module's `libs/ directory (e.g., yourproject/app/libs/)`


3. Then add the dependency in your build.gradle file based on your build script:


```Groovy DSL
implementation files("libs/taglib-release.aar")
```


```Kotlin DSL 
implementation(files("libs/taglib-release.aar"))
```



4. Make sure to declare the flatDir repository in your build.gradle:


```Groovy DSL
repositories { flatDir { dirs 'libs' } }
```


```Kotlin DSL
repositories { flatDir { dirs("libs") } }
```


# Without DSL (direct manual import in IDE):

• Open your module settings

 • Click "+" → "Import .JAR/.AAR Package"

 • Select the .aar from libs/

 • Confirm and sync project




# Features

Read & write audio metadata (title, artist, album, year, etc.)

Get & save cover art (supports multiple embedded artworks)

Read audio file properties (bitrate, duration, sample rate, etc.)


 # Example Usage

See `Tests.kt` for usage examples.

# Build from Source

To build the library manually:

1. Clone the repository


2. Make sure you have `Android NDK (tested with r29+)` and `CMake (4.0.2)` installed


3. Run the following Gradle command:

```Bash
./gradlew :library:assembleRelease
```

 or
 
```Bash
./gradlew assembleRelease
```



The resulting AAR will be located at: `.../build/outputs/aar/taglib-release.aar`

You can now include this AAR in your app's libs/ directory as shown above.

# Based on

TagLib `(https://taglib.org/)` — C++ library for audio metadata. This version is wrapped for Android with CMake + NDK + JNI. `Forked and modified from kyant0/taglib (Apache License 2.0).`

# Note:

This repository is a fork of `kyant0/taglib`, originally licensed under `Apache License 2.0`.

This fork `maintains` the Android integration from the original project, while providing additional build configurations, usage documentation, and packaged artifacts (.aar) for easier adoption in Android projects.
