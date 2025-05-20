#pragma once

struct UiCache
{
	virtual ~UiCache() = default;
};

struct InGameOverlayCache : UiCache
{
	QObject* rootItem = nullptr;
	QObject* minimapContainer = nullptr;

	QObject* playerHealth = nullptr;
	QObject* playerDamage = nullptr;
	QObject* playerDefense = nullptr;
	QObject* playerExperience = nullptr;

	QObject* mobTitle = nullptr;
	QObject* mobHealth = nullptr;
	QObject* mobDamage = nullptr;
	QObject* mobDefense = nullptr;
	QObject* mobExperience = nullptr;
};