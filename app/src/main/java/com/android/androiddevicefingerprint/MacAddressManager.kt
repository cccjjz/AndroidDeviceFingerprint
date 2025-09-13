package com.android.androiddevicefingerprint

import android.content.Context
import android.net.wifi.WifiManager
import android.net.wifi.WifiInfo
import android.util.Log
import java.net.NetworkInterface
import java.util.Collections

/**
 * MAC地址管理器
 * 负责通过多种方法获取MAC地址
 */
class MacAddressManager(private val context: Context) {

    /**
     * Method 1: Get MAC address using WifiManager
     */
    fun getMacAddressMethod1(): String? {
        return try {
            val wifiManager = context.getSystemService(Context.WIFI_SERVICE) as WifiManager
            val wifiInfo: WifiInfo = wifiManager.connectionInfo
            val macAddress = wifiInfo.macAddress
            if (macAddress != null && macAddress != "02:00:00:00:00:00") {
                macAddress
            } else {
                "Unable to retrieve (WifiManager returned null or default)"
            }
        } catch (e: Exception) {
            Log.e("MacAddressManager", "Error getting MAC address via WifiManager", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * Method 2: Get MAC address using NetworkInterface
     */
    fun getMacAddressMethod2(): String? {
        return try {
            val networkInterface = NetworkInterface.getByName("wlan0")
            if (networkInterface != null) {
                val macBytes = networkInterface.hardwareAddress
                if (macBytes != null) {
                    val macAddress = macBytes.joinToString(":") { "%02x".format(it) }
                    macAddress
                } else {
                    "Unable to retrieve (hardware address is null)"
                }
            } else {
                "Unable to retrieve (wlan0 interface not found)"
            }
        } catch (e: Exception) {
            Log.e("MacAddressManager", "Error getting MAC address via NetworkInterface", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * Method 3: Get MAC addresses from all network interfaces
     */
    fun getMacAddressMethod3(): String? {
        return try {
            val interfaces = Collections.list(NetworkInterface.getNetworkInterfaces())
            val macAddresses = mutableListOf<String>()
            
            for (networkInterface in interfaces) {
                val name = networkInterface.name
                val macBytes = networkInterface.hardwareAddress
                
                if (macBytes != null && macBytes.size == 6) {
                    val macAddress = macBytes.joinToString(":") { "%02x".format(it) }
                    macAddresses.add("$name: $macAddress")
                }
            }
            
            if (macAddresses.isNotEmpty()) {
                macAddresses.joinToString("\n")
            } else {
                "Unable to retrieve (no network interfaces with MAC addresses found)"
            }
        } catch (e: Exception) {
            Log.e("MacAddressManager", "Error getting MAC addresses from all interfaces", e)
            "Unable to retrieve: ${e.message}"
        }
    }

    /**
     * Compare MAC addresses obtained by different methods
     */
    fun compareMacAddresses(macAddresses: List<String?>): String {
        val validMacs = macAddresses.filterNotNull().filter { 
            it.isNotEmpty() && !it.startsWith("Unable to retrieve")
        }
        
        if (validMacs.isEmpty()) {
            return "All methods failed to retrieve MAC address"
        }
        
        if (validMacs.size == 1) {
            return "Only one method succeeded, unable to compare"
        }
        
        val uniqueMacs = validMacs.distinct()
        return if (uniqueMacs.size == 1) {
            "✅ All methods retrieved consistent MAC address"
        } else {
            "⚠️ Different MAC addresses detected!\nDifferent values:\n${uniqueMacs.joinToString("\n")}"
        }
    }

    /**
     * 获取所有MAC地址方法的结果
     */
    fun getAllMacAddresses(): List<String?> {
        return listOf(
            getMacAddressMethod1(),
            getMacAddressMethod2(),
            getMacAddressMethod3()
        )
    }
}
