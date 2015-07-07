// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.domokit.intents;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.net.Uri;
import android.util.Log;

import org.chromium.mojo.system.MojoException;
import org.chromium.mojom.intents.ActivityManager;
import org.chromium.mojom.intents.ComponentName;
import org.chromium.mojom.intents.Intent;
import org.chromium.mojom.intents.StringExtra;

/**
 * Android implementation of ActivityManager.
 */
public class ActivityManagerImpl implements ActivityManager {
    private static final String TAG = "ActivityManagerImpl";
    private static Activity sCurrentActivity;
    private Context mContext;

    public ActivityManagerImpl(Context context) {
        mContext = context;
    }

    public static void setCurrentActivity(Activity activity) {
        sCurrentActivity = activity;
    }

    @Override
    public void close() {}

    @Override
    public void onConnectionError(MojoException e) {}

    @Override
    public void startActivity(Intent intent) {
        if (sCurrentActivity == null) {
            Log.e(TAG, "Unable to startActivity");
            return;
        }

        final android.content.Intent androidIntent = new android.content.Intent(
                intent.action, Uri.parse(intent.url));

        if (intent.component != null) {
            ComponentName component = intent.component;
            android.content.ComponentName androidComponent =
                    new android.content.ComponentName(component.packageName, component.className);
            androidIntent.setComponent(androidComponent);
        }

        if (intent.stringExtras != null) {
            for (StringExtra extra : intent.stringExtras) {
                androidIntent.putExtra(extra.name, extra.value);
            }
        }

        if (intent.flags != 0) {
            androidIntent.setFlags(intent.flags);
        }

        try {
            sCurrentActivity.startActivity(androidIntent);
        } catch (ActivityNotFoundException e) {
            Log.e(TAG, "Unable to startActivity", e);
        }
    }

    @Override
    public void finishCurrentActivity() {
        if (sCurrentActivity != null) {
            sCurrentActivity.finish();
        } else {
            Log.e(TAG, "Unable to finishCurrentActivity");
        }
    }
}
