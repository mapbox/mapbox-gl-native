package com.mapbox.mapboxsdk.attribution;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.widget.TextView;

import java.util.Arrays;
import java.util.List;

public class AttributionMeasure {

  private Bitmap logo;
  private Bitmap logoSmall;
  private Bitmap snapshot;
  private TextView textView;
  private TextView textViewShort;
  private float margin;

  private boolean shorterText;

  AttributionMeasure(Bitmap snapshot, Bitmap logo, Bitmap logoSmall, TextView tv, TextView tvShort, float margin) {
    this.snapshot = snapshot;
    this.logo = logo;
    this.logoSmall = logoSmall;
    this.textView = tv;
    this.textViewShort = tvShort;
    this.margin = margin;
  }

  public AttributionLayout measure() {
    Chain chain = new Chain(
      new FullLogoLongTextCommand(),
      new FullLogoShortTextCommand(),
      new SmallLogoLongTextCommand(),
      new SmallLogoShortTextCommand(),
      new LongTextCommand(),
      new ShortTextCommand(),
      new NoTextCommand()
    );

    AttributionLayout attributionLayout = chain.start(this);
    shorterText = attributionLayout.isShortText();
    return attributionLayout;
  }


  private static class FullLogoLongTextCommand implements Command {
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getLogoContainerWidth() + measure.getTextViewContainerWidth();
      boolean fitBounds = width <= measure.getMaxSize();
      if (fitBounds) {
        PointF anchor = calculateAnchor(measure.snapshot, measure.textView, measure.margin);
        return new AttributionLayout(measure.logo, anchor, false);
      }
      return null;
    }
  }

  private static class FullLogoShortTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getLogoContainerWidth() + measure.getTextViewShortContainerWidth();
      boolean fitBounds = width <= measure.getMaxSizeShort();
      if (fitBounds) {
        PointF anchor = calculateAnchor(measure.snapshot, measure.textViewShort, measure.margin);
        return new AttributionLayout(measure.logo, anchor, true);
      }
      return null;
    }
  }

  private static class SmallLogoLongTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getLogoSmallContainerWidth() + measure.getTextViewContainerWidth();
      boolean fitBounds = width <= measure.getMaxSize();
      if (fitBounds) {
        PointF anchor = calculateAnchor(measure.snapshot, measure.textView, measure.margin);
        return new AttributionLayout(measure.logoSmall, anchor, false);
      }
      return null;
    }
  }

  private static class SmallLogoShortTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getLogoContainerWidth() + measure.getTextViewShortContainerWidth();
      boolean fitBounds = width <= measure.getMaxSizeShort();
      if (fitBounds) {
        PointF anchor = calculateAnchor(measure.snapshot, measure.textViewShort, measure.margin);
        return new AttributionLayout(measure.logoSmall, anchor, true);
      }
      return null;
    }
  }

  private static class LongTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getTextViewContainerWidth() + measure.margin;
      boolean fitBounds = width <= measure.getMaxSize();
      if (fitBounds) {
        return new AttributionLayout(null, calculateAnchor(measure.snapshot, measure.textView, measure.margin), false);
      }
      return null;
    }
  }

  private static class ShortTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      float width = measure.getTextViewShortContainerWidth() + measure.margin;
      boolean fitBounds = width <= measure.getMaxSizeShort();
      if (fitBounds) {
        PointF anchor = calculateAnchor(measure.snapshot, measure.textViewShort, measure.margin);
        return new AttributionLayout(null, anchor, true);
      }
      return null;
    }
  }

  private static class NoTextCommand implements Command {
    @Override
    public AttributionLayout execute(AttributionMeasure measure) {
      return new AttributionLayout(null, null, false);
    }
  }

  private static PointF calculateAnchor(Bitmap snapshot, TextView textView, float margin) {
    return new PointF(
      snapshot.getWidth() - textView.getMeasuredWidth() - margin,
      snapshot.getHeight() - margin - textView.getMeasuredHeight()
    );
  }

  public TextView getTextView() {
    return shorterText ? textViewShort : textView;
  }

  private class Chain {
    public List<Command> commands;

    Chain(Command... commands) {
      this.commands = Arrays.asList(commands);
    }

    public AttributionLayout start(AttributionMeasure measure) {
      AttributionLayout attributionLayout = null;
      for (Command command : commands) {
        attributionLayout = command.execute(measure);
        if (attributionLayout != null) {
          break;
        }
      }
      return attributionLayout;
    }
  }

  public interface Command {
    AttributionLayout execute(AttributionMeasure measure);
  }

  private float getTextViewContainerWidth() {
    return textView.getMeasuredWidth() + margin;
  }

  private float getLogoContainerWidth() {
    return logo.getWidth() + (2 * margin);
  }

  private  float getTextViewShortContainerWidth() {
    return textViewShort.getMeasuredWidth() + margin;
  }

  private float getLogoSmallContainerWidth() {
    return logoSmall.getWidth() + (2 * margin);
  }

  private float getMaxSize() {
    return snapshot.getWidth() * 8 / 10;
  }

  private float getMaxSizeShort() {
    return snapshot.getWidth();
  }

  public static class Builder {
    private Bitmap snapshot;
    private Bitmap logo;
    private Bitmap logoSmall;
    private TextView textView;
    private TextView textViewShort;
    private float marginPadding;

    public Builder setSnapshot(Bitmap snapshot) {
      this.snapshot = snapshot;
      return this;
    }

    public Builder setLogo(Bitmap logo) {
      this.logo = logo;
      return this;
    }

    public Builder setLogoSmall(Bitmap logoSmall) {
      this.logoSmall = logoSmall;
      return this;
    }

    public Builder setTextView(TextView textView) {
      this.textView = textView;
      return this;
    }

    public Builder setTextViewShort(TextView textViewShort) {
      this.textViewShort = textViewShort;
      return this;
    }

    public Builder setMarginPadding(float marginPadding) {
      this.marginPadding = marginPadding;
      return this;
    }

    public AttributionMeasure build() {
      return new AttributionMeasure(snapshot, logo, logoSmall, textView, textViewShort, marginPadding);
    }
  }
}
