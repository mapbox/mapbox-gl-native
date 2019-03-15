package com.mapbox.mapboxsdk.testapp.activity.maplayout

import android.annotation.SuppressLint
import com.mapbox.mapboxsdk.testapp.R

/**
 * TestActivity showcasing how to integrate a TexureView MapView in a RecyclerView.
 */
@SuppressLint("ClickableViewAccessibility")
class TextureRecyclerViewActivity : GLSurfaceRecyclerViewActivity() {

  override fun getMapItemLayoutId() : Int{
      return R.layout.item_map_texture
  }
}
