package org.jlantxa.notify.server;

import android.util.Log;

public class Message {
    // Server reserved message types
    public static final short TYPE_LOGIN  = 0x00;
    public static final short TYPE_LOGOUT = 0x01;
    public static final short TYPE_OK     = 0x02;
    public static final short TYPE_ERROR  = 0x03;

    // Application message types
    public static final short TYPE_REQUEST_TASKS  = 0x10;
    public static final short TYPE_RESPONSE_TASKS = 0x11;

    public static final int HEADER_SIZE = 5;
    private static final String TAG = "Message";

    private short mType;
    private byte mChecksum;
    private short mSize;
    private String mPayload;

    public Message(short type, String payload) {
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

        mType = (short) (stream[0] | (stream[1] << 8));
        mChecksum = stream[2];
        mSize = (short) (stream[3] | (stream[4] << 8));

        for (int i = 0; i < size; i++) {
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

    public short getType() {
        return mType;
    }

    public byte getChecksum() {
        return mChecksum;
    }

    public short getSize() {
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