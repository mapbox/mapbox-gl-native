package com.mapbox.mapboxsdk.testapp.utils

import android.content.Context
import android.os.AsyncTask
import java.io.File
import java.io.FileOutputStream
import java.lang.ref.WeakReference

class FileUtils {

  /**
   * Task that copies a file from the assets directory to a provided directory.
   * The asset's name is going to be kept in the new directory.
   */
  class CopyFileFromAssetsTask(context: Context, listener: OnFileCopiedFromAssetsListener) : AsyncTask<String, Void, Boolean>() {
    private val contextWeakReference: WeakReference<Context> = WeakReference(context)
    private val listenerWeakReference: WeakReference<OnFileCopiedFromAssetsListener> = WeakReference(listener)

    override fun doInBackground(vararg strings: String): Boolean? {
      val assetName = strings[0]
      val destinationPath = strings[1]
      contextWeakReference.get()?.let {
        try {
          copyAsset(it, assetName, destinationPath)
        } catch (ex: Exception) {
          return false
        }
      }

      return true
    }

    override fun onCancelled() {
      listenerWeakReference.get()?.onError()
    }

    override fun onPostExecute(result: Boolean) {
      if (result) {
        listenerWeakReference.get()?.onFileCopiedFromAssets()
      } else {
        listenerWeakReference.get()?.onError()
      }
    }

    private fun copyAsset(context: Context, assetName: String, destinationPath: String) {
      val bufferSize = 1024
      val assetManager = context.assets
      val inputStream = assetManager.open(assetName)
      val outputStream = FileOutputStream(File(destinationPath, assetName))
      try {
        inputStream.copyTo(outputStream, bufferSize)
      } finally {
        inputStream.close()
        outputStream.flush()
        outputStream.close()
      }
    }
  }

  interface OnFileCopiedFromAssetsListener {
    fun onFileCopiedFromAssets()
    fun onError()
  }
}