package org.jlantxa.notify;

import android.app.job.JobParameters;
import android.app.job.JobService;
import android.util.Log;

import org.jlantxa.notify.server.Message;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import static org.jlantxa.notify.server.Server.HOST_NAME;
import static org.jlantxa.notify.server.Server.NOTIFICATION_SERVER_PORT;

public class NotificationJobService extends JobService {
    private static final String TAG = "NotificationJobService";

    private SocketThread mSocketThread;

    public void startThread() {
        mSocketThread = new SocketThread();
        mSocketThread.start();
    }

    public void stopThread() {
        if (mSocketThread.isAlive()) {
            mSocketThread.interrupt();
        }
    }

    @Override
    public boolean onStartJob(JobParameters params) {
        Log.d(TAG, "Start job");
        startThread();
        return true;
    }

    @Override
    public boolean onStopJob(JobParameters params) {
        Log.d(TAG, "Stop job");
        stopThread();
        return true;
    }

    class SocketThread extends Thread {
        private Socket mSocket;
        OutputStream mSocketOut;

        public SocketThread() {
            try {
                mSocket = new Socket();
                mSocketOut = mSocket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        private void connect() throws IOException {
            mSocket.connect(new InetSocketAddress(HOST_NAME, NOTIFICATION_SERVER_PORT), 0);
        }

        private void login() throws IOException {
            Message loginMsg = new Message(Message.TYPE_LOGIN, "jLantxa");
            mSocketOut.write(loginMsg.serialize());
        }

        private void logout() throws IOException {
            Message logoutMsg = new Message(Message.TYPE_LOGOUT, null);
            mSocketOut.write(logoutMsg.serialize());
        }

        @Override
        public void run() {
            super.run();
            try {
                connect();
                login();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
