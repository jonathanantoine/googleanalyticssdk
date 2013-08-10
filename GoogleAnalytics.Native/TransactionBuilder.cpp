//
// TransactionBuilder.cpp
// Implementation of the TransactionBuilder class.
//

#include "pch.h"
#include "TransactionBuilder.h"
#include "ProductReceipt.h"
#include "AppReceipt.h"
#include <clocale>

using namespace GoogleAnalytics;
using namespace Platform;
using namespace Windows::ApplicationModel::Store;

String^ TransactionBuilder::storeName = "Windows 8 App Store";

Transaction^ TransactionBuilder::GetProductPurchaseTransaction(ListingInformation^ listingInformation, String^ receipt)
{
	auto productReceipt = ProductReceipt::Load(receipt);
	auto transactionId = productReceipt->Id;
	auto productId = productReceipt->ProductId;
	auto productType = productReceipt->ProductType;

	auto product = listingInformation->ProductListings->Lookup(productId);

	lconv* lc = localeconv();
	std::string isoCurrencySymbol(lc->int_curr_symbol);
	std::wstring isoCurrencySymbolW(isoCurrencySymbol.begin(), isoCurrencySymbol.end());
	auto currencyCode = ref new String(isoCurrencySymbolW.data());

	auto currencyFormatter = ref new Windows::Globalization::NumberFormatting::CurrencyFormatter(currencyCode);
	auto cost = currencyFormatter->ParseDouble(product->FormattedPrice);
	auto costInMicrons = cost != nullptr ? (long long)(cost->Value * 1000000) : 0L;

	auto transaction = ref new Transaction(transactionId, costInMicrons);
	transaction->Affiliation = StoreName;
	transaction->CurrencyCode = currencyCode;
	auto transactionItem = ref new TransactionItem(productId, product->Name, costInMicrons, 1);
	transactionItem->Category = productType;
	transaction->Items->Append(transactionItem);
	return transaction;
}

Transaction^ TransactionBuilder::GetAppPurchaseTransaction(ListingInformation^ listingInformation, String^ receipt)
{
	auto appReceipt = AppReceipt::Load(receipt);
	auto transactionId = appReceipt->Id;
	auto appId = appReceipt->AppId;
	auto licenseType = appReceipt->LicenseType;
	
	lconv* lc = localeconv();
	std::string isoCurrencySymbol(lc->int_curr_symbol);
	std::wstring isoCurrencySymbolW(isoCurrencySymbol.begin(), isoCurrencySymbol.end());
	auto currencyCode = ref new String(isoCurrencySymbolW.data());

	auto currencyFormatter = ref new Windows::Globalization::NumberFormatting::CurrencyFormatter(currencyCode);
	auto cost = currencyFormatter->ParseDouble(listingInformation->FormattedPrice);
	auto costInMicrons = cost != nullptr ? (long long)(cost->Value * 1000000) : 0L;

	auto transaction = ref new Transaction(transactionId, costInMicrons);
	transaction->Affiliation = StoreName;
	transaction->CurrencyCode = currencyCode;
	auto transactionItem = ref new TransactionItem(appId, listingInformation->Name, costInMicrons, 1);
	transactionItem->Category = licenseType;
	transaction->Items->Append(transactionItem);
	return transaction;
}


