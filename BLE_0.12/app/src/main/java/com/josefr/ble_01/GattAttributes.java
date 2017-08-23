package com.josefr.ble_01;

import java.util.HashMap;

/**
 * Created by josefr on 26/03/2015.
 */


public class GattAttributes {
    private static HashMap<String, String> attributes= new HashMap();
    public static String HEART_RATE_MEASUREMENT = "00002a37-0000-1000-8000-00805f9b34fb";
    public static String CLIENT_CHARACTERISTIC_CONFIG = "00002902-0000-1000-8000-00805f9b34fb";
    public static String CHAR_USER_DESC = "00002901-0000-1000-8000-00805f9b34fb";

    public static String SK_SERVICE = "0000ffe0-0000-1000-8000-00805f9b34fb";
    public static String KEYPRESS_CHARACTERISTIC = "0000ffe1-0000-1000-8000-00805f9b34fb";

    public static String ACCELEROMETER_SERVICE="0000ffa0-0000-1000-8000-00805f9b34fb";
        public static String ACCELEROMETER_ENABLE_CHARACTERISTIC="0000ffa1-0000-1000-8000-00805f9b34fb";
        public static String ACCEL_X_CHARACTERISTIC="0000ffa3-0000-1000-8000-00805f9b34fb";
        public static String ACCEL_Y_CHARACTERISTIC="0000ffa4-0000-1000-8000-00805f9b34fb";
        public static String ACCEL_Z_CHARACTERISTIC="0000ffa5-0000-1000-8000-00805f9b34fb";

    public static String BATTERY_SERVICE = "0000ffb0-0000-1000-8000-00805f9b34fb";
    public static String BATTERY_LEVEL_CHARACTERISTIC = "0000ffb1-0000-1000-8000-00805f9b34fb";

    static {
        //Services
        attributes.put("0000180d-0000-1000-8000-00805f9b34fb", "Heart Rate Service");
        attributes.put("0000180a-0000-1000-8000-00805f9b34fb", "Device Information Service");
        // Characteristics.
        attributes.put(HEART_RATE_MEASUREMENT, "Heart Rate Measurement");
        attributes.put("00002a29-0000-1000-8000-00805f9b34fb", "Manufacturer Name String");
    }

    public static String lookup(String uuid, String defaultName){
        String name = attributes.get(uuid);
        return name == null ? defaultName : name;
    }
}
