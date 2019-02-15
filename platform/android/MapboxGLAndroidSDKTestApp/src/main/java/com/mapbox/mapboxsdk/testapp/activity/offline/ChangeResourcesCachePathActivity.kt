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
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_change_resources_cache_path.*
import java.io.File

class ChangeResourcesCachePathActivity : AppCompatActivity(),
    AdapterView.OnItemClickListener,
    FileSource.SetResourcesCachePathCallback {

  lateinit var adapter: PathAdapter

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_change_resources_cache_path)

    Thread(Runnable {
      adapter = PathAdapter(this, obtainFilesPaths(this))
      listView.adapter = adapter
      listView.emptyView = empty
      listView.onItemClickListener = this
    }).start()
  }

  override fun onStart() {
    super.onStart()
    val path: String? = FileSource.getResourcesCachePath(this)
    Toast.makeText(this, "Current path: $path", Toast.LENGTH_LONG).show()
  }

  override fun onItemClick(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
    listView.onItemClickListener = null
    val path: String = adapter.getItem(position) as String
    FileSource.setResourcesCachePath(this, path, this)
  }

  override fun onError(message: String?) {
    listView.onItemClickListener = this
    Toast.makeText(this, "Error: $message", Toast.LENGTH_LONG).show()
  }

  override fun onSuccess(path: String?) {
    listView.onItemClickListener = this
    Toast.makeText(this, "New path: $path", Toast.LENGTH_LONG).show()
  }

  private fun obtainFilesPaths(context: Context): List<String> {
    val paths = ArrayList<String>()
    paths.add(context.filesDir.absolutePath)
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
      paths.addAll(obtainExternalFilesPathsKitKat(context))
    } else {
      paths.addAll(obtainExternalFilesPathsLegacy(context))
    }
    paths.add("${File.separator}invalid${File.separator}cache${File.separator}path")
    return paths
  }

  private fun obtainExternalFilesPathsLegacy(context: Context): List<String> {
    val postFix =
        "${File.separator}Android${File.separator}data${File.separator}${context.packageName}${File.separator}files"
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
  private fun obtainExternalFilesPathsKitKat(context: Context): List<String> {
    val paths = ArrayList<String>()
    val extDirs = context.getExternalFilesDirs(null)
    for (dir in extDirs) {
      if (dir != null) {
        paths.add(dir.absolutePath)
      }
    }
    return paths
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