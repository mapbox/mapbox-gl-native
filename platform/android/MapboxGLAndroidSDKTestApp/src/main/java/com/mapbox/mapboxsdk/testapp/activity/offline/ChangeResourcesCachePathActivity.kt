package com.mapbox.mapboxsdk.testapp.activity.offline

import android.content.Context
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.BaseAdapter
import android.widget.TextView
import android.widget.Toast
import com.mapbox.mapboxsdk.offline.OfflineRegion
import com.mapbox.mapboxsdk.storage.FileSource
import com.mapbox.mapboxsdk.testapp.R
import kotlinx.android.synthetic.main.activity_change_resources_cache_path.*

class ChangeResourcesCachePathActivity : AppCompatActivity(),
    AdapterView.OnItemClickListener,
    FileSource.MigrateResourcesCachePathCallback {

  lateinit var adapter: PathAdapter

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    setContentView(R.layout.activity_change_resources_cache_path)

    FileSource.requestPossibleResourcesCachePaths(this) { possiblePaths ->
      adapter = PathAdapter(this, possiblePaths)
      listView.adapter = adapter
      listView.emptyView = empty
      listView.onItemClickListener = this
    }
  }

  override fun onStart() {
    super.onStart()
    val path: String? = FileSource.getResourcesCachePath(this)
    Toast.makeText(this, "Current path: $path", Toast.LENGTH_LONG).show()
  }

  override fun onItemClick(parent: AdapterView<*>?, view: View?, position: Int, id: Long) {
    listView.onItemClickListener = null
    val path: String = adapter.getItem(position) as String
    FileSource.migrateToResourcesCachePath(this, path, this)
  }

  override fun onError(message: String?) {
    listView.onItemClickListener = this
    Toast.makeText(this, "Error: $message", Toast.LENGTH_LONG).show()
  }

  override fun onMigrate(migrationPath: String?, offlineRegions: Array<out OfflineRegion>?) {
    listView.onItemClickListener = this
    Toast.makeText(this, "Migrated ${offlineRegions?.size} regions to: $migrationPath", Toast.LENGTH_LONG).show()
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