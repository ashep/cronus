#!/bin/bash

echo -n "" > main/ca.pem

echo "github.com" >> main/ca.pem
openssl s_client -showcerts -connect github.com:443 </dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' >> main/ca.pem

echo -e "\nobjects.githubusercontent.com" >> main/ca.pem
openssl s_client -showcerts -connect objects.githubusercontent.com:443 </dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' >> main/ca.pem

echo -e "\ncronus.d5y.xyz" >> main/ca.pem
openssl s_client -showcerts -connect cronus.d5y.xyz:443 </dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' >> main/ca.pem
