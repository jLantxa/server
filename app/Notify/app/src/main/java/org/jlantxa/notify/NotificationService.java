package org.jlantxa.notify;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.RequiresApi;
import androidx.core.app.NotificationCompat;

import org.jlantxa.notify.server.Message;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import static androidx.core.app.NotificationCompat.PRIORITY_MIN;
import static org.jlantxa.notify.server.Server.HOST_NAME;
import static org.jlantxa.notify.server.Server.NOTIFICATION_SERVER_PORT;

public class NotificationService extends Service {
    private static final String TAG = "NotificationService";

    private static final int ID_SERVICE = 2236;
    private static final long SERVER_SLEEP_TIME = 10 * (60 * 1000);

    private Socket mSocket;
    private OutputStream mSocketOutputStream;
    private InputStream mSocketInputStream;

    private SocketReadThread mSocketReadThread;

    private static final int MAX_LOGIN_TRIES = 10;
    private boolean mIsLoggedIn = false;

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

        createSocket();
        mSocketReadThread = new SocketReadThread();
        mSocketReadThread.start();

        int loginTries = 0;
        do {
            login();
        } while(!mIsLoggedIn && loginTries < MAX_LOGIN_TRIES);

        if (loginTries >= MAX_LOGIN_TRIES) {
            Log.e(TAG, "Maximum number of login tries exceeded");
            return;
        }

        requestTasks();

        // TODO: Timeout until server sends OK

        scheduleNotifications();

        logout();

        try {
            Thread.sleep(SERVER_SLEEP_TIME);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void scheduleNotifications() {
        // TODO
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mSocketReadThread.isAlive()) {
            mSocketReadThread.interrupt();
        }
    }

    private void createSocket() {
        Thread socketCreateThread =  new Thread() {
            @Override
            public void run() {
                super.run();

                try {
                    mSocket = new Socket(HOST_NAME, NOTIFICATION_SERVER_PORT);
                    mSocketOutputStream = mSocket.getOutputStream();
                    mSocketInputStream = mSocket.getInputStream();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };
        socketCreateThread.start();

        try {
            socketCreateThread.join();
        } catch (InterruptedException ioException) {
            ioException.printStackTrace();
        }
    }

    private void writeMsg(Message msg) {
        SocketWriteThread writeThread = new SocketWriteThread(msg);
        writeThread.start();

        try {
            writeThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void login() {
        final Message loginMsg = new Message(Message.TYPE_LOGIN, "token");
        writeMsg(loginMsg);
    }

    private void logout() {
        final Message logoutMsg = new Message(Message.TYPE_LOGOUT, null);
        writeMsg(logoutMsg);
    }

    private void requestTasks() {
        final Message requestMsg = new Message(Message.TYPE_REQUEST_TASKS, null);
        writeMsg(requestMsg);
    }

    private void onMessageReceived(Message msg) {

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

        return START_STICKY;
    }

    class SocketWriteThread extends Thread {
        private Message msg;

        public SocketWriteThread(Message msg) {
            this.msg = msg;
        }

        @Override
        public void run() {
            super.run();
            try {
                mSocketOutputStream.write(msg.serialize());
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }
        }
    }

    class SocketReadThread extends Thread {
        private byte[] buffer = new byte[1024];
        private boolean running = false;

        @Override
        public void run() {
            super.run();
            running = true;

            while (running) {
                try {
                    int read = mSocketInputStream.read(buffer);
                    if (read != -1) {
                        Message msg = new Message(buffer, read);
                        onMessageReceived(msg);
                    }
                } catch (IOException ioException) {
                    ioException.printStackTrace();
                }

                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        @Override
        public void interrupt() {
            super.interrupt();
            running = false;
        }
    }
}
