package com.android.androiddevicefingerprint

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.recyclerview.widget.LinearLayoutManager
import com.android.androiddevicefingerprint.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var adapter: DeviceFingerprintAdapter
    
    // Manager instances
    private lateinit var deviceInfoManager: DeviceInfoManager
    private lateinit var androidIdManager: AndroidIdManager
    private lateinit var macAddressManager: MacAddressManager

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Initialize managers
        deviceInfoManager = DeviceInfoManager(this)
        androidIdManager = AndroidIdManager(this)
        macAddressManager = MacAddressManager(this)

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
        
        // Device Information
        fingerprints.addAll(loadDeviceInfo())
        
        // Android ID Information
        fingerprints.addAll(loadAndroidIdInfo())
        
        // MAC Address Information
        fingerprints.addAll(loadMacAddressInfo())
        
        // DRM ID Information
        val drmId = getDrmIdNative()
        fingerprints.add(
            DeviceFingerprint(
                name = "DRM ID (Widevine)",
                value = drmId,
                description = "DRM device unique ID using Widevine (Base64 encoded)"
            )
        )

        // System Files Information (Important Device Fingerprints)
        val systemFilesInfo = getSystemFilesInfoNative()
        fingerprints.add(
            DeviceFingerprint(
                name = "System Files Info",
                value = systemFilesInfo,
                description = "Boot ID, UUID, CID and other critical device fingerprints"
            )
        )

        // Kernel Files Information
        val kernelFilesInfo = getKernelFilesInfoNative()
        fingerprints.add(
            DeviceFingerprint(
                name = "Kernel Files Info",
                value = kernelFilesInfo,
                description = "Build.prop and system files information using custom file reader"
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

        adapter.updateFingerprints(fingerprints)
    }

    private fun loadDeviceInfo(): List<DeviceFingerprint> {
        return listOf(
            DeviceFingerprint(
                name = "Serial Number",
                value = deviceInfoManager.getSerialNumber(),
                description = "Device serial number"
            ),
            DeviceFingerprint(
                name = "IMEI",
                value = deviceInfoManager.getImei(),
                description = "International Mobile Equipment Identity"
            ),
            DeviceFingerprint(
                name = "IMSI",
                value = deviceInfoManager.getImsi(),
                description = "International Mobile Subscriber Identity"
            ),
            DeviceFingerprint(
                name = "ICCID",
                value = deviceInfoManager.getIccid(),
                description = "Integrated Circuit Card Identifier"
            ),
            DeviceFingerprint(
                name = "Line1Number",
                value = deviceInfoManager.getLine1Number(),
                description = "Phone number of line 1"
            ),
            DeviceFingerprint(
                name = "Device Model",
                value = deviceInfoManager.getDeviceModel(),
                description = "Device manufacturer and model"
            ),
            DeviceFingerprint(
                name = "Android Version",
                value = deviceInfoManager.getAndroidVersion(),
                description = "Android version and API level"
            ),
            DeviceFingerprint(
                name = "Carrier Info",
                value = deviceInfoManager.getCarrierInfo(),
                description = "Network carrier information"
            )
        )
    }

    private fun loadAndroidIdInfo(): List<DeviceFingerprint> {
        val androidIds = androidIdManager.getAllAndroidIds()
        
        val fingerprints = mutableListOf<DeviceFingerprint>()
        
        // Add individual Android ID methods
        val methodNames = listOf(
            "Settings.Secure.getString() basic method",
            "Get Settings cache through reflection",
            "ContentResolver.call() method",
            "Query through content command"
        )
        
        androidIds.forEachIndexed { index, androidId ->
            fingerprints.add(
                DeviceFingerprint(
                    name = "Android ID (Method ${index + 1})",
                    value = androidId ?: "Unable to retrieve",
                    description = methodNames[index]
                )
            )
        }
        
        // Add comparison result
        val comparisonResult = androidIdManager.compareAndroidIds(androidIds)
        fingerprints.add(
            DeviceFingerprint(
                name = "Android ID Tamper Detection",
                value = comparisonResult,
                description = "Comparison result of four methods, different values may indicate tampering"
            )
        )
        
        return fingerprints
    }

    private fun loadMacAddressInfo(): List<DeviceFingerprint> {
        val macAddresses = macAddressManager.getAllMacAddresses()
        
        val fingerprints = mutableListOf<DeviceFingerprint>()
        
        // Add individual MAC address methods
        val methodNames = listOf(
            "Get MAC address using WifiManager",
            "Get MAC address using NetworkInterface",
            "Get MAC addresses from all network interfaces"
        )
        
        macAddresses.forEachIndexed { index, macAddress ->
            fingerprints.add(
                DeviceFingerprint(
                    name = "MAC Address (Method ${index + 1})",
                    value = macAddress ?: "Unable to retrieve",
                    description = methodNames[index]
                )
            )
        }
        
        // Add comparison result
        val comparisonResult = macAddressManager.compareMacAddresses(macAddresses)
        fingerprints.add(
            DeviceFingerprint(
                name = "MAC Address Comparison",
                value = comparisonResult,
                description = "Comparison of different MAC address retrieval methods"
            )
        )
        
        return fingerprints
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

    /**
     * Native method to get DRM ID
     */
    external fun getDrmIdNative(): String

    /**
     * Native method to get kernel files information
     */
    external fun getKernelFilesInfoNative(): String

    /**
     * Native method to get system files information
     */
    external fun getSystemFilesInfoNative(): String

    companion object {
        // Used to load the 'androiddevicefingerprint' library on application startup.
        init {
            System.loadLibrary("androiddevicefingerprint")
        }
    }
}