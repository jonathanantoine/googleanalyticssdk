//
// TransactionBuilder.h
// Declaration of the TransactionBuilder class.
//

#pragma once

#include "Transaction.h"

namespace GoogleAnalytics
{

	public ref class TransactionBuilder sealed
	{
	private:
		static Platform::String^ storeName;

	public:
		/// <summary>
		/// Gets or sets the default strore name to be used when logging transactions
		/// </summary>
		static property Platform::String^ StoreName
		{
			Platform::String^ get()
			{
				return storeName;
			}
			void set(Platform::String^ value)
			{
				storeName = value;
			}
		}

		/// <summary>
		/// Constructs a transaction object with a transaction item for a product purchase
		/// </summary>
		/// <param name="listingInformation">The product listing information for the app</param>
		/// <param name="receipt">The receipt from the purchase</param>
		/// <returns>A transaction object all ready to get passed to Tracker.sendTransaction</returns>
		static GoogleAnalytics::Transaction^ GetProductPurchaseTransaction(Windows::ApplicationModel::Store::ListingInformation^ listingInformation, Platform::String^ receipt);

		/// <summary>
		/// Constructs a transaction object with a transaction item for an app purchase
		/// </summary>
		/// <param name="listingInformation">The product listing information for the app</param>
		/// <param name="receipt">The receipt from the purchase</param>
		/// <returns>A transaction object all ready to get passed to Tracker.sendTransaction</returns>
		static GoogleAnalytics::Transaction^ GetAppPurchaseTransaction(Windows::ApplicationModel::Store::ListingInformation^ listingInformation, Platform::String^ receipt);

	};
}
