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
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.example.uwowirelessmotionsensortrackingapp.data.MyDbHandler;
import com.example.uwowirelessmotionsensortrackingapp.params.FixedParameters;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class DeviceOptionsActivity extends AppCompatActivity {

    //declarations
    TextView ipAddressView, test1view, pingTestView;
    Button start, reset, download, history;

    int numBoards = FixedParameters.numBoards;
    int numPackets = FixedParameters.packetSize;


    //characteristics
    int dataStatus = 0;     //when device is receiving data from ESP 32 Wifi
    int timerStatus = 0;    //when timer is initiated
    int pingDuration = 0;
    int activeDevices = 0;
    String dataRate="";

    //variables to hold incoming data
    String time;
    String id;

    //other variables
    String statusString = "Not Connected";

    //variables for main loop (async task 1)
    int flag1;
    int flag2;
    int timeDly;

    //variables for response time
    int pingInProgress = 0;
    String postPing;
    int postFlag1;
    int postFlag2;
    int pingCounter;
    //step 1: post request to send a key value pair "message" and "ping" to ESP32 Wifi using /message
            //as soon as it is successful, a flag tracks the exact time at this instant in postFlag1
    //step 2: ESP 32 Wifi receives it and stores it in a string called "incomingMessage"
    //step 3: ESP 32 wifi invokes a function called pingTest() which, every 10 ms checks for value in message variable
                //if there is a value in incomingMessage, it makes postPing=1
                //postPing is transmitted to Android app via get request in async task 3
                //as soon as this variable turns to 1, it invokes an if statement inside task 4 which then initiates a second flag in postFlag2
                //difference in time /2 = pingDuration








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
        pingTestView = findViewById(R.id.ping_test);

        //retrieving IP address and initializing
        Intent intent = getIntent();
        String ipAddressValue = intent.getStringExtra("ipAddress");
        String temp = "IP Address: " + ipAddressValue + "\n"
                + "Status: " + statusString + "\n"
                + "Session Duration:";
        ipAddressView.setText(temp);

        //setting up volley for async task 1
        RequestQueue requestQueue;
        requestQueue = Volley.newRequestQueue(this);

        //creating timer - chronometer - for visual time
        Chronometer chronometer = findViewById(R.id.chronometer);
        chronometer.setFormat("%s");
        chronometer.setBase(SystemClock.elapsedRealtime());


        //url modifiers here - number corresponds to the async task using the url
        String url = "http://" + ipAddressValue + "/";
        String url3 = "http://" + ipAddressValue + "/values";
        String url4 = "http://" + ipAddressValue + "/message";

        //buttons here
        start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                if (dataStatus == 1) {

                    chronometer.setBase(SystemClock.elapsedRealtime());
                    chronometer.start();
                    timerStatus = 1;
                } else {
                    Toast.makeText(DeviceOptionsActivity.this, "Device not Connected", Toast.LENGTH_SHORT).show();
                }


            }
        });
        reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                chronometer.stop();
                chronometer.setBase(SystemClock.elapsedRealtime());
                timerStatus = 0;
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



        //String url = "https://run.mocky.io/v3/3276125e-d145-48b4-9dd0-42d232382435";
        ArrayList<String> accelxData = new ArrayList<>();
        ArrayList<String> accelyData = new ArrayList<>();
        ArrayList<String> accelzData = new ArrayList<>();
        ArrayList<String> gyroxData = new ArrayList<>();
        ArrayList<String> gyroyData = new ArrayList<>();
        ArrayList<String> gyrozData = new ArrayList<>();


        int[] boardStatus = new int[numBoards];
        int[] statusCounter = new int[numBoards];
        int[] prevTime = new int[numBoards];
        int[] currentTime = new int[numBoards];

//*******************************************************************************************************************
       //async task 1: get and store JSON data
        final android.os.Handler handler = new Handler();
        Runnable run = new Runnable() {
            @Override
            public void run() {

                //noting the start time of execution
                flag1 = (int) System.currentTimeMillis();

                JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET,
                        url, null, new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        try {
                            dataStatus = 1;
                            JSONArray boards = response.getJSONArray("boards");

                            for (int i = 0; i < boards.length(); i++) {
                                JSONObject board = boards.getJSONObject(i);
                                //time and id
                                time = board.getString("time");
                                id = board.getString("id");


                                //adding time in current time array for device status
                                currentTime[i] = Integer.valueOf(time);

                                //JSON arrays for sensor data
                                JSONArray accelx = board.getJSONArray("accelx");
                                JSONArray accely = board.getJSONArray("accely");
                                JSONArray accelz = board.getJSONArray("accelz");
                                JSONArray gyrox = board.getJSONArray("gyrox");
                                JSONArray gyroy = board.getJSONArray("gyroy");
                                JSONArray gyroz = board.getJSONArray("gyroz");

                                for (int j = 0; j < accelx.length(); j++) {

                                    accelxData.add(String.valueOf(accelx.get(j)));
                                    accelyData.add(String.valueOf(accely.get(j)));
                                    accelzData.add(String.valueOf(accelz.get(j)));
                                    gyroxData.add(String.valueOf(gyrox.get(j)));
                                    gyroyData.add(String.valueOf(gyroy.get(j)));
                                    gyrozData.add(String.valueOf(gyroz.get(j)));



                                }


                                Log.d("Shubham", "onResponse: ID is " +
                                        id + " and time is " + time + accelxData + accelyData + accelzData + gyroxData + gyroyData + gyrozData);

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

                        dataStatus = 0;
                    }
                });
                requestQueue.add(jsonObjectRequest);


                //noting the end time of execution
                flag2 = (int) System.currentTimeMillis();
                timeDly = FixedParameters.numBoards*10 - (flag2 - flag1);
                Log.d("shiv2", "time delay for task 1: " + timeDly);

                handler.postDelayed(this, timeDly);

            }
        };
        handler.post(run);

//*******************************************************************************************************************
        //async task 2: statements for:
        //dataStatus - whether sensor data is coming in
        //timerStatus - whether timer is on
        //activeDevices - how many senders are connected

        final android.os.Handler handler2 = new Handler();
        Runnable run2 = new Runnable() {
            @Override
            public void run() {

                //setting up status
                if (dataStatus == 1) {
                    statusString = "Connected";
                } else {
                    statusString = "Not Connected";
                    timerStatus = 0;
                }


                if (timerStatus == 0) {
                    chronometer.stop();
                    chronometer.setBase(SystemClock.elapsedRealtime());
                }

                //active devices
                for (int i = 0; i < currentTime.length; i++) {

                    if (currentTime[i] - prevTime[i] != 0) {
                        statusCounter[i]++;

                    } else {
                        statusCounter[i]--;

                    }

                    if (statusCounter[i] > 5) {
                        boardStatus[i] = 1;
                        statusCounter[i] = 0;
                    }
                    if (statusCounter[i] < -5) {
                        boardStatus[i] = 0;
                        statusCounter[i] = 0;
                    }

                    activeDevices = activeDevices + boardStatus[i];

                    prevTime[i] = currentTime[i];
                }


                //status update - first textview
                String temp = "IP Address: " + ipAddressValue + "\n"
                        + "Status: " + statusString + "\n"
                        + "Active Devices: " + activeDevices + "\n"
                        + "Datarate (Packets/s): " + dataRate + "\n"
                        + "Session Duration:";
                ipAddressView.setText(temp);



                Log.d("boardstatus", "boardStatus: " + Arrays.toString(statusCounter) + activeDevices);
                //must occur after status update


                handler2.postDelayed(this, timeDly);
                activeDevices = 0;
            }
        };
        handler2.post(run2);

//*******************************************************************************************************************
        //async task 3: statements for /values
        //retrieves and stores these values in variables
        //dataRate - transmits the amount of packets/second received by esp 32 wifi
        //postPing - incoming variable from ESP 32 wifi that states that it received the post request

        final android.os.Handler handler3 = new Handler();
        Runnable run3 = new Runnable() {
            @Override
            public void run() {

                if(dataStatus==1){
                    JsonObjectRequest jsonObjectRequest3 = new JsonObjectRequest(Request.Method.GET,
                            url3, null, new Response.Listener<JSONObject>() {
                        @Override
                        public void onResponse(JSONObject response3) {
                            try {
                                dataRate = response3.getString("dataRate");
                                postPing = response3.getString("postPing");
                                Log.d("jenn", dataRate);
                                Log.d("courtney", postPing);
                            } catch (JSONException e) {
                                e.printStackTrace();

                            }
                        }
                    }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            Log.d("jenn", "Something went wrong");
                            Log.d("courtney", "cannot make get request");

                        }
                    });
                    requestQueue.add(jsonObjectRequest3);

                    if(pingInProgress==1){
                        if(postPing!=""){
                            postFlag2 = (int) System.currentTimeMillis();
                            pingDuration = pingDuration + postFlag2-postFlag1;
                            pingInProgress=0;
                            pingCounter++;
                            //Log.d("courtney","pingTest Completed. Duration: "+pingDuration);

                        }
                    }

                    if(pingCounter==10){
                        pingDuration=pingDuration/10;
                        Log.d("courtney","pingTest Completed. Duration: "+ pingDuration);
                        pingCounter=0;
                        //status update - second textview
                        String temp2 = "Ping Duration: " + pingDuration + "ms"+"\n"

                                ;
                        pingTestView.setText(temp2);
                    }


                }

                handler3.postDelayed(this, 5);
            }
        };
        handler3.post(run3);
//*******************************************************************************************************************
        //async task 4: statements for /message

        final android.os.Handler handler4 = new Handler();
        Runnable run4 = new Runnable() {
            @Override
            public void run() {


                if (pingInProgress == 0) {
                    StringRequest postRequest = new StringRequest(Request.Method.POST, url4,
                            new Response.Listener<String>() {
                                @Override
                                public void onResponse(String response) {
                                    Log.d("courtney", "Message Sent");

                                    postFlag1 = (int) System.currentTimeMillis();
                                    pingInProgress = 1;
                                    // Handle response
                                }
                            },
                            new Response.ErrorListener() {
                                @Override
                                public void onErrorResponse(VolleyError error) {
                                    // Handle error
                                    Log.d("courtney", "Something went wrong");
                                }
                            }
                    ) {
                        @Override
                        protected Map<String, String> getParams() {
                            Map<String, String> params = new HashMap<String, String>();
                            params.put("message", "ping");
                            return params;
                        }
                    };
                    requestQueue.add(postRequest);//adding post request
                }


                    handler4.postDelayed(this, 500);//running task
            }
        };
        handler4.post(run4);

// *******************************************************************************************************************


    }


}