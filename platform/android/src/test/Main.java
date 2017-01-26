
public class Main {
    public native void runAllTests(String[] args);

    public static void main(String[] args) throws Exception {
        // Load the tests
        System.loadLibrary("mbgl-test");

        // Run the tests
        new Main().runAllTests(args);

        // Exit explicitly otherwise dalvikvm won't quit
        System.exit(0);
    }
}
