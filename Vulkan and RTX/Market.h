#pragma once

struct MarketOrder
{
	double price;
	double quantity;
	bool isBuy;
	bool isMatched = false;
};

struct Trader
{
	double wealth;
	double skill;

	std::vector<MarketOrder> orders;
};

namespace Market
{
	void simulateMarket(size_t tradersCount, size_t months);
	void simulateOrderBook(std::vector<Trader>& traders, size_t steps);
	void findMarketStats(std::vector<Trader>& traders, size_t months);
	using Orders = std::vector<std::pair<size_t, MarketOrder>>;
	void getOrderBook(Orders bids, Orders asks);
}

