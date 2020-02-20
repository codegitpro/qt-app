package net.ai.tmp.cloudstore;

import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;

public interface Gui
{
    void showCloudstore();
    void createForm(boolean clearBackstack);
    void preview();
    void runOnUiThread(Runnable runnable);
    Resources resources();
    void showMessage(String message);
    Context context();
    void startActivity(Intent intent);
}
