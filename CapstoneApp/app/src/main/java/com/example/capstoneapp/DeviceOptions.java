package com.example.capstoneapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class DeviceOptions extends AppCompatActivity {
    TextView ipAddressStore;
    Button rawData, extractedData;
    //test comment
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_options);

        ipAddressStore=findViewById(R.id.ip_Address_Store);
        rawData = findViewById(R.id.raw_Data);
        extractedData=findViewById(R.id.extracted_Data);

        Intent i=getIntent();
        String ipAddressValue = i.getStringExtra("ipAddress");
        ipAddressStore.setText("IP Address: "+ipAddressValue);

        rawData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent j = new Intent(getApplicationContext(),DataRaw.class);
                startActivity(j);

            }
        });

    }
}