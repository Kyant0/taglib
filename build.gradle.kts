plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
    `maven-publish`
}

group = "com.kyant"
version = libs.versions.lib.version.get()

android {
    namespace = "com.kyant.taglib"
    compileSdk = libs.versions.android.compileSdk.get().toInt()
    buildToolsVersion = libs.versions.android.buildToolsVersion.get()
    ndkVersion = libs.versions.android.ndkVersion.get()

    defaultConfig {
        minSdk = libs.versions.android.minSdk.get().toInt()
        consumerProguardFiles("consumer-rules.pro")
        ndk {
            abiFilters += arrayOf("arm64-v8a", "armeabi-v7a", "x86_64", "x86")
        }

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    externalNativeBuild {
        cmake {
            path("src/main/cpp/CMakeLists.txt")
            version = "3.31.1"
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }
    kotlin {
        jvmToolchain(21)
        explicitApi()
    }
    lint {
        checkReleaseBuilds = false
    }
}

dependencies {
    androidTestImplementation(libs.androidx.runner)
    androidTestImplementation(libs.androidx.rules)
}

afterEvaluate {
    publishing {
        publications {
            register("mavenRelease", MavenPublication::class) {
                groupId = "com.kyant"
                artifactId = "taglib"
                version = libs.versions.lib.version.get()
                from(components["release"])
            }
        }
    }
}
