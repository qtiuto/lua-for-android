
local java=require('java')
local unbox= java.unbox
local _M={
    classBuilder=java.type('com.oslorde.luadroid.ClassBuilder'),
    toTable=function (map)
        local t={}
        for k, v in pairs(map) do
            if(k) then t[unbox(k)]=v; end
        end
        return t;
    end
}
return _M