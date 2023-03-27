package com.example.uwowirelessmotionsensortrackingapp;

import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.example.uwowirelessmotionsensortrackingapp.data.MyDbHandler;
import com.example.uwowirelessmotionsensortrackingapp.params.Params;
import com.android.volley.toolbox.JsonArrayRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.example.uwowirelessmotionsensortrackingapp.data.MyDbHandler;
import com.example.uwowirelessmotionsensortrackingapp.params.FixedParameters;
import com.example.uwowirelessmotionsensortrackingapp.params.Params;

import java.util.List;

public class Test extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_options);
    //creating database
    MyDbHandler db = new MyDbHandler(Test.this);

    //creating board object now
    Board dbBoard0 = new Board();



for(int i=0;i<10;i++){
    dbBoard0.setTime("00");
    dbBoard0.setSensor1("00");
    dbBoard0.setSensor2("00");
    dbBoard0.setSensor3("00");
    dbBoard0.setSensor4("00");
    dbBoard0.setSensor5("00");
    dbBoard0.setSensor6("00");
    db.addSensorData(dbBoard0, Params.BOARD_TABLE_0);

    //creating list
    List<Board> allSensorData = db.getAllData(Params.BOARD_TABLE_0);

    for(Board tempBoard: allSensorData){
        Log.d("finn", "\nId: " + tempBoard.getId() + "\n" +
                "Time: " + tempBoard.getTime() + "\n"
                + "Sensor 1 Data: " + tempBoard.getSensor1() + "\n"
                + "Sensor 2 Data: " + tempBoard.getSensor2() + "\n"
                + "Sensor 3 Data: " + tempBoard.getSensor3() + "\n"
                + "Sensor 4 Data: " + tempBoard.getSensor4() + "\n"
                + "Sensor 5 Data: " + tempBoard.getSensor5() + "\n"
                + "Sensor 6 Data: " + tempBoard.getSensor6() + "\n");


    }
}

}}
