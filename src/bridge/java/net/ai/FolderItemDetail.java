// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

public final class FolderItemDetail {


    /*package*/ final int mItemId;

    /*package*/ final boolean mSelected;

    /*package*/ final String mTitle;

    public FolderItemDetail(
            int itemId,
            boolean selected,
            String title) {
        this.mItemId = itemId;
        this.mSelected = selected;
        this.mTitle = title;
    }

    public int getItemId() {
        return mItemId;
    }

    public boolean getSelected() {
        return mSelected;
    }

    public String getTitle() {
        return mTitle;
    }

    @Override
    public String toString() {
        return "FolderItemDetail{" +
                "mItemId=" + mItemId +
                "," + "mSelected=" + mSelected +
                "," + "mTitle=" + mTitle +
        "}";
    }

}