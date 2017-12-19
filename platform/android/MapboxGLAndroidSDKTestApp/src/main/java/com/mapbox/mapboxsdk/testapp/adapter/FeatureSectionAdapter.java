package com.mapbox.mapboxsdk.testapp.adapter;

import android.content.Context;
import android.support.annotation.IdRes;
import android.support.annotation.LayoutRes;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.testapp.utils.FontCache;

import java.util.Arrays;

public class FeatureSectionAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

  private static final int SECTION_TYPE = 0;

  private final Context context;
  private final SparseArray<Section> sections;
  private final RecyclerView.Adapter adapter;

  @LayoutRes
  private final int sectionRes;

  @IdRes
  private final int textRes;

  private boolean valid = true;

  public FeatureSectionAdapter(Context ctx, int sectionResourceId, int textResourceId,
                               RecyclerView.Adapter baseAdapter) {
    context = ctx;
    sectionRes = sectionResourceId;
    textRes = textResourceId;
    adapter = baseAdapter;
    sections = new SparseArray<>();
    adapter.registerAdapterDataObserver(new RecyclerView.AdapterDataObserver() {
      @Override
      public void onChanged() {
        valid = adapter.getItemCount() > 0;
        notifyDataSetChanged();
      }

      @Override
      public void onItemRangeChanged(int positionStart, int itemCount) {
        valid = adapter.getItemCount() > 0;
        notifyItemRangeChanged(positionStart, itemCount);
      }

      @Override
      public void onItemRangeInserted(int positionStart, int itemCount) {
        valid = adapter.getItemCount() > 0;
        notifyItemRangeInserted(positionStart, itemCount);
      }

      @Override
      public void onItemRangeRemoved(int positionStart, int itemCount) {
        valid = adapter.getItemCount() > 0;
        notifyItemRangeRemoved(positionStart, itemCount);
      }
    });
  }


  public static class SectionViewHolder extends RecyclerView.ViewHolder {

    public TextView title;

    public SectionViewHolder(@NonNull View view, @IdRes int textRes) {
      super(view);
      title = (TextView) view.findViewById(textRes);
      title.setTypeface(FontCache.get("Roboto-Medium.ttf", view.getContext()));
    }
  }

  @Override
  public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int typeView) {
    if (typeView == SECTION_TYPE) {
      final View view = LayoutInflater.from(context).inflate(sectionRes, parent, false);
      return new SectionViewHolder(view, textRes);
    } else {
      return adapter.onCreateViewHolder(parent, typeView - 1);
    }
  }

  @Override
  public void onBindViewHolder(RecyclerView.ViewHolder sectionViewHolder, int position) {
    if (isSectionHeaderPosition(position)) {
      String cleanTitle = sections.get(position).title.toString().replace("_", " ");
      ((SectionViewHolder) sectionViewHolder).title.setText(cleanTitle);
    } else {
      adapter.onBindViewHolder(sectionViewHolder, getConvertedPosition(position));
    }

  }

  @Override
  public int getItemViewType(int position) {
    return isSectionHeaderPosition(position)
      ? SECTION_TYPE
      : adapter.getItemViewType(getConvertedPosition(position)) + 1;
  }


  public static class Section {
    int firstPosition;
    int sectionedPosition;
    CharSequence title;

    public Section(int firstPosition, CharSequence title) {
      this.firstPosition = firstPosition;
      this.title = title;
    }

    public CharSequence getTitle() {
      return title;
    }
  }


  public void setSections(Section[] sections) {
    this.sections.clear();

    Arrays.sort(sections, (section, section1) -> (section.firstPosition == section1.firstPosition)
      ? 0
      : ((section.firstPosition < section1.firstPosition) ? -1 : 1));

    int offset = 0;
    for (Section section : sections) {
      section.sectionedPosition = section.firstPosition + offset;
      this.sections.append(section.sectionedPosition, section);
      ++offset;
    }

    notifyDataSetChanged();
  }

  public int getConvertedPosition(int sectionedPosition) {
    if (isSectionHeaderPosition(sectionedPosition)) {
      return RecyclerView.NO_POSITION;
    }

    int offset = 0;
    for (int i = 0; i < sections.size(); i++) {
      if (sections.valueAt(i).sectionedPosition > sectionedPosition) {
        break;
      }
      --offset;
    }
    return sectionedPosition + offset;
  }

  public boolean isSectionHeaderPosition(int position) {
    return sections.get(position) != null;
  }


  @Override
  public long getItemId(int position) {
    return isSectionHeaderPosition(position)
      ? Integer.MAX_VALUE - sections.indexOfKey(position)
      : adapter.getItemId(getConvertedPosition(position));
  }

  @Override
  public int getItemCount() {
    return (valid ? adapter.getItemCount() + sections.size() : 0);
  }

}
