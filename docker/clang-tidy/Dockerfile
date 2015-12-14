FROM mapbox/gl-native:travis

# Install compiler
RUN apt-get -y install clang-3.8 lldb-3.8 clang-tidy-3.8 clang-format-3.8

RUN useradd -ms /bin/bash mapbox
USER mapbox
ENV HOME /home/mapbox
WORKDIR /home/mapbox
