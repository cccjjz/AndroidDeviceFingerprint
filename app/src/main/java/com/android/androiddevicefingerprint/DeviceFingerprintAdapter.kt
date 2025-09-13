package com.android.androiddevicefingerprint

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.android.androiddevicefingerprint.databinding.ItemDeviceFingerprintBinding

/**
 * Device fingerprint RecyclerView adapter
 */
class DeviceFingerprintAdapter(
    private var fingerprints: List<DeviceFingerprint> = emptyList()
) : RecyclerView.Adapter<DeviceFingerprintAdapter.FingerprintViewHolder>() {

    class FingerprintViewHolder(private val binding: ItemDeviceFingerprintBinding) : 
        RecyclerView.ViewHolder(binding.root) {
        
        fun bind(fingerprint: DeviceFingerprint) {
            binding.tvFingerprintName.text = fingerprint.name
            binding.tvFingerprintValue.text = fingerprint.value
            binding.tvFingerprintDescription.text = fingerprint.description
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FingerprintViewHolder {
        val binding = ItemDeviceFingerprintBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return FingerprintViewHolder(binding)
    }

    override fun onBindViewHolder(holder: FingerprintViewHolder, position: Int) {
        holder.bind(fingerprints[position])
    }

    override fun getItemCount(): Int = fingerprints.size

    fun updateFingerprints(newFingerprints: List<DeviceFingerprint>) {
        fingerprints = newFingerprints
        notifyDataSetChanged()
    }
}
