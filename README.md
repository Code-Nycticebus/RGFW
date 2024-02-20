# Riley's Gui library FrameWork

## Build statuses
![cplus workflow](https://github.com/ColleagueRiley/RGFW/actions/workflows/linux.yml/badge.svg)
![cplus workflow windows](https://github.com/ColleagueRiley/RGFW/actions/workflows/windows.yml/badge.svg)
![cplus workflow windows](https://github.com/ColleagueRiley/RGFW/actions/workflows/macos.yml/badge.svg)

# About
This library is meant to be used as a very small and flexable alternative library to GLFW. 

This library 

1) is single header and portable (written in C89 in mind)
2) is very small compared to other libraries
3) only depends on system API libraries, winapi, X11, Cocoa (except for Silicon (for macOS) which is included)
4) let's you create a window with an graphics context (opengl, vulkan or directX) and manage the window and it's events only with a few function calls 

This library does not

1) Handle any rendering for you (other than creating your graphics context)
2) do anything above the bare minimum in terms of functionality 

# Documentation

There is a lot of in-header-documentation, but more doucmentation can be found [here](https://RSGL.github.io/RGFW)

If you wish to build the documentation your self, there is also an Doxygen file attached.

# RGFW vs GLFW

![alt text](https://github.com/ColleagueRiley/RGFW/blob/main/RGFWvsGLFW.png?raw=true)
