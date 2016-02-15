  function getAverage() {
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            console.log(xmlHttp.responseText);
    }
    xmlHttp.open("GET", 'http://kgb.emn.fr:8001/channels/4/field/6.json?key=94BREBU27ZFTXJ38&results=22000', true); // true for asynchronous 
    xmlHttp.send(null);
};
  //       var since = new Date(feeds[0].created_at).format("dd/mm/yyyy");
  //       var averageElectricity = (sum*220*24/(n*1000)).toFixed(3);
  //       var averageReady = true;
  //     });
  // };
  $scope.getAverage();
  var intervalAverage = $interval($scope.getAverage, 30000);
  
  $scope.getCurrent = function() {
    $scope.currentReady = false;
    if ($scope.currentElectricity === undefined) {
      $http.get('http://kgb.emn.fr:8001/channels/4/field/6.json?key=94BREBU27ZFTXJ38&results=20')
      .then(function(result){
        var i = 0;
        var feed = result.data.feeds;
        var n = feed.length;
        do {
          var value = feed[n - 1 - i].field6;
          if (value !== null) {
            $scope.currentElectricity = parseFloat(value).toFixed(1);
            $scope.currentReady = true;
          }
          else
            i++;
        }
        while ($scope.currentElectricity === undefined && (n - 1 - i) >= 0);
      });
    }
    else {
      $http.get('http://kgb.emn.fr:8001/channels/4/field/6.json?key=94BREBU27ZFTXJ38&results=1')
      .then(function(result){
        var feed = result.data.feeds[0];
        var value = feed.field6;
        if(value !== null) { 
          $scope.currentElectricity = parseFloat(value).toFixed(1);
        }
        $scope.currentReady = true;
      });
    }
  };

  $scope.getCurrent();
  var intervalCurrent = $interval($scope.getCurrent, 30000);

  $scope.$on('$destroy', function () { 
    $interval.cancel(intervalCurrent); 
    $interval.cancel(intervalAverage); 
  });



  var example = 'lazy-loading',
    theme = 'default';

  function afterSetExtremes(e) {
    var chart = $('#container').highcharts();
    chart.showLoading('Loading data from server...');
    $.getJSON('http://kgb.emn.fr:8001/util/my-from-sql.php?channel_id=4&field=field6&start=' + Math.round(e.min) +
        '&end=' + Math.round(e.max) + '&callback=?', function(data) {
        if (data[0].length === 2 && chart.series[0].type === 'candlestick') {
          chart.series[0].update({type: 'line'});
          chart.series[0].setData(data);
        } 
        else if (data[0].length === 5 && chart.series[0].type === 'line') {
          chart.series[0].setData(data);
          chart.series[0].update({type: 'candlestick'});
        } 
        else {
          chart.series[0].setData(data);
        } // if
        chart.hideLoading();
    });
  }  

  function getGlobal() {
    var ready = false;
    $.getJSON('http://kgb.emn.fr:8001/util/my-from-sql.php?channel_id=4&field=field6&callback=?', function(data) {
            $scope.$apply(function(){
              $scope.ready = true;
              // create the chart
              var myDate="30-01-2015";
              myDate=myDate.split("-");
              var newDate=myDate[1]+"/"+myDate[0]+"/"+myDate[2];
              var timestamp = new Date(newDate).getTime();
              $('#container').highcharts('StockChart', {
                  chart: {
                      type: 'line',
                      zoomType: 'x'
                  },
                  plotOptions: {
                      line: {
                          gapSize: 250
                      }
                  },
                  navigator: {
                      adaptToUpdatedData: false,
                      series: {
                          data: data
                      }
                  },
                  scrollbar: {
                      liveRedraw: false
                  },
                  title: {
                      text: 'Current consumption'
                  },
                  subtitle: {
                      text: 'Electricity'
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
                      events: {
                          afterSetExtremes: afterSetExtremes
                      },
                      plotLines: [{
                        color: 'red', // Color value
                        //dashStyle: 'longdashdot', // Style of the plot line. Default to solid
                        value: timestamp, // Value of where the line will appear
                        width: 2 // Width of the line    
                      }],
                      //                minRange: 3600 * 1000, // one hour
                      ordinal: false
                  },
                  yAxis: {
                      //                 min: 0,
                      //                 max: 50,
                      floor: 0
                  },
                  series: [{
                      data: data,
                      dataGrouping: {
                          enabled: false
                      }
                  }]
              });
              $('#container').highcharts().series[0].update({
                  type: 'candlestick'
              });
              });
        });
  };
  $scope.getGlobal();
  