//
//  MBXSettings.hpp
//  llmr
//
//  Created by Justin R. Miller on 1/27/14.
//
//

#import <llmr/map/settings.hpp>

namespace llmr
{
    class Settings_iOS : public Settings
    {
        public:
            Settings_iOS();
            virtual void load();
            virtual void persist();
            virtual void sync();
            virtual void clear();
    };
}