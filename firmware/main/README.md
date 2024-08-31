# Cronus 

## How to update ca.pem

```shell
openssl s_client -showcerts -connect github.com:443 </dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' > ca.pem
openssl s_client -showcerts -connect objects.githubusercontent.com:443 </dev/null | sed -n -e '/-.BEGIN/,/-.END/ p' >> ca.pem
```

Get the last certs of each host chain and put it into the `ca.pem` file.
