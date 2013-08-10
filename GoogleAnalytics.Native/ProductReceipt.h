//
// ProductReceipt.h
// Declaration of the ProductReceipt class.
//

#pragma once

namespace GoogleAnalytics
{

	public ref class ProductReceipt sealed
	{
	public:

		ProductReceipt()
		{
			Id = nullptr;
			ProductId = nullptr;
			ProductType = nullptr;
		}

		property Platform::String^ Id;

		property Platform::String^ ProductId;

		property Platform::String^ ProductType;

		static GoogleAnalytics::ProductReceipt^ Load(Platform::String^ receipt);

	};
}
