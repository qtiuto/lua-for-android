package com.sun.tools.javac.model;


public class EnumConstantNotPresentExceptionProxy extends ExceptionProxy {
    private static final long serialVersionUID = -604662101303187330L;
    Class<? extends Enum<?>> enumType;
    String constName;

    public EnumConstantNotPresentExceptionProxy(Class<? extends Enum<?>> enumType,
                                                String constName) {
        this.enumType = enumType;
        this.constName = constName;
    }

    protected RuntimeException generateException() {
        return new EnumConstantNotPresentException(enumType, constName);
    }
}
