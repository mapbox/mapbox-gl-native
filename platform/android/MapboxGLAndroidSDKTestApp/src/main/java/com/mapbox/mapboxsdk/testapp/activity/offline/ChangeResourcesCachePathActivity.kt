package com.mapbox.mapboxsdk.testapp.activity.offline

import android.annotation.TargetApi
import android.content.Context
import android.os.Build
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.BaseAdapter
import android.widget.TextView
import android.widget.Toast
import com.mapbox.mapboxsdk.log.Logger
import com.mapbox.mapboxsdk.offline.OfflineManager
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.R
import java.io.File
import kotlinx.android.synthetic.main.activity_change_resources_cache_path.*

class ChangeResourcesCachePathActivity : AppCompatActivity(),
  AdapterView.OnItemClickListener,
  FileSource.ResourcesCachePathChangeCallback {

  companion object {
    private const val TAG = "Mbgl-ChangeResourcesCachePathActivity"
  }

  private lateinit var adapter: PathAdapter

  private lateinit var offlineManager: OfflineManager

  private val callback = PathChangeCallback(this)

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_change_resources_cache_path)

    adapter = PathAdapter(this, obtainExternalFilesPaths())
    listView.adapter = adapter
    listView.emptyView = empty
    listView.onItemClickListener = this

    offlineManager = OfflineManager.getInstance(this)
  }

  override fun onStart() {
    super.onStart()
    val path: String? = FileSource.getResourcesCachePath(this)
    Toast.makeText(this, "Current path: $path", Toast.LENGTH_LONG).show()
  }

  override fun onDestroy() {
    super.onDestroy()
    callback.onActivityDestroy()
  }

  override fun onItemClick(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
    listView.onItemClickListener = null
    val path: String = adapter.getItem(position) as String
    FileSource.setResourcesCachePath(path, callback)
  }

  override fun onError(message: String) {
    listView.onItemClickListener = this
    Toast.makeText(this, "Error: $message", Toast.LENGTH_LONG).show()
  }

  override fun onSuccess(path: String) {
    listView.onItemClickListener = this
    Toast.makeText(this, "New path: $path", Toast.LENGTH_LONG).show()

    offlineManager.listOfflineRegions(object : OfflineManager.ListOfflineRegionsCallback {
      override fun onList(offlineRegions: Array<out OfflineRegion>?) {
        Logger.i(TAG, "Number of saved offline regions in the new path: ${offlineRegions?.size}")
      }

      override fun onError(error: String?) {
        Logger.e(TAG, error)
      }
    })
  }

  private fun Context.obtainExternalFilesPaths(): List<String> {
    val paths = ArrayList<String>()
    paths.add(this.filesDir.absolutePath)
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
      paths.addAll(obtainExternalFilesPathsKitKat())
    } else {
      paths.addAll(obtainExternalFilesPathsLegacy())
    }
    paths.add("${File.separator}invalid${File.separator}cache${File.separator}path")
    return paths
  }

  private fun Context.obtainExternalFilesPathsLegacy(): List<String> {
    val postFix =
      "${File.separator}Android${File.separator}data${File.separator}${this.packageName}${File.separator}files"
    val paths = ArrayList<String>()
    val externalStorage = System.getenv("EXTERNAL_STORAGE")
    val secondaryStorage = System.getenv("SECONDARY_STORAGE")
    if (externalStorage != null) {
      paths.add(externalStorage + postFix)
    }
    if (secondaryStorage != null) {
      val secPaths = secondaryStorage.split(":".toRegex()).dropLastWhile { it.isEmpty() }.toTypedArray()
      for (path in secPaths) {
        paths.add(path + postFix)
      }
    }
    return paths
  }

  @TargetApi(Build.VERSION_CODES.KITKAT)
  private fun Context.obtainExternalFilesPathsKitKat(): List<String> {
    val paths = ArrayList<String>()
    val extDirs = this.getExternalFilesDirs(null)
    for (dir in extDirs) {
      if (dir != null) {
        paths.add(dir.absolutePath)
      }
    }
    return paths
  }

  private class PathChangeCallback(private var activity: ChangeResourcesCachePathActivity?) : FileSource.ResourcesCachePathChangeCallback {

    override fun onSuccess(path: String) {
      activity?.onSuccess(path)
    }

    override fun onError(message: String) {
      activity?.onError(message)
    }

    fun onActivityDestroy() {
      activity = null
    }
  }

  class PathAdapter(private val context: Context, private val paths: List<String>) : BaseAdapter() {

    override fun getItem(position: Int): Any {
      return paths[position]
    }

    override fun getItemId(position: Int): Long {
      return position.toLong()
    }

    override fun getCount(): Int {
      return paths.size
    }

    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
      val viewHolder: ViewHolder
      val view: View

      if (convertView == null) {
        viewHolder = ViewHolder()
        view = LayoutInflater.from(context).inflate(android.R.layout.simple_list_item_1, parent, false)
        viewHolder.textView = view.findViewById(android.R.id.text1)
        view?.tag = viewHolder
      } else {
        view = convertView
        viewHolder = view.tag as ViewHolder
      }

      viewHolder.textView?.text = paths[position]

      return view
    }

    class ViewHolder {
      var textView: TextView? = null
    }
  }
}