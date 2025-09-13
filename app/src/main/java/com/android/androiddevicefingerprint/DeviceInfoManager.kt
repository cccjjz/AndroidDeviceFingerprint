package com.android.androiddevicefingerprint

import android.content.Context
import android.os.Build
import android.telephony.TelephonyManager
import android.util.Log
import android.Manifest
import androidx.core.content.ContextCompat
import android.content.pm.PackageManager

/**
 * 设备信息管理器
 * 负责获取各种设备标识信息
 */
class DeviceInfoManager(private val context: Context) {

    private val telephonyManager: TelephonyManager by lazy {
        context.getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager
    }

    /**
     * 获取设备序列号
     */
    fun getSerialNumber(): String {
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                Build.getSerial()
            } else {
                @Suppress("DEPRECATION")
                Build.SERIAL
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting serial number", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取IMEI（需要READ_PHONE_STATE权限）
     */
    fun getImei(): String {
        return try {
            if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_PHONE_STATE) 
                != PackageManager.PERMISSION_GRANTED) {
                "Permission denied (READ_PHONE_STATE required)"
            } else {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    telephonyManager.imei ?: "Unable to retrieve (IMEI is null)"
                } else {
                    @Suppress("DEPRECATION")
                    telephonyManager.deviceId ?: "Unable to retrieve (Device ID is null)"
                }
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting IMEI", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取IMSI（需要READ_PHONE_STATE权限）
     */
    fun getImsi(): String {
        return try {
            if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_PHONE_STATE) 
                != PackageManager.PERMISSION_GRANTED) {
                "Permission denied (READ_PHONE_STATE required)"
            } else {
                telephonyManager.subscriberId ?: "Unable to retrieve (IMSI is null)"
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting IMSI", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取ICCID（需要READ_PHONE_STATE权限）
     */
    fun getIccid(): String {
        return try {
            if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_PHONE_STATE) 
                != PackageManager.PERMISSION_GRANTED) {
                "Permission denied (READ_PHONE_STATE required)"
            } else {
                telephonyManager.simSerialNumber ?: "Unable to retrieve (ICCID is null)"
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting ICCID", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取Line1Number（需要READ_PHONE_STATE权限）
     */
    fun getLine1Number(): String {
        return try {
            if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_PHONE_STATE) 
                != PackageManager.PERMISSION_GRANTED) {
                "Permission denied (READ_PHONE_STATE required)"
            } else {
                telephonyManager.line1Number ?: "Unable to retrieve (Line1Number is null)"
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting Line1Number", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取设备型号
     */
    fun getDeviceModel(): String {
        return try {
            "${Build.MANUFACTURER} ${Build.MODEL}"
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting device model", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取Android版本
     */
    fun getAndroidVersion(): String {
        return try {
            "Android ${Build.VERSION.RELEASE} (API ${Build.VERSION.SDK_INT})"
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting Android version", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * 获取运营商信息
     */
    fun getCarrierInfo(): String {
        return try {
            if (ContextCompat.checkSelfPermission(context, Manifest.permission.READ_PHONE_STATE) 
                != PackageManager.PERMISSION_GRANTED) {
                "Permission denied (READ_PHONE_STATE required)"
            } else {
                val carrierName = telephonyManager.networkOperatorName
                val networkOperator = telephonyManager.networkOperator
                if (carrierName.isNullOrEmpty()) {
                    "Unable to retrieve carrier info"
                } else {
                    "$carrierName ($networkOperator)"
                }
            }
        } catch (e: Exception) {
            Log.e("DeviceInfoManager", "Error getting carrier info", e)
            "Unable to retrieve: ${e.message}"
        }
    }
}
