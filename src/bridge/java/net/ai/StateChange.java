// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

public final class StateChange {


    /*package*/ final int mIndex;

    /*package*/ final FileState mState;

    public StateChange(
            int index,
            FileState state) {
        this.mIndex = index;
        this.mState = state;
    }

    public int getIndex() {
        return mIndex;
    }

    public FileState getState() {
        return mState;
    }

    @Override
    public String toString() {
        return "StateChange{" +
                "mIndex=" + mIndex +
                "," + "mState=" + mState +
        "}";
    }

}
