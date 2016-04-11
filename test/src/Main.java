public class Main {
    public native void run();

    public static void main(String[] args) {
        System.loadLibrary("test-jni-lib");
        new Main().run();
    }
}
