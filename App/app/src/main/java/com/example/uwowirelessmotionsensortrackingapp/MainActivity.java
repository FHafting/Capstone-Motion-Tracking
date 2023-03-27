package com.example.uwowirelessmotionsensortrackingapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {
    EditText ipTextField;
    Button beginBtn;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ipTextField=findViewById(R.id.ipTextField);
        beginBtn=findViewById(R.id.beginBtn);


        beginBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String ipAddress = ipTextField.getText().toString();
                Intent i = new Intent(getApplicationContext(), Test2.class);
                i.putExtra("ipAddress",ipAddress);
                startActivity(i);
            }
        });





    }
}