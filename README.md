# Raspberry Pi Web UI
This project aims what beginners can configure easier a wired and wireless network of Linux using Web UI.
If someone use this Web UI, they can search Access Points and connect to a selected AP.
And this project supports a web server based on Luna to support RESTful API that are used by Web UI, and Web UI uses AngularJS and Bootstrap.

# How to build

## macOS(OS X) and Linux(i386 and Raspberry Pi2)
```bash
mkdir build
cd build
cmake ..
make -j4
```
# How to test
```bash
sudo ./bin/RPiWebUI.Luna ../../html/
```
You can connect to this Web Server using Web browser after executing this application.
The default port of Web Server is 8000, so you must use this url: http://localhost:8000
And this Web Server requires the username and password to login.
Default username and password are:
```bash
username: root
password: admin
```

# Reference Sources
1. AngularJS User Registration and Login Example & Tutorial: https://github.com/cornflourblue/angular-registration-login-example
2. Mongoose Embedded Web Server Library: https://github.com/cesanta/mongoose

