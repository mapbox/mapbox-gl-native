#!/usr/bin/env bash

mason install node 0.10.35
export PATH="`mason prefix node 0.10.35`/bin":"$PATH"
