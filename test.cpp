

#include"Restaurant.h"

// Run queries to find newly opened restaurants
vector<string>RunQueryLatestRestaurants(std::vector<std::tuple<string, float, bool, Date>> resturantVectorTuple, map<std::string, std::tuple<float, bool>>& restaurantMap, int topCount) {
	vector<string> filteredVector{};
	vector<string> resturantVectorTupleFiltered{};
	std::sort(resturantVectorTuple.begin(), resturantVectorTuple.end(), SortComparatorDateAndRating);

	for (std::vector<std::tuple<string, float, bool, Date>>::iterator it(resturantVectorTuple.begin()); it != resturantVectorTuple.end();it++)
	{
		string id = get<0>(*it);
		if (restaurantMap.count(id) > 0) {
			resturantVectorTupleFiltered.push_back(id);
		}
	}


	int sizeLimiter = std::min((int)resturantVectorTupleFiltered.size(), 4);
	auto top4NewResByDate = std::vector<std::string>(resturantVectorTupleFiltered.begin(), resturantVectorTupleFiltered.begin() + sizeLimiter);
	for (auto str : top4NewResByDate) {
		filteredVector.push_back(str);
	}

	for (std::vector<string>::iterator it(filteredVector.begin()); it != filteredVector.end(); ++it)
	{
		restaurantMap.erase(*it);
	}

	return filteredVector;
}


/// <summary>
/// Main function that is to be implemented
/// </summary>
/// <param name="user"></param>
/// <param name="availableRestaurants"></param>
/// <param name="topCount"></param>
/// <returns></returns>
std::vector<string> GetRestaurantRecommendations(User user, std::vector<Restaurant> availableRestaurants) {
	int topCount = 100;
	if (user.costBracket.size() <= 0 || user.cuisines.size() <= 0) {
		return {};
	}

	// store the cuisines by orders in desc order
	std::sort(user.cuisines.rbegin(), user.cuisines.rend(), SortCuisineTrackingComparator);
	// store the cost brakets by orders in desc order 
	std::sort(user.costBracket.rbegin(), user.costBracket.rend(), SortCostTrackingComparator);

	std::map<Cuisine, vector<string>> cuisineRestaurantMap;
	std::map<int, vector<string>> costRestaurantMap;
	std::vector<std::tuple<std::string, float, bool, Date>> resturantDataVectorTuple;
	std::map<std::string, std::tuple<float, bool>> restaurantDataMap;


	for (Restaurant currentRestaurant : availableRestaurants) {

		Cuisine currentCuisine = currentRestaurant.cuisine;
		int currentCostBraket = currentRestaurant.costBracket;

		if (cuisineRestaurantMap.count(currentCuisine) <= 0) {
			cuisineRestaurantMap.insert({ currentCuisine,{currentRestaurant.restaurantId} });
		}
		else {
			cuisineRestaurantMap[currentCuisine].push_back(currentRestaurant.restaurantId);
		}

		if (costRestaurantMap.count(currentCostBraket) <= 0) {
			costRestaurantMap.insert({ currentCostBraket,{currentRestaurant.restaurantId} });
		}
		else {
			costRestaurantMap[currentCostBraket].push_back(currentRestaurant.restaurantId);
		}

		resturantDataVectorTuple.push_back({ currentRestaurant.restaurantId,currentRestaurant.rating,currentRestaurant.isRecommended,currentRestaurant.onboardedTime });
	}

	std::sort(resturantDataVectorTuple.begin(), resturantDataVectorTuple.end(), SortComparatorRating);

	for (auto tuple : resturantDataVectorTuple) {
		string id = get<0>(tuple);
		float rating = get<1>(tuple);
		bool recommanded = get<2>(tuple);
		restaurantDataMap[id] = { rating,recommanded };
	}


	std::sort(resturantDataVectorTuple.begin(), resturantDataVectorTuple.end(), SortComparatorDate);

	// cached values of restaurants
	vector<string> restaurantWithPrimaryCusine = cuisineRestaurantMap[user.cuisines[0].type];
	vector<string> restaurantWithSecondaryCusine{};
	vector<string> restaurantWithPrimaryCostBracket = costRestaurantMap[user.costBracket[0].type];
	vector<string> restaurantWIthSecondaryCostBraket{};

	if (user.cuisines.size() > 2) {
		restaurantWithSecondaryCusine = MergeVector(cuisineRestaurantMap[user.cuisines[1].type], cuisineRestaurantMap[user.cuisines[2].type]);
	}
	else if (user.cuisines.size() > 1) {
		restaurantWithSecondaryCusine = cuisineRestaurantMap[user.cuisines[1].type];
	}

	if (user.costBracket.size() > 2) {
		restaurantWIthSecondaryCostBraket = MergeVector(costRestaurantMap[user.costBracket[1].type], costRestaurantMap[user.costBracket[2].type]);
	}
	else if (user.costBracket.size() > 1) {
		restaurantWIthSecondaryCostBraket = costRestaurantMap[user.costBracket[1].type];
	}

	vector<string> query1 = RunQuery(restaurantWithPrimaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, true, [](float rating) ->bool {
		return true;
		});


	vector<string> query2 = RunQuery(restaurantWithPrimaryCusine, restaurantWIthSecondaryCostBraket, restaurantDataMap, true, [](float rating) ->bool {
		return true;
		});


	vector<string> query3 = RunQuery(restaurantWithSecondaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, true, [](float rating) ->bool {
		return true;
		});


	//all restaurant of primary cuisine,primary cost bracket, rating >=4.0f
	vector<string> query4 = RunQuery(restaurantWithPrimaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, false, [](float rating) ->bool {
		return rating >= 4.0f;
		});

	//all restaurant of primary cuisine,primary cost bracket, rating >=4.5f
	vector<string> query5 = RunQuery(restaurantWithPrimaryCusine, restaurantWIthSecondaryCostBraket, restaurantDataMap, false, [](float rating) ->bool {
		return rating >= 4.5f;
		});


	//Get top 4 restaurants sorted by date and rating in desc order
	vector<string> query6 = RunQueryLatestRestaurants(resturantDataVectorTuple, restaurantDataMap, 4);

	for (std::vector<string>::iterator it(query6.begin()); it != query6.end(); ++it)
	{
		restaurantDataMap.erase(*it);
	}

	//all restaurant of secondary cuisine,primary cost bracket, rating >=4.5f
	vector<string> query7 = RunQuery(restaurantWithSecondaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, false, [](float rating) ->bool {
		return rating >= 4.5f;
		});

	//all restaurant of secondary cuisine,primary cost bracket, rating <4.0f
	vector<string> query8 = RunQuery(restaurantWithPrimaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, false, [](float rating) ->bool {
		return rating < 4.0f;
		});

	//all restaurant of secondary cuisine,primary cost bracket, rating <4.5f
	vector<string> query9 = RunQuery(restaurantWithPrimaryCusine, restaurantWIthSecondaryCostBraket, restaurantDataMap, false, [](float rating) ->bool {
		return rating < 4.0f;
		});


	//all restaurant of secondary cuisine,primary cost bracket, rating <4.5f
	vector<string> query10 = RunQuery(restaurantWithSecondaryCusine, restaurantWithPrimaryCostBracket, restaurantDataMap, false, [](float rating) ->bool {
		return rating < 4.5f;
		});


	std::vector<string> output{};
	output += query1 + query2 + query3 + query4 + query5 + query6 + query7 + query8 + query9 + query10;

	int maxx = std::min(topCount, (int)output.size());
	int remaining = topCount - maxx;

	for (auto element : restaurantDataMap) {
		string id = element.first;
		output.push_back(id);
		remaining--;
		if (remaining <= 0) {
			break;
		}
	}

	return output;
}

int main()
{
	std::vector<Restaurant> restaurant{
		{"1",Cuisine::NorthIndian,4,4.5f,true,23} ,
		{"2",Cuisine::SouthIndian,3,3.5f,true,22} ,
		{"3",Cuisine::NorthIndian,3,4.0f,true,1} ,
		{"4",Cuisine::NorthIndian,3,4.0f,true,2} ,
		{"5",Cuisine::NorthIndian,4,2.5f,true,3} ,
		{"6",Cuisine::SouthIndian,4,2.0f,true,5} ,
		{"7",Cuisine::SouthIndian,4,5.0f,true,6} ,
		{"8",Cuisine::SouthIndian,4,4.0f,true,4} ,
		{"9",Cuisine::Chinese,2,4.5f,true,7} ,
		{"10",Cuisine::Chinese,4,4.5f,true,8} ,
		{"11",Cuisine::Chinese,4,4.0f,true,3} ,
		{"12",Cuisine::Chinese,3,4.0f,true,6} ,
		{"13",Cuisine::NorthIndian,3,4.5f,true,22} ,
		{"14",Cuisine::SouthIndian,3,4.5f,true,12} ,
		{"15",Cuisine::SouthIndian,3,4.0f,true,15} ,
		{"16",Cuisine::NorthIndian,3,4.5f,true,11}

	};
	User user{
		{
			{Cuisine::SouthIndian,20},
			{Cuisine::NorthIndian,30},
			{Cuisine::Chinese,26},
		},
		{
			{2,20},
			{3,11},
			{4,30},
			{5,10},
			{1,5}
		}
	};

	std::vector<string> recommandedList = GetRestaurantRecommendations(user, restaurant);
}
