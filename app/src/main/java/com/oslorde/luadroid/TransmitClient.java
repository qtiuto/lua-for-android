package com.oslorde.luadroid;

import android.util.Log;

import com.oslorde.luadroid.ui.IClassList;
import com.oslorde.luadroid.ui.IScriptContext;
import com.oslorde.luadroid.ui.TransmitObj;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.reflect.Array;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class TransmitClient implements IScriptContext,TransmitInfo {


    private static Method sGetRef;

    static {
        try {
            sGetRef=ProxyObj.class.getMethod("getRef");
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    private final Socket socket;
    private Socket printer;
    private int port;
    private ExecutorService pool= Executors.newSingleThreadExecutor();

    public TransmitClient(int port) throws IOException {
        this.port=port;
        socket=new Socket("127.0.0.1",port);
        new DataOutputStream(socket.getOutputStream()).writeInt(CONNECT_MAGIC);

    }

    private static void writeObj(DataOutputStream out,Object object) throws Exception{
        if(object==null){
            out.write(0);
        }else if(object instanceof Number||object instanceof Boolean||object instanceof Character){
            out.write(1);
            if(object instanceof Long){
                out.write(1);
                out.writeLong((Long) object);
            }else if(object instanceof Integer){
                out.write(2);
                out.writeInt((Integer) object);
            }else if(object instanceof Boolean){
                out.write(3);
                out.writeBoolean((Boolean) object);
            }else if(object instanceof Character){
                out.write(4);
                out.writeShort((Character) object);
            }else if(object instanceof Byte){
                out.write(5);
                out.writeShort((Byte) object);
            }else if(object instanceof Short){
                out.write(6);
                out.writeShort((Short) object);
            }
        }else if(object instanceof File){
            out.write(2);
            out.writeUTF(((File) object).getPath());
        } else if(object instanceof String){
            out.write(3);
            out.writeUTF(object.toString());
        }else if(object instanceof ArrayList){
            out.writeInt(4);
            out.write(((List)object).size());
            for (Object obj:(List)object){
                writeObj(out,obj);
            }
        }else if(object.getClass().isArray()){
            out.writeInt(5);
            out.writeUTF(object.getClass().getName());
            out.write(Array.getLength(object));
            for (int i=0,len=Array.getLength(object);i<len;++i){
                writeObj(out,Array.get(object,i));
            }
        }
        else if(object instanceof TransmitObj){
            out.write(6);
            out.writeUTF(object.getClass().getName());
            List<Object> objects=((TransmitObj) object).save();
            out.write(objects.size());
            for (Object obj:objects){
                writeObj(out,obj);
            }
        }else if(object instanceof ProxyObj){
            out.write(7);
            out.write(((ProxyObj) object).getRef());
        }else if(object instanceof Serializable){
            out.write(0x7f);
            ObjectOutputStream objOut=new ObjectOutputStream(out);
            objOut.writeObject(object);
        }else
            throw new UnsupportedOperationException();
    }

    @Override
    public String run(String s) {
        try {
            return (String) callRemoteMethod(0,HANDLE_RUN,s);
        } catch (Throwable throwable) {
            Log.e(TAG,"runS",throwable);
            return null;
        }
    }

    @Override
    public String run(File file) {
        try {
            return (String) callRemoteMethod(0,HANDLE_RUN_FILE,file);
        } catch (Throwable throwable) {
            Log.e(TAG,"runF",throwable);
            return null;
        }
    }

    @Override
    public void setLogger(Logger out, Logger err) {

        new Thread(() -> {
            try {
                if (out != null||err!=null) {
                    if (printer == null) {
                        printer = new Socket("127.0.0.1", port);
                        new DataOutputStream(printer.getOutputStream()).writeInt(PRINT_MAGIC);
                    }
                } else{
                    if (printer != null) {
                        Socket p=printer;
                        printer = null;
                        p.shutdownInput();
                        p.close();
                    }
                    return;
                }

            } catch (IOException e) {
                e.printStackTrace();
                return;
            }
            ByteBuffer buffer=ByteBuffer.allocate(2048);//max length for one print chunk;
            CharBuffer charBuffer=buffer.asReadOnlyBuffer().order(ByteOrder.LITTLE_ENDIAN).asCharBuffer();
            int length;
            try (DataInputStream in =new DataInputStream(printer.getInputStream()) ) {
                while ((length = in.readInt()) > 0) {
                    int type=in.read();
                    buffer.position(0).limit(length);
                    in.read(buffer.array(),0,length);
                    charBuffer.position(0).limit(length>>1);
                    if(type==0&&out!=null){
                        out.onNewLog(charBuffer,buffer);
                    }else if(err!=null&&type!=0)
                        err.onNewLog(charBuffer,buffer);
                    else {
                        if(buffer.array()[length-1]=='\n'){
                            charBuffer.limit(charBuffer.limit()-1);
                        }
                        Log.println(type==0?Log.INFO:Log.ERROR,"Lua",charBuffer.toString() );
                    }
                }
            } catch (IOException e) {
            }

        }).start();

    }

    @Override
    public void addToLua(String name, Object obj, boolean local) {
    }

    @Override
    public void addToLua(String luaName, String memberName, Object inst, Class type, boolean local) {
    }

    @Override
    public IClassList getClasses() {
        try {
            return (IClassList) callRemoteMethod(0,HANDLE_CLASSES);
        } catch (Throwable throwable) {
            Log.e(TAG,"getClasses",throwable);
            return null;
        }
    }

    @Override
    public void flushLog() {
        try {
            callRemoteMethod(0,HANDLE_FLUSH_LOG);
            Thread.sleep(100);
        } catch (Throwable throwable) {
            Log.e(TAG,"flush",throwable);
        }
    }

    private Object generateRemoteObj(int ref,String clsName) throws Exception {
        Class c=Class.forName(clsName);
        if(!c.isInterface()) throw new IllegalArgumentException();
        Method [] methods=c.getMethods();
        Arrays.sort(methods, METHOD_COMPARATOR);
        return Proxy.newProxyInstance(getClass().getClassLoader(), new Class[]{c,ProxyObj.class}, new InvocationHandler() {
            @Override
            public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
                if(method.equals(sGetRef)){
                    return ref;
                }
                int handle=Arrays.binarySearch(methods,method);
                return callRemoteMethod(ref,handle,args );
            }

            @Override
            protected void finalize() throws Throwable {
                super.finalize();
                synchronized (socket){
                    DataOutputStream out=new DataOutputStream(socket.getOutputStream());
                    out.writeInt(CLEAN_MAGIC);//magic
                    out.writeInt(ref);
                }
            }
        });
    }

    private Object callRemoteMethod(int ref,int handle,Object... args) throws Exception {
        return pool.submit(()->{
            synchronized (socket){
                DataOutputStream out=new DataOutputStream(socket.getOutputStream());
                out.writeInt(METHOD_MAGIC);//magic
                out.writeInt(ref);
                out.writeInt(handle);
                out.writeInt(args.length);//avoid writing un-used class name;
                for (Object arg:args){
                    writeObj(out,arg);
                }
                socket.getOutputStream().flush();
                DataInputStream in=new DataInputStream(socket.getInputStream());
                return readObj(in);
            }
        }).get();

    }

    private  Object readObj(DataInputStream in) throws Exception{
        switch (in.read()){
            case 0:return null;
            case 1:{
                switch (in.read()){
                    case 1:
                        return in.readLong();
                    case 2:
                        return in.readInt();
                    case 3:
                        return in.readBoolean();
                    case 4:
                        return in.readChar();
                    case 5:
                        return in.readByte();
                    case 6:
                        return in.readShort();
                }
            }
            case 2:
                return new File(in.readUTF());
            case 3:
                return in.readUTF();
            case 4:{
                int size = in.readInt();
                ArrayList<Object> list=new ArrayList<>(size);
                for (int i=size;i--!=0;){
                    list.add(readObj(in));
                }
                return list;
            }
            case 5:{
                int size = in.readInt();
                Object arr=Array.newInstance(Class.forName(in.readUTF()),size);
                for (int i=0;i<size;++i){
                    Array.set(arr,i,readObj(in));
                }
                return arr;
            }
            case 6:{
                int size = in.readInt();
                TransmitObj obj= (TransmitObj) Class.forName(in.readUTF()).newInstance();
                ArrayList<Object> list=new ArrayList<>(size);
                for (int i=size;i--!=0;){
                    list.add(readObj(in));
                }
                obj.restoreFrom(list);
                return obj;
            }
            case 7:{
                int ref=in.readInt();
                String cls=in.readUTF();
                return generateRemoteObj(ref,cls);
            }
            case 0x7f:{
                ObjectInputStream objOut=new ObjectInputStream(in);
                return objOut.readObject();
            }
            default:{
                throw new UnsupportedOperationException();
            }
        }
    }

    public void close(){
        try (Socket s=socket;Socket s1=printer){
        }catch (Exception e){}
    }

    interface ProxyObj{
        int getRef();
    }
}
