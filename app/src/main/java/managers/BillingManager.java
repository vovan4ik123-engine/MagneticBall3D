package managers;

public class BillingManager {

    // Called from C++ code to purchase.
    public static void makeInAppPurchase(String productID) {

    }

    public static native void billingSystemSuccessCallback();
    public static native void billingSystemErrorCallback();
}
