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
    public static AtomicBoolean isAdLoaded = new AtomicBoolean(false);
    public static RewardedAd rewardedAd;
    public static SDLActivity activity;

    public static void init(SDLActivity activ)  {
        Log.v("AdsManager", "init(SDLActivity activity)");
        AdsManager.activity = activ;
        MobileAds.initialize(AdsManager.activity, initializationStatus -> {}); // calls onInitializationComplete().
        Log.v("AdsManager", "loadAd()");
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
        Log.v("AdsManager", "loadAd()");
        AdsManager.isAdLoaded.set(false);
        AdsManager.rewardedAd = null;

        AdRequest adRequest = new AdRequest.Builder().build();
        RewardedAd.load(AdsManager.activity, "ca-app-pub-3940256099942544/5224354917", adRequest, new RewardedAdLoadCallback() {
            @Override
            public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                Log.v("AdsManager", "Load ads error: " + loadAdError.toString());
                AdsManager.isAdLoaded.set(false);
                AdsManager.rewardedAd = null;
            }

            @Override
            public void onAdLoaded(@NonNull RewardedAd ad) {
                Log.v("AdsManager","Ad was loaded.");
                AdsManager.isAdLoaded.set(true);
                AdsManager.rewardedAd = ad;

                AdsManager.rewardedAd.setFullScreenContentCallback(new FullScreenContentCallback() {
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
                    }

                    @Override
                    public void onAdFailedToShowFullScreenContent(AdError adError) {
                        // Called when ad fails to show.
                        Log.v("AdsManager", "Ad failed to show fullscreen content.");
                        AdsManager.rewardedAd = null;
                        AdsManager.isAdLoaded.set(false);
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

    private static void showAd() {
        AdsManager.activity.runOnUiThread(new Runnable() {
            @Override public void run() {
                Log.v("AdsManager", "showAd()");
                if (AdsManager.rewardedAd != null) {
                    AdsManager.rewardedAd.show(AdsManager.activity, new OnUserEarnedRewardListener() {
                        @Override
                        public void onUserEarnedReward(@NonNull RewardItem rewardItem) {
                            Log.v("AdsManager", "The user earned the reward.");
                            AdsManager.rewardedAd = null;
                            AdsManager.isAdLoaded.set(false);
                            loadAd(false);
                            rewardedAdSuccessCallback();
                        }
                    });
                }
            }
        });
    }

    // Called from C++ code.
    public static void showRewardedAd() {
        Log.v("AdsManager", "showRewardedAd()");
        if(AdsManager.isAdLoaded.get()) {
            showAd();
        } else {
            loadAd(true);
        }
    }

    public static native void rewardedAdSuccessCallback();
    public static native void rewardedAdErrorCallback();
}
