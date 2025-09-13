package com.android.androiddevicefingerprint

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.Settings
import android.net.Uri
import androidx.recyclerview.widget.LinearLayoutManager
import com.android.androiddevicefingerprint.databinding.ActivityMainBinding
import java.lang.reflect.Field
import android.util.ArrayMap
import android.os.Bundle as AndroidBundle
import android.util.Log

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var adapter: DeviceFingerprintAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setupRecyclerView()
        loadDeviceFingerprints()
    }

    private fun setupRecyclerView() {
        adapter = DeviceFingerprintAdapter()
        binding.recyclerViewFingerprints.apply {
            layoutManager = LinearLayoutManager(this@MainActivity)
            adapter = this@MainActivity.adapter
        }
    }

    private fun loadDeviceFingerprints() {
        val fingerprints = mutableListOf<DeviceFingerprint>()
        
        // Method 1: Basic Android ID retrieval
        val androidId1 = getAndroidIdMethod1()
        fingerprints.add(
            DeviceFingerprint(
                name = "Android ID (Method 1)",
                value = androidId1 ?: "Unable to retrieve",
                description = "Settings.Secure.getString() basic method"
            )
        )

        // Method 2: Get cache through reflection
        val androidId2 = getAndroidIdMethod2()
        fingerprints.add(
            DeviceFingerprint(
                name = "Android ID (Method 2)",
                value = androidId2 ?: "Unable to retrieve",
                description = "Get Settings cache through reflection"
            )
        )

        // Method 3: Get through ContentResolver.call
        val androidId3 = getAndroidIdMethod3()
        fingerprints.add(
            DeviceFingerprint(
                name = "Android ID (Method 3)",
                value = androidId3 ?: "Unable to retrieve",
                description = "ContentResolver.call() method"
            )
        )

        // Method 4: Query through content command
        val androidId4 = getAndroidIdMethod4()
        fingerprints.add(
            DeviceFingerprint(
                name = "Android ID (Method 4)",
                value = androidId4 ?: "Unable to retrieve",
                description = "Query through content command"
            )
        )

        // File System Information
        val fileSystemInfo = getFileSystemInfoNative()
        fingerprints.add(
            DeviceFingerprint(
                name = "File System Info",
                value = fileSystemInfo,
                description = "File system information using native methods"
            )
        )

        // Comparison result
        val comparisonResult = compareAndroidIds(listOf(androidId1, androidId2, androidId3, androidId4))
        fingerprints.add(
            DeviceFingerprint(
                name = "Tamper Detection Result",
                value = comparisonResult,
                description = "Comparison result of four methods, different values may indicate tampering"
            )
        )

        adapter.updateFingerprints(fingerprints)
    }

    /**
     * Method 1: Basic Android ID retrieval
     */
    private fun getAndroidIdMethod1(): String? {
        return try {
            Settings.Secure.getString(contentResolver, Settings.Secure.ANDROID_ID)
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    /**
     * Method 2: Get Settings cache through reflection
     */
    private fun getAndroidIdMethod2(): String? {
        return try {
            val sNameValueCache: Field = Settings.Secure::class.java.getDeclaredField("sNameValueCache")
            sNameValueCache.isAccessible = true
            val sLockSettings = sNameValueCache.get(null)
            val fieldmValues: Field = sLockSettings.javaClass.getDeclaredField("mValues")
            fieldmValues.isAccessible = true
            @Suppress("UNCHECKED_CAST")
            val mValues = fieldmValues.get(sLockSettings) as ArrayMap<String, String>
            mValues["android_id"]
        } catch (e: Throwable) {
            e.printStackTrace()
            null
        }
    }

    /**
     * Method 3: Get through ContentResolver.call
     */
    private fun getAndroidIdMethod3(): String? {
        return try {
            val callResult = contentResolver.call(
                Uri.parse("content://settings/secure"), 
                "GET_secure", 
                "android_id", 
                AndroidBundle()
            )
            callResult?.getString("value")
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    /**
     * Method 4: Query through content command
     * Note: This method executes shell command to get Android ID
     */
    private fun getAndroidIdMethod4(): String? {
        return try {
            Log.d("MainActivity", "Executing content command...")
            val process = Runtime.getRuntime().exec("content query --uri content://settings/secure --where \"name='android_id'\"")
            
            // Wait for process to complete
            val exitCode = process.waitFor()
            Log.d("MainActivity", "Process exit code: $exitCode")
            
            val inputStream = process.inputStream
            val reader = inputStream.bufferedReader()
            val result = reader.readText()
            reader.close()
            inputStream.close()
            
            Log.d("MainActivity", "Raw result: $result")
            
            // Parse the result to extract android_id value
            val lines = result.split("\n")
            Log.d("MainActivity", "Total lines: ${lines.size}")
            
            for (line in lines) {
                Log.d("MainActivity", "Line: $line")
                if (line.contains("android_id")) {
                    Log.d("MainActivity", "Found android_id line: $line")
                    val parts = line.split(",")
                    for (part in parts) {
                        Log.d("MainActivity", "Part: $part")
                        if (part.contains("value=")) {
                            val value = part.substringAfter("value=").trim()
                            Log.d("MainActivity", "Extracted value: $value")
                            return value
                        }
                    }
                }
            }
            "Unable to parse result"
        } catch (e: Exception) {
            Log.e("MainActivity", "Error in method 4", e)
            e.printStackTrace()
            "Command execution failed: ${e.message}"
        }
    }

    /**
     * Compare Android IDs obtained by four methods
     */
    private fun compareAndroidIds(ids: List<String?>): String {
        val validIds = ids.filterNotNull().filter { it.isNotEmpty() }
        
        if (validIds.isEmpty()) {
            return "All methods failed to retrieve Android ID"
        }
        
        if (validIds.size == 1) {
            return "Only one method succeeded, unable to compare"
        }
        
        val uniqueIds = validIds.distinct()
        return if (uniqueIds.size == 1) {
            "✅ All methods retrieved consistent Android ID, no tampering detected"
        } else {
            "⚠️ Inconsistent Android ID detected, possible tampering!\nDifferent values: ${uniqueIds.joinToString(", ")}"
        }
    }

    /**
     * Get file system information using native methods
     */
    private fun getFileSystemInfo(): String {
        return try {
            getFileSystemInfoNative()
        } catch (e: Exception) {
            Log.e("MainActivity", "Error getting file system info", e)
            "Failed to get file system information: ${e.message}"
        }
    }

    /**
     * A native method that is implemented by the 'androiddevicefingerprint' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    /**
     * Native method to get file system information
     */
    external fun getFileSystemInfoNative(): String

    companion object {
        // Used to load the 'androiddevicefingerprint' library on application startup.
        init {
            System.loadLibrary("androiddevicefingerprint")
        }
    }
}