package com.example.uwowirelessmotionsensortrackingapp.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import com.example.uwowirelessmotionsensortrackingapp.Board;
import com.example.uwowirelessmotionsensortrackingapp.params.Params;

import java.util.ArrayList;
import java.util.List;


public class MyDbHandler extends SQLiteOpenHelper {

    public MyDbHandler(Context context) {
        super(context, Params.DB_NAME, null, Params.DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String create = "CREATE TABLE " + Params.BOARD_TABLE_0 + "("
                + Params.KEY_ID + " INTEGER PRIMARY KEY," + Params.KEY_TIME
                + " TEXT,"
                + Params.KEY_SENSOR1 + " TEXT,"
                +Params.KEY_SENSOR2 + " TEXT,"
                +Params.KEY_SENSOR3 + " TEXT,"
                +Params.KEY_SENSOR4 + " TEXT,"
                + Params.KEY_SENSOR5 + " TEXT,"
                + Params.KEY_SENSOR6 + " TEXT" +")";

        Log.d("finn", "Query being run is : "+ create);
        db.execSQL(create);

    }



    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
    public void addSensorData(Board board, String table){
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(Params.KEY_TIME, board.getTime());
        values.put(Params.KEY_SENSOR1, board.getSensor1());
        values.put(Params.KEY_SENSOR2, board.getSensor2());
        values.put(Params.KEY_SENSOR3, board.getSensor3());
        values.put(Params.KEY_SENSOR4, board.getSensor4());
        values.put(Params.KEY_SENSOR5, board.getSensor5());
        values.put(Params.KEY_SENSOR6, board.getSensor6());


        db.insert(table, null, values);
        Log.d("finn", "Successfully inserted");
        db.close();


    }

    public List<Board> getAllData(String table){
        List<Board> sensorDataList = new ArrayList<>();
        SQLiteDatabase db = this.getReadableDatabase();

        // Generate the query to read from the database
        String select = "SELECT * FROM " + table;
        Cursor cursor = db.rawQuery(select, null);

        //Loop through now
        if(cursor.moveToFirst()){
            do{
                Board board = new Board();
                board.setId(Integer.parseInt(cursor.getString(0)));
                board.setTime(cursor.getString(1));
                board.setSensor1(cursor.getString(2));
                board.setSensor2(cursor.getString(3));
                board.setSensor3(cursor.getString(4));
                board.setSensor4(cursor.getString(5));
                board.setSensor5(cursor.getString(6));
                board.setSensor6(cursor.getString(7));
                sensorDataList.add(board);
            }while(cursor.moveToNext());
        }
        return sensorDataList;
    }

}
