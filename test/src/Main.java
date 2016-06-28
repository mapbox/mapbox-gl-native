public class Main {
	public native void runAllTests();

	public static void main(String[] args) throws Exception {
		//Need to load the mapbox-gl library explicitly first
		System.loadLibrary("mapbox-gl");
		//Load the tests
		System.loadLibrary("test-jni-lib");
		System.out.println("Libraries loaded");
		//Run the tests
		new Main().runAllTests();
	}
}
