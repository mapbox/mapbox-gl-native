package com.mapbox.mapboxsdk.testapp.activity;

import android.os.Bundle;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.appcompat.app.AppCompatActivity;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.testapp.R;

/**
 * Used for testing fragments inside a fake activity.
 */
public class SingleFragmentActivity extends AppCompatActivity {
  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    FrameLayout content = new FrameLayout(this);
    content.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
      ViewGroup.LayoutParams.MATCH_PARENT));
    content.setId(R.id.container);
    setContentView(content);
  }

  public void setFragment(Fragment fragment) {
    getSupportFragmentManager().beginTransaction()
      .add(R.id.container, fragment, "TEST")
      .commit();
  }

  public void replaceFragment(Fragment fragment) {
    getSupportFragmentManager().beginTransaction()
      .replace(R.id.container, fragment).commit();
  }
}