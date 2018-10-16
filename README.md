[![Build Status](https://travis-ci.org/qtiuto/lua-for-android.svg?branch=master)](https://travis-ci.org/qtiuto/lua-for-android)
## General

Both lua(5.1-5.3) and luajit(2.0.5 and 2.1.0-beta3) are supported.
     
Configure Android.mk by setting `LUA_LIB :=(lua or luajit)`
to choose the proper version.
     
Currently, lua 5.3.5 is in **lua** dir and luajit 2.1.0 beta-3 is in **luajit** dir.
To support languages other than English(cause java support them),I alter the llex.c 
for lua 5.3.5. Thus, all non-ascii characters will be supported(utf-8 only)

Load the lib by LibLoader.java in module app or add `System.loadLibrary("luadroid.so")`
to your code.
     
Module lib is the true library that you can embedded into your code.
Module app is a lua editor for running test in android.
     
## Performance

  Around 2000000 method call(Math.abs) per second on my Oneplus 5 device
  Note that if you run it in debug mode,CheckJni mode is enabled by the
  vm,and it will take away more than half of the efficency.
  
## Documentation
     
### Functions

  The below are functions exported as global values in lua.**'...'** means
  repeating the previous one if following some code or means any if it's 
  inside a body.
  * **import**:
  
    Imports a package or a class. 
    
    Usage: `import "android.os.*" `or `import "android.os.Process"` 
     
    If you import a package then you can call `Type('shortName')` to get
    a type.  For example，
     ```lua
         import "android.os.*"
         local p=Type('Process')
         p.myPid()
         --or 
         local pr=Type('android.os.Process')
         pr.myPid()
     ```
    If you import a Type then the returned value is the type and a
    global value named the short name is set.If the short name contains
    illegal characters like '$' or '-' which are allowed by java class name,
    these characters will be changed to '_'.
    For example，
     ```lua
        local p=import "android.os.Process"
        p.myPid() 
        --or 
        import "android.os.Process"
        Process.myPid()
        --or
        import "android.view.View$OnClickListener"
        print(View_OnClickListener.onClick)
     ```
      By default,**"java.lang.\*"**, **"java.util.\*"**, **"android.view.\*"**,
      **"android.widget.\*"**, and **"android.app.\*"** are imported,but only
      **"java.lang.\*"** is allowed to be overwritten.
      
  * **using**
      Import all of the classes under the specified package, or add a external class loader.
      
      Usage:  `using  'java.lang'`or using(externalClassLoader)
      
      For example:
      ```lua
          using "android.os"
          Process.myPid()
          --or
          using "java.lang"
          using(ClassLoader.getSystemClassLoader())
          --for xposed environment only
          using "de.robv.android.xposed"
      ``` 
      Note:
      The call is pretty time-consuming (about 1s at first call)and will hold a large memory
      to store all loaded class names.On the other hand,it can't make sure that all classes 
      can be found,cause some classloaders may use proxy classloader to loadClass.
      
      Internally,it use `dalvik.system.DexFile#entries` to get all classes.
      
      For illegal characters in class name, see **import**
      
      If you add a external class loader, then any class inside it is available for **type** or **import**.

  * **new**:  
      Constructs a new instance.
      
      Usage: `new(type,args...)`
      
      This is equal to call  `type(args...)`.
      
      e.g.
      ```lua
       import "java.lang.StringBuilder"
       new(StringBuilder,"haha") 
       --or 
       StringBuilder("haha")
      ```
      
  * **newArray**:  
  
      Constructs an array. 
       
      Usage: `newArray(ComponentType,length,initial args...) ` 
      
      The length of initial args must be less than or equal to the length
      
      Alternative: `new(ArrayType,length,initial args...)`
      ,but the previous one is more efficient.
      
      e.g.
      ```lua
        newArray(Type('String'),3,'hfh','hfht')
        --or
        new(Type('int[]'),3,1,2,3)
      ```
      Note:Never pass a type into the initial args.Type cast won't be 
      accepted,since only one type is acceptable.Auto conversion will
      be performed if needed.
      
  * **Type**:  
  
      Returns a type by give className,shortName or class
      
      Usage:`Type(full name)` or `Type(short name)` or` Type(Class Object)`
      ```lua
         Type('java.lang.Object')
         --or
         Type('Object') -- the package must be imported
         --or
         Type(Type('java.lang.Object').class)
      ```
      
      Note:
      In module java the name is "type",the global function is
      changed to upper case to avoid conflict with the internal
      function **type**,which returns the type of a lua object.
      
      To get a primitive type,pass it's name,e.g to get 'int',
      pass in 'int', rather than jni identifier 'I'.
      
      The full name you pass in is the name from Class#getName,and the short Name is
      from getSimpleName. 
      
      Inner class must pass in qualified name like
      **'View$OnClickListener'** instead of **'View.OnClickListener'** or 
      **'OnClickListener'**
      
      You can put in multiple args actually, and the return value is according to
      the args number. e.g.
      ```lua
         ob,str=Type('Object','String') 
      ```
      
  * **instanceof**:  
  
      To check whether the given object is an instance of the give type.
      
      Usage: `instanceof(object,type)`
      
  * **sync**:  
  
      Equals to **sychronized(object){...}**
      
      Usage:`sync(object,function()... end)`
      
      Note:yield is not supported cause it will break lock scope and have
       object unlocked
      
  * **object**:  
  
      Converts a lua object to java object,if no type provided, it works according to
      the following mapping,or it will works according to **Automatic Conversion** section
      The type is passed in like a general java method call.Don't pass in a primitive type,
      or nil will be return.
      
      * number ->java.lang.Long or java.lang.Double,according to the value
      * string ->java.lang.String
      * boolean ->java.lang.Boolean
      * userdata ->null
      * table ->java.util.LinkedHashMap
      * function ->com.oslorde.luadroid.LuaFunction
      * java object is unchanged
      
      Usage:`object(lua object...)
      e.g.
      ```lua
      using 'java.lang'
      i,k=object("Yu",Integer,1)
      --then i is a String object and k is an Integer object 
      ```
      
  * **charString**:  
  
      Converts an integer to an one-character string by its value,e.g.
      10 -> '\n'
      
      Usage:`charString(integer)`
      
  * **charValue**:  
  
      Convert one-character string to a an integer  by its value,e.g.
      '\n'-> 10
      
      Usage:`charValue(on-character string)`
      
  * **try**:  
      Starts java try,both lua error and java exception are caught
      Usage:
      ```lua
       try(body function,[exception type,ecpection handler function]...
           ,["all",all exception handler function],[finally function])
       --or 
       try{
              function () end,--body
              catch=function (e) end --catch all exception,alternative
              or --alternative
              { [exception type 1]=function (e) end,
                [exception type 2]=function (e) end,
                ...
                all=function catchAll(e) end
              },
              finally=function () end--alternative
          }
       ```   
       e.g.
       ```lua
       try{
          function() 
            throw(Type('RuntimeException')("haha"))
          end,
          catch=function (e) throw e end,
          finally=function print "JJ" end
       }
       --or
       try{
          function() 
             throw(Type('RuntimeException')("haha"))
          end,
          catch={
          [Type('RuntimeException']=function (e) print(e) end,
          [Type('Exception']=function (e) print(e) end,
          all=function (e) throw e end
          },
          finally=function print "JJ" end
       }
       ```
       The handler order and re-throw is according to java specifications
       However, you can just try without finally or catch, which means nothing
       
       Note:**yeild** is not supported inside any function you passed in,
        cause it may have finally function un-called
       
  * **throw**:  
  
      Throws a java exception to the code,it will be caught by the above
      try function or try catch in your java code.
      
      Usage: `throw(exception object)`
      
      e.g.
      ```lua
         throw(Type('RuntimeException')("haha"))
     ```
      Notice: the exception must be java obejct of type or sub-type of
      java.lang.Throwable
      
  * **proxy**:  
  
      To implement interfaces ,alternatively plus with extending a class
      Usage:
      ```lua
      proxy([class to extend or object of class to extend],
      [interfaces to implement]..., [method name,parameter types...,
      handler function]..., [shared classloader],--for extension type only
      [initial args for constructor])--for extension type only
      ```
      or 
      ```lua
      proxy{
       --auto dectect,nilable
       super=class to extends or implement or object of class to extend,
       
       interfaces={interfaces to implement},--nilable
       
       --super and interfaces can't be nil at the same time
       
       methods=function (...) end,--function to handle all method call
       or {
        methodName=function(...) end,--function to handle all method
        --with the same name
        or
        {
         [param types...,function (...) end]...--handle specific method
        },
       } ,
       
       --alternative,default is false for extension type only
       shared=true or false, 
       
       --alternative for extension type only,constrctor args
       args={...}
      }
      ```
      e.g.
      ```lua
      proxy(Type('Runanble'),function() print 'ty' end).run()
      --or
      proxy(Type('Runanble'),'run',function() print 'ty' end).run()
      --or
      local obj=Type('ArrayList')(5)
      proxy(obj,'equals',function(obj,thiz) return obj==thiz end).
      equals(obj)
      --or
      proxy{
       super=Type('ArrayList'),
       methods={
        equals=function(obj,thiz) return obj==thiz end
       },
       shared=false,
       args={5}
      }
      ```
      ##### Note:
      
       If you passed an object to extend,then the proxy object is 
       directly allocated and have all fields from the object to be
       copied into the return object without constructor call.
       See `ClassBuilder#cloneFromSuper` for more information
       
       A value representing the proxy object is appended after args during
       function callback
       
       For extension usage,a value represents the super is appended
       
       Interface implentation is supported by java.lang.reflect.Proxy
       but class extension is supported by dexmaker, which may consume
       several time to generate and load a dex file.
      
       Whether the object is multi-thread supported is determined by
       whether localFunction is set in the constructor of ScriptContext
    
## Multi-thread Support

   A userdata named 'cross' is imported to support cross-thread communication.
   It behaves like a table,so you can just put any lua object to 
   it.
   
   Note that userdata is shared by memory copy so if it has a _gc
   metamethod,error will be raised, since it may induce memory corrupt
   ion.Corutine can't be shared also since it has it's own lua state,
   and a lua state can't be shared cross thread.Take care that all key 
   will be converted to string.
     
## Member Access
     
   Once you have a type from import or Type,you can access its  static 
   fields  like **Type.name** or  **Type\['name']** and its methods like 
   **Type.name(...)** or **Type\['name'](...)**. Once you got a java object 
   you can do the similar operation to its non-static fields or methods.
     
   However,if the class has some fields and methods with the same name,
   you can't get or set the field by Type.name or obj.name. Instead,
   you must use **(Type or obj).name\[any value you like]** to use the field.
   
   Another special case is that the field with the same name but
   different types,which is not allowed by the compiler but ok in jvm.
   To distinguish it,use **(Type or obj).name\[according type]** to operate
   on the field.
     
   ##### Note:  
   **Type.class** will return the Class object represented by the type
   
   **'.length'** of an array object will return its length,you can use '#' 
   operator to get the length of an array object also.'#' operator also
   works for any object with **length()** or **size()** method decalred.
      
      
   The lua method **'tostring'** works for any java object,and toString method
   will be invoked.
      
   **'=='** operator for java object will always return true if the two object
   is the same java object
   
   **'..'** operator will concat a java object with any lua object with **tostring**
   to be called,and return a lua string.
   
   The lua method **'pairs'** works for any java object work **Collection** or **Map**
   or **SparseArray** or **Array**, or you can add custom iterator.
   
   You can index any java object with **set/put get/at** methods.When you index or add an 
   index these two method will be invoked,or you can add custom indexer.
   
   ```lua
      using "java.util"
      local arr=HashMap({a=5,b=6})
      arr.c='kkk'
      arr[2]=8
      print(#arr,arr[])
   ```
     
## Type Specification
   You can add a type before the arg in method call,new(),newArray
   or the args in proxy(),so as to indicate the type you wish the
   argument to be, and then  have the proper method or constructor to
   be called.
      
## Method Deduction
   If type is supplied,the closet type is considered first.
     
   For integer value,if it's in int32 range,then it's aussumed to be 
   int.The rank is according to which range is closer to the value.
   Float type can accpet integer value if no integer type found and 
   float is considered before double.For long type below lua 5.3,a 
   library call 'i64' is imported to reprsent value can't be represent
   by double,so long type accept i64 value also.Boxed type will be used
   if no primitive type found.
     
   For float value,double has more weight than float.
     
   For string,if the type is char and the string length is one,then
   ranks first,string ranks second,followed by any assignble from
   java.lang.String.
     
   For nil,treated as (Type)null;
     
   For java object,evaluating in terms of class hierarchy.Specically,
   if no object type found for a boxed type, primitive type will be
   in consideration.
     
   For function,any functional interface is considered ok
     
   For table,if the type is convertible from table in registered
   table converters,the type is considered first,or if the value have the
   below layout 
   
   ```lua
      {methodName=function() end...}
   ```
   and the type is interface,
   then it's considered as an implementation for the interface.
   
   User date is treated as integer.
     

## Automatic Conversion
    
   For field set and method call or proxy return,lua object will be converted
   automatically according to the type.Type check ha thse same rule in 
   method deduction.If type check failed,an exception may be thrown
     
   For primitive types,like a static_cast<type>(value).
     
   For char type, only one-character is acceptable,and return value
   will be converted to one-character string.
     
   For object types,no conversion.
     
   For function,like call proxy with only one type and one function
   provided.
     
   For table, if the table is convertible, it's converted by the table converter
   else it's like call proxy with super set to the type and methods set to 
   the table if it's valid to be converted to be proxied. Additionally, generic type
   check and conversion will be performed automatically, That's to say, Type such as 
   List<Integer>,List<Short> or List<List<String>> will be supported.
     
   User date is treated as integer.
      
   Note: All primitive type accept their boxed type and all boxed type
   accept their primitive type.Autobox and auto-unbox will be performed
   if necessary.
          
## Module name in lua
     
   The module java is loaded once the lua state initialize with
   all its members exported to the global table if you specified
   importAllFunctions (default true) in the constructor of ScriptContext.
   And global value **"java"** refers to the table.Note that
   **java.type** is exported as **‘Type’** to avoid conflict.
   e.g.
   
   
   ```lua
     print(print(java.type == Type)--true
     print(java.import == import)--true
     print(java.new == new)--true
   ```
      
## ScriptContext Api
     
   * constructor:see Module name in lua and proxy
     
   * compile:return a Compiled Script you can pass to run
     
   * run:run a script with several arguments,and all it returned is stored
     in an Object[] and values are converted the same as **object()** function
     with no type provided
     
   * changeCallSite: change the method you wish the lua function to
     agent,so that it can accept proper arguments and you can receive
     proper result.
     
   * putTableConverter:put a converter to support automatic table conversion
   
   * putIndexer:put an indexer to support index or new index operation
   
   * putIteratorFactory:put an iterator factory to generate a proper iterator
    
   e.g.
   
   ```java  
    import com.oslorde.luadroid.*;
    ScriptContext context=new ScriptContext();
    Object[] results = context.run("print 'Hello World'");
   ```
## ClassBuilder Api
   Class Builder is imported default to support dynamic class generation
   Lua function callback rule is the same as proxy.
   e.g.
   
   ```lua
    ClassBuilder.declare().addMethod("run:,"V",function () print "ggg" end)
   .newInstance(Type("Object")()).run()
   ```
## More     
   For more information,see java doc in **doc** directory
     
      
## Email: 
 <qtiuto@gmail.com>     












