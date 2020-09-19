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
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import static androidx.core.app.NotificationCompat.PRIORITY_MIN;
import static org.jlantxa.notify.server.Server.HOST_NAME;
import static org.jlantxa.notify.server.Server.NOTIFICATION_SERVER_PORT;

public class NotificationService extends Service {
    private static final String TAG = "NotificationService";

    private static final int ID_SERVICE = 2236;

    SocketThread mSocketThread = new SocketThread();

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

    @Override
    public void onDestroy() {
        super.onDestroy();

        if (mSocketThread.isAlive()) {
            mSocketThread.interrupt();
        }
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private String createNotificationChannel(NotificationManager notificationManager){
        String channelId = "Notify";
        String channelName = "Notification service";
        NotificationChannel channel = new NotificationChannel(channelId, channelName, NotificationManager.IMPORTANCE_HIGH);
        // omitted the LED color
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

        mSocketThread.start();

        return START_STICKY;
    }

    class SocketThread extends Thread {
        private Socket mSocket;
        OutputStream mSocketOut;

        public SocketThread() {
            mSocket = new Socket();
        }

        private void connect() throws IOException {
            mSocket.connect(new InetSocketAddress(HOST_NAME, NOTIFICATION_SERVER_PORT), 0);
            mSocketOut = mSocket.getOutputStream();
        }

        private void login() throws IOException {
            final Message loginMsg = new Message(Message.TYPE_LOGIN, "token");
            mSocketOut.write(loginMsg.serialize());
        }

        private void logout() throws IOException {
            final Message logoutMsg = new Message(Message.TYPE_LOGOUT, null);
            mSocketOut.write(logoutMsg.serialize());
        }

        @Override
        public void run() {
            super.run();
            try {
                connect();
                for (int i = 0; i < 5; i++) {
                    Thread.sleep(1000);
                    login();
                }
            } catch (IOException | InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
