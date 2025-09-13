package com.android.androiddevicefingerprint

/**
 * Device fingerprint data model
 */
data class DeviceFingerprint(
    val name: String,        // Fingerprint name
    val value: String,       // Fingerprint value
    val description: String  // Description information
)
