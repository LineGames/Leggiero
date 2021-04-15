===============
Getting Started
===============

This document is a getting started guide for game developlment with Leggiero engine.
Before try the contents in this document, please set developlment environment as `Dependency <10_dependency.html>`_ document.

.. Attention:: Opening the project solution in Visual Studio without Nsight Tegra, Visual Studio Edition can corrupt some project settings. We recommend to try following instructions after install all dependencies.


Engine Setup
============

Get Engine Source Code
----------------------
The latest version of Leggiero engine source is available from GitHub repository:

.. code-block:: console
  
  git clone https://github.com/LineGames/Leggiero.git
  

You can also use another release, or checkout a branch you want.

Put engine source code into where you want. There will be *Documents*, *Engine*, *Tools* sub-directories in the **Engine Root** directory.

.. Note:: Engine source is shared between game projects. Each project has their own project directory at the **Engine Root** directory.

Get Engine Library Source Code
------------------------------
External libraries used by the engine is hosted in another repository for convenient version control.

Please clone library codes in *Engine* sub-directory of engine code as following:

.. code-block:: console
  
  git clone https://github.com/LineGames/Leggiero-Library.git Libraries
  

.. Note:: The location of engine library codes should be *Engine/Libraries* from engine root directory. Be aware of the name of directory which is different from the name of library repository.

*Engine* sub-directory of engine root directory should have *Common*, *Libraries*, *Modules*, *Platform* directories as its children.

Do I need to build engine?
--------------------------
No. You don't need to build engine code separately. Engine will be built with each game project.


Build and try LegToy
====================

LegToy is an example project for Leggiero engine including demonstration for functions and usages.

You can get source of LegToy project in the **Engine Root Directory** as:

.. code-block:: console
  
  git clone https://github.com/LineGames/LegToy.git
  

WinPC and Android App Build
---------------------------
Android and Windows PC application of LegToy can be built using Visual Studio.

- Open the solution file **LegToy.sln** in the *Project Folder of LegToy* .
  
- Building Windows PC Binary
  
  + Set **LegToy.WinPC** project as StartUp Project in Solution Explorer.
    
  + Set x64 for Active solution platform in Configuration Manager.
    
  + Now you can build WinPC binary of LegToy and can do runtime debugging. Binary is in *Binary* sub-directory of the project folder.
    
  
- Building an Android App
  
  + Set **LegToy.Android** project as StartUp Project in Solution Explorer.
    
  + Set **AArch64-Android-NVIDIA** for Active solution platform in Configuration Manager. We recommend to try build in Debug configuration first.
    
  + Build the entire solution using :guilabel:`Build` > :guilabel:`Build Solution` menu. Then you can get .so archive of the game application for ARM64 architecture.
    
  + Next, set **ARM-Android-NVIDIA** as Active solution platform.
    
  + Build the entire solution using :guilabel:`Build` > :guilabel:`Build Solution` menu. Game application\'s .so archive for 32-bit ARM architectures have been built.
    
  + Now, change Active solution platform to **Tegra-Android**.
    
  + Right click **LegToy.Android** project in Solution Explorer, and select :guilabel:`Project Only` > :guilabel:`Build Only LegToy.Android` menu to build a APK file.
    
    • APK will be created under Binary folder in the prolect directory.
      
    • Although it is annoying to build .so shared library and APK separately, it is for building Fat APK supporting 32-bit and 64-bit architecture both. Nsight Tegra, Visual Studio Edition doesn\'t handle dependency and build process properly.
      
    
  

.. Hint:: Build can fails if engine and project directory is in a folder having long path. It is suspected that OS\'s path length limit(about 260 characters) cause the error. We recommend to put engine and project in a folder with swallow path.

iOS App Build
-------------

- Open **LegToy.xcodeproj** project in *Project Folder of LegToy* with XCode.
  
- Select LegToy app for build target, and build it for a device platform you want.
  
  + Sign and certificate settings may be needed for real devices. Set project setting for your Apple developer account before build.
    
  


Start with a NEW project
========================

You can create a new project for Leggiero engine with Project Creator tool.

Project Creator
---------------
There are prebuilt tool binaries *(requires .NET 5.0 runtime)* in *Tools/ProjectCreator* directory in the engine repository.

- **ProjectCreatorTool.exe** is a command line tool for project creation.
  
- **ProjectCreatorGUI.exe** supports to create a project with GUI.
  
Create a Project
----------------
You can set some properties for new project in Project Creator tool.

The project directory of new project will be located in the **Engine Root** directory. You can build the new project using Visual Studio solution and XCode project in project directory.

.. Note:: Changing the relative path from the engine root directory and the project directory is NOT supported for now.


Notes for Android Release Build
===============================

App Signning is needed to build and run Release APK or to distribute your app via stores.

You can set a secure property file in project properties windows of LegToy.Android project for do signning in the build process. Set **Secure Properties Location** property under :guilabel:`Gradle Build` > :guilabel:`General` in Tegra-Android platform.
