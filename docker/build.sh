#!/usr/bin/env bash

#!/usr/bin/env bash

set -e
set -o pipefail

docker build -t mapbox/gl-native:travis docker
