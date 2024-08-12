package managers;

import android.util.Log;

import androidx.annotation.NonNull;

import java.util.concurrent.atomic.AtomicBoolean;

import com.google.android.gms.ads.AdError;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.FullScreenContentCallback;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.OnUserEarnedRewardListener;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;

import org.libsdl.app.SDLActivity;

public class AdsManager { // implements OnInitializationCompleteListener

    //public static AtomicBoolean isAdInitialized = new AtomicBoolean(false);
    private static AtomicBoolean m_isAdLoaded = new AtomicBoolean(false);
    // True = success call back called when window with add is closing.
    // False = success call back called when reward earned(last second of ad).
    private static AtomicBoolean m_callbackAtCloseWindow = new AtomicBoolean(false);
    private static RewardedAd m_rewardedAd;
    private static SDLActivity m_activity;

    public static void init(SDLActivity activ)  {
        Log.v("AdsManager", "init(SDLActivity activity)");
        AdsManager.m_activity = activ;
        MobileAds.initialize(AdsManager.m_activity, initializationStatus -> {}); // calls onInitializationComplete().
        loadAd(false);
    }

//    @Override
//    public void onInitializationComplete(@NonNull InitializationStatus initializationStatus) {
//        Log.v("AdsManager", "onInitializationComplete()");
//        Map<String, AdapterStatus> networks = initializationStatus.getAdapterStatusMap();
//
//        for (Map.Entry<String, AdapterStatus> pair : networks.entrySet()) {
//            Log.v("AdsManager", "network name: " + pair.getKey() + " status: " + pair.getValue().getInitializationState().toString());
//            // If any network is ready set isAdsInitialized = true.
//            if(pair.getValue().getInitializationState() == AdapterStatus.State.READY) {
//                AdsManager.isAdInitialized.set(true);
//            }
//        }
//
//        if(AdsManager.isAdInitialized.get()) {
//            // Preload ad if initialization successful.
//            loadAd(false);
//        }
//    }

    private static void loadAd(boolean showAfterLoad) {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                Log.v("AdsManager", "loadAd()");
                AdsManager.m_isAdLoaded.set(false);
                AdsManager.m_rewardedAd = null;

                AdRequest adRequest = new AdRequest.Builder().build(); // Test ad id: ca-app-pub-3940256099942544/5224354917
                RewardedAd.load(AdsManager.m_activity, "ca-app-pub-3940256099942544/5224354917", adRequest, new RewardedAdLoadCallback() {
                    @Override
                    public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                        Log.v("AdsManager", "Load ads error: " + loadAdError.toString());
                        AdsManager.m_isAdLoaded.set(false);
                        AdsManager.m_rewardedAd = null;
                        loadAd(showAfterLoad);
                    }

                    @Override
                    public void onAdLoaded(@NonNull RewardedAd ad) {
                        AdsManager.m_activity.runOnUiThread(new Runnable() {
                            @Override public void run() {
                                Log.v("AdsManager","Ad was loaded.");
                                AdsManager.m_isAdLoaded.set(true);
                                AdsManager.m_rewardedAd = ad;

                                AdsManager.m_rewardedAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                                    @Override
                                    public void onAdClicked() {
                                        // Called when a click is recorded for an ad.
                                        Log.v("AdsManager", "Ad was clicked.");
                                    }

                                    @Override
                                    public void onAdDismissedFullScreenContent() {
                                        // Called when ad is dismissed.
                                        // Set the ad reference to null so you don't show the ad a second time.
                                        Log.v("AdsManager", "Ad dismissed fullscreen content.");
                                        if(AdsManager.m_callbackAtCloseWindow.get()) {
                                            rewardedAdSuccessCallback();
                                        }
                                    }

                                    @Override
                                    public void onAdFailedToShowFullScreenContent(AdError adError) {
                                        // Called when ad fails to show.
                                        Log.v("AdsManager", "Ad failed to show fullscreen content.");
                                        AdsManager.m_rewardedAd = null;
                                        AdsManager.m_isAdLoaded.set(false);
                                        rewardedAdErrorCallback();
                                    }

                                    @Override
                                    public void onAdImpression() {
                                        // Called when an impression is recorded for an ad.
                                        Log.v("AdsManager", "Ad recorded an impression.");
                                    }

                                    @Override
                                    public void onAdShowedFullScreenContent() {
                                        // Called when ad is shown.
                                        Log.v("AdsManager", "Ad showed fullscreen content.");
                                    }
                                });

                                if(showAfterLoad) {
                                    showAd();
                                }
                            }
                        });
                    }
                });
            }
        });
    }

    private static void showAd() {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                Log.v("AdsManager", "showAd()");
                if (AdsManager.m_rewardedAd != null) {
                    AdsManager.m_rewardedAd.show(AdsManager.m_activity, new OnUserEarnedRewardListener() {
                        @Override
                        public void onUserEarnedReward(@NonNull RewardItem rewardItem) {
                            Log.v("AdsManager", "The user earned the reward.");
                            AdsManager.m_rewardedAd = null;
                            AdsManager.m_isAdLoaded.set(false);
                            loadAd(false);
                            if(!AdsManager.m_callbackAtCloseWindow.get()) {
                                rewardedAdSuccessCallback();
                            }
                        }
                    });
                }
            }
        });
    }

    // Called from C++ code.
    public static void showRewardedAd(boolean callbackAtCloseWindow) {
        Log.v("AdsManager", "showRewardedAd()");
        AdsManager.m_callbackAtCloseWindow.set(callbackAtCloseWindow);
        if(AdsManager.m_isAdLoaded.get()) {
            showAd();
        } else {
            loadAd(true);
        }


//        AdsManager.m_activity.runOnUiThread(new Runnable() {
//            @Override public void run() {
//
//            }
//        });
    }

    public static native void rewardedAdSuccessCallback();
    public static native void rewardedAdErrorCallback();
}
