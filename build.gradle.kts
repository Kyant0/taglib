import org.jetbrains.kotlin.gradle.dsl.JvmTarget
import org.jetbrains.kotlin.gradle.dsl.KotlinVersion

plugins {
    alias(libs.plugins.android.library)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.kyant.taglib"
    compileSdk = 36
    buildToolsVersion = "36.0.0"
    ndkVersion = "29.0.13599879"

    defaultConfig {
        minSdk = 23
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
            version = "4.0.2"
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }
    kotlin {
        compilerOptions {
            apiVersion = KotlinVersion.KOTLIN_2_3
            languageVersion = KotlinVersion.KOTLIN_2_3
            jvmTarget = JvmTarget.JVM_21
        }
    }
    lint {
        checkReleaseBuilds = false
    }
}

kotlin {
    explicitApi()
}

dependencies {
    androidTestImplementation(libs.androidx.runner)
    androidTestImplementation(libs.androidx.rules)
}
