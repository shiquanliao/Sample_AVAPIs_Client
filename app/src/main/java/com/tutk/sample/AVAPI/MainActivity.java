package com.tutk.sample.AVAPI;

import android.app.Activity;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.common.jniFun.MyHandler;
import com.common.view.XPlay;
import com.tutk.sample.AVAPI.Client;
import com.tutk.sample.AVAPI.*;

import java.io.IOException;

public class MainActivity extends Activity {

    static {
        System.loadLibrary("native-lib");
    }

    static final String UID = "U4SMNNDT79Y3FF5E111A";
    private static final String URL = "http://fairee.vicp.net:83/2016rm/0116/baishi160116.mp4";
    private int position;//记录位置

//    static final String UID = "C1KAB554Z3RMHH6GU1Z1";

    private XPlay surfaceView;
    private Button mButton;
    private EditText text_uid;
    private TextView textView;
    private MyHnadler myHnadler = null;
    private MediaPlayer mediaPlayer;

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
        //初始化
        mediaPlayer = new MediaPlayer();
        surfaceView = findViewById(R.id.surfaceView);
        surfaceView.setUrl("rtsp://admin:cvte123456@172.18.223.100:554/mpeg4/ch1/main/av_stream",new MyHandler());
        //添加回调接口
//        surfaceView.getHolder().addCallback(callback);
        mButton.setOnClickListener(view -> {
            textView.setText("");
            (new Thread() {
                public void run() {
                    String uid = text_uid.getText().toString().trim();
//                        Client.start(UID,myHnadler);
                    Client.start(uid,myHnadler);
                }
            }).start();
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

    public void play() {
        try {
            mediaPlayer.reset();//重置
            mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mediaPlayer.setDataSource(URL);//设置播放路径
            mediaPlayer.setDisplay(surfaceView.getHolder());//视频输出到SurfaceView上
            mediaPlayer.prepare();//使用同步方式
            mediaPlayer.start();//开始播放

        } catch (IOException e) {
            e.printStackTrace();
            Toast.makeText(MainActivity.this, "路径错误", Toast.LENGTH_SHORT).show();
        }
    }
}
