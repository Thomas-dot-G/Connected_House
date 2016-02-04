pistApp.controller('WeatherController', ['$scope', '$http', "$interval", function($scope, $http, $interval){

	$scope.control = function(){
	$http.get('http://kgb.emn.fr:8001/channels/5/feed.json?key=ZSAVTBI11WQOSJWY&results=1')
	.then(function(result) {
		var feed = result.data.feeds[0];
		$scope.temp = parseFloat(feed.field2).toFixed(1);
		$scope.humidity = Math.floor(feed.field3);
		$scope.direction = Math.floor(feed.field7);
        $scope.speed = parseFloat(feed.field8).toFixed(1);
		$scope.pluviometry = Math.floor(feed.field4);
        $scope.pressure = Math.floor(feed.field1/100);
        $scope.luminosity = Math.floor(feed.field5);

		});
	};

	$scope.getCapteurTemp = function() {
		$http.get('http://localhost:3000/appli/getCapteurTemp.json')
		.then(function(result){
			console.log(result);
			var data = result.data;
			$scope.capteur =[1, 2, 3];
			for(i=0; i<data.length; i=i+1){
				$scope.capteur.push(parseFloat(data[i]).toFixed(1));
			}
		});
	};

	$scope.getCapteurTemp();


	$scope.control2 = function(i) {
		console.log(i);
		if ($scope.inside === undefined) {
			$http.get('http://kgb.emn.fr:8001/channels/4/field/1.json?key=94BREBU27ZFTXJ38&results=20')
			.then(function(result){
				var i = 0;
				var feed = result.data.feeds;
				var n = feed.length;
				do {
					var inter = feed[n - 1 - i].field1;
					if (inter !== null)
						$scope.inside = parseFloat(inter).toFixed(1);
					else
						i++;
				}
				while ($scope.inside === undefined && (n - 1 - i) >= 0);
			});
		}
		else {
			$http.get('http://kgb.emn.fr:8001/channels/4/field/1.json?key=94BREBU27ZFTXJ38&results=1')
			.then(function(result){
				var feed = result.data.feeds[0];
				var inter = feed.field1;
				if(inter === null)
					$scope.control2();
				else
					$scope.inside = parseFloat(inter).toFixed(1);
			});
		}
	};
	$scope.control();
	$scope.control2();

	var intervalTempInt = $interval($scope.control, 10000);
	var intervalReste= $interval($scope.control2, 10000);


  $scope.$on('$destroy', function () {
    $interval.cancel(intervalTempInt);
    $interval.cancel(intervalReste);
  });

}]);
