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
                + " TEXT," + Params.KEY_SENSOR1 + " TEXT"+ ")";

        Log.d("finn", "Query being run is : "+ create);
        db.execSQL(create);

    }



    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
    public void addSensorData(Board board){
        SQLiteDatabase db = this.getWritableDatabase();

        ContentValues values = new ContentValues();
        values.put(Params.KEY_TIME, board.getTime());
        values.put(Params.KEY_SENSOR1, board.getSensor1());



        db.insert(Params.BOARD_TABLE_0, null, values);
        Log.d("dbharry", "Successfully inserted");
        db.close();


    }

    public List<Board> getAllData(){
        List<Board> sensorDataList = new ArrayList<>();
        SQLiteDatabase db = this.getReadableDatabase();

        // Generate the query to read from the database
        String select = "SELECT * FROM " + Params.BOARD_TABLE_0;
        Cursor cursor = db.rawQuery(select, null);

        //Loop through now
        if(cursor.moveToFirst()){
            do{
                Board board = new Board();
                board.setId(Integer.parseInt(cursor.getString(0)));
                board.setTime(cursor.getString(1));
                board.setSensor1(cursor.getString(2));

                sensorDataList.add(board);
            }while(cursor.moveToNext());
        }
        return sensorDataList;
    }

}
