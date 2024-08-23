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
import com.google.android.gms.ads.interstitial.InterstitialAd;
import com.google.android.gms.ads.interstitial.InterstitialAdLoadCallback;

import org.libsdl.app.SDLActivity;

public class AdsManager {
    // True = success call back called when window with add is closing.
    // False = success call back called when reward earned(last second of ad).
    private static AtomicBoolean m_rewardedCallbackAtCloseWindow = new AtomicBoolean(false);
    private static RewardedAd m_rewardedAd;
    private static InterstitialAd m_interstitialAd;
    private static SDLActivity m_activity;

    public static void init(SDLActivity activ)  {
        Log.v("AdsManager", "init(SDLActivity activity)");
        AdsManager.m_activity = activ;
        MobileAds.initialize(AdsManager.m_activity, initializationStatus -> {}); // calls onInitializationComplete().
        loadRewarded(false);
        loadInter(false);
    }

    private static void loadRewarded(boolean showAfterLoad) {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "loadRewarded()");
                AdsManager.m_rewardedAd = null;
                AdRequest adRequest = new AdRequest.Builder().build(); // Test rewarded ad id: ca-app-pub-3940256099942544/5224354917
                RewardedAd.load(AdsManager.m_activity, "ca-app-pub-3940256099942544/5224354917", adRequest, new RewardedAdLoadCallback() {
                    @Override
                    public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                        Log.v("AdsManager", "loadRewarded() error: " + loadAdError.toString());
                        loadRewarded(showAfterLoad);
                    }

                    @Override
                    public void onAdLoaded(@NonNull RewardedAd rewardedAd) {
                        AdsManager.m_activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Log.v("AdsManager","Rewarded ad was loaded.");
                                AdsManager.m_rewardedAd = rewardedAd;

                                AdsManager.m_rewardedAd.setFullScreenContentCallback(new FullScreenContentCallback() {
                                    @Override
                                    public void onAdClicked() { Log.v("AdsManager", "Rewarded ad was clicked."); }

                                    @Override
                                    public void onAdDismissedFullScreenContent() {
                                        Log.v("AdsManager", "Rewarded ad dismissed fullscreen content.");
                                        if(AdsManager.m_rewardedCallbackAtCloseWindow.get()) {
                                            rewardedAdSuccessCallback();
                                        }
                                    }

                                    @Override
                                    public void onAdFailedToShowFullScreenContent(AdError adError) {
                                        Log.v("AdsManager", "Rewarded ad failed to show fullscreen content.");
                                        loadRewarded(true);
                                        rewardedAdErrorCallback();
                                    }

                                    @Override
                                    public void onAdImpression() { Log.v("AdsManager", "Rewarded ad recorded an impression."); }

                                    @Override
                                    public void onAdShowedFullScreenContent() { Log.v("AdsManager", "Rewarded ad showed fullscreen content."); }
                                });

                                if(showAfterLoad) {
                                    showRewarded();
                                }
                            }
                        });
                    }
                });
            }
        });
    }

    private static void loadInter(boolean showAfterLoad) {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "loadInter()");
                AdsManager.m_interstitialAd = null;
                AdRequest adRequest = new AdRequest.Builder().build(); // Test interstitial ad id: ca-app-pub-3940256099942544/1033173712
                InterstitialAd.load(AdsManager.m_activity,"ca-app-pub-3940256099942544/1033173712", adRequest, new InterstitialAdLoadCallback() {
                    @Override
                    public void onAdFailedToLoad(@NonNull LoadAdError loadAdError) {
                        Log.v("AdsManager", "loadInter() error: " + loadAdError.toString());
                        AdsManager.m_interstitialAd = null;
                        loadInter(showAfterLoad);
                    }
                    @Override
                    public void onAdLoaded(@NonNull InterstitialAd interstitialAd) {
                        AdsManager.m_activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Log.v("AdsManager","Interstitial ad was loaded.");
                                AdsManager.m_interstitialAd = interstitialAd;

                                AdsManager.m_interstitialAd.setFullScreenContentCallback(new FullScreenContentCallback(){
                                    @Override
                                    public void onAdClicked() { Log.v("AdsManager", "Interstitial ad was clicked."); }

                                    @Override
                                    public void onAdDismissedFullScreenContent() {
                                        Log.v("AdsManager", "Interstitial ad dismissed fullscreen content.");
                                        loadInter(false);
                                        interstitialAdSuccessCallback();
                                    }

                                    @Override
                                    public void onAdFailedToShowFullScreenContent(AdError adError) {
                                        Log.v("AdsManager", "Interstitial ad failed to show fullscreen content.");
                                        loadInter(true);
                                        interstitialAdErrorCallback();
                                    }

                                    @Override
                                    public void onAdImpression() { Log.v("AdsManager", "Interstitial ad recorded an impression."); }

                                    @Override
                                    public void onAdShowedFullScreenContent() { Log.v("AdsManager", "Interstitial ad showed fullscreen content."); }
                                });

                                if(showAfterLoad) {
                                    showInter();
                                }
                            }
                        });
                    }
                });
            }
        });
    }

    private static void showRewarded() {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "showRewarded()");
                AdsManager.m_rewardedAd.show(AdsManager.m_activity, new OnUserEarnedRewardListener() {
                    @Override
                    public void onUserEarnedReward(@NonNull RewardItem rewardItem) {
                        Log.v("AdsManager", "The user earned the reward.");
                        loadRewarded(false);
                        if(!AdsManager.m_rewardedCallbackAtCloseWindow.get()) {
                            rewardedAdSuccessCallback();
                        }
                    }
                });
            }
        });
    }

    private static void showInter() {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "showInter()");
                AdsManager.m_interstitialAd.show(AdsManager.m_activity);
            }
        });
    }

    // Called from C++ code.
    public static void showRewardedAd(boolean callbackAtCloseWindow) {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "showRewardedAd()");
                AdsManager.m_rewardedCallbackAtCloseWindow.set(callbackAtCloseWindow);
                if(AdsManager.m_rewardedAd != null)
                    showRewarded();
                else
                    loadRewarded(true);
            }
        });
    }

    public static void showInterstitialAd() {
        AdsManager.m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.v("AdsManager", "showInterstitialAd()");
                if(AdsManager.m_interstitialAd != null)
                    showInter();
                else
                    loadInter(true);
            }
        });
    }

//        AdsManager.m_activity.runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//
//            }
//        });

    public static native void rewardedAdSuccessCallback();
    public static native void rewardedAdErrorCallback();

    public static native void interstitialAdSuccessCallback();
    public static native void interstitialAdErrorCallback();
}
