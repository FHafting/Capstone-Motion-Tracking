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
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.example.uwowirelessmotionsensortrackingapp.data.MyDbHandler;
import com.example.uwowirelessmotionsensortrackingapp.params.FixedParameters;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class DeviceOptionsActivity extends AppCompatActivity {

    //declarations
    TextView ipAddressView, test1view;
    Button start,reset,download,history;
    int dataStatus=0;
    String statusString = "Not Connected";
    int timerStatus=0;
    String time;
    String id;
    int activeDevices=0;
    int temp100;
    int flag1;
    int flag2;
    int timeDly;
    int numBoards = FixedParameters.numBoards;
    int numPackets = FixedParameters.packetSize;
    String dataRate;
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

        //creating database
        MyDbHandler db = new MyDbHandler(com.example.uwowirelessmotionsensortrackingapp.DeviceOptionsActivity.this);

        //creating board object now
        Board dbBoard0 = new Board();

        db.deleteDb(com.example.uwowirelessmotionsensortrackingapp.DeviceOptionsActivity.this);



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
                //**************************************************************************************************************************
               // db.deleteDb(com.example.uwowirelessmotionsensortrackingapp.DeviceOptionsActivity.this);
                //**************************************************************************************************************************
            }
        });


        download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

            }
        });


        String url="http://"+ipAddressValue+"/";
        //String url = "https://run.mocky.io/v3/3276125e-d145-48b4-9dd0-42d232382435";
        ArrayList<String> accelxData = new ArrayList<>();
        ArrayList<String> accelyData = new ArrayList<>();
        ArrayList<String> accelzData = new ArrayList<>();
        ArrayList<String> gyroxData = new ArrayList<>();
        ArrayList<String> gyroyData = new ArrayList<>();
        ArrayList<String> gyrozData = new ArrayList<>();


        int [] boardStatus=new int[numBoards];
        int [] statusCounter=new int[numBoards];
        int [] prevTime=new int[numBoards];
        int [] currentTime=new int[numBoards];

        // Task Scheduler using Handler and Runnable
//*******************************************************************************************************************
        final android.os.Handler handler = new Handler();
        Runnable run = new Runnable(){
            @Override
            public void run() {

                //noting the start time of execution
                flag1 = (int) System.currentTimeMillis();

                JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET,
                        url, null, new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        try {
                            dataStatus=1;
                            JSONArray boards = response.getJSONArray("boards");

                            for (int i=0; i<boards.length();i++){
                                JSONObject board = boards.getJSONObject(i);
                                //time and id
                                time=board.getString("time");
                                id=board.getString("id");


                                //adding time in current time array for device status
                                currentTime[i] = Integer.valueOf(time);

                                //JSON arrays for sensor data
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

                                    dbBoard0.setTime(time+j*10);
                                    dbBoard0.setSensor1(accelxData.get(j));
                                    dbBoard0.setSensor2(accelyData.get(j));
                                    dbBoard0.setSensor3(accelzData.get(j));
                                    dbBoard0.setSensor4(gyroxData.get(j));
                                    dbBoard0.setSensor5(gyroyData.get(j));
                                    dbBoard0.setSensor6(gyrozData.get(j));

                                    //**************************************************************************************************************************
                                  /*  if(timerStatus==1){
                                        switch(i){
                                            case 0:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_0);
                                                break;
                                            case 1:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_1);
                                                break;
                                            case 2:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_2);
                                                break;
                                            case 3:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_3);
                                                break;
                                            case 4:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_4);
                                                break;
                                            case 5:
                                                db.addSensorData(dbBoard0, MyDbHandler.BOARD_TABLE_5);
                                                break;

                                        }
                                    }*/

                                    //**************************************************************************************************************************
                                }









                                //**************************************************************************************************************************
                                //creating list

                                 /*   String [] dbBoards = {MyDbHandler.BOARD_TABLE_0,MyDbHandler.BOARD_TABLE_1,MyDbHandler.BOARD_TABLE_2,MyDbHandler.BOARD_TABLE_3,MyDbHandler.BOARD_TABLE_4,MyDbHandler.BOARD_TABLE_5};
                                    List<Board> allSensorData = db.getAllData(dbBoards[i]);
                                    for(Board tempBoard: allSensorData){
                                        Log.d("finn", "\nId: " + tempBoard.getId() + "\n"
                                                +dbBoards[i] + "\n"
                                                +"Time: " + tempBoard.getTime() + "\n"
                                                + "Sensor 1 Data: " + tempBoard.getSensor1() + "\n"
                                                + "Sensor 2 Data: " + tempBoard.getSensor2() + "\n"
                                                + "Sensor 3 Data: " + tempBoard.getSensor3() + "\n"
                                                + "Sensor 4 Data: " + tempBoard.getSensor4() + "\n"
                                                + "Sensor 5 Data: " + tempBoard.getSensor5() + "\n"
                                                + "Sensor 6 Data: " + tempBoard.getSensor6() + "\n");
                                    }*/

                                //**************************************************************************************************************************




                                Log.d("Shubham", "onResponse: ID is " +
                                        id + " and time is " + time+accelxData+accelyData+accelzData+gyroxData+gyroyData+gyrozData);

                                //clear data
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


                //noting the end time of execution
                flag2 = (int) System.currentTimeMillis();
                timeDly = 90-(flag2-flag1);
                Log.d("shiv2", "time delay for task 1: "+ timeDly);

                handler.postDelayed(this, timeDly);

            }
        };
        handler.post(run);

//*******************************************************************************************************************

        final android.os.Handler handler2 = new Handler();
        Runnable run2 = new Runnable(){
            @Override
            public void run() {

                //setting up status
                if(dataStatus==1){
                    statusString="Connected";
                }else{
                    statusString="Not Connected";
                    timerStatus=0;
                }


                if(timerStatus==0){
                    chronometer.stop();
                    chronometer.setBase(SystemClock.elapsedRealtime());
                }

                //active devices
                for(int i =0; i<currentTime.length; i++){

                    if(currentTime[i]-prevTime[i]!=0){
                        statusCounter[i] ++;

                    }else{
                        statusCounter[i] --;

                    }

                    if(statusCounter[i]>5){
                        boardStatus[i]=1;
                        statusCounter[i]=0;
                    }
                    if(statusCounter[i]<-5){
                        boardStatus[i]=0;
                        statusCounter[i]=0;
                    }

                    activeDevices=activeDevices+boardStatus[i];

                    prevTime[i]=currentTime[i];
                }



                //status update
                String temp="IP Address: " + ipAddressValue + "\n"
                        +"Status: "+statusString+"\n"
                        +"Active Devices: "+activeDevices+ "\n"
                        +"DataRate (Packets/s): "+dataRate+ "\n"
                        +"Session Duration:";
                ipAddressView.setText(temp);

                Log.d("boardstatus", "boardStatus: "+ Arrays.toString(statusCounter)+activeDevices);
                //must occur after status update



                handler2.postDelayed(this, timeDly);
                activeDevices=0;
            }
        };
        handler2.post(run2);


//*******************************************************************************************************************
        String url2="http://"+ipAddressValue+"/datarate";

        final android.os.Handler handler3 = new Handler();
        Runnable run3 = new Runnable(){
            @Override
            public void run() {

                JsonObjectRequest jsonObjectRequest2 = new JsonObjectRequest(Request.Method.GET,
                        url2, null, new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response2) {
                        try {
                            dataRate = response2.getString("dataRate");
                            Log.d("jenn",dataRate);
                        } catch (JSONException e) {
                            e.printStackTrace();

                        }
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.d("jenn", "Something went wrong");


                    }
                });
                requestQueue.add(jsonObjectRequest2);

                handler3.postDelayed(this, 1000);

            }
        };
        handler3.post(run3);
//*******************************************************************************************************************

    }


}