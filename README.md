objc-atratocli
==============

> Deprecated.

Atrato Command Line Interface.
A C-implementation for the Atrato Fundamental API.

TODO
- ~~Make SSL work~~
- ~~No LDAP pass show in terminal - http://stackoverflow.com/questions/2600528/c-readline-function~~
- ~~Store credentials on local SQLite db~~
- Encrypt SQLite local data
- Other authentication maybe? - http://www.idontplaydarts.com/2011/07/google-totp-two-factor-authentication-for-php/
- Datacenter access request?
- ~~Replace const X* by const X* const where possible~~
- Auto setup SSH sessions? http://stackoverflow.com/questions/9602153/bring-process-to-foreground

Compile on Ubuntu
=============
```bash
sudo apt-get install build-essential pkg-config libsqlite3-dev libcurl4-openssl-dev
make
```
