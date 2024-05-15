package managers;

import android.os.Bundle;
import android.util.Log;

import com.google.firebase.analytics.FirebaseAnalytics;
import org.libsdl.app.SDLActivity;

public class GoogleAnalyticsManager {

    private static FirebaseAnalytics firebaseAnalytics;

    public static void init(SDLActivity activity) {
        firebaseAnalytics = FirebaseAnalytics.getInstance(activity);
    }

    // Called from C++ code to send event.
    public static void sendEventEmpty(String eventType) {
        Log.v("GoogleAnalyticsManager", "sendEventEmpty(): " + eventType);

        Bundle bundle = new Bundle();

        firebaseAnalytics.logEvent(eventType, bundle);
    }

    public static void sendEventStringParam(String eventType, String paramName, String paramValue) {
        // Example: sendEventStringParam("levelStart", "levelName", "first");
        Log.v("GoogleAnalyticsManager", "sendEventStringParam(): " + eventType + " " + paramName + " " + paramValue);

        Bundle bundle = new Bundle();
        bundle.putString(paramName, paramValue);

        firebaseAnalytics.logEvent(eventType, bundle);
    }

    public static void sendEventFloatParam(String eventType, String paramName, float paramValue) {
        // Example: sendEventFloatParam("playerDie", "second", 145);
        Log.v("GoogleAnalyticsManager", "sendEventFloatParam(): " + eventType + " " + paramName + " " + paramValue);

        Bundle bundle = new Bundle();
        bundle.putFloat(paramName, paramValue);

        firebaseAnalytics.logEvent(eventType, bundle);
    }
}
