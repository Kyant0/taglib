# TagLib

An Android library which can read and write tags of audio files, using [TagLib](https://github.com/taglib/taglib).

## Add to project

[![JitPack Release](https://jitpack.io/v/Kyant0/taglib.svg)](https://jitpack.io/#Kyant0/taglib)

```kotlin
// build.gradle.kts
allprojects {
    repositories {
        maven("https://jitpack.io")
    }
}

// app/build.gradle.kts
implementation("com.github.Kyant0:taglib:<version>")
```

## Example

See [Tests.kt](/src/androidTest/kotlin/Tests.kt).

## TODO

- [ ] Support writing complex properties
