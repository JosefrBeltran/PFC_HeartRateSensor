package com.josefr.ble_01;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Created by josefr on 01/04/2015.
 */
public class NotificationActivity extends Activity {
    public static final String TAG=NotificationActivity.class.getSimpleName();
    public static final String EXTRAS_DEVICE_NAME="DEVICE_NAME";
    public static final String EXTRAS_DEVICE_ADDRESS="DEVICE_ADDRESS";

    private TextView mConnectionState;
    private TextView mNotificationState;
    private String mDeviceName;
    private String mDeviceAddress;
    private BluetoothLeService mBluetoothLeService;
    private List<BluetoothGattCharacteristic> mNotifyCharacteristicList;
    private BluetoothGattDescriptor mDescriptor;
    private LinearLayout mLayoutBackground;
    private LinearLayout mLayoutNotifications;
    private boolean mConnected=false;


    //Code to manage Service lifecycle
    private final ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService=((BluetoothLeService.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()){
                Log.e(TAG, "Unable to initialize Bluetooth.");
                finish();
            }
            //Automatically connects to the device upon successful start-up initialization
            mBluetoothLeService.connect(mDeviceAddress);

        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService=null;
        }
    };


    // Handles various events fired by the Service.
    // ACTION_GATT_CONNECTED: connected to a GATT server.
    // ACTION_GATT_DISCONNECTED: disconnected from a GATT server.
    // ACTION_GATT_SERVICES_DISCOVERED: discovered GATT services.
    // ACTION_DATA_AVAILABLE: received data from the device. This can be a
    // result of read or notification operations.
    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                mConnected = true;
                updateConnectionState(R.string.connected);
                invalidateOptionsMenu();
            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                mConnected = false;
                updateConnectionState(R.string.disconnected);
                invalidateOptionsMenu();
                clearUI();
            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
                // Pasa la lista de servicios a la función que detecta la caracterisitca del KeyPress del Keyfob
                //getCharacteristicKeyPress(mBluetoothLeService.getSupportedGattServices());
                //getCharacteristicsAccelerometer(mBluetoothLeService.getSupportedGattServices());
                //getCharacteristic(GattAttributes.ACCELEROMETER_SERVICE,GattAttributes.ACCEL_X_CHARACTERISTIC);
                getCharacteristic(GattAttributes.ACCELEROMETER_SERVICE,GattAttributes.ACCELEROMETER_ENABLE_CHARACTERISTIC);
                getCharacteristic(GattAttributes.ACCELEROMETER_SERVICE,GattAttributes.ACCEL_X_CHARACTERISTIC);

            } else if (BluetoothLeService.ACTION_DATA_AVAILABLE.equals(action)) {
                displayData(intent.getStringExtra(BluetoothLeService.EXTRA_DATA));

            } else if (BluetoothLeService.ACTION_DESCRIPTOR_AVAILABLE.equals(action)) {
                Log.i(TAG,"Valor del descriptor: "+ intent.getStringExtra(BluetoothLeService.EXTRA_DATA));
            }
        }
    };

    private void clearUI(){
    mNotificationState.setText(R.string.no_data);
    }

    @Override
    public void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        setContentView(R.layout.keyfob);

        final Intent intent= getIntent();
        mDeviceName=intent.getStringExtra(EXTRAS_DEVICE_NAME);
        mDeviceAddress=intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

        ((TextView) findViewById(R.id.label_device_name)).setText(mDeviceName);
        ((TextView) findViewById(R.id.device_address)).setText(mDeviceAddress);
        mConnectionState= (TextView) findViewById(R.id.connection_state);
        mNotificationState= (TextView) findViewById(R.id.notification_display);
        mLayoutBackground= (LinearLayout) findViewById(R.id.layout_Background);
        mLayoutNotifications= (LinearLayout) findViewById(R.id.layoutNotificaciones);

        mNotifyCharacteristicList = new ArrayList<>();

        getActionBar().setTitle("Control");
        getActionBar().setDisplayHomeAsUpEnabled(true);
        Intent gattServiceIntent= new Intent(this, BluetoothLeService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        final Button mBotonAceptarNotificaciones= (Button) findViewById(R.id.boton_acaptar_notificaciones);
        mBotonAceptarNotificaciones.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mNotifyCharacteristicList != null){
                    for(BluetoothGattCharacteristic mNotifyCharacteristic : mNotifyCharacteristicList){

                        if (mNotifyCharacteristic.getUuid().toString().equals(GattAttributes.ACCEL_X_CHARACTERISTIC)){
                            //Activar notificaciones mNotifyCharacteristic
                            mBluetoothLeService.setCharacteristicNotification(mNotifyCharacteristic,true);
                            mBluetoothLeService.setClientCharacteristicConfig(mNotifyCharacteristic);
                        }
                        else if (mNotifyCharacteristic.getUuid().toString().equals(GattAttributes.ACCELEROMETER_ENABLE_CHARACTERISTIC)){
                            mBluetoothLeService.setCharacteristicNotification(mNotifyCharacteristic,true);
                            mBluetoothLeService.writeCharacteristic(mNotifyCharacteristic,true);
                        }
                    }

                }

                mLayoutNotifications.setVisibility(View.VISIBLE);
            }
        });

    }

    @Override
    public void onResume(){
        super.onResume();
        registerReceiver(mGattUpdateReceiver, makeGattUpdateIntentFilter() );
        if (mBluetoothLeService!=null){
            final boolean result=mBluetoothLeService.connect(mDeviceAddress);
            Log.d(TAG, "Connect request result: "+result);
        }
    }

    @Override
    public void onPause(){
        super.onPause();
        unregisterReceiver(mGattUpdateReceiver);
    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        unbindService(mServiceConnection);
        mBluetoothLeService = null;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu){
        getMenuInflater().inflate(R.menu.gatt_services, menu);
        if(mConnected){
            menu.findItem(R.id.menu_connect).setVisible(false);
            menu.findItem(R.id.menu_disconnect).setVisible(true);
        } else {
            menu.findItem(R.id.menu_connect).setVisible(true);
            menu.findItem(R.id.menu_disconnect).setVisible(false);
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        switch (item.getItemId()){
            case R.id.menu_connect:
                mBluetoothLeService.connect(mDeviceAddress);
                return true;
            case R.id.menu_disconnect:
                mBluetoothLeService.disconnect();
                return true;
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    private void updateConnectionState(final int resourceId){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mConnectionState.setText(resourceId);
            }
        });
    }

    private void displayData (String data){
        if (data != null){
            mNotificationState.setText(data);
       // mNotificationState.setText(data.substring(2,4));
 /*           switch (data.substring(3,4)) {
                case "0":
                    mLayoutBackground.setBackgroundColor(getResources().getColor(R.color.background0));
                    break;
                case "1":
                    mLayoutBackground.setBackgroundColor(getResources().getColor(R.color.background1));
                    break;
                case "2":
                    mLayoutBackground.setBackgroundColor(getResources().getColor(R.color.background2));
                    break;
                case "3":
                    mLayoutBackground.setBackgroundColor(getResources().getColor(R.color.background3));
                    break;
            }*/
        }
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(BluetoothLeService.ACTION_DATA_AVAILABLE);
        return intentFilter;
    }


    // Obtiene una caracteristica GATT y la añade a a lista de características
    public void getCharacteristic(String gattAttributeService, String gattAttributeCharacteristic){
        List<BluetoothGattService> mGattServices=mBluetoothLeService.getSupportedGattServices();
        for(BluetoothGattService mGService : mGattServices){
            if(mGService.getUuid().toString().equals(gattAttributeService)){
                for(BluetoothGattCharacteristic characteristic : mGService.getCharacteristics()){
                    if (characteristic.getUuid().toString().equals(gattAttributeCharacteristic)){
                        mNotifyCharacteristicList.add(characteristic);
                    }
                }
            }
        }
    }


    public void getCharacteristicDescriptor(String gattAttributeService,
                                                String gattAttributeCharacteristic, String gattAttributeDescriptor){
        List<BluetoothGattService> mGattServices=mBluetoothLeService.getSupportedGattServices();
        for(BluetoothGattService mGService : mGattServices){
            if(mGService.getUuid().toString().equals(gattAttributeService)){
                for(BluetoothGattCharacteristic characteristic : mGService.getCharacteristics()){
                    if (characteristic.getUuid().toString().equals(gattAttributeCharacteristic)){
                        for(BluetoothGattDescriptor descriptor : characteristic.getDescriptors()){
                            if (descriptor.getUuid().toString().equals(gattAttributeDescriptor)){
                                mDescriptor=descriptor;
                            }
                        }
                    }
                }
            }
        }

    }
}
