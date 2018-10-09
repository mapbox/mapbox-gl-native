public final class Greeter {
    public Greeter(String greeting) {
        initialize(this, greeting);
    }

    private Greeter(long peer) {
        this.peer = peer;
    }

    public native String greeting(String name);

    private native void initialize(Greeter self, String greeting);
    protected native void finalize() throws Throwable;
    private long peer;
}
