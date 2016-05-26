FROM mapbox/gl-native:travis

# Install compiler
RUN apt-get -y install gdb g++-5 gcc-5 libllvm3.4

RUN useradd -ms /bin/bash mapbox
USER mapbox
ENV HOME /home/mapbox
WORKDIR /home/mapbox

# Node
RUN git clone https://github.com/creationix/nvm.git ~/.nvm && \
    . ~/.nvm/nvm.sh && \
    NVM_DIR=~/.nvm nvm install 4.4.5
