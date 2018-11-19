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

  Around 2100000 method call(Math.abs) per second on my Oneplus 5 device.
  Note that if you run it in debug mode,CheckJni mode is enabled by the
  vm,and it will take away more than half of the efficiency.
  
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
    the character **'$'**  which is used by inner class name,
    the character will be changed to **'_'**.If the short name contains **'-'** or
    **non-ascii** characters, the global value won't be set, but you can still
    use the return value to get it;
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
      
  * **using**:
  
      Import all of the classes under the specified package, or add a external class loader.
      See **import** for the import way.
      
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
      The function is kind of time-consuming, especially at first call(about 40ms  on my device),
      and will hold a large memory(around 1.5 m) to store all loaded class names.On the other hand,
      it can't make sure that all classes can be found,cause some classloaders may use 
      proxy classloader to load classes.
      
      Internally,it gets all classes like `dalvik.system.DexFile#entries` .
      For DexFile constructor has has been deprecated since Android O and unusable
      for boot class loader since Android P, I turned to read boot dex files from ClassLinker
      and return class list from classdefs since Android L. Dalvik need no optimization as no sort is
      performed, and current art runtime has been optimize by getClassList.
      
      For **non-ascii** characters or '$' or '-' in class name, see **import**
      
      If a previously imported class has the same name with the a class under the package,
      then the class won't be imported, but the previous class will be set to the globle table.
      
      If you add a external class loader, then any class inside it is available for **type** or **import**.

  * **new**: 
   
      Constructs a new instance.
      
      Usage: `new(type,args...)`
      
      This is equal to call  `type(args...)` or `type.new(args...)`.
      
      e.g.
      ```lua
       import "java.lang.StringBuilder"
       new(StringBuilder,"haha") 
       --or 
       StringBuilder("haha")
       --or
       StringBuilder.new("haha")
      ```
      
  * **newArray**:  
  
      Constructs an array. 
       
      Usage: `newArray(ComponentType,length,initial args...) ` 
      
      The length of initial args must be less than or equal to the length. 
      If the length==-1 then the length = the length of initial args
      
      Alternative: `new(ArrayType,length,initial args...)`
      ,but the previous one is more efficient.
      
      e.g.
      ```lua
        newArray(Type('String'),3,'hfh','hfht')
        --or
        new(Type('int[]'),-1,1,2,3)
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
      
      The full name you pass in is the name from **Class#getName**,and the short Name is
      from **Class#getSimpleName**. 
      
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
      
      Equals to `object.instanceof(type)`
      
      Usage: `instanceof(object,type)`
      
  * **sync**:  
  
      Equals to **synchronized(object){...}**
      
      Usage:`sync(object,function()... end)`
      
      Note: before lua 5.3, **yield** is not supported cause it will break lock scope and have
       object unlocked
      
  * **object**:  
  
      Converts a lua object to java object,if no type provided, it works according to
      the following mapping,or it will works according to **Automatic Conversion** section
      The type is passed in like a general java method call. Primitive type is treated as its box type.
      
      * number ->java.lang.Long or java.lang.Double,according to the value
      * string ->java.lang.String
      * boolean ->java.lang.Boolean
      * userdata ->null
      * table ->java.util.LinkedHashMap
      * function ->com.oslorde.luadroid.LuaFunction
      * java object is unchanged
      
      Usage:`object(lua object...)`
      
      e.g.
      ```lua
      using 'java.lang'
      i,k=object("Yu",Integer,1)
      --then i is a String object and k is an Integer object 
      ```
  
  * **unbox**
      UnBox a boxed value,e.g. java.lang.Integer-> int
      Object will not be changed when unboxing.This method
      is designed for map operation for type like SparseArray.
      
      Usage: 'unbox(boxedValue)'
      
  * **charString**:  
  
      Converts an integer to an one-character string by its value,e.g.
      10 -> '\n'
      
      Usage:`charString(integer)`
      
  * **charValue**:  
  
      Convert one-character string to an integer by its value,e.g.
      '\n'-> 10
      
      Usage:`charValue(on-character string)`
      
  * **try**:
    
      Starts java try,both lua error and java exception are caught.
      
      Usage:
      
      ```lua
       try(body function,[exception type,ecpection handler function]...
           ,["all",all exception handler function],[finally function])
       --or 
       try{
              function () end,--body
              catch=function (e) end --catch all exceptions,alternative
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
             throw "haha"
          end,
          catch={
          [Type('RuntimeException']=function (e) print(e) end,
          Exception=function (e) print(e) end,
          all=function (e) throw e end
          },
          finally=function print "JJ" end
       }
       ```
       
       The handler order and re-throw is according to java specifications
       However, you can just try without finally or catch, which will return
       the error code and the exception on error and nil on the other cases.
       The type can be a type name string rather than a java type.
       
       Note:**yeild** is supported inside any function you passed in since lua 5.3,
        but finally function may be un-called if yield not return. 
        Lua built-in function **pcall** can also catch the error,
        while **try** will append stacktrace info to the error but **pcall** won't. 
       
  * **throw**:  
  
      Throws a java exception, which ,while not an exception, will be converted to a string and
      wrapped as a **LuaException**, and finally, it will be caught by the above try function or 
      try catch in your java code.
      
      Usage: `throw(exception object)`
      
      e.g.
      ```lua
         throw(Type('RuntimeException')("haha"))
         throw "Ha Ha"
     ```
      Notice:
        If you use **pcall** you will receive a java exception object as error info,
        and the exception won't be caught by upper **try** anymore.
        The call **throw(java excption object)** has the close meaning as **error(java exception object)**
        but differs by  throw adding stacktrace info to the exception while error won't
      
  * **proxy**:  
  
      To implement interfaces ,alternatively plus with extending a class
      
      Usage:
      
      ```lua
      proxy([class to extend or object of class to extend],
       [interfaces to implement]..., 
       [method name,parameter types...,handler function]...,--or
       [function] --for functional interface only
       [shared classloader],--for extension type only
       [initial args for constructor]...)--for extension type only
      ```
      or 
      ```lua
      proxy{
       --auto detected,nilable
       super=class to extend or implement, object of class to extend,
       
       interfaces={interfaces to implement},--nilable
       
       --super and interfaces can't be nil at the same time
       
       methods=function (...) end,--function to handle the only method of a functional interface
       --or 
        methodName=function(...) end,--function to handle all method with the same name
        --or
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
      
     **Note:**
      
       If you passed an object to extend,then the proxy object is 
       directly allocated and have all fields from the object to be
       copied into the return object without constructor call.
       See `ClassBuilder#cloneFromSuper` for more information
       
       A value representing the proxy object is appended after args during
       function callback
       
       For extension usage,a value represents the super is appended
       
       Interface implementation is supported by **java.lang.reflect.Proxy**
       but class extension is supported by **dexmaker**, which may consume
       several time to generate and load a dex file.
      
       Whether the object is multi-thread supported is determined by
       whether localFunction is set in the constructor of ScriptContext
       
       When the object is multi-thread supported, then when function is
       saved, its upvalues will be copied in. However, since the global
       values it refers to can't be determined, make sure the function
       doesn't refers to any value it can't access in another thread.
    
### Multi-thread Support

   A userdata named 'cross' is imported to support cross-thread communication.
   It behaves like a table,so you can just put any lua object to 
   it.
   
   Note that userdata is shared by memory copy so if it has a _gc
   metamethod,error will be raised, since it may induce memory corrupt
   ion.Corutine can't be shared also since it has it's own lua state,
   and a lua state can't be shared cross thread.Take care that all key 
   will be converted to string.Never put a value has references to the 
   global table, or a stack overflow error may occur. 
     
### Member Access
     
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
   
   Google ban access to some black list members and warn about gray list members since Android Pie.
   However, it makes me feel sad cause logcat is performance killer, so I 
   disable this feature once the lib is loaded. 
     
   **Note:**
     
   **Type.class** will return the class object represented by the type
   
   **Type.new(args...)** will construct a new object like **new** does if no
   static field or method named new in that type.This is syntax sugar for **new**. 
   
   **Type.assignableFrom(anotherType)** will return whether the object of this type can
    be cast to the other type if no static field or method named assignableFrom in that type.
     
   **javaObject.instanceof(givenType)** will return whether the object  can
   be cast to the given type if no non-static field or method named instanceof in that type.
   This is a short way for the **instanceof** method.
        
   **'.length'** of an array object will return its length,you can use **'#'** 
   operator to get the length of an array object also.'#' operator also
   works for any object with **length()** or **size()** method declared.
      
   The lua method **'tostring'** works for any java object and java type,and **toString()** 
   method will be invoked.
      
   **'=='** operator for java object will always return true if the two object
   is the same java object
   
   **'..'** operator will concat a java object with any lua object with **tostring**
   to be called,and return a lua string.
   
   The lua method **'pairs'** works for any java object work **Collection** or **Map**
   or **SparseArray** or **Array**, or you can add custom iterator,**false** is return 
   for **null** key or value. Though I don't add a function to convert a map to lua table,
   you can use iteration to work on it like the below
   ```lua
   function toTable(map)
      local t={}
      for k,v in ipairs(map) do
          t[k]=v
      end
      return t
   end
   ```
   
   You can index any java object with **set/put**,**get/at** methods.When you index or add an 
   index, these two methods will be invoked,or you can add custom indexer.
   For example,
   
   ```lua
      using "java.util"
      local arr=HashMap({a=5,b=6})
      arr.c='kkk'
      arr[2]=8
      print(#arr,arr[2])
      for k,v in ipairs(arr) do
         print(k,v)
      end
   ```
   
   If an object has **getter/setter** method for an non-static property without associated field, 
   then you can operator on the property like a true field. However If the property has only getter method,
   then set operation will raise an error,and vise versa.
   For Example,
   
   ```lua
      using "android.view"
      using "android.widget"
      view=TextView(context)
      view.visibilty=View.GONE
      print(view.visibilty)
   ```
   
### Type Specification
   You can add a type before the arg in method call,new(),newArray
   or the args in proxy(),so as to indicate the type you wish the
   argument to be, and then  have the proper method or constructor to
   be called.
      
### Method Deduction
   If type is supplied,the closet type is considered first.
     
   For integer value,if it's in int32 range,then it's assumed to be 
   int.The rank is according to which range is closer to the value.
   Float type can accept integer value if no integer type found and 
   float is considered before double.For long type below lua 5.3,a 
   library call 'i64' is imported to represent value can't be represent
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
   For a convertible type, no type can be superior to the other,
   so for specified class like Arrays.copyOf, where all th array types
   has the same weight, which method is chosen is unspecified. To clarify
   your intention, add a type before the table when calling a method.
   
   User date is treated as integer.
     

### Automatic Conversion
    
   For field set and method call or proxy return,lua object will be converted
   automatically according to the type.Type check ha thse same rule in 
   method deduction.If type check failed,an exception may be thrown
     
   For primitive types, value in its range and associated box type.
     
   For char type, only one-character is acceptable,and return value
   will be converted to one-character string.
   
   Object.class receives all values including primitive types.
   
   Number.class receives all primitive types and its sub class.
     
   For object types,no conversion.
     
   For function,like call proxy with only one type and one function
   provided.
     
   For table, if the table is convertible, it's converted by the table converter
   else it's like call proxy with super set to the type and methods set to 
   the table if it's valid to be converted to be proxied. Additionally, generic type
   check and conversion will be performed automatically, That's to say, 'Type' such as 
   List<Integer>,List<Short> or List<List<String>> will be supported.
   
   Actually, the  table converter is a little bit inefficient cause it needs to
   store a lua table to a c++ vector and then to a java map and finally to the object specified
   If you really care about efficiency, avoid using table auto cast.
     
   User date is treated as integer.
      
   Note: All primitive types accept their boxed types and all boxed types
   accept their primitive types.Autobox and auto-unbox will be performed
   if necessary.
          
### Module Name in Lua
     
   The module java is loaded once the lua state initialize with
   all its members exported to the global table if you specified
   importAllFunctions (default true) in the constructor of ScriptContext.
   And the global value **java** refers to the table. Note that
   **java.type** is exported as **Type** to avoid conflict.
   e.g.
   
   
   ```lua
     print(java.type == Type)--true
     print(java.import == import)--true
     print(java.new == new)--true
   ```
      
### ScriptContext Api
     
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
   
### ClassBuilder Api
   Class Builder is imported default to support dynamic class generation.
   Lua function callback rule is the same as proxy.
   e.g.
   
   ```lua
    ClassBuilder.declare().addMethod("run:,"V",function () print "ggg" end)
   .newInstance(Type("Object")()).run()
   ```
   
   It doesn't support generating static methods or constructors cause class object
   won't be freed in dalvik. When the mini sdk version of this project turn to 21,
   I will add support for them.
 ###LUA FILE SYSTEM API
   Module lfs is imported default to support chdir operation.
   Check [here](http://keplerproject.github.io/luafilesystem/manual.html#reference)
   for documenation
   
## More     
   For more information,see java doc in **doc** directory
     
## Email: 
 <qtiuto@gmail.com>     












