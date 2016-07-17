BasilC
=======

[![Build Status](https://travis-ci.org/shawnanastasio/BasilC.svg?branch=master)](https://travis-ci.org/shawnanastasio/BasilC)

BasilC is an esoteric interpreted programming language aimed at rapid development and deployment.
BasilC introduces the new programming paradigm of procedural non-typed languages.

Quick start
-----------

Since The BasilC interpreter is written in pure C, it can be compiled with 0 dependencies on nearly any platform with a C compiler.

To compile on UNIX-like systems with GNU Make, simply run:
```
$ make
```

If you wish to use shebang lines (such as the one in example.basilc) to execute BasilC programs as scripts, or if you wish to read the manpage with the man command, install the compiled binary to your path:
```
$ sudo make install
```

You can test your installation by running the test script in the `examples` folder:
```
$ basilc helloworld.basilc
```
Or, alternatively:
```
$ chmod +x helloworld.basilc
$ ./helloworld.basilc
```

Documentation is available in the manpage format, and it can be accessed with the following command: 
```
man basilc
```


Technical Explanation
---------------------
The BasilC interpreter is written in 100% C in order to provide fast run times and portability
across nearly all POSIX platforms, and even Windows.

_BasilC abstains from traditional programming paradigms and puts the static vs dynamically typed variable debate to an end once and for all._

All variables in BasilC are stored internally as char literals. This provides enhanced flexibility
for all types of data, while still allowing for type-dependent operations, such as arithmetic, that
can still be performed by leveraging C's powerful casting system on a per-function basis.
