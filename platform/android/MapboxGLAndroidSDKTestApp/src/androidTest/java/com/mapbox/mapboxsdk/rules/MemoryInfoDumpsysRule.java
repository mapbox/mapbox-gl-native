package com.mapbox.mapboxsdk.rules;

public class MemoryInfoDumpsysRule extends AbstractDumpsysRule {

  @Override
  protected String dumpsysService() {
    return "procstats";
  }
}
