#pragma once

#include <iostream>
#include<string>
#include<vector>
#include<set>
#include<map>
#include <algorithm>
#include <unordered_set>
#include <functional>

using namespace std;


using Date = unsigned long long;

enum class Cuisine {
	SouthIndian, NorthIndian, Chinese
};

class Restaurant {
public:
	string restaurantId;
	Cuisine cuisine;
	int costBracket;
	float rating;
	bool isRecommended;
	Date onboardedTime;
};

class CuisineTracking {
public:
	Cuisine type;
	int noOfOrders;
};

class CostTracking {
public:
	int type;
	int noOfOrders;
};

class User {
public:
	std::vector<CuisineTracking>  cuisines;
	std::vector<CostTracking> costBracket;
};



// sort cuising tracking by orders
bool SortCuisineTrackingComparator(CuisineTracking left, CuisineTracking right) {
	return left.noOfOrders < right.noOfOrders;
}

// sort cost tracking by orders
bool SortCostTrackingComparator(CostTracking left, CostTracking right) {
	return left.noOfOrders < right.noOfOrders;
}


// overloaded to support concatenation of vectors
template <typename T>
std::vector<T> operator+(std::vector<T> const& x, std::vector<T> const& y)
{
	std::vector<T> vec;
	vec.reserve(x.size() + y.size());
	vec.insert(vec.end(), x.begin(), x.end());
	vec.insert(vec.end(), y.begin(), y.end());
	return vec;
}


// overloaded to support concatenation of vectors
template <typename T>
std::vector<T>& operator+=(std::vector<T>& x, const std::vector<T>& y)
{
	x.reserve(x.size() + y.size());
	x.insert(x.end(), y.begin(), y.end());
	return x;
}


// find intersection of 2 vectors by using set property
vector<string> FindIntersection(const std::vector<string>& v1, const std::vector<string>& v2) {
	vector<string> result;
	unordered_set<string> set;

	for (int i = 0; i < v2.size(); i++) {
		set.insert(v2[i]);
	}

	for (int i = 0; i < v1.size(); i++) {
		if (set.count(v1[i]) > 0) {
			result.push_back(v1[i]);
		}
	}

	return result;
}


bool SortComparatorRating(std::tuple<string, float, bool, Date> left, std::tuple<string, float, bool, Date> right) {
	return get<1>(left) > get<1>(right);
}

bool SortComparatorDate(std::tuple<string, float, bool, Date> left, std::tuple<string, float, bool, Date> right) {
	return get<3>(left) > get<3>(right);
}

bool SortComparatorDateAndRating(std::tuple<string, float, bool, Date> left, std::tuple<string, float, bool, Date> right) {
	if (get<3>(left) > get<3>(right)) {
		return true;
	}
	if (get<3>(left) < get<3>(right)) {
		return false;
	}

	if (get<1>(left) > get<1>(right)) {
		return true;
	}
	if (get<1>(left) < get<1>(right)) {
		return false;
	}
	return false;
}


// Run queries based on cuisine, cost brackets,featured,rating filter
vector<string> RunQuery(vector<string> cuisineRestaurantIds, vector<string> costBracketRestaurantIds,
	map<std::string, std::tuple<float, bool>>& restaurantMap, bool featured, std::function<bool(float)> ratingFilter) {
	if (restaurantMap.size() <= 0) {
		return {};
	}

	vector<string> intersection = FindIntersection(cuisineRestaurantIds, costBracketRestaurantIds);
	vector<string> filteredVector{};

	std::copy_if(intersection.begin(), intersection.end(), std::back_inserter(filteredVector), [&ratingFilter, &restaurantMap, featured](std::string restaurantId) {
		std::tuple<float, bool> restaurantData = restaurantMap[restaurantId];
		bool isFeatured = get<1>(restaurantData);
		float rating = get<0>(restaurantData);
		// call the rating lambda
		return (featured == false || featured == isFeatured) && ratingFilter(rating);
		});

	for (std::vector<string>::iterator it(filteredVector.begin()); it != filteredVector.end(); ++it)
	{
		restaurantMap.erase(*it);
	}

	return filteredVector;
}


// Merge 2 vectors i.e append  vector 2 into vector 1
std::vector<string> MergeVector(std::vector<string> v1, std::vector<string> v2) {
	v1.insert(v1.end(), v2.begin(), v2.end());
	return v1;
}