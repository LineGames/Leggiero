===============
Getting Started
===============

This document is a getting started guide for game developlment with Leggiero engine.
Before try the contents in this document, please set developlment environment as `Dependency <10_dependency.html>`_ document.


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
  

WinPC Binary Build
------------------
Windows PC application of LegToy can be built using Visual Studio.

- Open the solution file **LegToy.sln** in the *Project Folder of LegToy* .
  
- Building Windows PC Binary
  
- Set **LegToy.WinPC** project as StartUp Project in Solution Explorer.
  
- Set x64 for Active solution platform in Configuration Manager.
  
- Now you can build WinPC binary of LegToy and can do runtime debugging. Binary is in *Binary* sub-directory of the project folder.
  

.. Hint:: Build can fails if engine and project directory is in a folder having long path. It is suspected that OS\'s path length limit(about 260 characters) cause the error. We recommend to put engine and project in a folder with swallow path.

iOS App Build
-------------

- Open **LegToy.xcodeproj** project in *Project Folder of LegToy* with XCode.
  
- Select LegToy app for build target, and build it for a device platform you want.
  
  + Sign and certificate settings may be needed for real devices. Set project setting for your Apple developer account before build.
    
  
.. Note:: Building the app for iOS simulators is not supported.

Android App Build
-----------------

- **Sources/App/LegToy.Android** folder under the *Project Folder of LegToy* is a project for Android Studio.
  
- Build the app with Android Studio or the other tools whatever you want.
  
.. Hint:: It may useful to set version of tools in Gradle setting.
  
  - *ext.ndkVersion* in project gradle file and *android.ndkVersion* in module gradle file
  
  - *android.externalNativeBuild.cmake.version* in module gradle file
  


Start with a NEW project
========================

You can create a new project for Leggiero engine with Project Creator tool.

Project Creator
---------------
There are prebuilt tool binaries *(requires .NET 6.0 runtime)* in *Tools/ProjectCreator* directory in the engine repository.

- **ProjectCreatorTool.exe** is a command line tool for project creation.
  
- **ProjectCreatorGUI.exe** supports to create a project with GUI.
  
Create a Project
----------------
You can set some properties for new project in Project Creator tool.

The project directory of new project will be located in the **Engine Root** directory. You can build the new project using Visual Studio solution, Android Studio project(**Source/App/*.Android**) and XCode project in project directory.

.. Note:: Changing the relative path from the engine root directory and the project directory is NOT supported for now.

