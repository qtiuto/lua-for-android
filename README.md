[![Build Status](https://travis-ci.org/qtiuto/lua-for-android.svg?branch=master)](https://travis-ci.org/qtiuto/lua-for-android)
[![Download](https://api.bintray.com/packages/karven/maven/luadroid/images/download.svg) ](https://bintray.com/karven/maven/luadroid/_latestVersion)
<br>

## General

Both lua(5.1-5.3) and luajit(2.0.5 and 2.1.0-beta3) are supported.
     
Configure Android.mk by setting `LUA_LIB :=(lua or luajit)`
to choose the proper version.
     
Currently, lua 5.3.5 is in **lua** dir and luajit 2.1.0 beta-3 is in **luajit** dir.

Load the lib by LibLoader.java in the module app or add `System.loadLibrary("luadroid.so")`
to your code.
     
Module lib is the true library that you can embedded into your code.
Module app is a lua editor for running test in android.
     
## Performance

  Around 2500000 method call(Math.abs) per second on my Oneplus 5 device.
  Around 1800000 member access plus with method call(Math.abs) per second.
  Note that if you run it in debug mode,CheckJni mode is enabled by the
  vm,and it will take away more than half of the efficiency.
  
  Here is the test script:
  ```lua
  using "java.lang"
  --for method call test
  local abs=Math.abs
  local t=os.clock()
  for i=1,2.5e6 do
      abs(0)
  end
  print(os.clock()-t)
  
  --for member access test
  t=os.clock()
  for i=1,1.8e6 do
      Math.abs(0)
  end
  print(os.clock()-t)
  ```
 
## Documentation
   
   For documentation,see [wiki](../../wiki/Home).
     
## Email: 
 <qtiuto@gmail.com>     












