pistApp.controller('PhotovoltaicController', ['$scope', '$http', '$interval', function($scope, $http, $interval){
	$scope.errors = false;
	$scope.loaded = false;
	$scope.inverter = {
		number: 1,
	};

	$scope.getPhotoData = function(start, end){
		$scope.scatter = false;
		if (start > end) { 
			$scope.errors = true;
			$scope.message = 'Please enter dates in chronological order';
		}
		else { 
			$scope.errors = false;
			$scope.loaded = true;
			$http.get('http://photovolta2.univ-nantes.fr/photovolta-from-sql.php?ondul='+$scope.inverter.number+'&start='+new Date(start).getTime()+'&end='+new Date(end).getTime())
			.then(function(result) {
				$scope.feeds = result.data.feeds;
				var donnees = result.data.feeds.map(function(a) {
			    	var x = new Date(parseInt(a.date)).getTime();
			    	var y = parseInt(a.value);
			    	return [x, y];
		      	});
				$('#container').highcharts({
					chart: {
			          type: 'scatter',
			        },
			        title: {
			            text: 'Power AC - Inverter '+$scope.inverter.number,
			            x: -20 //center
			        },
			        subtitle: {
			            text: 'Source: http://photovolta2.univ-nantes.fr/',
			            x: -20
			        },
			        xAxis: {
			            type: 'datetime',
			            ordinal: false,
			        },
			        yAxis: {
			            title: {
			                text: 'Power AC (W)'
			            },
			            plotLines: [{
			                value: 0,
			                width: 1,
			                color: '#808080'
			            }],
			            floor: 0
			        },
			        tooltip: {
			            valueSuffix: 'W',
			            headerFormat: '<b>{series.name}</b><br>',
                    	pointFormat: '{point.x} , {point.y}',
                    	formatter: function() {
		                    return  '<b>' + this.series.name +'</b><br/>' + Highcharts.dateFormat('%e %b. %Y', new Date(this.x)) + ', ' + this.y + 'W';
		                }
			        },
			        legend: {
			            layout: 'vertical',
			            align: 'right',
			            verticalAlign: 'middle',
			            borderWidth: 0
			        },
			        series: [{
			        	name: 'Power AC',
			        	data: donnees
			       }]
			    });
			$scope.scatter = true;
			});
		}
	};

	$scope.back = function() {
		$scope.loaded = false;
		$scope.scatter = false;
	};

	//Date picker settings
	$scope.datepickers = {
	    startDt: false,
	    endDt: false
    };

	$scope.today = function() {
		$scope.startDt = Date.parse(new Date());
		$scope.endDt = Date.parse(new Date());
  	};
  	$scope.today();

	$scope.clear = function () {
		$scope.startDt = null;
		$scope.endDt = null;
	};


	$scope.disabled = function(date, mode) {
	    return ( mode === 'day' && ( date > new Date()));
	  };


	$scope.open = function($event, which) {
        $event.preventDefault();
        $event.stopPropagation();

        $scope.datepickers[which]= true;
      };

	$scope.dateOptions = {
		formatYear: 'yy',
		startingDay: 1
	};

	$scope.formats = ['dd/MM/yyyy', 'dd-MMMM-yyyy', 'yyyy/MM/dd', 'dd.MM.yyyy', 'shortDate'];
	$scope.format = $scope.formats[0];

	//http://stackoverflow.com/questions/22269964/how-to-use-2-or-more-angular-ui-bootstrap-datepicker-in-1-form
}]);