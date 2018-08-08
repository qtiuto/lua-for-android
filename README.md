    Both lua(5.1-5.3) and luajit(2.0.5 and 2.1.0-beta3) are supported.
    Configure Android.mk by setting LOCAL_STATIC_LIBRARIES :=(lua or luajit)
    to choose the proper version.
     
    Currently, lua 5.3.5 is in \lua and luajit 2.1.0 beta-3 is in \luajit.
    Load the lib by LibLoader.java or add System.loadLibrary("luadroid.so")
    to your code.
     
    A little Editor is embedded in the code,you can run it to test.
     
    Performance:
     700000 method call per second on my Oneplus 5 device
    Functions:
     
     import:
      import a package or a class.
      usage: import "android.os.*" or import "android.os.Process"
      if you import a package then you can call Type(shortName) to get
      a type,but no value returned.
      If you import a Type then the returned value is the type and a
      global value named the short name is set.For example
        local p=import "android.os.Process"
        p.myPid() --or Process.myPid()
      By default,"java.lang.*","java.util.*","android.view.*",
      "android.widget.*",and "android.app." are imported,but only
      "java.lang.*" is allowed to be overwritted.
     new:
      construct a new instance.
      usage: new(Type,args...)
      This is equal to call Type(args...).
      e.g.
        import "java.lang.StringBuilder"
        new(StringBuilder,"haha") or StringBuilder("haha")
        
     newArray:
      construct an array.
      usage: newArray(ComponentType,length,initial args...)
      The length of inital args must be less than or equal to the length
      Alternative: new(ArrayType,,length,initial args...)
      
     Type:
      return a type by give className,shortName or class
      usage:Type("java.lang.Object") or Type("Object") or Type(ObjectClass)
      Notice:in module java the name is "type",the global function is
      changed to upper case to avoid confiction with the internal
      function type,with return the type of a lua object.
      
     instanceof:
      to check whether the given object is an instance of the give type.
      usage: instanceof(object,type)
      
     lock/unlock:
      equals to sychronized(object),don't forget to unlock after lock
      usages:lock(object) code unlock(object)
      
     object:
      convert a lua object to java object,here is the mapping
      number ->java.lang.Long or java.lang.Double,according to the value
      string ->java.lang.String
      boolean ->java.lang.Boolean
      userdata ->java.lang.Long,the value is its address
      table ->java.util.HashMap
      function ->com.oslorde.luadroid.LuaFunction
      java object is unchanged
      usage:object(lua object)
      
     charString:
      convert an integer to an one-character string by its value,e.g.
      10 -> '\n'
      usage:charString(integer)
      
     charValue:
      convert one-character string to a an integer  by its value,e.g.
      '\n'-> 10
      usage:charValue(on-character string)
      
     try:
      start java try,both lua error and java exception are caught
      usage:
       try(body function,[exception type,ecpection handler function]...
           ,["all",all exception handler function],[finally function])
       or try{
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
       The handler order and re-throw is according to java specification
       However, you can just try without finally or catch
       
     throw:
      throw a java exception to the code,it will be caught by the above
      try function or try catch in your java code.
      usage: throw(exception object)
      Notice: the exception must be java obejct of type or sub-type of
      java.lang.Throwable
      
     proxy:
      to implement interfaces ,alternatively plus with extending a class
      usage:proxy([class to extend],[interfaces to implement]...,
      [method name,parameter types...,handler function]...,
      [shared classloader],--extend type only
      [initial args for constructor])--extend type only
      or proxy{
       super=class to extends or implement,--auto dectect,nilable
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
       shared=true or false, --alternative,default is false
       args={...}---alternative
      }
      Notice:
       A value represent the proxy object is appended after args
       
       For extension usage,a value represent the super is appended
       
       Interface implentation is supported by java.lang.reflect.Proxy
       but class extension is support by dexmaker, which may consume
       several time to generate and load a dex file.
      
       Whether the object is multi-thread supported is determined by
       whether localLunction is set in the constructor of ScriptContext
     put
      put an object to the ScriptContext,so you can get it from another
      thread,all data can be transfered,including metatable.
      usage:put(name,lua object)--the object is nilable
      Notice:
      userdata is transfered by memory copy.If it has a __gc metamethod
      some data it refers to may be corrupted and put will failed with
      error .So make sure the data is muli-thread-able
     get
      push a previously saved data to current lua state,with the name as
      a global key.every call to it will push a new value.You can call it
      from the same thread you put it.
      usage: get(name)
      
     remove
      remove a previously save data.
      
    Member Access:
     
     Once you have a type from import or Type,you can access its  static 
     fields and methods like Type.name or Type.name(...) or Type['name'] 
     or Type['name'](...).Once you got a java object you can do the simi-
     liar opertation to its Object fields or methods.
     
     However,if the class have some field and method with the same name,
     you can't get or set the field by Type.name or obj.name.Instead,
     you must use (Type or obj).name[any value you like] to use the field.
     Another special case is that the field with the same name but
     different types,which is not allowed by the complier but ok in jvm.
     To distinguish it,use (Type or obj).name[according type] to operate
     on the field.
     
    Type Specifaction:
         You can add a type before the arg in method call,new(),newArray
     or the args in proxy(),so as to indicate the type you wish the
     argument to be, and then  have the proper method or constructor to
     be called.
      
    Method deduction:
     If type is supplied,the closet type is considered first.
     
     For integer value,if it's in int32 range,then int ranks first, or
     the rank is according to which range is closer to the value.Float
     type can accpet integer value if no integer type found and float
     is considered berfore double.For long type below lua 5.3,a library
     call 'i64' is imported to reprsent value can't be represent by
     double,so long type accept i64 value also.
     
     For float value,double have more weight than float.
     
     For string,if the type is char and the string length is one,then
     ranks first,string ranks second,followed by any assignble from
     java.lang.String.
     
     For nil,treated as (Type)null;
     
     For java object,evaluating in terms of class hierarchy.
     
     For function,any functional interface is considered ok
     
     For table,if the type is convertiable from table in registered
     table converters,the type is considered first,or the value have the
     below layout{[methodName=function () end]...},and the type is inter-
     face then it's considered right.
     User date is treated as integer.
     

    Automatic Conversion:
    
     For field set and method call,lua object will be converted.
     automatically according to the parameter.Type check have the same
     rule in method deduction.
     
     For primitive types,like a static_cast<type>(value).
     
     For char type, only one-character is acceptible,and return value
     will be converted to one-charater string.
     
     For object types,no conversion.
     
     For function,like call proxy with only one type and one one function
     provided.
     
     For table ,if the table convertible,it's converted by the table converter
     else it's like call proxy with super set to the type and methods set to 
     the table.
     
      User date is treated as integer.
          
    Module name in lua:
     
     The module java is loaded once the lua state initialize with
     all its members exported to the global table if you specified
     importAllFunctions (default true) in the constructor of Script-
     Context.And global value "java" refers to the table.Note that
     java.type is exported as Type to avoid confiction.
      
    ScriptContext api:
     
     constructor:see Module name in lua and proxy
     
     compile:return a Compiled Script you can pass to run
     
     run:run a scirpt with serval arguments,and all it returned is stored
     in an Object[] and values are converted the same as object() function
     
     changeCallSite: change the method you wish the lua function to
     agent,so that it can accept proper arguemnts and you can receive
     proper result.
     
     putTableConverter:put a converter to support automatic table conversion












