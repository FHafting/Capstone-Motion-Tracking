package com.example.uwowirelessmotionsensortrackingapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonArrayRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;



public class DeviceOptionsActivity extends AppCompatActivity {

    //declarations
    TextView ipAddressView, test1view;
    Button start,reset,download,history;
    int dataStatus=0;
    String statusString = "Not Connected";
    int timerStatus=0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_options);


        //linking
        ipAddressView = findViewById(R.id.ip_address);
        test1view = findViewById(R.id.test1_view);
        test1view.setMovementMethod(new ScrollingMovementMethod());
        start = findViewById(R.id.start_btn);
        reset = findViewById(R.id.reset_btn);
        download = findViewById(R.id.download_btn);
        history = findViewById(R.id.history_btn);
        //retrieving IP address
        Intent intent = getIntent();
        String ipAddressValue = intent.getStringExtra("ipAddress");



        String temp="IP Address: " + ipAddressValue + "\n"
                +"Status: "+statusString+"\n"
                +"Session Duration:";

        ipAddressView.setText(temp);

        //setting up volley
        RequestQueue requestQueue;
        requestQueue = Volley.newRequestQueue(this);


        //adding timer
        Chronometer chronometer = findViewById(R.id.chronometer);
        chronometer.setFormat("%s");
        chronometer.setBase(SystemClock.elapsedRealtime());


        //adding timer functionality
        start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if(dataStatus==1){

                    chronometer.setBase(SystemClock.elapsedRealtime());
                    chronometer.start();
                    timerStatus=1;
                }else{
                    Toast.makeText(DeviceOptionsActivity.this, "Device not Connected" , Toast.LENGTH_SHORT).show();
                }



            }
        });

        reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                chronometer.stop();
                chronometer.setBase(SystemClock.elapsedRealtime());
                timerStatus=0;
            }
        });



        String url="http://"+ipAddressValue+"/";
        ArrayList<String> accelxData = new ArrayList<>();
        ArrayList<String> accelyData = new ArrayList<>();
        ArrayList<String> accelzData = new ArrayList<>();
        ArrayList<String> gyroxData = new ArrayList<>();
        ArrayList<String> gyroyData = new ArrayList<>();
        ArrayList<String> gyrozData = new ArrayList<>();
        // Task Scheduler using Handler and Runnable

                final android.os.Handler handler = new Handler();
                Runnable run = new Runnable(){
                    @Override
                    public void run() {
                        JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET,
                                url, null, new Response.Listener<JSONObject>() {
                            @Override
                            public void onResponse(JSONObject response) {
                                try {
                                    dataStatus=1;
                                    JSONArray boards = response.getJSONArray("boards");

                                    for (int i=0; i<boards.length();i++){
                                        JSONObject board = boards.getJSONObject(i);

                                        JSONArray accelx = board.getJSONArray("accelx");
                                        JSONArray accely = board.getJSONArray("accely");
                                        JSONArray accelz = board.getJSONArray("accelz");
                                        JSONArray gyrox = board.getJSONArray("gyrox");
                                        JSONArray gyroy = board.getJSONArray("gyroy");
                                        JSONArray gyroz = board.getJSONArray("gyroz");

                                        for (int j=0; j<accelx.length();j++) {

                                            accelxData.add(String.valueOf(accelx.get(j)));
                                            accelyData.add(String.valueOf(accely.get(j)));
                                            accelzData.add(String.valueOf(accelz.get(j)));
                                            gyroxData.add(String.valueOf(gyrox.get(j)));
                                            gyroyData.add(String.valueOf(gyroy.get(j)));
                                            gyrozData.add(String.valueOf(gyroz.get(j)));

                                        }

                                        Log.d("Shubham", "onResponse: ID is " +
                                                board.getString("id") + " and time is " + board.getString("time")+accelxData+accelyData+accelzData+gyroxData+gyroyData+gyrozData);
                                        accelxData.clear();
                                        accelyData.clear();
                                        accelzData.clear();
                                        gyroxData.clear();
                                        gyroyData.clear();
                                        gyrozData.clear();
                                    }
                                } catch (JSONException e) {
                                    e.printStackTrace();

                                }
                            }
                        }, new Response.ErrorListener() {
                            @Override
                            public void onErrorResponse(VolleyError error) {
                                Log.d("Shubham", "Something went wrong");

                                dataStatus=0;
                            }
                        });
                        requestQueue.add(jsonObjectRequest);

                        //setting up status
                        if(dataStatus==1){
                            statusString="Connected";
                        }else{
                            statusString="Not Connected";
                            timerStatus=0;
                        }
                        String temp="IP Address: " + ipAddressValue + "\n"
                                +"Status: "+statusString+"\n"
                                +"Session Duration:";
                        ipAddressView.setText(temp);

                        if(timerStatus==0){
                            chronometer.stop();
                            chronometer.setBase(SystemClock.elapsedRealtime());
                        }
                        //terminating session when status loses connection

                        handler.postDelayed(this, 90);

                    }
                };
                handler.post(run);






    }
}