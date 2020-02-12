#!/bin/bash -e
export PS4="$ "
set -x

echo -n | openssl s_client -connect scan.coverity.com:443 | sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' | sudo tee -a /etc/ssl/certs/ca-
pushd /tmp
curl https://scan.coverity.com/download/`uname` -F token=$COVERITY_SCAN_TOKEN -F project=$COVERITY_PROJECT_NAME -o coverity.tar.gz
tar xzf coverity.tar.gz
rm coverity.tar.gz
popd
#wget https://scan.coverity.com/scripts/travisci_build_coverity_scan.sh
