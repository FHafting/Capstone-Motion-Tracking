package com.example.capstoneapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;
//test comment
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class DataRaw extends AppCompatActivity {
private TextView mTextViewResult;
private RequestQueue mQueue;

incomingData board1;
incomingData board2;
incomingData board3;
incomingData board4;
incomingData board5;
incomingData board6;

incomingData boardData[] = {board1,board2,board3,board4,board5,board6};
int testVar;

@Override
    protected void onCreate(Bundle savedInstanceState){
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_data_raw);

    mTextViewResult = findViewById(R.id.text_view_result);
    Button buttonParse = findViewById(R.id.button_parse);

    mQueue = Volley.newRequestQueue(this);

    buttonParse.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            jsonParse();
        }
    });

    mTextViewResult.setMovementMethod(new ScrollingMovementMethod());
}

    private void jsonParse() {

        String url = "https://run.mocky.io/v3/3276125e-d145-48b4-9dd0-42d232382435";

        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        try {
                            JSONArray jsonArray = response.getJSONArray("boards");

                            for (int i = 0; i < jsonArray.length(); i++) {
                                JSONObject board = jsonArray.getJSONObject(i);

                                String id = board.getString("id");
                                String time = board.getString("time");

 /*
                                JSONArray accelxArray = board.getJSONArray("accelx");
                                JSONArray accelyArray = board.getJSONArray("accelx");
                                JSONArray accelzArray = board.getJSONArray("accelx");
                                JSONArray gyroxArray = board.getJSONArray("accelx");
                                JSONArray gyroyArray = board.getJSONArray("accelx");
                                JSONArray gyrozArray = board.getJSONArray("accelx");

                                for (int j = 0; j< accelxArray.length();j++){
                                    JSONObject accelxData = accelxArray.getJSONObject(j);
                                    JSONObject accelyData = accelyArray.getJSONObject(j);
                                    JSONObject accelzData = accelzArray.getJSONObject(j);
                                    JSONObject gyroxData = gyroxArray.getJSONObject(j);
                                    JSONObject gyroyData = gyroyArray.getJSONObject(j);
                                    JSONObject gyrozData = gyrozArray.getJSONObject(j);

                                    boardData[i].accelx[j] = String.valueOf(accelxData);
                                    boardData[i].accely[j] = String.valueOf(accelyData);
                                    boardData[i].accelz[j] = String.valueOf(accelzData);
                                    boardData[i].gyrox[j] = String.valueOf(gyroxData);
                                    boardData[i].gyroy[j] = String.valueOf(gyroyData);
                                    boardData[i].gyroz[j] = String.valueOf(gyrozData);


                                }*/


                                mTextViewResult.append("ID: " + id + "Time: " +  time +"\n");
                            }
                        } catch (JSONException e) {
                            e.printStackTrace();
                        }
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                error.printStackTrace();
            }
        });

        mQueue.add(request);
    }
}