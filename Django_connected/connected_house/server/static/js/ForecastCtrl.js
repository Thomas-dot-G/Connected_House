var jours = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];

$(document).ready(function () {
	//storage.clearAll();
	$(".ready").hide();
	$(".error").hide();
	console.log("start");

	if (localStorage.getItem('forecastcity') !== null && localStorage.getItem('forecastexp') !== null) {
		if (new Date(localStorage.getItem('forecastexp').expire) <= new Date()) {
			localStorage.removeItem('forecastcity');
		}
		else {
			var city = localStorage.getItem('forecastcity');
			var days = [];
			var temp = [];
			var image = [];
			var weather = [];
			var wind = [];
			$(".ready").hide();
			$(".error").hide();
			var xmlHttp = new XMLHttpRequest();
    		xmlHttp.onreadystatechange = function() { 
        		if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
			//$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+localStorage.getItem('forecast').city+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER)
			//.then(function(result) {
					console.log("getresult");
					var result = JSON.parse(xmlHttp.responseText);
					var list = result.list;
					var city = result.city.name;
					list.forEach(function(element){
						var dateObject = new Date(element.dt * 1000);
						var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
						days.push(dateReadable);
						temp.push({morn: Math.floor(element.temp.morn), eve: Math.floor(element.temp.eve)});
						image.push(element.weather[0].icon);
						weather.push(element.weather[0].description);
						wind.push(Math.floor(element.speed));
					});
					var htmldays = "";
					for(var i=0; i<days.length; i++){
						htmldays = htmldays+'<th class="days">'+days[i]+'</th>';

					}
					$( ".days" ).replaceWith(htmldays);

					var htmltemp = "";
					for(var i=0; i<temp.length; i++){
						htmltemp = htmltemp+'<td class="temperature">'+temp[i].morn+'°C | '+temp[i].eve+'°C</td>';

					}
					$( ".temperature" ).replaceWith(htmltemp);

					var htmlweather = "";
					for(var i=0; i<weather.length; i++){
						htmlweather = htmlweather+'<td class="weather">'+weather[i]+'</td>';

					}
					$( ".weather" ).replaceWith(htmlweather);

					var htmlimg = "";
					for(var i=0; i<image.length; i++){
						htmlimg = htmlimg+'<td class="image"><img src="../static/images/weather/'+image[i]+'.png"></td>';
					}
					console.log(htmlimg);
					$( ".image" ).replaceWith(htmlimg);

					var htmlwind = "";
					for(var i=0; i<wind.length; i++){
						htmlwind = htmlwind+'<td>'+wind[i]+' km/h</td>';
					}
					$( ".wind" ).replaceWith(htmlwind);
					var tomorrow = new Date();
					tomorrow.setDate(tomorrow.getDate() + 2);
					localStorage.removeItem('forecastcity');
					localStorage.removeItem('forecastexp');
					localStorage.setItem('forecastcity', city);
					localStorage.setItem('forecastexp', tomorrow);
					$(".ready").show();
					$(".notready").hide();
				}
			};
			xmlHttp.open("GET", 'http://api.openweathermap.org/data/2.5/forecast/daily?q='+localStorage.getItem('forecastcity')+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER, true); // true for asynchronous 
    		xmlHttp.send(null);
			
    			
		}
	}
});

	function submitCity() {
		console.log('submitcity');
		var days = [];
		var temp = [];
		var image = [];
		var weather = [];
		var wind = [];
		var city = document.getElementById("city").value;
		console.log(city);
		$(".ready").hide();
		$(".error").hide();
		var xmlHttp = new XMLHttpRequest();
		xmlHttp.onreadystatechange = function() { 
    		if (xmlHttp.readyState == 4 && xmlHttp.status == 200){
	//$http.get('http://api.openweathermap.org/data/2.5/forecast/daily?q='+$scope.city+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER)
	//.then(function(result) {
				var result = JSON.parse(xmlHttp.responseText);
				console.log(result);
				var list = result.list;
				var city = result.city.name;
				list.forEach(function(element){
					var dateObject = new Date(element.dt * 1000);
					var dateReadable = jours[dateObject.getDay()]+' '+dateObject.getDate();
					days.push(dateReadable);
					temp.push({morn: Math.floor(element.temp.morn), eve: Math.floor(element.temp.eve)});
					image.push(element.weather[0].icon);
					weather.push(element.weather[0].description);
					wind.push(Math.floor(element.speed));
				});
				var tomorrow = new Date();
				tomorrow.setDate(tomorrow.getDate() + 2);
				/*var a = new Date();
				a.setTime(a.getTime() + 1000 * 60);*/
				localStorage.removeItem('forecastcity');
				localStorage.removeItem('forecastexp');
				localStorage.setItem('forecastcity', city);
				localStorage.setItem('forecastexp', tomorrow);
				$(".ready").show();
			}
			else {
				$( ".cityname" ).replaceWith('<strong class="cityname"> '+ city +'</strong>');
				$(".error").show();
			}
		};
		xmlHttp.open("GET", 'http://api.openweathermap.org/data/2.5/forecast/daily?q='+city+',FR&mode=json&units=metric&cnt=7&APPID='+APIKEYOPENWEATHER, true); // true for asynchronous 
		xmlHttp.send(null);
		
	};

