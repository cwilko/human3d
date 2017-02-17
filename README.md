# human3D

A Walking 3D Person.

See a video demo here : https://youtu.be/TyKssRmYhfg

Human3D is a Windows application which demonstrates a framework for creating 3D animations. It was written in 2001 and demonstrates programming in Windows, DirectX, and C++. The framework makes use of Windows DirectX technology. Specifically, this revision of the software depends on DirectX 3 which is now very old. Fortunately DirectX is backwards compatible so any version of the SDK will suffice. 

The models created in this demo were generated using 3D Studio, and exported as ASCII files to make it easy to read them in and process them.

# Building the code
If you are compiling using Visual Studio, the DirectX includes and libraries will already be available to you on the Developer command line. If you don't have these available, you can get hold of the DirectX 7 SDK here : https://archive.org/details/MicrosoftDirectX7SDK

The following command should compile and link the executable. To run the demo, start the Human3D.exe file.

*cl /EHsc user32.lib gdi32.lib winmm.lib ddraw.lib dxguid.lib Texture.cpp TextureList.cpp Vertex.cpp Element.cpp Object.cpp Frame.cpp Game.cpp LoadFile.cpp WinMain.cpp /oHuman3D.exe*

