#include <string>

class Tap {
public:
    class Test;
    Tap();
    ~Tap();

private:
    static int total;
    static int errored;
    static bool started;
};

class Tap::Test {
public:
    Test(const std::string &name);
    ~Test();

    void ok(bool status, const std::string &message);
    void fail(const std::string &message);
    void end();

private:
    bool failed = false;
    bool finished = false;
};
