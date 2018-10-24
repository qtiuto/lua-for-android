

#ifndef LUADROID_FUNC_INFO_H
#define LUADROID_FUNC_INFO_H

#include <string>
#include <unordered_map>
#include "myarray.h"
#include "base_func.h"
#include "Vector.h"

class JavaType;

struct Import {
    struct TypeInfo{
        JavaType* type;
        const char* pack;
        String cachePack;
    };
    typedef Map<String, TypeInfo> TypeCache;
    std::unordered_set<String> packages;
    Vector<jobject> externalLoaders;
    TypeCache stubbed;

    Import():packages{"java.lang."
            ,"java.util.","android.view."
            ,"android.widget.","android.app."} {
    }

    Import(Import &&other) : packages(std::move(other.packages)),
                             stubbed(std::move(other.stubbed)) {
    }

    Import(const Import &other) : packages(other.packages), stubbed(other.stubbed) {
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

