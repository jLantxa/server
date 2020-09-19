package org.jlantxa.notify.server;

public class Message {
    public static final short TYPE_LOGIN  = 0;
    public static final short TYPE_LOGOUT = 1;

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