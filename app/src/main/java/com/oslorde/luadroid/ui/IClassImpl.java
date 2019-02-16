package com.oslorde.luadroid.ui;

import java.lang.reflect.*;
import java.util.*;

public class IClassImpl implements IClass ,RemoteObj{
    private Class c;
    private Constructor[] constructors;
    private Method[] methods;
    private Field[] fields;
    IClassImpl(Class c){
        this.c=c;
        constructors=c.getDeclaredConstructors();
        Set<Field> fields=new HashSet<>();
        Set<Method> methods=new HashSet<>();
        Class cl=c;
        do{
            Collections.addAll(fields,cl.getDeclaredFields());
            Collections.addAll(methods,cl.getDeclaredMethods());
        }while ((cl=cl.getSuperclass())!=null);
        this.methods=methods.toArray(new Method[methods.size()]);
        this.fields=fields.toArray(new Field[fields.size()]);
        Arrays.sort(this.methods,  (o1, o2) -> o1.getName().compareTo(o2.getName()));
        Arrays.sort(this.fields,  (o1, o2) -> o1.getName().compareTo(o2.getName()));
    }




    private String simpleGenericType(Type type){
        if(type instanceof Class) return ((Class) type).getSimpleName();
        else if(type instanceof ParameterizedType){
            StringBuilder builder=new StringBuilder();
            builder.append(simpleGenericType(((ParameterizedType) type).getRawType()));
            Type[] actualTypeArguments = ((ParameterizedType) type).getActualTypeArguments();
            if(actualTypeArguments.length>0){
                builder.append('<');
                for (Type t: actualTypeArguments){
                    builder.append(simpleGenericType(t)).append(',');
                }
                builder.setCharAt(builder.length()-1,'>');
            }
            return builder.toString();
        }else if(type instanceof TypeVariable){
            StringBuilder builder=new StringBuilder();
            builder.append(((TypeVariable) type).getName());
            Type[] bounds = ((TypeVariable) type).getBounds();
            if(bounds.length>0){
                builder.append(" extends ");
                for (Type t: bounds){
                    builder.append(simpleGenericType(t)).append('&');
                }
                builder.deleteCharAt(builder.length()-1);
            }
            return builder.toString();
        }else if(type instanceof WildcardType){
            StringBuilder builder=new StringBuilder();
            builder.append('?');
            Type[] bounds = ((WildcardType) type).getUpperBounds();
            if(bounds.length>0){
                builder.append(" extends ");
                for (Type t: bounds){
                    builder.append(simpleGenericType(t)).append('&');
                }
                builder.deleteCharAt(builder.length()-1);
            }
            bounds=((WildcardType) type).getLowerBounds();
            if(bounds.length>0){
                builder.append(" super ");
                for (Type t: bounds){
                    builder.append(simpleGenericType(t)).append('&');
                }
                builder.deleteCharAt(builder.length()-1);
            }
            return builder.toString();
        }else if(type instanceof GenericArrayType){
            return simpleGenericType(((GenericArrayType) type).getGenericComponentType())+"[]";
        }
        System.out.println("Type Not Found for "+type);
        return "Object";
    }


    private String getMemberDes(Member m){

        int mod = m.getModifiers();
        if(m instanceof Field){
            Type fieldType = ((Field) m).getGenericType();
            return ((mod == 0) ? "" : (Modifier.toString(mod) + ' '))
                    + m.getName()+ ':'+ simpleGenericType(fieldType) ;
        }else if(m instanceof Method){
            StringBuilder builder=new StringBuilder();
            if(mod!=0) builder.append( Modifier.toString(mod)).append(' ');
            builder.append(m.getName()).append('(');
            for (Type type:((Method) m).getGenericParameterTypes()){
                builder.append(simpleGenericType(type)).append(", ");
            }
            if(builder.charAt(builder.length()-1)!='(')
                builder.delete(builder.length()-2,builder.length());
            builder.append(')');
            return builder.append(':').append(simpleGenericType(((Method) m).getReturnType())).toString();
        }else if(m instanceof Constructor){
            StringBuilder builder=new StringBuilder();
            if(mod!=0) builder.append( Modifier.toString(mod)).append(' ');
            builder.append(simpleGenericType(m.getDeclaringClass())).append('(');
            for (Type type:((Constructor) m).getGenericParameterTypes()){
                builder.append(simpleGenericType(type)).append(", ");
            }
            if(builder.charAt(builder.length()-1)!='(')
                builder.delete(builder.length()-2,builder.length());
            builder.append(')');
            return builder.toString();
        }
        throw new UnsupportedOperationException();
    }
    private static void binarySearchMember( Member[] inList,List<Member> outList,String prefix){
        int index=Arrays.binarySearch(inList, prefix,((o1, o2) ->
                ((Member)o1).getName().startsWith((String) o2)?0:
                        ((Member)o1).getName().compareTo((String) o2)
        ));
        if(index<0) return;
        int start;
        for (start=index;start>=0;--start){
            if(!inList[start].getName().startsWith(prefix)){
                break;
            }
        }
        ++start;
        int end;
        int len=inList.length;
        for (end=index+1;end<len;++end){
            if(!inList[end].getName().startsWith(prefix)){
                break;
            }
        }
        outList.addAll(Arrays.asList(inList).subList(start, end));
    }
    @Override
    public List<JMember> filterMembers(String constraint) {
        List<Member> members=new ArrayList<>();
        if (c.getSimpleName().startsWith(constraint))
            Collections.addAll(members, constructors);
        binarySearchMember(fields, members, constraint);
        binarySearchMember(methods, members, constraint);
        List<JMember> ret=new ArrayList<>(members.size());
        for (Member m:members){
            ret.add(new JMember(m instanceof Constructor?0:m instanceof Method?1:2,getMemberDes(m),m instanceof Constructor?c.getName():m.getName()));
        }
        return ret;
    }

    @Override
    public Class getProxyType() {
        return IClass.class;
    }
}
