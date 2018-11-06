package com.mapbox.mapboxsdk.perf;

import com.mapbox.mapboxsdk.rules.*;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import org.junit.Rule;

public abstract class BasePerfTest extends BaseActivityTest {

  @Rule
  public TraceRule traceRule = new TraceRule();

  @Rule
  public BatteryStatsDumpsysRule batteryRule = new BatteryStatsDumpsysRule();

  @Rule
  public GraphicsDumpsysRule graphicsRule = new GraphicsDumpsysRule();

  @Rule
  public CpuInfoDumpsysRule cpuInfoRule = new CpuInfoDumpsysRule();

  @Rule
  public MemoryInfoDumpsysRule memoryInfoRule = new MemoryInfoDumpsysRule();

}
