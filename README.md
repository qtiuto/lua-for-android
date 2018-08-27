    Both lua(5.1-5.3) and luajit(2.0.5 and 2.1.0-beta3) are supported.
     
    Configure Android.mk by setting LOCAL_STATIC_LIBRARIES :=(lua or luajit)
    to choose the proper version.
     
    Currently, lua 5.3.5 is in \lua and luajit 2.1.0 beta-3 is in \luajit.
    Load the lib by LibLoader.java or add System.loadLibrary("luadroid.so")
    to your code.
     
    A little Editor is embedded in the code,you can run it to test.
     
    Performance:
     Around 1900000 method call(Math.abs) per second on my Oneplus 5 device
     Note that if you run it in debug mode,CheckJni mode is enabled by the
     vm,and it will take away more than half of the efficency.
     
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
      Note:Never pass a type into the initial args.Type cast won't be 
      accepted,since only one type is acceptable.Auto converion will
      be performed if needed.
      
     Type:
      return a type by give className,shortName or class
      usage:Type("java.lang.Object") or Type("Object") or Type(ObjectClass)
      Notice:in module java the name is "type",the global function is
      changed to upper case to avoid confiction with the internal
      function type,with return the type of a lua object.
      Note:To get a primitive type,pass it's name,e.g to get 'int',
      pass int, rather than jni identifier 'I'.For array type,
      you must pass in full name,rather than shortName.The full name 
      you pass in is the name from Class#getName,and the short Name is
      from getSimpleName.Inner class must pass in qulified name like
      'View$OnClickListener' instead of 'View.OnClickListener' or 
      'OnClickListener'
      
     instanceof:
      to check whether the given object is an instance of the give type.
      usage: instanceof(object,type)
      
     sync:
      equals to sychronized(object)
      usages:sync(object,function()... end)
      Note:yield is not supported cause it will break lock scope and have
       object unlocked
      
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
       Note:yeild is not supported inside any function you passed in,
        cause it may have finally function un-called
       
     throw:
      throw a java exception to the code,it will be caught by the above
      try function or try catch in your java code.
      usage: throw(exception object)
      Notice: the exception must be java obejct of type or sub-type of
      java.lang.Throwable
      
     proxy:
      to implement interfaces ,alternatively plus with extending a class
      usage:proxy([class to extend or object of class to extend],
      [interfaces to implement]..., [method name,parameter types...,
      handler function]..., [shared classloader],--extend type only
      [initial args for constructor])--extend type only
      or proxy{
       super=class to extends or implement or object of class to extend,
       --auto dectect,nilable
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
       if you passed an object to extend,then the proxy object is 
       directly allocated and have all fields from the object to be
       copied into the proxied object without constructor call.
       See ClassBuilder#cloneFromSuper for more informations
       
       A value represent the proxy object is appended after args during
       function callback
       
       For extension usage,a value represent the super is appended
       
       Interface implentation is supported by java.lang.reflect.Proxy
       but class extension is support by dexmaker, which may consume
       several time to generate and load a dex file.
      
       Whether the object is multi-thread supported is determined by
       whether localLunction is set in the constructor of ScriptContext
    
    Cross thread support
     A userdata named 'cross' is imported to support cross thread communi-
     cation.It behaves like a table,so you can just put any lua object to 
     it.Note that userdata is shared by memory copy so if it has a _gc
     metamethod,error will be raised, since it may induce memory corrupt-
     ion.Corutine can't be shared also since it has it's own lua state,
     and a lua state can't be shared cross thread.Take care that all key 
     will be converted to string.
     
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
     
     Note:
       Type.class will return the Class object represented by the type
       '.length' of an array object will return its length,you can use '#' 
       operator to get the length of an array object also.'#' operator also
       works for Collection,Map,JsonArray and SparseArray.
       The lua method tostring works for any java object,and toString method
       will be invoked.
       '==' operator for java object will always return true if the two object
       is the same java object
       '..' operator will concat a java object with any lua object with tostring
       to be called,and return a lua string.
       
     
    Type Specifaction:
         You can add a type before the arg in method call,new(),newArray
     or the args in proxy(),so as to indicate the type you wish the
     argument to be, and then  have the proper method or constructor to
     be called.
      
    Method deduction:
     If type is supplied,the closet type is considered first.
     
     For integer value,if it's in int32 range,then it's aussumed to be 
     int.The rank is according to which range is closer to the value.
     Float type can accpet integer value if no integer type found and 
     float is considered before double.For long type below lua 5.3,a 
     library call 'i64' is imported to reprsent value can't be represent
     by double,so long type accept i64 value also.Boxed type will be used
     if no primitive type found.
     
     For float value,double have more weight than float.
     
     For string,if the type is char and the string length is one,then
     ranks first,string ranks second,followed by any assignble from
     java.lang.String.
     
     For nil,treated as (Type)null;
     
     For java object,evaluating in terms of class hierarchy.Specically,
     if no object type found for a boxed type, primitive type will be
     in consideration.
     
     For function,any functional interface is considered ok
     
     For table,if the type is convertiable from table in registered
     table converters,the type is considered first,or the value have the
     below layout{[methodName=function () end]...},and the type is inter-
     face then it's considered right.
     User date is treated as integer.
     

    Automatic Conversion:
    
     For field set and method call or proxy return,lua object will be converted
     automatically according to the type.Type check have the same rule in 
     method deduction.If type check failed,an exception may be thrown
     
     For primitive types,like a static_cast<type>(value).
     
     For char type, only one-character is acceptible,and return value
     will be converted to one-charater string.
     
     For object types,no conversion.
     
     For function,like call proxy with only one type and one one function
     provided.
     
     For table ,if the table convertible,it's converted by the table converter
     else it's like call proxy with super set to the type and methods set to 
     the table.Additionally,generic type check and conversion will be performed
     automatically,That's to say,Type such as List<Integer>,List<Short>
     or List<List<String>> will be supported.
     
     User date is treated as integer.
      
     Note: All primitive type accept their boxed type and all boxed type
     accept their primitive type.Autobox and auto-unbox will be performed
     if necessary
          
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
     
    ClassBuilder api:
     Class Builder is imported default to support dynamic class generation
     Lua function callback rule is the same as proxy
     
    For more information,see java doc in doc directory  
      
     












