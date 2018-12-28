

#ifndef LUADROID_FUNC_INFO_H
#define LUADROID_FUNC_INFO_H

#include <common.h>
#include "myarray.h"
#include "base_func.h"
#include "Vector.h"
#include "AutoJNIEnv.h"

class JavaType;
class  DeleteOrNotString{
    const char* s;
    bool shouldDel;
public:
    DeleteOrNotString():s(nullptr),shouldDel(false){}

    explicit DeleteOrNotString(const String& str, bool del=false):s(str.data()),shouldDel(del){
        if(del){
            size_t length = str.length()+1;
            char* tmp=new char[length];
            memcpy(tmp, s, length);
            s=tmp;
        }
    }
    explicit DeleteOrNotString(const char* str, bool del=false):s(str),shouldDel(del){
        if(del){
            size_t len=strlen(str)+1;
            char* tmp=new char[len];
            memcpy(tmp,s,len);
            s=tmp;
        }
    }
    DeleteOrNotString(DeleteOrNotString&& other):s(other.s), shouldDel(other.shouldDel){
        other.s= nullptr;
        other.shouldDel=false;
    }

    DeleteOrNotString(const DeleteOrNotString& )= delete;
    DeleteOrNotString&operator=(const DeleteOrNotString& )= delete;
    DeleteOrNotString&operator=(DeleteOrNotString&& other){
        this->~DeleteOrNotString();
        s=other.s;
        shouldDel=other.shouldDel;
        other.s= nullptr;
        other.shouldDel=false;
        return *this;
    }

    operator const char*(){
        return s;
    }

    const char* get(){
        return s;
    }

    bool cached(){
        return !shouldDel;
    }

    ~DeleteOrNotString(){
        if(shouldDel){
            delete [] s;
        }

    }
};
struct Import {
    struct TypeInfo{
        JavaType* type;
        DeleteOrNotString pack;

        TypeInfo():type(nullptr),pack(){}
        TypeInfo(JavaType* type,DeleteOrNotString&&string):type(type),pack(std::move(string)){}
        TypeInfo(TypeInfo&& o):type(o.type),pack(std::move(o.pack)){}
        TypeInfo&operator=(TypeInfo&& o){
            type=o.type;
            pack=std::move(o.pack);
            return *this;
        }
        TypeInfo&operator=(const TypeInfo& o)= delete;
        TypeInfo(const TypeInfo&)= delete;
    };

    typedef Map<String, TypeInfo> TypeCache;
    std::HashSet<String> packages;
    Vector<jobject> externalLoaders;
    TypeCache stubbed;

    Import():packages{"java.lang."
            ,"java.util.","android.view."
            ,"android.widget.","android.app."} {
    }

    Import(Import &&other) : packages(std::move(other.packages)),
                             externalLoaders(std::move(other.externalLoaders)),
                             stubbed(std::move(other.stubbed)) {
    }

    Import(const Import &other) : packages(other.packages),
                                  externalLoaders(other.externalLoaders){
        //No need to copy cache cause it's too large
        if(externalLoaders.size()>0){
            AutoJNIEnv env;
            for(auto iter=externalLoaders.begin();iter!=externalLoaders.end();++iter){
                *iter=env->NewGlobalRef(*iter);
            }
        }
    }

    ~Import(){
        if(externalLoaders.size()>0){
            AutoJNIEnv env;
            for (auto loader:externalLoaders) {
                env->DeleteGlobalRef(loader);
            }
        }
    }
};



class CrossThreadLuaObject;
class FuncInfo : public BaseFunction {
    Import *imported = nullptr;
    Array<CrossThreadLuaObject> upvalues;
public:
    const Array<char> funcData;
    const lua_CFunction cFunc;
    const bool isCFunc;
#if LUA_VERSION_NUM >502
    int globalIndex=0;
#endif

    explicit FuncInfo(const lua_CFunction func) : cFunc(func), isCFunc(true) {}

    explicit FuncInfo(Array<char> &&ptr) : funcData(std::forward<Array<char>>(ptr)), cFunc(nullptr),
                                           isCFunc(false) {}

    FuncInfo(FuncInfo &&other) : upvalues(std::move(other.upvalues)),
                                 funcData(std::move(other.funcData)), cFunc(other.cFunc),
                                 isCFunc(other.isCFunc), imported(other.imported) {
        other.imported = nullptr;
    }

    inline void setImport(const Import *imported) {
        this->imported = imported == nullptr ? new Import() : new Import(*imported);
    }

    inline Import *getImport() const {
        return imported;
    }

    inline void setUpValues(Array<CrossThreadLuaObject> &&upvalues) {
        this->upvalues = std::forward<Array<CrossThreadLuaObject>>(upvalues);
    }

    inline const Array<CrossThreadLuaObject> &getUpValues() const {
        return upvalues;
    }

    FuncInfo &operator=(const FuncInfo &)= delete;

    FuncInfo &operator=(FuncInfo &&o)= delete;

    virtual bool isLocal() { return false; };

    virtual ~FuncInfo() {
        delete imported;
    }
};

class LocalFunctionInfo : public BaseFunction {
    lua_State *L;
public:
    LocalFunctionInfo(lua_State *L) : L(L) {}

    virtual bool isLocal() { return true; };

    virtual ~LocalFunctionInfo() {
        lua_pushlightuserdata(L, this);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
    };
};

#endif //LUADROID_FUNCINFO_H

