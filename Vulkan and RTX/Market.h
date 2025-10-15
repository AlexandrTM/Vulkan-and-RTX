#pragma once

struct Trader
{
	double wealth;
	double skill;
};

struct MarketOrder
{
	size_t traderId;
	double price;
	double quantity;
	bool isBuy;
};

namespace Market
{
	void simulateMarket(size_t tradersCount, size_t months);
	void simulateOrderBook(std::vector<Trader>& traders, size_t steps);
	void findMarketStats(std::vector<Trader>& traders, size_t months);
}

