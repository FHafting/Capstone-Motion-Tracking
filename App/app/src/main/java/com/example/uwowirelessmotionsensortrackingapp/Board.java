package com.example.uwowirelessmotionsensortrackingapp;

public class Board {

    private int id;
    private String time;
    private String sensor1;
    private String sensor2;
    private String sensor3;
    private String sensor4;
    private String sensor5;
    private String sensor6;

    public Board(int id, String time, String sensor1, String sensor2, String sensor3, String sensor4, String sensor5, String sensor6) {
        this.id = id;
        this.time = time;
        this.sensor1 = sensor1;
        this.sensor2 = sensor2;
        this.sensor3 = sensor3;
        this.sensor4 = sensor4;
        this.sensor5 = sensor5;
        this.sensor6 = sensor6;
    }
    public Board(String time, String sensor1, String sensor2, String sensor3, String sensor4, String sensor5, String sensor6) {
        this.time = time;
        this.sensor1 = sensor1;
        this.sensor2 = sensor2;
        this.sensor3 = sensor3;
        this.sensor4 = sensor4;
        this.sensor5 = sensor5;
        this.sensor6 = sensor6;
    }
    public Board() {

    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getTime() {
        return time;
    }

    public void setTime(String time) {
        this.time = time;
    }

    public String getSensor1() {
        return sensor1;
    }

    public void setSensor1(String sensor1) {
        this.sensor1 = sensor1;
    }

    public String getSensor2() {
        return sensor2;
    }

    public void setSensor2(String sensor2) {
        this.sensor2 = sensor2;
    }

    public String getSensor3() {
        return sensor3;
    }

    public void setSensor3(String sensor3) {
        this.sensor3 = sensor3;
    }

    public String getSensor4() {
        return sensor4;
    }

    public void setSensor4(String sensor4) {
        this.sensor4 = sensor4;
    }

    public String getSensor5() {
        return sensor5;
    }

    public void setSensor5(String sensor5) {
        this.sensor5 = sensor5;
    }

    public String getSensor6() {
        return sensor6;
    }

    public void setSensor6(String sensor6) {
        this.sensor6 = sensor6;
    }
}
