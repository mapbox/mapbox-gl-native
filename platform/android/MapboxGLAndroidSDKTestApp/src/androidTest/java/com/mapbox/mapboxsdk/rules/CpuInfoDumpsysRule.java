package com.mapbox.mapboxsdk.rules;

public class CpuInfoDumpsysRule extends AbstractDumpsysRule {

  @Override
  protected String dumpsysService() {
    return "cpuinfo";
  }
}
