pistApp.controller('ForecastController', ['$scope', '$http', 'storage', function($scope, $http, storage){
	var jours = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
	//storage.clearAll();
	$scope.ready = true;
	$scope.errors = false;

	if (storage.get('forecast') !== null) {
		if (new Date(storage.get('forecast').expire) <= new Date()) {
			storage.remove('forecast');
		}
		else {
			$scope.city = storage.get('forecast').city;
			$scope.days = [];
			$scope.temp = [];
			$scope.image = [];
			$scope.weather = [];
			$scope.wind = [];
			$scope.ready = false;
			$scope.errors = false;
			$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+storage.get('forecast').city+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER)
			.then(function(result) {
				var list = result.data.list;
				$scope.city = result.data.city.name;
				list.forEach(function(element){
					var dateObject = new Date(element.dt * 1000);
					var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
					$scope.days.push(dateReadable);
					$scope.temp.push({morn: Math.floor(element.temp.morn), eve: Math.floor(element.temp.eve)});
					$scope.image.push(element.weather[0].icon);
					$scope.weather.push(element.weather[0].description);
					$scope.wind.push(Math.floor(element.speed));
				});
				var tomorrow = new Date();
				tomorrow.setDate(tomorrow.getDate() + 2);
				storage.remove('forecast');
				storage.set('forecast', {city: $scope.city, expire: tomorrow});
				$scope.ready = true;
			});
		}
	}

	

	

	$scope.submitCity = function() {
		$scope.days = [];
		$scope.temp = [];
		$scope.image = [];
		$scope.weather = [];
		$scope.wind = [];
		$scope.ready = false;
		$scope.errors = false;
		$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+$scope.city+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER)
		.then(function(result) {
			if (result.data.cod === "200") {
				var list = result.data.list;
				$scope.city = result.data.city.name;
				list.forEach(function(element){
					var dateObject = new Date(element.dt * 1000);
					var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
					$scope.days.push(dateReadable);
					$scope.temp.push({morn: Math.floor(element.temp.morn), eve: Math.floor(element.temp.eve)});
					$scope.image.push(element.weather[0].icon);
					$scope.weather.push(element.weather[0].description);
					$scope.wind.push(Math.floor(element.speed));
				});
				var tomorrow = new Date();
				tomorrow.setDate(tomorrow.getDate() + 2);
				/*var a = new Date();
				a.setTime(a.getTime() + 1000 * 60);*/
				storage.remove('forecast');
				storage.set('forecast', {city: $scope.city, expire: tomorrow});
				$scope.ready = true;
			}
			else {
				$scope.errorCity = $scope.city;
				$scope.errors = true;
			}
		});
	};

	
	
}]);