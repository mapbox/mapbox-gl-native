#!/usr/bin/env bash

cd misc
curl https://raw.githubusercontent.com/curl/curl/master/lib/mk-ca-bundle.pl | perl
rm certdata.txt
