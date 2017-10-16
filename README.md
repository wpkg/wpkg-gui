![WPKG Logo](https://wpkg.org/wpkg.png)

# WPKG Client and Service

The source codes of the WPKG Client and WPKG Service.

## What need for compiling on a Visual Studio 2008

For building of this project you need at least
Visual Studio 2008 and Windows XP with SP3, because
Windows XP have MSXML v1.0.

## What need for compiling on a modern Visual Studio

But if you don't have a VS2008 you can compile it with VS2017.
It makes sense to build it against the VS2008 target to avoid
having to install a newer VCRedist on clients.

Install [MSXML6](https://www.microsoft.com/en-us/download/details.aspx?id=6276)
then in folder `Sources/Components`, inside files `Xmlsettings.cpp`
and `Xmlsettings.h` you need to make the following replacement:

    from: msxml.dll
    to: msxml3.dll

and

    from: MSXML
    to: MSXML2

Good luck!

# Some links

* Main the WPKG website - https://wpkg.org/
* WPKG documentation page - https://wpkg.org/Documentation
* Article on Wikipedia - https://en.wikipedia.org/wiki/WPKG_(software)
