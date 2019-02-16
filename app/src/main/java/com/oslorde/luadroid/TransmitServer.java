package com.oslorde.luadroid;

import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.util.SparseArray;

import com.oslorde.luadroid.ui.MainActivity;
import com.oslorde.luadroid.ui.RemoteObj;
import com.oslorde.luadroid.ui.TransmitObj;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.lang.reflect.Array;
import java.lang.reflect.Method;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

public class TransmitServer implements TransmitInfo {
    private static final int PORT_ST=4589;

    static class RemoteObject{
        RemoteObj obj;

        Method[] methods;

        public RemoteObject(RemoteObj obj) {
            this.obj = obj;
            Method[] methods=obj.getProxyType().getDeclaredMethods();
            Arrays.sort(methods,METHOD_COMPARATOR);
        }
    }
    private DefaultScriptContext context;
    private ServerSocketChannel server;
    private Thread serverThread;
    //Now it is only expected to has one client;
    private List<SocketChannel> clients=new ArrayList<>();
    private List<SocketChannel> logChannels=new ArrayList<>();
    private int port;
    private boolean closed;
    private Runnable clientDiedCallback;

    private TransmitServer(int port) throws IOException {
        //use channel to write log
        server=ServerSocketChannel.open();
        server.socket().bind(new InetSocketAddress(port));
        context=new DefaultScriptContext();
        this.port=port;
    }

    private void printLog(SocketChannel channel,DataOutputStream out,ByteBuffer raw,boolean isError){
        try {
            out.writeInt(raw.limit());
            out.write(isError?1:0);
            channel.write(raw);
        } catch (IOException e) {
            e.printStackTrace();
            context.setLogger(null,null);
        }
    }
    private  Object readObj(DataInputStream in,SparseArray<RemoteObject> remoteObjects) throws Exception{
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
                    list.add(readObj(in,remoteObjects));
                }
                return list;
            }
            case 5:{
                int size = in.readInt();
                Object arr=Array.newInstance(Class.forName(in.readUTF()),size);
                for (int i=0;i<size;++i){
                    Array.set(arr,i,readObj(in,remoteObjects));
                }
                return arr;
            }
            case 6:{
                int size = in.readInt();
                TransmitObj obj= (TransmitObj) Class.forName(in.readUTF()).newInstance();
                ArrayList<Object> list=new ArrayList<>(size);
                for (int i=size;i--!=0;){
                    list.add(readObj(in,remoteObjects));
                }
                obj.restoreFrom(list);
                return obj;
            }
            case 7:{
                int ref=in.readInt();
                return remoteObjects.get(ref).obj;
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

    private void writeObj(DataOutputStream out,SparseArray<RemoteObject> remoteObjects, Object object) throws Exception{
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
                writeObj(out,remoteObjects,obj);
            }
        }else if(object.getClass().isArray()){
            out.writeInt(5);
            out.writeUTF(object.getClass().getName());
            out.write(Array.getLength(object));
            for (int i=0,len=Array.getLength(object);i<len;++i){
                writeObj(out,remoteObjects,Array.get(object,i));
            }
        }
        else if(object instanceof TransmitObj){
            out.write(6);
            out.writeUTF(object.getClass().getName());
            List<Object> objects=((TransmitObj) object).save();
            out.write(objects.size());
            for (Object obj:objects){
                writeObj(out,remoteObjects,obj);
            }
        }else if(object instanceof RemoteObj){
            out.write(7);
            int ref=remoteObjects.keyAt(remoteObjects.size()-1)+1;
            remoteObjects.append(ref, new RemoteObject((RemoteObj) object));
            out.writeInt(ref);
            out.writeUTF(((RemoteObj) object).getProxyType().getName());
        }else if(object instanceof Serializable){
            out.write(0x7f);
            ObjectOutputStream objOut=new ObjectOutputStream(out);
            objOut.writeObject(object);
        }else
            throw new UnsupportedOperationException();
    }

    private void handleClient(SocketChannel channel){
        SparseArray<RemoteObject> remoteObjects=new SparseArray<>();
        remoteObjects.append(0,null);
        clients.add(channel);
        try (Socket socket=channel.socket();
             DataInputStream in = new DataInputStream(socket.getInputStream());
             DataOutputStream out=new DataOutputStream(socket.getOutputStream())) {
            Log.e(TAG,"remote start handle");
            while (true){
                int magic = in.readInt();
                switch (magic){
                    case METHOD_MAGIC:{
                        int ref=in.readInt();
                        int handle=in.readInt();
                        int argLen = in.readInt();
                        Object[] args=new Object[argLen];
                        for (int i=0;i<argLen;++i){
                            args[i]=readObj(in,remoteObjects);
                        }
                        Object result;
                        if(ref==0){
                            switch (handle){
                                case HANDLE_RUN:
                                    result=context.run((String) args[0]);
                                    break;
                                case HANDLE_RUN_FILE:
                                    result=context.run((File)args[0]);
                                    break;
                                case HANDLE_CLASSES:
                                    result=context.getClasses();
                                    break;
                                case HANDLE_FLUSH_LOG:
                                    context.flushLog();
                                    for (SocketChannel ch:logChannels){
                                        ch.socket().getOutputStream().flush();
                                    }
                                    result=null;
                                    break;
                                default:result=null;
                            }
                        }else {
                            RemoteObject object = remoteObjects.get(ref);
                            Method method=  object.methods[handle];
                            result = method.invoke(object,args);
                        }
                        writeObj(out,remoteObjects,result);
                        break;
                    }
                    case CLEAN_MAGIC:{
                        int ref=in.readInt();
                        remoteObjects.remove(ref);
                        break;
                    }
                    default:
                        Log.e(TAG,"remoteError,unexpected magic:"+magic);
                }
            }

        }catch (Exception ignored){
        }
        finally {
            remoteObjects.clear();
            clients.remove(channel);
            if(clientDiedCallback!=null)
                clientDiedCallback.run();
        }
    }

    public void start(){
        try {
            serverThread=Thread.currentThread();
            while (true){
                SocketChannel channel= server.accept();
                int magic=new DataInputStream(channel.socket().getInputStream()).readInt();
                if(magic==CONNECT_MAGIC){
                    Log.e(TAG,"Receive connection");
                    new Thread(()->{
                        handleClient(channel);
                    }).start();
                }else if(magic==PRINT_MAGIC){
                    logChannels.add(channel);
                    Log.d(TAG,"remote got printer");
                    DataOutputStream out=new DataOutputStream(channel.socket().getOutputStream());
                    context.setLogger((log,r) -> printLog(channel,out, r, false),
                            (log,r) -> printLog(channel,out, r, true));
                }else {
                    Log.e(TAG,"remoteError,unexpected client magic:"+magic);
                }
            }
        }catch (Exception e){
            serverThread=null;
            shutdown();
        }

    }

    private static TransmitServer generateSever(){
        int st=PORT_ST;
        Random random=new Random();
        for (;;++st){
            int port=Math.abs((random.nextInt()&0xffff)-st)+st;
            try {
                return new TransmitServer(port);
            }catch (IOException  ignored){
            }
        }

    }

    public static TransmitServer start(Context context){
        TransmitServer server=generateSever();
        context.startActivity(new Intent().setClassName(BuildConfig.APPLICATION_ID, MainActivity.class.getName())
        .putExtra("remotePort",server.port).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                .addFlags(Intent.FLAG_ACTIVITY_TASK_ON_HOME));
        server.context.addToLua("context",context);
        new Thread(server::start).start();
        return server;
    }

    public static TransmitServer getInstance(){
        return generateSever();
    }
    public void shutdown(){
        if(closed) return;
        closed=true;
        if(serverThread!=null)
            serverThread.interrupt();
        for (SocketChannel channel:clients){
            try { channel.socket().shutdownInput(); }
            catch (IOException ignored) { }
            try { channel.close(); }catch (IOException ignored) { }
        }
        serverThread=null;
        clients.clear();
        logChannels.clear();
    }
    public ScriptContext getScriptContext(){
        return context.get();
    }

    public int getPort(){
        return server.socket().getLocalPort();
    }
    public boolean isClosed(){
        return closed;
    }

    public int runningClients(){
        return clients.size();
    }

    public void registerClientDiedCallback(Runnable r){
        clientDiedCallback=r;
    }
}
