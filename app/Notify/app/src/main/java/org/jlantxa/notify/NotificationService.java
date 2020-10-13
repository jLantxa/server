package org.jlantxa.notify;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.RequiresApi;
import androidx.core.app.NotificationCompat;

import org.jlantxa.notify.server.Message;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import static androidx.core.app.NotificationCompat.PRIORITY_MIN;

public class NotificationService extends Service {
    private static final String TAG = "NotificationService";

    private static final int ID_SERVICE = 2236;
    private static final long SERVER_SLEEP_TIME = 10 * (60 * 1000);

    private boolean mRunning = false;

    private Socket mSocket;
    private byte[] mBuffer = new byte[1024];
    private OutputStream mSocketOutputStream;
    private InputStream mSocketInputStream;

    private String mHostName;
    private int mPort;
    private String mUserToken;

    private static final int MAX_LOGIN_TRIES = 10;

    public NotificationService() {

    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        Log.d(TAG, "onCreate");
        super.onCreate();
    }

    private void run() {
        mRunning =  true;

        Thread notificationRequestThread = new NotificationRequestThread();
        notificationRequestThread.start();
    }


    @Override
    public void onDestroy() {
        super.onDestroy();

        mRunning =  false;
    }

    private void createSocket(String hostName, int port) {
        try {
            mSocket = new Socket();
            mSocket.connect(new InetSocketAddress(hostName, port), 0);
            mSocketOutputStream = mSocket.getOutputStream();
            mSocketInputStream = mSocket.getInputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void writeMsg(Message msg) {
        try {
            mSocketOutputStream.write(msg.serialize());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean login(String userToken) {
        int read;
        Message message;

        int loginTries = 0;
        final Message loginMsg = new Message(Message.TYPE_LOGIN, userToken);
        writeMsg(loginMsg);

        try {
            do {
                read = mSocketInputStream.read(mBuffer);
                Thread.sleep(100);
                loginTries++;
            } while ((loginTries < MAX_LOGIN_TRIES) && (read < 0));

            message = new Message(mBuffer, read);
            final short type =  message.getType();
            if (type != Message.TYPE_OK) {
                return false;
            }
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }

        Log.i(TAG, "Login successful (numTries=" + loginTries + ")");
        return true;
    }

    private void logout() {
        final Message logoutMsg = new Message(Message.TYPE_LOGOUT, null);
        writeMsg(logoutMsg);
    }

    private void requestTasks() {
        final Message requestMsg = new Message(Message.TYPE_REQUEST_TASKS, null);
        writeMsg(requestMsg);
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private String createNotificationChannel(NotificationManager notificationManager){
        String channelId = "Notify";
        String channelName = "Notification service";
        NotificationChannel channel = new NotificationChannel(channelId, channelName, NotificationManager.IMPORTANCE_HIGH);
        // Omitted the LED color
        channel.setLightColor(0xFFFF0000);
        channel.setImportance(NotificationManager.IMPORTANCE_NONE);
        channel.setLockscreenVisibility(Notification.VISIBILITY_PRIVATE);
        notificationManager.createNotificationChannel(channel);
        return channelId;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        String channelId = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O ? createNotificationChannel(notificationManager) : "";
        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, channelId);
        Notification notification = notificationBuilder.setOngoing(true)
                .setSmallIcon(R.mipmap.ic_launcher)
                .setPriority(PRIORITY_MIN)
                .setCategory(NotificationCompat.CATEGORY_SERVICE)
                .setContentTitle("Notification service enabled")
                .build();

        startForeground(ID_SERVICE, notification);

        Bundle extras = intent.getExtras();
        mHostName = extras.getString("host");
        mPort = extras.getInt("port");
        mUserToken = extras.getString("token");

        run();

        return START_STICKY;
    }

    class NotificationRequestThread extends Thread {
        @Override
        public void run() {
            while (mRunning) {
                createSocket(mHostName, mPort);
                login(mUserToken);
                requestTasks();
                logout();

                try {
                    Thread.sleep(SERVER_SLEEP_TIME);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
