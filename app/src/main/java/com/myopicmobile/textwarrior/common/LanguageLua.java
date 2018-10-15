/*
 * Copyright (c) 2013 Tah Wei Hoon.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License Version 2.0,
 * with full text available at http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * This software is provided "as is". Use at your own risk.
 */
package com.myopicmobile.textwarrior.common;

/**
 * Singleton class containing the symbols and operators of the Javascript language
 */
public class LanguageLua extends Language {
    private final static String keywordTarget = "and|break|do|else|elseif|end|false|for|function|goto|if|in|local|nil|not|or|repeat|return|then|true|until|while";

      private final static String globalTarget = "__add|__band|__bnot|__bor|__bxor|__call|__concat|__div|__eq|__idiv|__index|__le|__len|__lt|__mod|__mul|__newindex|__pow|__shl|__shr" +
              "|__sub|__unm|_ENV|_G|assert|collectgarbage|dofile|error|findtable|getmetatable|ipairs|load|loadfile|loadstring|module|next|pairs" +
              "|pcall|print|rawequal|rawget|rawlen|rawset|require|select|self|setmetatable|tointeger|tonumber|tostring|type|unpack|xpcall";
    private final static String packageName = "coroutine|debug|io|luajava|math|os|package|string|table|utf8";
    private final static String package_coroutine = "create|isyieldable|resume|running|status|wrap|yield";
    private final static String package_debug = "debug|gethook|getinfo|getlocal|getmetatable|getregistry" +
            "|getupvalue|getuservalue|sethook|setlocal|setmetatable|setupvalue|setuservalue|traceback|upvalueid|upvaluejoin";
    private final static String package_io = "close|flush|input|lines|open|output|popen|read|stderr|stdin|stdout|tmpfile|type|write";
    private final static String package_java = "type|newArray|proxy|instanceof|import|object|charString|charValue|new|sync|try|throw|using";
    private final static String package_math = "abs|acos|asin|atan|atan2|ceil|cos|cosh|deg|exp|floor|fmod|frexp|huge|ldexp|log|" +
            "log10|max|maxinteger|min|mininteger|modf|pi|pow|rad|random|randomseed|sin|sinh|sqrt|tan|tanh|tointeger|type|ult";
    private final static String package_os = "clock|date|difftime|execute|exit|getenv|remove|rename|setlocale|time|tmpname";
    private final static String package_package = "config|cpath|loaded|loaders|loadlib|path|preload|searchers|searchpath|seeall";
    private final static String package_string = "byte|char|dump|find|format|gfind|gmatch|gsub|len|lower|match|pack|packsize|rep|reverse|sub|unpack|upper";
    private final static String package_table = "concat|foreach|foreachi|insert|maxn|move|pack|remove|sort|unpack";
    private final static String package_utf8 = "char|charpattern|codepoint|codes|len|offset";
    private final static String extFunctionTarget = "Type|newArray|proxy|instanceof|import|object|charString|charValue|new|sync|try|throw|cross|using";
    private final static String functionTarget = globalTarget + "|" + extFunctionTarget + "|" + packageName;
    private final static String[] keywords = keywordTarget.split("\\|");
    private final static String[] names = functionTarget.split("\\|");
    private final static char[] LUA_OPERATORS = {
            '(', ')', '{', '}', ',', ';', '=', '+', '-',
            '/', '*', '&', '!', '|', ':', '[', ']', '<', '>',
            '?', '~', '%', '^'
    };
    private static Language _theOne = null;

    private LanguageLua() {
        super.setOperators(LUA_OPERATORS);
        super.setKeywords(keywords);
        super.setNames(names);
        super.addBasePackage("io", package_io.split("\\|"));
        super.addBasePackage("string", package_string.split("\\|"));
        super.addBasePackage("java", package_java.split("\\|"));
        super.addBasePackage("os", package_os.split("\\|"));
        super.addBasePackage("table", package_table.split("\\|"));
        super.addBasePackage("math", package_math.split("\\|"));
        super.addBasePackage("utf8", package_utf8.split("\\|"));
        super.addBasePackage("coroutine", package_coroutine.split("\\|"));
        super.addBasePackage("package", package_package.split("\\|"));
        super.addBasePackage("debug", package_debug.split("\\|"));
    }

    public static Language getInstance() {
        if (_theOne == null) {
            _theOne = new LanguageLua();
        }
        return _theOne;
    }

    /**
     * Whether the word after c is a token
     */
    public boolean isWordStart2(char c) {
        return (c == '.');
    }

    public boolean isLineAStart(char c) {
        return false;
    }

    /**
     * Whether c0c1L is a token, where L is a sequence of characters until the end of the line
     */
    public boolean isLineStart(char c0, char c1) {
        return (c0 == '-' && c1 == '-');
    }

    /**
     * Whether c0c1 signifies the start of a multi-line token
     */
    public boolean isMultilineStartDelimiter(char c0, char c1) {
        return (c0 == '[' && c1 == '[');
    }

    /**
     * Whether c0c1 signifies the end of a multi-line token
     */
    public boolean isMultilineEndDelimiter(char c0, char c1) {
        return (c0 == ']' && c1 == ']');
    }

}
