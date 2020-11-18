# Real-Time-Embedded-System-NXP

### Semiconductor NXP LPC408x + FreeRTOS firmware + Embedded System (SJSU SJ2 Board)
All lab are based on [Cmpe146 - SJSU -  Professor Preet Kang](http://books.socialledge.com/books/embedded-drivers-real-time-operating-systems)

An `SJ2` board is used at San Jose State University (SJSU) to teach Embedded Systems' courses. Part of this Git repository also includes development environment for not just an ARM controller, but also support to compile and test software on your host machine such as Windows, Mac etc.

<img style="-webkit-user-select: none;margin: auto;cursor: zoom-in;" src="http://socialledge.com/sjsu/images/a/a5/Slide2.png" width="790" height="483">

---

## Next Steps

- [Build and Flash Project](README-GETTING-STARTED.md)
- [Read more about SCons](README-SCons.md) to figure out how to build projects

---

## Build System

We use [SCons](https://scons.org/) as our build platform. The developers of SJ2-C applied experience of diverse set of build systems acquired over many years, and resorted to this one. The candidates were:

- SCons (used at Tesla, and many other companies)
- Bazel (used at Google, Zoox and many other companies)
- Make

SCons discusses the advantages and disadvantages of other existing build systems: [Build System Comparison](https://github.com/SCons/scons/wiki/sconsvsotherbuildtools)

From experience, we can state that `bazel` is really cool, but hard to extend and problematic in Windows. SCons dependencies are tricky, but it is based on python and easy to understand and extend. SCons takes advantage of a Python interpreter making it portable across multiple platforms (Mac, Linux, Windows).

---

## History

We wanted to build a strong foundational sample project for SJ-2 development board that is used at SJSU.

"SJ2" [original](https://github.com/kammce/SJSU-Dev2)

The code may appear less fancy, but it is simple to understand and traceable with minimal abstractions. The goal is to avoid designing yet another Arduino platform. There is no such thing as magic in the field of firmware engineering.

---
