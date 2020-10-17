package org.jlantxa.notify.server;

import android.util.Log;

public class Message {
    // Server reserved message types
    public static final int TYPE_LOGIN  = 0x00;
    public static final int TYPE_LOGOUT = 0x01;
    public static final int TYPE_OK     = 0x02;
    public static final int TYPE_ERROR  = 0x03;

    // Application message types
    public static final int TYPE_REQUEST_TASKS  = 0x10;
    public static final short TYPE_RESPONSE_TASKS = 0x11;

    public static final int HEADER_SIZE = 5;
    private static final String TAG = "Message";

    private int mType = 0;
    private byte mChecksum = 0;
    private int mSize = 0;
    private String mPayload = "";

    public Message(int type, String payload) {
        mType =  type;
        mPayload = payload;

        if (mPayload == null) {
            mSize = 0;
        } else {
            mSize = (short) (payload.length() + 1);
        }

        mChecksum = calculateChecksum();
    }

    public Message(byte[] stream, int size) throws Exception {
        if (stream.length < HEADER_SIZE || size < HEADER_SIZE || stream.length < size) {
            Log.d(TAG, stream.length + ", " + size + ", " + HEADER_SIZE);
            throw new Exception("Corrupt message.");
        }

        mType = (stream[0] & 0xFF) | ((stream[1] << 8) & 0xFF);
        mChecksum = (byte) (stream[2] & 0xFF);
        mSize = (stream[3] & 0xFF) | ((stream[4] << 8) & 0xFF);

        Log.d(TAG, "Message from buffer: " +
                "type=" + mType + ", checksum=" + mChecksum + ", size=" + mSize);

        if (mSize > 0) {
            mPayload = new String(stream, 5, mSize);
        }
    }

    private byte calculateChecksum() {
        byte sum = (byte) (mType + mSize);

        if (mPayload != null) {
            for (char c : mPayload.toCharArray()) {
                sum += c;
            }
        }

        return (byte) (0xFF - sum);
    }

    public int getType() {
        return mType;
    }

    public byte getChecksum() {
        return mChecksum;
    }

    public int getSize() {
        return mSize;
    }

    public String getPayload() {
        return mPayload;
    }

    public byte[] serialize() {
        byte[] serial = new byte[mSize + 5];

        // Type
        serial[0] = (byte) (mType & 0x00FF);
        serial[1] = (byte) ((mType >> 8) & 0xFF00);

        // Checksum
        serial[2] = mChecksum;

        // Size
        serial[3] = (byte) (mSize & 0x00FF);
        serial[4] = (byte) ((mSize >> 8) & 0xFF00);

        if (mPayload == null) {
            return serial;
        }

        // Payload
        for (int i = 0; i < mPayload.length(); i++) {
            serial[i+5] = (byte) mPayload.charAt(i);
        }
        serial[serial.length - 1] = '\0';

        return serial;
    }
}