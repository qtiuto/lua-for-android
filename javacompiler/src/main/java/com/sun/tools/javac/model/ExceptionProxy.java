package com.sun.tools.javac.model;

public abstract class ExceptionProxy implements java.io.Serializable {
    protected abstract RuntimeException generateException();
}