ofxOSXBoost
===========

Addon with Boost 1.55.0 for OSX / Xcode - Precompiled and Command to build yourself - Master is currently targeted for i386 (32bit) - Check Branches for x86_64 (64bit) - Designed for use as an open frameworks addon, however should definitely work for other OSX projects

Boost C++ Libraries 1.55.0 
===========
License: See Boost License LICENSE.MD


Where to checkout?
===========
- For openframeworks: Checkout in the addons folder like so: addons/ofxOSXBoost
- For others: anywhere you please


How to Build?
===========

1. You don't need to. This has the pre-compiled versions of BOOST for you to use
2. If you would prefer to build it yourself checkout the build-yourself branch https://github.com/danoli3/ofxOSXBoost/tree/build-yourself

Architectures?
===========
1. Figure out what target architecture you require (i386 or x86_64) (http://tinyurl.com/i386vsx8464osx)

- Master is currently built for i386 - https://github.com/danoli3/ofxOSXBoost/tree/master
- x86_64 branch for the Universal 32bit/64bit for newer macs/osx. - https://github.com/danoli3/ofxOSXBoost/tree/x86_64
- i386 for all 32bit (works on 64bit machines too) - https://github.com/danoli3/ofxOSXBoost/tree/i386

- build-yourself - https://github.com/danoli3/ofxOSXBoost/tree/build-yourself




================================================================================

How to get working with a project in Xcode?
============

In Xcode Build Settings for your project:
- Add to Library Search Paths: "$(SRCROOT)/../../../addons/ofxOSXBoost/libs/boost/lib/osx_i386/"
- Add to Header Search Paths: "$(SRCROOT)/../../../addons/ofxOSXBoost/libs/boost/include"


In Xcode Build Phases
- Add the libs in the addons/ofxOSXBoost/libs/boost/lib/osx_i386/ directory to Link Binary With Libraries

- Enjoy boost :)

Documentation on Boost 1.55.0
===========

See: http://www.boost.org/users/history/version_1_55_0.html