public class Main {
	public native void runAllTests();

	public static void main(String[] args) throws Exception {
		//System.out.println(Class.forName("com.mapbox.mapboxsdk.geometry.LatLng"));
		System.loadLibrary("mapbox-gl");
		System.loadLibrary("test-jni-lib");
		System.out.println("Libraries loaded");
		new Main().runAllTests();
	}
}
