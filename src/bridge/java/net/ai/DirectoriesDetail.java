// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

import java.util.ArrayList;

public final class DirectoriesDetail {


    /*package*/ final ArrayList<Integer> mIds;

    /*package*/ final ArrayList<Integer> mTitles;

    public DirectoriesDetail(
            ArrayList<Integer> ids,
            ArrayList<Integer> titles) {
        this.mIds = ids;
        this.mTitles = titles;
    }

    public ArrayList<Integer> getIds() {
        return mIds;
    }

    public ArrayList<Integer> getTitles() {
        return mTitles;
    }

    @Override
    public String toString() {
        return "DirectoriesDetail{" +
                "mIds=" + mIds +
                "," + "mTitles=" + mTitles +
        "}";
    }

}
