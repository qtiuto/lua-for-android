package com.sun.tools.javac.model;


import java.lang.annotation.AnnotationTypeMismatchException;
import java.lang.reflect.Method;

public class AnnotationTypeMismatchExceptionProxy extends ExceptionProxy {
    private static final long serialVersionUID = 7844069490309503934L;
    private Method member;
    private String foundType;

    /**
     * It turns out to be convenient to construct these proxies in
     * two stages.  Since this is a private implementation class, we
     * permit ourselves this liberty even though it's normally a very
     * bad idea.
     */
    AnnotationTypeMismatchExceptionProxy(String foundType) {
        this.foundType = foundType;
    }

    AnnotationTypeMismatchExceptionProxy setMember(Method member) {
        this.member = member;
        return this;
    }

    protected RuntimeException generateException() {
        return new AnnotationTypeMismatchException(member, foundType);
    }
}
