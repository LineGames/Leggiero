============
Known Issues
============

There are some known, but not fixed, issues:

* In iOS platform, screen is black-outed during a few frame after splash screen shown.
  

* OpenGL ES output is now shown after re-launch of the app from termination by back button.
  EGL\'s internal state problem is a suspected cause, because exit by back button destory no the process but the Activity.
  
  We tried some solution but could not fix. So we changed default behavior of back button to preventing propagation.
  It can be a solution to make app terminate completely, e.g. using System.exit(0), via overriding back button handler in the Activity of each project application.
  
