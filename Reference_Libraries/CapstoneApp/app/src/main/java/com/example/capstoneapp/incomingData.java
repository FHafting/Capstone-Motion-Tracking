package com.example.capstoneapp;

public class incomingData {
    String id;
    String time;
    String[] accelx;
    String[] accely;
    String[] accelz;
    String[] gyrox;
    String[] gyroy;
    String[] gyroz;

    public incomingData(String id, String time, String[] accelx, String[] accely, String[] accelz, String[] gyrox, String[] gyroy, String[] gyroz) {
        this.id = id;
        this.time = time;
        this.accelx = accelx;
        this.accely = accely;
        this.accelz = accelz;
        this.gyrox = gyrox;
        this.gyroy = gyroy;
        this.gyroz = gyroz;
    }

    public String getId() {
        return id;
    }

    public String getTime() {
        return time;
    }

    public String[] getAccelx() {
        return accelx;
    }

    public String[] getAccely() {
        return accely;
    }

    public String[] getAccelz() {
        return accelz;
    }

    public String[] getGyrox() {
        return gyrox;
    }

    public String[] getGyroy() {
        return gyroy;
    }

    public String[] getGyroz() {
        return gyroz;
    }
}
