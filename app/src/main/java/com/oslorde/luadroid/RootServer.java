package com.oslorde.luadroid;

import android.util.Log;

import java.io.DataOutputStream;
import java.net.Socket;

public class RootServer  {
    public static void main(String[] args){
        int port=Integer.parseInt(args[0]);
        System.loadLibrary("luadroid");
        TransmitServer server=TransmitServer.getInstance();
        try (Socket socket=new Socket("127.0.0.1",port);
            DataOutputStream out=new DataOutputStream(socket.getOutputStream())){
            out.writeInt(server.getPort());
            server.registerClientDiedCallback(server::shutdown);
            server.start();
        }catch (Throwable e){
            Log.e("Osl_Root","remote error",e);
        }
        finally {
            server.shutdown();
        }
    }
}
