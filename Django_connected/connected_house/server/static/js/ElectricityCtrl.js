 $(document).ready(function () {
    $( ".averageWaiting" ).hide();
    $( ".currentWaiting" ).hide();
    getGlobal();
});

  function getAverage() {
    var xmlHttp = new XMLHttpRequest();
    $( ".averageWaiting" ).show();
    $( ".averageReady" ).hide();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
          console.log(xmlHttp.responseText);
          var jsonResponse = JSON.parse(xmlHttp.responseText);
          $( ".averageReady" ).show();
          $( ".averageWaiting" ).hide();
          $( ".averageReady" ).replaceWith( "<h3 class='averageReady'>"+(jsonResponse["data"])+" kWh <small> Since " +(jsonResponse["since"])+ "</small> </h3>" );
        }
    }
    xmlHttp.open("GET", 'http://127.0.0.1:8000/energy/electricity/avg', true); // true for asynchronous 
    xmlHttp.send(null);
};

  function getCurrent() {
    var xmlHttp = new XMLHttpRequest();
    $( ".currentWaiting" ).show();
    $( ".currentReady" ).hide();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
          $( ".currentReady" ).show();
          $( ".currentWaiting" ).hide();
          $( ".currentReady" ).replaceWith( "<h3 class='currentReady'>"+xmlHttp.responseText+" W</h3>" );
        }
    }
    xmlHttp.open("GET", 'http://127.0.0.1:8000/energy/electricity/current', true); // true for asynchronous 
    xmlHttp.send(null);
};
  
  

window.setInterval(function(){
   getCurrent();
   getAverage();
}, 50000);




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
    $( ".globalwaiting" ).show();
    $( ".globalready" ).hide();
    var ready = false;
    $.getJSON('http://kgb.emn.fr:8001/util/my-from-sql.php?channel_id=4&field=field6&callback=?', function(data) {
              // create the chart
              console.log(data[0]);
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
              $( ".globalwaiting" ).hide();
              $( ".globalready" ).show();
        });
  };
  