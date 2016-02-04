var pistApp = angular.module('pistApp', ['ngRoute', 'angularLocalStorage', 'ui.bootstrap']);
var APIKEYOPENWEATHER = 'e2cc44b4dc40d5a89d3fe28740dd1c18';


pistApp.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
    	when('/', {
    		templateUrl: 'pages/index.html',
        controller: 'MainController',
    	}).
    	when('/energy', {
        templateUrl: 'pages/energy.html',
        controller: 'MainController'
      }).
      when('/energy/electricity', {
        templateUrl: 'pages/electricity.html',
        controller: 'ElectricityController'
      }).
      when('/energy/water', {
        templateUrl: 'pages/water.html',
        controller: 'MainController'
      }).
      when('/energy/photovoltaic', {
        templateUrl: 'pages/photovoltaic.html',
        controller: 'PhotovoltaicController'
      }).
    	when('/weather', {
      	templateUrl: 'pages/weather.html',
      	controller: 'WeatherController'
    	}).
      when('/forecast', {
        templateUrl: 'pages/forecast.html',
        controller: 'ForecastController'
      }).
	  when('/advanced', {
        templateUrl: 'pages/advanced.html',
        controller: 'AdvancedWeatherController'
      }).
    when('/configuration', {
        templateUrl: 'pages/configuration.html',
        controller: 'ConfigurationController'
      }).
    	otherwise({
      	redirectTo: '/'
    	});
  	}
]);

