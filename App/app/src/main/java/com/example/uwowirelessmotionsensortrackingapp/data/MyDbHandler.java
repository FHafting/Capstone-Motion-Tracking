package com.example.uwowirelessmotionsensortrackingapp.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;

import com.example.uwowirelessmotionsensortrackingapp.Board;

import java.util.ArrayList;
import java.util.List;


public class MyDbHandler extends SQLiteOpenHelper {


    public static final int DB_VERSION = 2;
    public static final String DB_NAME = "board_data_db_2";


    public static final String BOARD_TABLE_0 = "board_table_0";
    public static final String BOARD_TABLE_1 = "board_table_1";
    public static final String BOARD_TABLE_2 = "board_table_2";
    public static final String BOARD_TABLE_3 = "board_table_3";
    public static final String BOARD_TABLE_4 = "board_table_4";
    public static final String BOARD_TABLE_5 = "board_table_5";


    //Keys of our table in db
    public static final String KEY_ID = "id";
    public static final String KEY_TIME = "time";
    public static final String KEY_SENSOR1 = "sensor1";
    public static final String KEY_SENSOR2 = "sensor2";
    public static final String KEY_SENSOR3 = "sensor3";
    public static final String KEY_SENSOR4 = "sensor4";
    public static final String KEY_SENSOR5 = "sensor5";
    public static final String KEY_SENSOR6 = "sensor6";




    public MyDbHandler(Context context) {
        super(context, DB_NAME, null, DB_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String create0 = "CREATE TABLE " + BOARD_TABLE_0 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
               KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
               KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";
        String create1 = "CREATE TABLE " + BOARD_TABLE_1 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
                KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
                KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";
        String create2 = "CREATE TABLE " + BOARD_TABLE_2 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
                KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
                KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";
        String create3 = "CREATE TABLE " + BOARD_TABLE_3 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
                KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
                KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";
        String create4 = "CREATE TABLE " + BOARD_TABLE_4 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
                KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
                KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";
        String create5 = "CREATE TABLE " + BOARD_TABLE_5 + "("
                + KEY_ID + " INTEGER PRIMARY KEY," + KEY_TIME
                + " TEXT," + KEY_SENSOR1 + " TEXT,"+
                KEY_SENSOR2 + " TEXT,"+
                KEY_SENSOR3 + " TEXT,"+
                KEY_SENSOR4 + " TEXT,"+
                KEY_SENSOR5 + " TEXT,"+
                KEY_SENSOR6 + " TEXT"+
                ")";

        db.execSQL(create0);
        db.execSQL(create1);
        db.execSQL(create2);
        db.execSQL(create3);
        db.execSQL(create4);
        db.execSQL(create5);
        Log.d("shiv", "Successfully created database");
    }



    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
    public void addSensorData(Board board, String table){
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(KEY_TIME, board.getTime());
        values.put(KEY_SENSOR1, board.getSensor1());
        values.put(KEY_SENSOR2, board.getSensor2());
        values.put(KEY_SENSOR3, board.getSensor3());
        values.put(KEY_SENSOR4, board.getSensor3());
        values.put(KEY_SENSOR5, board.getSensor3());
        values.put(KEY_SENSOR6, board.getSensor3());


        db.insert(table, null, values);
        Log.d("shiv", "Successfully inserted");
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


    public void deleteDb(Context context){
        context.deleteDatabase(DB_NAME);
    }

    public int getCount(String table){
        String query = "SELECT  * FROM " + table;
        SQLiteDatabase db = this.getReadableDatabase();
        Cursor cursor = db.rawQuery(query, null);
        return cursor.getCount();

    }
}