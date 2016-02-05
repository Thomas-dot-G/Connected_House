pistApp.controller('AdvancedWeatherController', ['$scope', '$http', "$interval", function($scope, $http, $interval){
	$scope.tempercu;

    $scope.control = function(){
        $http.get('http://kgb.emn.fr:8001/channels/5/feed.json?key=ZSAVTBI11WQOSJWY&results=1')
	.then(function(result) {
		var feed = result.data.feeds[0];
		$scope.temp = parseFloat(feed.field2).toFixed(1);
		$scope.humidity = Math.floor(feed.field3);
		$scope.direction = Math.floor(feed.field7);
        $scope.speed = Math.floor(feed.field8);
        $scope.intermediaire =(17,27*$scope.temp)/(237,7+$scope.temp)+Math.log($scope.humidity/100);

        $scope.dewpoint=parseFloat((237,7*$scope.intermediaire)/(17,27-$scope.intermediaire)).toFixed(1);

		 if($scope.speed >= 4.8) {
			 $scope.tempercu = parseFloat(13.12 + 0.6215*$scope.temp - 11.37*Math.pow($scope.speed, 0.16) + 0.3965*$scope.temp*Math.pow($scope.speed, 0.16)).toFixed(1);
		 }
		 else {
			 $scope.tempercu =parseFloat($scope.temp + 0.2*(0.1345*$scope.temp-1.59)*$scope.speed).toFixed(1);
		 }

		//On veut faire la pluie sur la journée
		$scope.pluie = parseFloat(feed.field4).toFixed(1);

    });
		
    };


    $scope.control2 = function(){
		$http.get('http://kgb.emn.fr:8001/channels/5/field/8.json?key=ZSAVTBI11WQOSJWY&results=100')
	.then(function(result) {
		var sum = 0;
        var n = 0;
        var feeds = result.data.feeds;
        feeds.forEach(function(element){
          if (element.field8 !== null)
            sum += parseFloat(element.field8);
            n++;
        });
        $scope.averageWind = (sum/n).toFixed(1);
        //gusts
        $scope.gust = 0;
        feeds.forEach(function(element) {
        	if (element.field8 > 1.5*$scope.averageWind && element.field8 > $scope.gust) {
        		$scope.gust = element.field8;
        	}
        });
	});
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
