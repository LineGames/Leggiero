=========
Platforms
=========

Supported Platforms
===================

Leggiero is targeting **iOS** and **Android** platforms officially.
Moreover, we also support **x64 Windows PC** platform for development.


Comparison
==========

+----------------------+-----------------+-----------------------------+------------------+
|             Platform |       iOS       |      Android                |       WinPC      |
+======================+=================+=============================+==================+
|          **Support** | Official        | Official                    | Development      |
+----------------------+-----------------+-----------------------------+------------------+
|     **Graphics API** | OpenGL ES 3.0   | OpenGL ES 3.0               | OpenGL ES 3.0    |
|                      |                 |                             |                  |
|                      | *w/ MetalANGLE* |                             | *w/ PowerVR SDK* |
+----------------------+-----------------+-----------------------------+------------------+
|        **Sound API** | OpenAL soft     | Oboe                        | OpenAL soft      |
+----------------------+-----------------+-----------------------------+------------------+
|        **Threading** | Pthread         | Pthread                     | Pthread          |
|                      |                 |                             |                  |
|                      |                 |                             | *w/ PThreads4W*  |
+----------------------+-----------------+-----------------------------+------------------+
| **Platform Interop** | Objective-C++   | Java                        | C++              |
|                      |                 |                             |                  |
|                      |                 | *w/ NDK*                    | *w/ WinAPI*      |
+----------------------+-----------------+-----------------------------+------------------+
|        **Dev Tools** | XCode           | Android Studio              | Visual Studio    |
|                      |                 |                             |                  |
|                      |                 | *w/ CMake for native codes* |                  |
+----------------------+-----------------+-----------------------------+------------------+
