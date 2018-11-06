package com.mapbox.mapboxsdk.rules;

public class BatteryStatsDumpsysRule extends AbstractDumpsysRule {

  private static final String BATTERY_STATS = "batterystats";

  @Override
  protected String dumpsysService() {
    return BATTERY_STATS;
  }
}
