package com.example.uwowirelessmotionsensortrackingapp;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.example.uwowirelessmotionsensortrackingapp.data.MyDbHandler;
import com.example.uwowirelessmotionsensortrackingapp.params.Params;

import java.util.List;

import android.util.Log;
import android.widget.TextView;

public class Test2 extends AppCompatActivity {
    TextView test2_view;

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_device_options);

            test2_view = findViewById(R.id.test2_view);

            //creating database
            MyDbHandler db = new MyDbHandler(com.example.uwowirelessmotionsensortrackingapp.Test2.this);

            //creating board object now
            Board dbBoard0 = new Board();

// Creating a contact object for the db
            Board board = new Board();
            board.setTime("12345");
            board.setSensor1("sensor1");

            // Adding a contact to the db
            db.addSensorData(board);


            // Get all contacts
            List<Board> allSensorData = db.getAllData();
            for(Board contact: allSensorData){
                Log.d("dbharry", "\nId: " + contact.getId() + "\n" +
                        "Time: " + contact.getTime() + "\n"+
                        "Sensor 1 data: " + contact.getSensor1() + "\n" );

            }





        }}

