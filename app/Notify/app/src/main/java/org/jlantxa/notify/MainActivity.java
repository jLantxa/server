package org.jlantxa.notify;

import android.content.Intent;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity
{
    private static final String TAG = "Notify";

    private Switch mSwEnableServer;

    private EditText mHostNameEditText;
    private EditText mPortEditText;
    private EditText mUserTokenEditText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mSwEnableServer = findViewById(R.id.swEnableServer);
        mSwEnableServer.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    startNotifyService();
                } else{
                    stopNotifyService();
                }
            }
        });

        mHostNameEditText = findViewById(R.id.teHostName);
        mPortEditText = findViewById(R.id.tePort);
        mUserTokenEditText = findViewById(R.id.teToken);
    }

    @Override
    protected void onResume() {
        super.onResume();

    }

    private void startNotifyService() {
        final String hostName = mHostNameEditText.getText().toString();
        final int port = Integer.parseInt(mPortEditText.getText().toString());
        final String token = mUserTokenEditText.getText().toString();

        if (hostName.length() == 0 || port < 1024 || token.length() == 0) {
            mSwEnableServer.setChecked(false);
            return;
        }

        Intent intent = new Intent(this, NotificationService.class);
        intent.putExtra("host", hostName);
        intent.putExtra("port", port);
        intent.putExtra("token", token);
        startService(intent);
    }

    private void stopNotifyService() {
        Intent intent = new Intent(this, NotificationService.class);
        stopService(intent);
    }
}