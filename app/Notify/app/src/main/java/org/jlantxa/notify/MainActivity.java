package org.jlantxa.notify;

import android.app.job.JobInfo;
import android.app.job.JobScheduler;
import android.content.ComponentName;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.Switch;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity
{
    private static final String TAG = "Notify";

    private JobScheduler mJobScheduler;
    private JobInfo mNotificationJobInfo;
    private Switch mSwEnableServer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // JobInfo for the NotificationJObService
        mJobScheduler = getSystemService(JobScheduler.class);
        JobInfo.Builder builder =
                new JobInfo.Builder(0, new ComponentName(getApplicationContext(), NotificationJobService.class))
                .setPeriodic(15 * 60 * 1000)  // 15 minutes
                .setRequiredNetworkType(JobInfo.NETWORK_TYPE_ANY);
        mNotificationJobInfo = builder.build();

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
    }

    @Override
    protected void onResume() {
        super.onResume();

    }

    private void startNotifyService() {
        Intent intent = new Intent(this, NotificationService.class);
        startService(intent);
    }

    private void stopNotifyService() {
        Intent intent = new Intent(this, NotificationService.class);
        stopService(intent);
    }

    private void scheduleNotificationJobService() {
        Log.d(TAG, "scheduleNotificationJobService");
        NotificationJobService service = new NotificationJobService();
        service.startThread();
        mJobScheduler.schedule(mNotificationJobInfo);
    }

    private void cancelNotificationJobService() {
        mJobScheduler.cancel(mNotificationJobInfo.getId());
    }

}