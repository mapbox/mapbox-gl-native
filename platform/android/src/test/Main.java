
public class Main {
    public native void runAllTests();

    public static void main(String[] args) throws Exception {
        //Load the tests
        System.loadLibrary("mbgl-test");

        //Run the tests
        new Main().runAllTests();

        //Exit explicitly otherwise dalvikvm won't quit
        System.exit(0);
    }
}
