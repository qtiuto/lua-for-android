[![Build Status](https://travis-ci.org/qtiuto/lua-for-android.svg?branch=master)](https://travis-ci.org/qtiuto/lua-for-android)
[![Download](https://api.bintray.com/packages/karven/maven/luadroid/images/download.svg) ](https://bintray.com/karven/maven/luadroid/_latestVersion)
<br>

## General

This is a friendly and high performance bridge for java and lua in android. With it, you can call java 
functions, use java objects easily in lua. It supports auto method deduction, auto data conversion, 
import java package/class, multi-thread, exception handling, class extension/implementation and other facilitative
features. See [wiki](../../wiki/Home) for more detailed description.

Module **luaffi**,  **luasec**,  **luasocket** are optional package. And module **lfs** is embeded.

Both lua(5.1-5.3) and luajit(2.0.5 and 2.1.0-beta3) are supported.
     
Configure Android.mk by setting `LUA_LIB :=(lua or luajit)`
to choose the proper version.
     
Currently, lua 5.3.5 is in **lua** dir and luajit 2.1.0 beta-3 is in **luajit** dir.

Load the lib by LibLoader.java in the module app or add `System.loadLibrary("luadroid.so")`
to your code.
     
Module **lib** is the true library that you can embedded into your code.

Module **app** is a lua editor for running test in android.

Module **luaffi** is the ffi lib for luadroid to invoke c apis, and it requires luadroid.so to be statically linked
     
## Performance

  Under release mode, the bridge around 3+ times slower than jni  because of consumption
  in method deduction and data conversion. Yet, if no argument provided, it is almost as fast as jni.
  
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












