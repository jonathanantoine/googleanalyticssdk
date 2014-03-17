//
// TransactionItem.h
// Declaration of the TransactionItem class.
//

#pragma once

namespace GoogleAnalytics
{

	public ref class TransactionItem sealed
	{
	public:

		TransactionItem()
		{
			Name = nullptr;
			PriceInMicros = 0L;
			Quantity = 0L;
			SKU = nullptr;
			Category = nullptr;
		}

		TransactionItem(Platform::String^ sku, Platform::String^ name, long long priceInMicros, long long quantity)
		{
			Name = name;
			PriceInMicros = priceInMicros;
			Quantity = quantity;
			SKU = sku;
			Category = nullptr;
		}

		property Platform::String^ Name;

		property long long PriceInMicros;

		property long long Quantity;

		property Platform::String^ SKU;

		property Platform::String^ Category;

	};
}
