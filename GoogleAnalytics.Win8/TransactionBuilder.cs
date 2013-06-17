using Windows.ApplicationModel.Store;

namespace GoogleAnalytics
{
    public static class TransactionBuilder
    {
        static TransactionBuilder()
        {
            StoreName = "Windows 8 App Store";
        }

        /// <summary>
        /// Gets or sets the default strore name to be used when logging transactions
        /// </summary>
        public static string StoreName { get; set; }

        /// <summary>
        /// Constructs a transaction object with a transaction item for a product purchase
        /// </summary>
        /// <param name="listingInformation">The product listing information for the app</param>
        /// <param name="receipt">The receipt from the purchase</param>
        /// <returns>A transaction object all ready to get passed to Tracker.sendTransaction</returns>
        public static Transaction GetProductPurchaseTransaction(ListingInformation listingInformation, string receipt)
        {
            var productReceipt = ProductReceipt.Load(receipt);
            var transactionId = productReceipt.Id;
            var productId = productReceipt.ProductId;
            var productType = productReceipt.ProductType;

            var product = listingInformation.ProductListings[productId];
            var regionInfo = new System.Globalization.RegionInfo(listingInformation.CurrentMarket);
            var currencyCode = regionInfo.ISOCurrencySymbol;
            var currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(currencyCode);
            var cost = currencyFormatter.ParseDouble(product.FormattedPrice);
            var costInMicrons = (long)(cost.GetValueOrDefault(0) * 1000000);

            var transaction = new Transaction(transactionId, costInMicrons);
            transaction.Affiliation = StoreName;
            transaction.CurrencyCode = currencyCode;
            var transactionItem = new TransactionItem(productId, product.Name, costInMicrons, 1);
            transactionItem.Category = productType;
            transaction.Items.Add(transactionItem);
            return transaction;
        }

        /// <summary>
        /// Constructs a transaction object with a transaction item for an app purchase
        /// </summary>
        /// <param name="listingInformation">The product listing information for the app</param>
        /// <param name="receipt">The receipt from the purchase</param>
        /// <returns>A transaction object all ready to get passed to Tracker.sendTransaction</returns>
        public static Transaction GetAppPurchaseTransaction(ListingInformation listingInformation, string receipt)
        {
            var appReceipt = AppReceipt.Load(receipt);
            var transactionId = appReceipt.Id;
            var appId = appReceipt.AppId;
            var licenseType = appReceipt.LicenseType;

            var regionInfo = new System.Globalization.RegionInfo(listingInformation.CurrentMarket);
            var currencyCode = regionInfo.ISOCurrencySymbol;
            var currencyFormatter = new Windows.Globalization.NumberFormatting.CurrencyFormatter(currencyCode);
            var cost = currencyFormatter.ParseDouble(listingInformation.FormattedPrice);
            var costInMicrons = (long)(cost.GetValueOrDefault(0) * 1000000);

            var transaction = new Transaction(transactionId, costInMicrons);
            transaction.Affiliation = StoreName;
            transaction.CurrencyCode = currencyCode;
            var transactionItem = new TransactionItem(appId, listingInformation.Name, costInMicrons, 1);
            transactionItem.Category = licenseType;
            transaction.Items.Add(transactionItem);
            return transaction;
        }
    }
}
