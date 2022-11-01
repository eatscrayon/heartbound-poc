# CVE-2022-3602-poc
#### Out-of-bounds read in OpenSSL 3.0 < 3.0.7

Useage:
```
$ gcc -o heartbound-check heartbound-check.c -lpthread
$ ./heartbound-check 127.0.0.1 443
```

#### CVSS 9.1
Attack Vector:network

Attack Complexity:low

Privileges Required:none

User Interaction:none

Scope:unchanged

Confidentiality:high

Integrity:none

Availability:high


This release will be made available on Tuesday 1st November 2022 between 
1300-1700 UTC.

OpenSSL 3.0.7 is a security-fix release. The highest severity issue 
fixed in this release is CRITICAL:

https://www.openssl.org/policies/general/security-policy.html
