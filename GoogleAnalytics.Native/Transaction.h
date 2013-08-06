//
// Transaction.h
// Declaration of the Transaction class.
//

#pragma once

#include <collection.h>
#include "TransactionItem.h"

namespace GoogleAnalytics
{

	public ref class Transaction sealed
	{
	private:
		Windows::Foundation::Collections::IVector<GoogleAnalytics::TransactionItem^>^ items;

	public:

		Transaction()
		{
			items = ref new Platform::Collections::Vector<GoogleAnalytics::TransactionItem^>();
		}

		Transaction(Platform::String^ transactionId, long long totalCostInMicros)
		{
			items = ref new Platform::Collections::Vector<GoogleAnalytics::TransactionItem^>();
            TransactionId = transactionId;
            TotalCostInMicros = totalCostInMicros;
		}

		property Platform::String^ TransactionId;

		property Platform::String^ Affiliation;

		property long long TotalCostInMicros;

		property long long ShippingCostInMicros;

		property long long TotalTaxInMicros;

		property Platform::String^ CurrencyCode;

		property Windows::Foundation::Collections::IVector<GoogleAnalytics::TransactionItem^>^ Items
		{
			Windows::Foundation::Collections::IVector<GoogleAnalytics::TransactionItem^>^ get()
			{
				return items;
			}
		}


	};
}
