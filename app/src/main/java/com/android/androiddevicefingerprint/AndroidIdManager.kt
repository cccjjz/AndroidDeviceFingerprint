package com.android.androiddevicefingerprint

import android.content.Context
import android.provider.Settings
import android.net.Uri
import android.os.Bundle
import android.util.Log
import java.lang.reflect.Field
import android.util.ArrayMap

/**
 * Android ID管理器
 * 负责通过多种方法获取Android ID
 */
class AndroidIdManager(private val context: Context) {

    /**
     * Method 1: Basic Android ID retrieval
     */
    fun getAndroidIdMethod1(): String? {
        return try {
            Settings.Secure.getString(context.contentResolver, Settings.Secure.ANDROID_ID)
        } catch (e: Exception) {
            Log.e("AndroidIdManager", "Error in method 1", e)
            null
        }
    }

    /**
     * Method 2: Get Settings cache through reflection
     */
    fun getAndroidIdMethod2(): String? {
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
            Log.e("AndroidIdManager", "Error in method 2", e)
            null
        }
    }

    /**
     * Method 3: Get through ContentResolver.call
     */
    fun getAndroidIdMethod3(): String? {
        return try {
            val callResult = context.contentResolver.call(
                Uri.parse("content://settings/secure"), 
                "GET_secure", 
                "android_id", 
                Bundle()
            )
            callResult?.getString("value")
        } catch (e: Exception) {
            Log.e("AndroidIdManager", "Error in method 3", e)
            null
        }
    }

    /**
     * Method 4: Query through content command
     * Note: This method executes shell command to get Android ID
     */
    fun getAndroidIdMethod4(): String? {
        return try {
            Log.d("AndroidIdManager", "Executing content command...")
            val process = Runtime.getRuntime().exec("content query --uri content://settings/secure --where \"name='android_id'\"")
            
            // Wait for process to complete
            val exitCode = process.waitFor()
            Log.d("AndroidIdManager", "Process exit code: $exitCode")
            
            val inputStream = process.inputStream
            val reader = inputStream.bufferedReader()
            val result = reader.readText()
            reader.close()
            inputStream.close()
            
            Log.d("AndroidIdManager", "Raw result: $result")
            
            // Parse the result to extract android_id value
            val lines = result.split("\n")
            Log.d("AndroidIdManager", "Total lines: ${lines.size}")
            
            for (line in lines) {
                Log.d("AndroidIdManager", "Line: $line")
                if (line.contains("android_id")) {
                    Log.d("AndroidIdManager", "Found android_id line: $line")
                    val parts = line.split(",")
                    for (part in parts) {
                        Log.d("AndroidIdManager", "Part: $part")
                        if (part.contains("value=")) {
                            val value = part.substringAfter("value=").trim()
                            Log.d("AndroidIdManager", "Extracted value: $value")
                            return value
                        }
                    }
                }
            }
            "Unable to parse result"
        } catch (e: Exception) {
            Log.e("AndroidIdManager", "Error in method 4", e)
            "Command execution failed: ${e.message}"
        }
    }

    /**
     * Compare Android IDs obtained by four methods
     */
    fun compareAndroidIds(ids: List<String?>): String {
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
     * 获取所有Android ID方法的结果
     */
    fun getAllAndroidIds(): List<String?> {
        return listOf(
            getAndroidIdMethod1(),
            getAndroidIdMethod2(),
            getAndroidIdMethod3(),
            getAndroidIdMethod4()
        )
    }
}
