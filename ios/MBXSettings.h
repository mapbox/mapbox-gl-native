#import <llmr/map/settings.hpp>

namespace llmr {
class Settings_iOS : public Settings {
public:
    Settings_iOS();
    virtual Configuration load();
    virtual void persist(const Configuration &config);
    virtual void sync();
    virtual void clear();
};
}