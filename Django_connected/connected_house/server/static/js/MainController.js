	var jours = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];

 $(document).ready(function () {
    current();
});

	if (storage.get('forecast') !== null) {
		if (new Date(storage.get('forecast').expire) <= new Date()) {
			storage.remove('forecast');
		}
		else {
			$scope.days = [];
			$scope.temperature = [];
			$scope.icon = [];
			$scope.input = false;
			$scope.load = false;
			$scope.errors = false;
			$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+storage.get('forecast').city+',FR&mode=json&units=metric&cnt=7&APPID='+
	APIKEYOPENWEATHER)
			.then(function(result) {
				var list = result.data.list;
				$scope.city = result.data.city.name;
				list.forEach(function(element){
					$scope.icon.push({image:element.weather[0].icon, alt:element.weather[0].main, title:element.weather[0].description});
					var dateObject = new Date(element.dt * 1000);
					var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
					$scope.days.push(dateReadable);
					$scope.temperature.push(Math.floor(element.temp.day));
				});
				var tomorrow = new Date();
				tomorrow.setDate(tomorrow.getDate() + 2);
				storage.remove('forecast');
				storage.set('forecast', {city: $scope.city, expire: tomorrow});
				$scope.load = true;
				$scope.ready = true;
				
			});
		}
	}
	
	$scope.submitCity = function() {
		$scope.days = [];
		$scope.temperature = [];
		$scope.icon = [];
		$scope.input = false;
		$scope.load = false;
		$scope.errors = false;
		$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+$scope.city+',FR&mode=json&units=metric&cnt=7&APPID='+
APIKEYOPENWEATHER)
		.then(function(result) {
			if (result.data.cod === "200") {
				var list = result.data.list;
				$scope.city = result.data.city.name;
				list.forEach(function(element){
					$scope.icon.push({image:element.weather[0].icon, alt:element.weather[0].main, title:element.weather[0].description});
					var dateObject = new Date(element.dt * 1000);
					var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
					$scope.days.push(dateReadable);
					$scope.temperature.push(Math.floor(element.temp.day));
				});
				var tomorrow = new Date();
				tomorrow.setDate(tomorrow.getDate() + 2);
				storage.remove('forecast');
				storage.set('forecast', {city: $scope.city, expire: tomorrow});
				$scope.load = true;
				$scope.ready = true;
			}
			else {
				$scope.errorCity = $scope.city;
				$scope.errors = true;
				$scope.load = true;
				$scope.ready = false;
				$scope.input = true;
			}
			
		});
	};
	

	$scope.button = function() {
		$scope.city = "";
		storage.remove('forecast');
		$scope.load = true;
		$scope.ready = false;
		$scope.input = true;
		$scope.errors = false;
	};
	
	function control(){
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
	}
	
	function control2(){
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
				if(inter !== null)
					$scope.inside = parseFloat(inter).toFixed(1);
			});
		}
	}
	
	control();
	control2();
	
	var intervalTempInt = $interval(control, 10000);
	var intervalReste= $interval(control2, 10000);

	function current() {
		$.getJSON('http://127.0.0.1:8000/energy/electricity/global/', function(result) {
				var donnees = result
					.filter(function(element) {
						return element.field6 !== null;
					})
					.map(function(a) {
				        var date = a.created_at;
				        var x = new Date(date).getTime();
				        var y = parseInt(a.field6);
				        return [x, y*220];
				    });
				$('#container').highcharts({
			        chart: {
			          type: 'line',
			          zoomType: 'x'
			        },
			        title: {
			          text: 'Electricity'
			        },
			        subtitle: {
			          text: 'Current consumption'
			        },
			        rangeSelector: {
			          buttons: [{
			            type: 'hour',
			            count: 1,
			            text: '1h'
			          }, {
			            type: 'day',
			            count: 1,
			            text: '1d'
			          }, {
			            type: 'month',
			            count: 1,
			            text: '1m'
			          }, {
			            type: 'year',
			            count: 1,
			            text: '1y'
			          }, {
			            type: 'all',
			            text: 'All'
			          }],
			          inputEnabled: false, // it supports only days
			          selected: 4 // all
			        },
			        xAxis: {
			          type: 'datetime',
			          //                minRange: 3600 * 1000, // one hour
			          ordinal: false
			        },
			        yAxis: {
			        //                 min: 0,
			        //                 max: 50,
			          floor: 0,
			        },
			        plotOptions: {
			          line: {
			            gapSize: 250
			          }
			        },
			        navigator: {
			          adaptToUpdatedData: false,
			          series: {
			            data: donnees
			          }
			        },
			        scrollbar: {
			          liveRedraw: false
			        },
			        series: [{
			        	showInLegend: false,
			          	data: donnees,
			          	dataGrouping: {
			            enabled: false
			          }
			        }]
			      });
			});
	}
	current();
	current();//deuxieme appel pour forcer le redimensionnement (car sinon bug sur Safari)
	var intervalPromiseCurrent = $interval(current, 20000);


/*pistApp.directive('btn', function() {
	return {
		link: function (scope, element, attrs) {
			
		}
	}
});*/