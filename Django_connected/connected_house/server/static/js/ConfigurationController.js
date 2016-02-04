ConfigurationApp.controller('ConfigurationController', ['$scope', '$http', '$interval', function($scope, $http, $interval){
	$scope.channels = [];

	function configuration() {
		$scope.channels = [];
		$http.get('http://localhost:3000/appli/configuration.json')
		.then(function(result) {
			data = result.data;
			for (var i = 0; i<data.channels.length; i++) {
				var obj = {
					channel: data.channels[i].channel,
					field: data.fields[i],
				};
				$scope.channels.push(obj);
			}
		});
	}

	String.prototype.capitalizeFirstLetter = function() {
	    return this.charAt(0).toUpperCase() + this.slice(1);
	};


	configuration();
	var intervalConfig = $interval(configuration, 10000);

	$scope.$on('$destroy', function () {
		$interval.cancel(intervalConfig);
	});
}]);
