package com.example.uwowirelessmotionsensortrackingapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.Spinner;
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
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.opencsv.CSVWriter;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class DeviceOptionsActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener{

    //declarations
    TextView ipAddressView, test1view, pingTestView, xlabel;
    Button start, reset, download, history;
    Spinner boardSelect, sensorSelect;
    LineChart chart1;

    int numBoards = FixedParameters.numBoards;
    int numPackets = FixedParameters.packetSize;


    //characteristics
    int dataStatus = 0;     //when device is receiving data from ESP 32 Wifi
    int timerStatus = 0;    //when timer is initiated
    int pingDuration = 0;
    int activeDevices = 0;
    String dataRate = "";

    //variables to hold incoming data
    int time;
    int id;
    ArrayList<String> allBoardData = new ArrayList<>();
    float[][] allBoardData2 = new float[numPackets][7*numBoards];
    String[][] allBoardData3 = new String[numPackets][7*numBoards];
    ArrayList<String> board0Data = new ArrayList<>();
    ArrayList<String> board1Data = new ArrayList<>();
    ArrayList<String> board2Data = new ArrayList<>();
    ArrayList<String> board3Data = new ArrayList<>();
    ArrayList<String> board4Data = new ArrayList<>();
    ArrayList<String> board5Data = new ArrayList<>();
    ArrayList<String> board6Data = new ArrayList<>();
    ArrayList<String> board7Data = new ArrayList<>();
    ArrayList<String> board8Data = new ArrayList<>();
    ArrayList<String> board9Data = new ArrayList<>();
    //data format: time, accelx, accely, accelz, qyrox, qyroy, qyroz repeats for number of packets

    ArrayList<Entry>chartData = new ArrayList<Entry>();
    int chartIncrement=0;
    int parameterChanged = 0;
    float x = 0;
    float numChartEntries=700;

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

    //initializing shared preferences here
    SharedPreferences prefs;
    SharedPreferences.Editor editor;


    //csv file parameters
    int csvInProgress = 0;


    //spinner
    int boardSelected = 0;
    int sensorSelected = 0;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_options);


        //linking
        ipAddressView = findViewById(R.id.ip_address);
        boardSelect = findViewById(R.id.spinner1);
        sensorSelect = findViewById(R.id.spinner2);
        start = findViewById(R.id.start_btn);
        reset = findViewById(R.id.reset_btn);
        download = findViewById(R.id.download_btn);
        chart1 = findViewById(R.id.line_chart);
        xlabel = findViewById(R.id.x_label);
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

        //creating database
        MyDbHandler db = new MyDbHandler(com.example.uwowirelessmotionsensortrackingapp.DeviceOptionsActivity.this);

        //creating board object now
        Board dbBoard0 = new Board();
        db.deleteDb(com.example.uwowirelessmotionsensortrackingapp.DeviceOptionsActivity.this);

        //adding/updating data in sharedPreferences
        prefs = getSharedPreferences("my_prefs", MODE_PRIVATE);
        editor = prefs.edit();

        //url modifiers here - number corresponds to the async task using the url
        String tempUrl = "https://run.mocky.io/v3/51a34b15-6260-4e3e-9cf4-95021e42b4be";
        String url = "http://" + ipAddressValue + "/";
        String url3 = "http://" + ipAddressValue + "/values";
        String url4 = "http://" + ipAddressValue + "/message";


        //spinners here
        // Create an ArrayAdapter using the string array and a default spinner layout
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
                R.array.boardTypes, android.R.layout.simple_spinner_item);
        // Specify the layout to use when the list of choices appears
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Apply the adapter to the spinner
        boardSelect.setAdapter(adapter);
        boardSelect.setOnItemSelectedListener(this);

        // Create an ArrayAdapter using the string array and a default spinner layout
        ArrayAdapter<CharSequence> adapter2 = ArrayAdapter.createFromResource(this,
                R.array.sensorTypes, android.R.layout.simple_spinner_item);
        // Specify the layout to use when the list of choices appears
        adapter2.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        // Apply the adapter to the spinner
        sensorSelect.setAdapter(adapter2);
        sensorSelect.setOnItemSelectedListener(this);



        //chart stuff here
        for(float i=0;i<numChartEntries;i++){
            chartData.add(new Entry(0,0));
        }


        XAxis xAxis = chart1.getXAxis();
        xAxis.setPosition(XAxis.XAxisPosition.BOTTOM);
        xAxis.setTextColor(Color.WHITE);



        YAxis leftAxis = chart1.getAxisLeft();
        leftAxis.setTextColor(Color.WHITE);

//        leftAxis.setDrawLabels(false); // Disable left Y axis labels
        YAxis rightAxis = chart1.getAxisRight();
        rightAxis.setDrawLabels(false); // Disable right Y axis labels
        chart1.getDescription().setEnabled(false);
        chart1.setBackgroundColor(0xff000000);
        chart1.setDrawBorders(true);


        Legend legend = chart1.getLegend();
        legend.setEnabled(true);
        legend.setTextColor(Color.WHITE);
        legend.setTextSize(15);
        legend.setForm(Legend.LegendForm.LINE);
        legend.setHorizontalAlignment(Legend.LegendHorizontalAlignment.CENTER); // Set the horizontal alignment of the legend to the center
        legend.setVerticalAlignment(Legend.LegendVerticalAlignment.TOP); // Set the vertical alignment of the legend to the top

        xlabel.setBackgroundColor(0xff000000);


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

            }
        });
        download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {


            }
        });

        String temp2 = "Ping Duration: " + pingDuration + "ms" + "\n"
                + "Session Duration:";
        pingTestView.setText(temp2);

        //variables for calculating Active Devices

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

                            //clear data
                            board0Data.clear();
                            board1Data.clear();
                            board2Data.clear();
                            board3Data.clear();
                            board4Data.clear();
                            board5Data.clear();
                            board6Data.clear();
                            board7Data.clear();
                            board8Data.clear();
                            board9Data.clear();

                            for (int i = 0; i < boards.length(); i++) {     //cycles through number of board objects

                                allBoardData.clear();
                                JSONObject board = boards.getJSONObject(i);
                                //time and id
                                time = Integer.parseInt(board.getString("time"));
                                id = Integer.parseInt(board.getString("id"));

                                Log.d("unique", String.valueOf(time));

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
                                    allBoardData.add(String.valueOf((time+10*j)));
                                    allBoardData.add((String.valueOf(accelx.get(j))));
                                    allBoardData.add((String.valueOf(accely.get(j))));
                                    allBoardData.add((String.valueOf(accelz.get(j))));
                                    allBoardData.add((String.valueOf(gyrox.get(j))));
                                    allBoardData.add((String.valueOf(gyroy.get(j))));
                                    allBoardData.add((String.valueOf(gyroz.get(j))));

                                    allBoardData2[j][7*i] =  Float.parseFloat(String.valueOf(((time+10*j))));
                                    allBoardData2[j][7*i+1] = Float.parseFloat((String.valueOf(accelx.get(j))));
                                    allBoardData2[j][7*i+2] =  Float.parseFloat((String.valueOf(accely.get(j))));
                                    allBoardData2[j][7*i+3] =  Float.parseFloat((String.valueOf(accelx.get(j))));
                                    allBoardData2[j][7*i+4] =  Float.parseFloat((String.valueOf(gyrox.get(j))));
                                    allBoardData2[j][7*i+5] =  Float.parseFloat((String.valueOf(gyroy.get(j))));
                                    allBoardData2[j][7*i+6] =  Float.parseFloat((String.valueOf(gyroz.get(j))));

                                }

                                switch(i){
                                    case 0:
                                        board0Data.addAll(allBoardData);
                                        break;
                                    case 1:
                                        board1Data.addAll(allBoardData);
                                        break;
                                    case 2:
                                        board2Data.addAll(allBoardData);
                                        break;
                                    case 3:
                                        board3Data.addAll(allBoardData);
                                        break;
                                    case 4:
                                        board4Data.addAll(allBoardData);
                                        break;
                                    case 5:
                                        board5Data.addAll(allBoardData);
                                        break;
                                    case 6:
                                        board6Data.addAll(allBoardData);
                                        break;
                                    case 7:
                                        board7Data.addAll(allBoardData);
                                        break;
                                    case 8:
                                        board8Data.addAll(allBoardData);
                                        break;
                                    case 9:
                                        board9Data.addAll(allBoardData);
                                        break;
                                }

                                Log.d("wowza",String.valueOf(board0Data)+ String.valueOf(board9Data));




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
                timeDly = FixedParameters.numBoards * 10 - (flag2 - flag1);
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
                        + "Active Senders: " + activeDevices + "\n"
                        + "Datarate (Packets/s): " + dataRate + "\n"
                        ;
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

                if (dataStatus == 1) {
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

                    if (pingInProgress == 1) {
                        if (postPing != "") {
                            postFlag2 = (int) System.currentTimeMillis();
                            pingDuration = pingDuration + postFlag2 - postFlag1;
                            pingInProgress = 0;
                            pingCounter++;
                            //Log.d("courtney","pingTest Completed. Duration: "+pingDuration);

                        }
                    }

                    if (pingCounter == 5) {
                        pingDuration = pingDuration / 10;
                        Log.d("courtney", "pingTest Completed. Duration: " + pingDuration);
                        pingCounter = 0;
                        //status update - second textview
                        String temp2 = "Ping Duration: " + pingDuration + "ms" + "\n"
                                + "Session Duration:";
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
        //async task 5: csv file
/*
        final android.os.Handler handler5 = new Handler();
        Runnable run5 = new Runnable() {
            @Override
            public void run() {
                if(timerStatus==1){
                    csvInProgress=1;
                    }else{
                        for(int j=0;j<numPackets;j++){
                            String temp[] = allBoardData2[j];
                            writer.writeNext(temp);
                        }
                    }
                }else{
                    if(csvInProgress==1){
                        try {
                            writer.close();
                            Log.d("csv","file closed!");
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        csvInProgress=0;
                    }
                }
                handler5.postDelayed(this, timeDly);//running task
            }
        };
        handler5.post(run5);
*/
// *******************************************************************************************************************


        //async task 6: displaying charts
        final android.os.Handler handler6 = new Handler();
        Runnable run6 = new Runnable() {
            @Override
            public void run() {


                //float x = allBoardData2[chartIncrement][7*boardSelected];


                for(int j=0;j<numPackets;j++){
                    float x = allBoardData2[j][7*boardSelected];
                    float y = allBoardData2[j][7*boardSelected+(sensorSelected+1)];
                    chartData.remove(0);
                    chartData.add(new Entry(x,y));

                }

                LineDataSet lineDataSet = new LineDataSet(chartData, (String) adapter.getItem(boardSelected)+": "+(String) adapter2.getItem(sensorSelected));
                lineDataSet.setDrawValues(false);
                lineDataSet.setDrawCircles(false);
                lineDataSet.setAxisDependency(YAxis.AxisDependency.LEFT);
                List<ILineDataSet> dataSets = new ArrayList<ILineDataSet>();
                dataSets.add(lineDataSet);
                LineData data = new LineData(dataSets);
                chart1.setData(data);
                chart1.animateXY(10,10 );
                chart1.invalidate();

                if(parameterChanged==1){
                    chartData.clear();
                    for(float i=0;i<numChartEntries;i++){
                        chartData.add(new Entry(0,0));
                    }
                    parameterChanged=0;

                }

                handler6.postDelayed(this, timeDly);//running task
            }
        };
        handler6.post(run6);

// *******************************************************************************************************************


    }

    public void createCsvFile(String fileName, List<String[]> dataList) {
        String baseDir = android.os.Environment.getExternalStorageDirectory().getAbsolutePath();
        String filePath = baseDir + File.separator + fileName;
        File f = new File(filePath);

        try {
            FileWriter writer = new FileWriter(f);
            CSVWriter csvWriter = new CSVWriter(writer);
            csvWriter.writeAll(dataList);
            csvWriter.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        switch (adapterView.getId()) {
            case R.id.spinner1:
                boardSelected = i;
                Log.d("coco","Board Selected: "+boardSelected);
                parameterChanged=1;
                break;
            case R.id.spinner2:
                sensorSelected = i;
                Log.d("coco","Sensor Selected: "+sensorSelected);
                parameterChanged=1;
                break;

        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
}