# TagLib

An Android library which can read and write tags of audio files, using [TagLib](https://github.com/taglib/taglib).

## Add to project

Download the [aar](https://github.com/Kyant0/taglib/releases) and copy it to your module's `libs` directory.

```kotlin
implementation(files("libs/taglib-release.aar"))
```

## Functions

* Get and save audio properties and all metadata of audio files.
* Get and save cover art of audio files, support multiple cover arts.

## Example

See [Tests.kt](/src/androidTest/kotlin/Tests.kt).
