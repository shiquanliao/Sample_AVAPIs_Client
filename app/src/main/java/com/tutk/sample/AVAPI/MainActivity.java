package com.tutk.sample.AVAPI;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.tutk.sample.AVAPI.Client;
import com.tutk.sample.AVAPI.*;

public class MainActivity extends Activity {
    static final String UID = "U4SMNNDT79Y3FF5E111A";
//    static final String UID = "C1KAB554Z3RMHH6GU1Z1";

    private SurfaceView surfaceView;
    private Button mButton;
    private EditText text_uid;
    private TextView textView;
    private MyHnadler myHnadler = null;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

        mButton = (Button) findViewById(R.id.btn_start);
        text_uid = (EditText) findViewById(R.id.edit_text);
        textView = (TextView) findViewById(R.id.textView);

        myHnadler = new MyHnadler();

        surfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                textView.setText("");
                (new Thread() {
                    public void run() {
                        String uid = text_uid.getText().toString().trim();
//                        Client.start(UID,myHnadler);
                        Client.start(uid,myHnadler);
                    }
                }).start();
            }
        });

    }

    final class MyHnadler extends Handler{
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if(msg.what == 110) { // 更新UI
                Bundle data = msg.getData();
                String dataString = data.getString("test");
                textView.append(dataString);
            }

        }
    }
}
